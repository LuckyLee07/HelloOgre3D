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
	[switch]$CaptureLegacyExternal,
	[switch]$Visible,
	[switch]$KeepAlive
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
		public static extern bool SetForegroundWindow(IntPtr hWnd);

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
		if ($Process.MainWindowHandle -ne [IntPtr]::Zero) {
			return $Process.MainWindowHandle
		}
		Start-Sleep -Milliseconds 100
	} while ((Get-Date) -lt $deadline)

	throw "Timed out waiting for main window: pid=$($Process.Id)"
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
		[void][Chapter9VisualCapture.NativeMethods]::MoveWindow($handle, $WindowX, $WindowY, $WindowWidth, $WindowHeight, $true)
		[void][Chapter9VisualCapture.NativeMethods]::SetForegroundWindow($handle)
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
			[void][Chapter9VisualCapture.NativeMethods]::SetForegroundWindow($handle)
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
		HELLO_RENDER_CAPTURE = "1"
		HELLO_RENDER_CAPTURE_DIR = $OldOutputDir
		HELLO_RENDER_CAPTURE_PREFIX = "old"
		HELLO_RENDER_CAPTURE_MS = ($sortedCaptures -join ",")
	}

	$process = $null
	Set-ProcessEnvironment -Values $legacyEnv -Body {
		$script:CapturedProcess = Start-Process -FilePath $LegacyExe -WorkingDirectory $LegacyWorkDir -WindowStyle Normal -PassThru
	}
	$process = $script:CapturedProcess
	$script:CapturedProcess = $null
	Write-Host "[CH9_VISUAL] started name=old pid=$($process.Id) seconds=$Seconds"

	Start-Sleep -Seconds $Seconds

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
	Get-Process -Name "HelloOgre3D" -ErrorAction SilentlyContinue | Stop-Process -Force
	Remove-Item -LiteralPath $ModernLog -Force -ErrorAction SilentlyContinue
	Remove-Item -LiteralPath $ModernDebugLog -Force -ErrorAction SilentlyContinue

	$modernEnv = @{
		HELLO_SANDBOX_SMOKE_TEST = "1"
		HELLO_SANDBOX_SAMPLE = "Sandbox17"
		HELLO_SANDBOX_SMOKE_RUN_ID = "visual-$RunId"
		HELLO_SAMPLE_PRESET = "chapter9_tactics_legacy_parity"
		HELLO_SAMPLE_SEED = "$Seed"
		HELLO_RENDER_CAPTURE = "1"
		HELLO_RENDER_CAPTURE_DIR = $NewOutputDir
		HELLO_RENDER_CAPTURE_PREFIX = "new"
		HELLO_RENDER_CAPTURE_MS = ($sortedCaptures -join ",")
	}

	$process = $null
	Set-ProcessEnvironment -Values $modernEnv -Body {
		if ($Visible) {
			$script:CapturedProcess = Start-Process -FilePath $ModernExe -WorkingDirectory $ModernWorkDir -WindowStyle Normal -PassThru
		}
		else {
			$script:CapturedProcess = Start-Process -FilePath $ModernExe -WorkingDirectory $ModernWorkDir -WindowStyle Normal -PassThru
		}
	}
	$process = $script:CapturedProcess
	$script:CapturedProcess = $null
	Write-Host "[CH9_VISUAL] started name=new pid=$($process.Id) seconds=$Seconds"

	Start-Sleep -Seconds $Seconds

	if ($KeepAlive) {
		Write-Host "[CH9_VISUAL] keepAlive name=new pid=$($process.Id)"
	}
	elseif (-not $process.HasExited) {
		Stop-Process -Id $process.Id -Force
		Write-Host "[CH9_VISUAL] stopped name=new pid=$($process.Id)"
	}
	elseif ($process.ExitCode -ne 0) {
		throw "Modern process exited with code $($process.ExitCode)."
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
	if ($missing.Count -gt 0) {
		throw "Modern render capture missing files: $($missing -join '; ')"
	}
}

Write-Host "[CH9_VISUAL] runId=$RunId"
Write-Host "[CH9_VISUAL] outputDir=$OutputDir"
Write-Host "[CH9_VISUAL] capturesMs=$($sortedCaptures -join ',')"
Write-Host "[CH9_VISUAL] seconds=$Seconds captureLegacyExternal=$($CaptureLegacyExternal.IsPresent)"

if ($CaptureLegacyExternal) {
	Invoke-LegacyExternalCapture
}
else {
	Invoke-LegacyRenderCapture
}

Invoke-ModernRenderCapture

Write-Host "[CH9_VISUAL] status=PASS outputDir=$OutputDir"
