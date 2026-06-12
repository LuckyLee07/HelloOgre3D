param(
	[string]$LegacyRoot = "E:\Workspace\HelloOgre3DX",
	[string]$OutputDir = "",
	[string[]]$CaptureMs = @("1000", "2500", "4500", "6500", "8500"),
	[int]$Seed = 20260608,
	[int]$FirstAgentId = 115,
	[int]$LightCount = 3,
	[int]$Seconds = 14,
	[int]$StartupTimeoutMs = 15000,
	[int]$LegacyWarmupMs = 6000,
	[int]$SettleMs = 800,
	[int]$WindowX = 40,
	[int]$WindowY = 40,
	[int]$WindowWidth = 1280,
	[int]$WindowHeight = 720,
	[int]$ModernCaptureRetries = 3,
	[int]$ModernRetryDelaySeconds = 5,
	[switch]$CaptureLegacyExternal,
	[string]$ReuseLegacyCaptureDir = "",
	[switch]$SkipAnalysis,
	[switch]$ModernNoSmoke,
	[switch]$Visible,
	[switch]$KeepAlive,
	[switch]$InfluenceOnly,
	[switch]$CoverageRed,
	[ValidateSet("current", "top")]
	[string]$CameraPreset = "current"
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
$TmpDir = Join-Path $RepoRoot "tmp"
$RunId = "{0:yyyyMMddHHmmssfff}-{1}" -f (Get-Date), $PID
if ([string]::IsNullOrWhiteSpace($OutputDir)) {
	$OutputDir = Join-Path $TmpDir "chapter9_visual_$RunId"
}

function Convert-CaptureTimes {
	param(
		[string[]]$Values
	)

	$result = @()
	foreach ($value in $Values) {
		if ([string]::IsNullOrWhiteSpace($value)) {
			continue
		}
		foreach ($token in ([string]$value -split "[,\s]+")) {
			if ([string]::IsNullOrWhiteSpace($token)) {
				continue
			}
			$parsed = 0
			if (-not [int]::TryParse($token, [ref]$parsed) -or $parsed -lt 0) {
				throw "Invalid capture time: $token"
			}
			$result += $parsed
		}
	}
	return @($result | Sort-Object -Unique)
}

$LegacyWorkDir = Join-Path $LegacyRoot "bin\x32\release"
$LegacyExe = Join-Path $LegacyWorkDir "chapter_9_tactics.exe"
$ModernWorkDir = Join-Path $RepoRoot "bin"
$ModernExe = Join-Path $ModernWorkDir "HelloOgre3D.exe"
$ModernLog = Join-Path $ModernWorkDir "Sandbox.log"
$ModernDebugLog = Join-Path $ModernWorkDir "Sandbox_d.log"

New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
$OldOutputDir = Join-Path $OutputDir "old"
$NewOutputDir = Join-Path $OutputDir "new"
New-Item -ItemType Directory -Force -Path $OldOutputDir | Out-Null
New-Item -ItemType Directory -Force -Path $NewOutputDir | Out-Null

$sortedCaptures = @(Convert-CaptureTimes -Values $CaptureMs)
if ($sortedCaptures.Count -eq 0) {
	throw "CaptureMs is empty."
}
$maxCaptureMs = ($sortedCaptures | Measure-Object -Maximum).Maximum
$requiredSeconds = [int][Math]::Ceiling(($maxCaptureMs + $SettleMs) / 1000.0) + 8
if ($Seconds -lt $requiredSeconds) {
	$Seconds = $requiredSeconds
}
$LegacySeconds = $Seconds
$ModernSeconds = [int][Math]::Max($Seconds, [Math]::Ceiling((($maxCaptureMs + $SettleMs) / 1000.0) * 6.0) + 20)
$TraceDelayMs = 1000
$TraceIntervalMs = 500
$ModernTraceMaxSamples = [int][Math]::Max(1, [Math]::Ceiling(($maxCaptureMs - $TraceDelayMs) / [double]$TraceIntervalMs) + 2)
$ModernTraceMaxAgents = [int][Math]::Max(1, $LightCount * 2)

if (-not (Test-Path -LiteralPath $ModernExe)) {
	throw "Modern HelloOgre3D exe not found: $ModernExe"
}

if (-not (Test-Path -LiteralPath $LegacyExe)) {
	throw "Legacy Chapter9 exe not found: $LegacyExe"
}

if (-not ("Chapter9VisualCapture.NativeMethods" -as [type])) {
	Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Chapter9VisualCapture
{
	public struct Rect
	{
		public int Left;
		public int Top;
		public int Right;
		public int Bottom;
	}

	public struct Point
	{
		public int X;
		public int Y;
	}

	public static class NativeMethods
	{
		public delegate bool EnumWindowsProc(IntPtr hWnd, IntPtr lParam);

		[DllImport("user32.dll")]
		public static extern bool EnumWindows(EnumWindowsProc callback, IntPtr extraData);

		[DllImport("user32.dll")]
		public static extern bool GetWindowRect(IntPtr hWnd, out Rect rect);

		[DllImport("user32.dll")]
		public static extern bool GetClientRect(IntPtr hWnd, out Rect rect);

		[DllImport("user32.dll")]
		public static extern bool ClientToScreen(IntPtr hWnd, ref Point point);

		[DllImport("user32.dll")]
		public static extern int GetClassName(IntPtr hWnd, StringBuilder className, int maxCount);

		[DllImport("user32.dll")]
		public static extern int GetWindowText(IntPtr hWnd, StringBuilder title, int maxCount);

		[DllImport("user32.dll")]
		public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out int processId);

		[DllImport("user32.dll")]
		public static extern bool IsWindowVisible(IntPtr hWnd);

		[DllImport("user32.dll")]
		public static extern bool MoveWindow(IntPtr hWnd, int x, int y, int width, int height, bool repaint);

		[DllImport("user32.dll")]
		public static extern bool ShowWindowAsync(IntPtr hWnd, int nCmdShow);

		[DllImport("user32.dll")]
		public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int x, int y, int cx, int cy, uint flags);

		public static IntPtr FindWindowForProcess(int targetProcessId)
		{
			IntPtr best = IntPtr.Zero;
			int bestArea = 0;
			EnumWindows(delegate(IntPtr hWnd, IntPtr lParam)
			{
				int processId;
				GetWindowThreadProcessId(hWnd, out processId);
				if (processId == targetProcessId && IsWindowVisible(hWnd))
				{
					StringBuilder className = new StringBuilder(256);
					GetClassName(hWnd, className, className.Capacity);
					string cls = className.ToString();
					if (cls == "ConsoleWindowClass" || cls.StartsWith("BAIDU_CLASS_IME"))
						return true;

					Rect rect;
					if (!GetWindowRect(hWnd, out rect))
						return true;
					int width = rect.Right - rect.Left;
					int height = rect.Bottom - rect.Top;
					int area = width > 0 && height > 0 ? width * height : 0;
					if (cls == "OgreD3D9Wnd")
						area += 100000000;
					if (area > bestArea)
					{
						best = hWnd;
						bestArea = area;
					}
				}
				return true;
			}, IntPtr.Zero);
			return best;
		}
	}
}
"@
}

function Set-ProcessEnvironment {
	param(
		[hashtable]$Values,
		[scriptblock]$Body
	)

	$oldValues = @{}
	foreach ($key in $Values.Keys) {
		$oldValues[$key] = [Environment]::GetEnvironmentVariable($key, "Process")
		Set-Item -Path "Env:$key" -Value ([string]$Values[$key])
	}

	try {
		& $Body
	}
	finally {
		foreach ($key in $Values.Keys) {
			if ($null -eq $oldValues[$key]) {
				Remove-Item -Path "Env:$key" -ErrorAction SilentlyContinue
			}
			else {
				Set-Item -Path "Env:$key" -Value $oldValues[$key]
			}
		}
	}
}

function Test-ScreenCaptureAvailable {
	try {
		Add-Type -AssemblyName System.Drawing
		Add-Type -AssemblyName System.Windows.Forms
		$bounds = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds
		if ($bounds.Width -le 0 -or $bounds.Height -le 0) {
			return "Primary screen has invalid bounds."
		}
		$bitmap = New-Object System.Drawing.Bitmap 1, 1
		$graphics = [System.Drawing.Graphics]::FromImage($bitmap)
		try {
			$graphics.CopyFromScreen($bounds.Left, $bounds.Top, 0, 0, $bitmap.Size)
		}
		finally {
			$graphics.Dispose()
			$bitmap.Dispose()
		}
		return ""
	}
	catch {
		return $_.Exception.Message
	}
}

function Wait-MainWindowHandle {
	param(
		[System.Diagnostics.Process]$Process,
		[int]$TimeoutMs
	)

	$deadline = (Get-Date).AddMilliseconds($TimeoutMs)
	do {
		$Process.Refresh()
		if ($Process.HasExited) {
			throw "Process exited before creating a window: pid=$($Process.Id) exitCode=$($Process.ExitCode)"
		}
		$handle = [Chapter9VisualCapture.NativeMethods]::FindWindowForProcess($Process.Id)
		if ($handle -ne [IntPtr]::Zero) {
			return $handle
		}
		Start-Sleep -Milliseconds 100
	} while ((Get-Date) -lt $deadline)

	throw "Timed out waiting for main window: pid=$($Process.Id)"
}

function Set-WindowNoActivateLayout {
	param(
		[IntPtr]$WindowHandle,
		[int]$X,
		[int]$Y,
		[int]$Width,
		[int]$Height
	)

	if ($WindowHandle -eq [IntPtr]::Zero) {
		return
	}

	$SW_SHOWNOACTIVATE = 4
	$SWP_NOZORDER = 0x0004
	$SWP_NOACTIVATE = 0x0010
	$SWP_SHOWWINDOW = 0x0040
	$SWP_NOOWNERZORDER = 0x0200
	$flags = [uint32]($SWP_NOZORDER -bor $SWP_NOACTIVATE -bor $SWP_SHOWWINDOW -bor $SWP_NOOWNERZORDER)

	[void][Chapter9VisualCapture.NativeMethods]::ShowWindowAsync($WindowHandle, $SW_SHOWNOACTIVATE)
	[void][Chapter9VisualCapture.NativeMethods]::SetWindowPos($WindowHandle, [IntPtr]::Zero, $X, $Y, $Width, $Height, $flags)
}

function Save-WindowScreenshot {
	param(
		[IntPtr]$WindowHandle,
		[string]$Path
	)

	$rect = New-Object Chapter9VisualCapture.Rect
	$topLeft = New-Object Chapter9VisualCapture.Point
	if ([Chapter9VisualCapture.NativeMethods]::GetClientRect($WindowHandle, [ref]$rect) -and
		[Chapter9VisualCapture.NativeMethods]::ClientToScreen($WindowHandle, [ref]$topLeft)) {
		$rect.Right = $topLeft.X + ($rect.Right - $rect.Left)
		$rect.Bottom = $topLeft.Y + ($rect.Bottom - $rect.Top)
		$rect.Left = $topLeft.X
		$rect.Top = $topLeft.Y
	}
	elseif (-not [Chapter9VisualCapture.NativeMethods]::GetWindowRect($WindowHandle, [ref]$rect)) {
		throw "GetClientRect/GetWindowRect failed."
	}

	$width = $rect.Right - $rect.Left
	$height = $rect.Bottom - $rect.Top
	if ($width -le 0 -or $height -le 0) {
		throw "Invalid window rect: $($rect.Left),$($rect.Top),$($rect.Right),$($rect.Bottom)"
	}

	$bitmap = New-Object System.Drawing.Bitmap $width, $height
	$graphics = [System.Drawing.Graphics]::FromImage($bitmap)
	try {
		$graphics.CopyFromScreen($rect.Left, $rect.Top, 0, 0, $bitmap.Size)
		$bitmap.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
	}
	finally {
		$graphics.Dispose()
		$bitmap.Dispose()
	}
}

function Get-WindowInfoText {
	param(
		[IntPtr]$WindowHandle
	)

	$className = New-Object System.Text.StringBuilder 256
	$title = New-Object System.Text.StringBuilder 512
	[void][Chapter9VisualCapture.NativeMethods]::GetClassName($WindowHandle, $className, $className.Capacity)
	[void][Chapter9VisualCapture.NativeMethods]::GetWindowText($WindowHandle, $title, $title.Capacity)
	return "class=$($className.ToString()) title=$($title.ToString())"
}

function Invoke-LegacyExternalCapture {
	$preflightError = Test-ScreenCaptureAvailable
	if (-not [string]::IsNullOrWhiteSpace($preflightError)) {
		Write-Host "[CH9_VISUAL] legacyExternalCapture=SKIP reason=$preflightError"
		return
	}

	Write-Host "[CH9_VISUAL] legacyExternalCapture=START exe=$LegacyExe"
	Get-Process -Name "chapter_9_tactics" -ErrorAction SilentlyContinue | Stop-Process -Force

	$legacyTracePath = Join-Path $OutputDir "legacy_trace.jsonl"
	$legacyEnv = @{
		HELLO_PARITY_TRACE = "1"
		HELLO_PARITY_TRACE_FILE = $legacyTracePath
		HELLO_PARITY_FIXED_SPAWN = "1"
		HELLO_PARITY_FIXED_TEAMS = "1"
		HELLO_PARITY_LIGHT_COUNT = "$LightCount"
		HELLO_PARITY_FIRST_AGENT_ID = "$FirstAgentId"
		HELLO_SAMPLE_PRESET = "legacy_chapter9_fixed"
		HELLO_PARITY_SEED = "$Seed"
		HELLO_SAMPLE_SEED = "$Seed"
		HELLO_CH9_CAMERA_PRESET = $CameraPreset
	}
	if ($InfluenceOnly) {
		$legacyEnv["HELLO_CH9_VISUAL_ISOLATION"] = "1"
		$legacyEnv["HELLO_CH9_HIDE_UI"] = "1"
		$legacyEnv["HELLO_CH9_HIDE_AGENT_RENDER"] = "1"
	}
	if ($CoverageRed) {
		$legacyEnv["HELLO_CH9_FORCE_GRID_RED"] = "1"
	}

	$process = $null
	Set-ProcessEnvironment -Values $legacyEnv -Body {
		$script:CapturedProcess = Start-Process -FilePath $LegacyExe -WorkingDirectory $LegacyWorkDir -WindowStyle Normal -PassThru
	}
	$process = $script:CapturedProcess
	$script:CapturedProcess = $null

	try {
		$handle = Wait-MainWindowHandle -Process $process -TimeoutMs $StartupTimeoutMs
		Write-Host "[CH9_VISUAL] legacyWindow $(Get-WindowInfoText -WindowHandle $handle)"
		Set-WindowNoActivateLayout -WindowHandle $handle -X $WindowX -Y $WindowY -Width $WindowWidth -Height $WindowHeight
		Start-Sleep -Milliseconds $SettleMs
		Start-Sleep -Milliseconds $LegacyWarmupMs

		$stopwatch = [System.Diagnostics.Stopwatch]::StartNew()
		foreach ($captureTime in $sortedCaptures) {
			$remainingMs = $captureTime - [int]$stopwatch.ElapsedMilliseconds
			if ($remainingMs -gt 0) {
				Start-Sleep -Milliseconds $remainingMs
			}
			$process.Refresh()
			if ($process.HasExited) {
				throw "Legacy process exited before capture ${captureTime}ms: exitCode=$($process.ExitCode)"
			}
			Start-Sleep -Milliseconds 100
			$filePath = Join-Path $OldOutputDir ("old_{0:D5}ms.png" -f $captureTime)
			Save-WindowScreenshot -WindowHandle $handle -Path $filePath
			Write-Host "[CH9_VISUAL] captured $filePath"
		}
	}
	finally {
		if ($KeepAlive) {
			Write-Host "[CH9_VISUAL] keepAlive name=old pid=$($process.Id)"
		}
		elseif ($process -ne $null -and -not $process.HasExited) {
			Stop-Process -Id $process.Id -Force
			Write-Host "[CH9_VISUAL] stopped name=old pid=$($process.Id)"
		}
	}
}

function Invoke-ReuseLegacyCapture {
	param(
		[string]$SourceDir
	)

	if ([string]::IsNullOrWhiteSpace($SourceDir)) {
		throw "ReuseLegacyCaptureDir is empty."
	}

	$resolvedSourceDir = (Resolve-Path -LiteralPath $SourceDir).Path
	$sourceOldDir = Join-Path $resolvedSourceDir "old"
	$sourceTracePath = Join-Path $resolvedSourceDir "legacy_trace.jsonl"
	if (-not (Test-Path -LiteralPath $sourceOldDir)) {
		throw "Reuse legacy old image directory not found: $sourceOldDir"
	}
	if (-not (Test-Path -LiteralPath $sourceTracePath)) {
		throw "Reuse legacy trace not found: $sourceTracePath"
	}

	Write-Host "[CH9_VISUAL] legacyReuse=START source=$resolvedSourceDir"
	Get-ChildItem -LiteralPath $sourceOldDir -File | Copy-Item -Destination $OldOutputDir -Force
	Copy-Item -LiteralPath $sourceTracePath -Destination (Join-Path $OutputDir "legacy_trace.jsonl") -Force

	$missing = @()
	foreach ($captureTime in $sortedCaptures) {
		$filePath = Join-Path $OldOutputDir ("old_{0:D5}ms.png" -f $captureTime)
		if (-not (Test-Path -LiteralPath $filePath)) {
			$missing += $filePath
		}
	}
	if ($missing.Count -gt 0) {
		throw "Reuse legacy capture missing files: $($missing -join '; ')"
	}
	Write-Host "[CH9_VISUAL] legacyReuse=PASS source=$resolvedSourceDir"
}

function Invoke-LegacyRenderCapture {
	Write-Host "[CH9_VISUAL] legacyRenderCapture=START exe=$LegacyExe"
	Get-Process -Name "chapter_9_tactics" -ErrorAction SilentlyContinue | Stop-Process -Force

	$legacyTracePath = Join-Path $OutputDir "legacy_trace.jsonl"
	$legacyEnv = @{
		HELLO_PARITY_TRACE = "1"
		HELLO_PARITY_TRACE_FILE = $legacyTracePath
		HELLO_PARITY_FIXED_SPAWN = "1"
		HELLO_PARITY_FIXED_TEAMS = "1"
		HELLO_PARITY_LIGHT_COUNT = "$LightCount"
		HELLO_PARITY_FIRST_AGENT_ID = "$FirstAgentId"
		HELLO_SAMPLE_PRESET = "legacy_chapter9_fixed"
		HELLO_PARITY_SEED = "$Seed"
		HELLO_SAMPLE_SEED = "$Seed"
		HELLO_CH9_CAMERA_PRESET = $CameraPreset
		HELLO_RENDER_CAPTURE = "1"
		HELLO_RENDER_CAPTURE_DIR = $OldOutputDir
		HELLO_RENDER_CAPTURE_PREFIX = "old"
		HELLO_RENDER_CAPTURE_MS = ($sortedCaptures -join ",")
		HELLO_RENDER_CAPTURE_CLOCK = "simulation"
	}
	if ($InfluenceOnly) {
		$legacyEnv["HELLO_CH9_VISUAL_ISOLATION"] = "1"
		$legacyEnv["HELLO_CH9_HIDE_UI"] = "1"
		$legacyEnv["HELLO_CH9_HIDE_AGENT_RENDER"] = "1"
	}
	if ($CoverageRed) {
		$legacyEnv["HELLO_CH9_FORCE_GRID_RED"] = "1"
	}

	$process = $null
	Set-ProcessEnvironment -Values $legacyEnv -Body {
		$script:CapturedProcess = Start-Process -FilePath $LegacyExe -WorkingDirectory $LegacyWorkDir -WindowStyle Normal -PassThru
	}
	$process = $script:CapturedProcess
	$script:CapturedProcess = $null
	Write-Host "[CH9_VISUAL] started name=old pid=$($process.Id) seconds=$LegacySeconds"
	try {
		$handle = Wait-MainWindowHandle -Process $process -TimeoutMs $StartupTimeoutMs
		Write-Host "[CH9_VISUAL] legacyWindow $(Get-WindowInfoText -WindowHandle $handle)"
		Set-WindowNoActivateLayout -WindowHandle $handle -X $WindowX -Y $WindowY -Width $WindowWidth -Height $WindowHeight
	}
	catch {
		Write-Host "[CH9_VISUAL] legacyWindow=SKIP reason=$($_.Exception.Message)"
	}

	Start-Sleep -Seconds $LegacySeconds

	if ($KeepAlive) {
		Write-Host "[CH9_VISUAL] keepAlive name=old pid=$($process.Id)"
	}
	elseif (-not $process.HasExited) {
		Stop-Process -Id $process.Id -Force
		Write-Host "[CH9_VISUAL] stopped name=old pid=$($process.Id)"
	}
	elseif ($process.ExitCode -ne 0) {
		throw "Legacy process exited with code $($process.ExitCode)."
	}

	$missing = @()
	foreach ($captureTime in $sortedCaptures) {
		$filePath = Join-Path $OldOutputDir ("old_{0:D5}ms.png" -f $captureTime)
		if (-not (Test-Path -LiteralPath $filePath)) {
			$missing += $filePath
		}
		else {
			Write-Host "[CH9_VISUAL] captured $filePath"
		}
	}
	if ($missing.Count -gt 0) {
		throw "Legacy render capture missing files: $($missing -join '; ')"
	}
}

function Invoke-ModernRenderCapture {
	Write-Host "[CH9_VISUAL] modernRenderCapture=START exe=$ModernExe"

	$modernTracePath = Join-Path $OutputDir "modern_trace.jsonl"
	$modernPreset = "chapter9_tactics_legacy_parity"
	if ($InfluenceOnly) {
		$modernPreset = "chapter9_tactics_influence_only"
	}
	$modernEnv = @{
		HELLO_SANDBOX_SAMPLE = "Sandbox17"
		HELLO_SAMPLE_PRESET = $modernPreset
		HELLO_SAMPLE_SEED = "$Seed"
		HELLO_SIM_FPS = "30"
		HELLO_PARITY_TRACE = "1"
		HELLO_PARITY_TRACE_DELAY_MS = "$TraceDelayMs"
		HELLO_PARITY_TRACE_INTERVAL_MS = "$TraceIntervalMs"
		HELLO_PARITY_TRACE_MAX_SAMPLES = "$ModernTraceMaxSamples"
		HELLO_PARITY_TRACE_MAX_AGENTS = "$ModernTraceMaxAgents"
		HELLO_PARITY_TRACE_FILE = $modernTracePath
		HELLO_PARITY_TRACE_LOG = "0"
		HELLO_RENDER_CAPTURE = "1"
		HELLO_RENDER_CAPTURE_DIR = $NewOutputDir
		HELLO_RENDER_CAPTURE_PREFIX = "new"
		HELLO_RENDER_CAPTURE_MS = ($sortedCaptures -join ",")
		HELLO_RENDER_CAPTURE_CLOCK = "simulation"
		HELLO_CH9_CAMERA_PRESET = $CameraPreset
	}
	if ($InfluenceOnly) {
		$modernEnv["HELLO_CH9_VISUAL_ISOLATION"] = "1"
		$modernEnv["HELLO_CH9_HIDE_UI"] = "1"
		$modernEnv["HELLO_CH9_HIDE_AGENT_RENDER"] = "1"
	}
	if ($CoverageRed) {
		$modernEnv["HELLO_CH9_FORCE_GRID_RED"] = "1"
	}
	if (-not $ModernNoSmoke) {
		$modernEnv["HELLO_SANDBOX_SMOKE_TEST"] = "1"
		$modernEnv["HELLO_SANDBOX_SMOKE_RUN_ID"] = "visual-$RunId"
	}

	if ($ModernCaptureRetries -lt 1) {
		$ModernCaptureRetries = 1
	}

	for ($attempt = 1; $attempt -le $ModernCaptureRetries; $attempt++) {
		Write-Host "[CH9_VISUAL] modernRenderCaptureAttempt=$attempt/$ModernCaptureRetries"

		$existingProcesses = @(Get-Process -Name "HelloOgre3D" -ErrorAction SilentlyContinue)
		if ($existingProcesses.Count -gt 0) {
			$existingProcesses | Stop-Process -Force
			foreach ($existing in $existingProcesses) {
				try { Wait-Process -Id $existing.Id -Timeout 5 -ErrorAction SilentlyContinue } catch {}
			}
		}
		Start-Sleep -Milliseconds 500

		Remove-Item -LiteralPath $ModernLog -Force -ErrorAction SilentlyContinue
		Remove-Item -LiteralPath $ModernDebugLog -Force -ErrorAction SilentlyContinue
		Get-ChildItem -LiteralPath $NewOutputDir -Filter "new_*.png" -File -ErrorAction SilentlyContinue | Remove-Item -Force
		Remove-Item -LiteralPath $modernTracePath -Force -ErrorAction SilentlyContinue

		$process = $null
		Set-ProcessEnvironment -Values $modernEnv -Body {
			$script:CapturedProcess = Start-Process -FilePath $ModernExe -WorkingDirectory $ModernWorkDir -WindowStyle Normal -PassThru
		}
		$process = $script:CapturedProcess
		$script:CapturedProcess = $null
		Write-Host "[CH9_VISUAL] started name=new pid=$($process.Id) seconds=$ModernSeconds"
		try {
			$handle = Wait-MainWindowHandle -Process $process -TimeoutMs $StartupTimeoutMs
			Write-Host "[CH9_VISUAL] modernWindow $(Get-WindowInfoText -WindowHandle $handle)"
			Set-WindowNoActivateLayout -WindowHandle $handle -X ($WindowX + $WindowWidth + 24) -Y $WindowY -Width $WindowWidth -Height $WindowHeight
		}
		catch {
			Write-Host "[CH9_VISUAL] modernWindow=SKIP reason=$($_.Exception.Message)"
		}

		$deadline = (Get-Date).AddSeconds($ModernSeconds)
		while ((Get-Date) -lt $deadline) {
			Start-Sleep -Milliseconds 500
			if ($process.HasExited) {
				break
			}

			$allCaptured = $true
			foreach ($captureTime in $sortedCaptures) {
				$filePath = Join-Path $NewOutputDir ("new_{0:D5}ms.png" -f $captureTime)
				if (-not (Test-Path -LiteralPath $filePath)) {
					$allCaptured = $false
					break
				}
			}

			$traceFinished = $false
			if (Test-Path -LiteralPath $modernTracePath) {
				$traceFinished = $null -ne (Select-String -LiteralPath $modernTracePath -Pattern '"type":"finish"' -SimpleMatch | Select-Object -First 1)
			}

			if ($allCaptured -and $traceFinished) {
				break
			}
		}

		$processError = ""
		if ($KeepAlive) {
			Write-Host "[CH9_VISUAL] keepAlive name=new pid=$($process.Id)"
		}
		elseif (-not $process.HasExited) {
			Stop-Process -Id $process.Id -Force
			Write-Host "[CH9_VISUAL] stopped name=new pid=$($process.Id)"
		}
		elseif ($process.ExitCode -ne 0) {
			$processError = "Modern process exited with code $($process.ExitCode)."
		}

		$missing = @()
		foreach ($captureTime in $sortedCaptures) {
			$filePath = Join-Path $NewOutputDir ("new_{0:D5}ms.png" -f $captureTime)
			if (-not (Test-Path -LiteralPath $filePath)) {
				$missing += $filePath
			}
			else {
				Write-Host "[CH9_VISUAL] captured $filePath"
			}
		}

		if ($processError -eq "" -and $missing.Count -eq 0) {
			return
		}

		$deviceFailure = $false
		if (Test-Path -LiteralPath $ModernLog) {
			$deviceFailure = [bool](Select-String -LiteralPath $ModernLog -Pattern "Cannot create device" -Quiet)
		}
		$errorMessage = $processError
		if ($missing.Count -gt 0) {
			$errorMessage = "Modern render capture missing files: $($missing -join '; ')"
		}

		if ($attempt -lt $ModernCaptureRetries -and $deviceFailure) {
			Write-Host "[CH9_VISUAL] modernRenderCapture=RETRY reason=D3D9DeviceCreation delaySeconds=$ModernRetryDelaySeconds"
			Start-Sleep -Seconds $ModernRetryDelaySeconds
			continue
		}

		throw $errorMessage
	}
}

function Invoke-CaptureAnalysis {
	if ($SkipAnalysis) {
		Write-Host "[CH9_VISUAL] analysis=SKIP reason=SkipAnalysis"
		return
	}

	$analysisScript = Join-Path $ScriptRoot "analyze_chapter9_visual_capture.py"
	if (-not (Test-Path -LiteralPath $analysisScript)) {
		Write-Host "[CH9_VISUAL] analysis=SKIP reason=scriptMissing path=$analysisScript"
		return
	}

	$pythonCommand = Get-Command python -ErrorAction SilentlyContinue
	if ($null -eq $pythonCommand) {
		Write-Host "[CH9_VISUAL] analysis=SKIP reason=pythonMissing"
		return
	}

	try {
		& $pythonCommand.Source $analysisScript --capture-dir $OutputDir --capture-ms ($sortedCaptures -join ",")
		if ($LASTEXITCODE -ne 0) {
			throw "analysis exited with code $LASTEXITCODE"
		}
	}
	catch {
		Write-Host "[CH9_VISUAL] analysis=SKIP reason=$($_.Exception.Message)"
	}
}

Write-Host "[CH9_VISUAL] runId=$RunId"
Write-Host "[CH9_VISUAL] outputDir=$OutputDir"
Write-Host "[CH9_VISUAL] capturesMs=$($sortedCaptures -join ',')"
Write-Host "[CH9_VISUAL] seconds=$Seconds legacySeconds=$LegacySeconds modernSeconds=$ModernSeconds modernRetries=$ModernCaptureRetries traceMaxSamples=$ModernTraceMaxSamples modernSmoke=$(-not $ModernNoSmoke) captureLegacyExternal=$($CaptureLegacyExternal.IsPresent) reuseLegacy=$ReuseLegacyCaptureDir cameraPreset=$CameraPreset coverageRed=$($CoverageRed.IsPresent)"

if (-not [string]::IsNullOrWhiteSpace($ReuseLegacyCaptureDir)) {
	Invoke-ReuseLegacyCapture -SourceDir $ReuseLegacyCaptureDir
}
elseif ($CaptureLegacyExternal) {
	Invoke-LegacyExternalCapture
}
else {
	Invoke-LegacyRenderCapture
}

Invoke-ModernRenderCapture
Invoke-CaptureAnalysis

Write-Host "[CH9_VISUAL] status=PASS outputDir=$OutputDir"
