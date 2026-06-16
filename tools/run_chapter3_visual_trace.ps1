param(
	[string]$LegacyRoot = "E:\Workspace\HelloOgre3DX",
	[string]$OutputDir = "",
	[string[]]$CaptureMs = @("2000"),
	[int]$Seed = 20260615,
	[int]$Seconds = 18,
	[int]$SimulationFps = 30,
	[int]$TraceDelayMs = 0,
	[int]$StartupTimeoutMs = 15000,
	[ValidateSet("A", "B", "Both")]
	[string]$Mode = "Both",
	[switch]$Visible,
	[switch]$KeepAlive
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
$TmpDir = Join-Path $RepoRoot "tmp"
$RunId = "{0:yyyyMMddHHmmssfff}-{1}" -f (Get-Date), $PID
if ([string]::IsNullOrWhiteSpace($OutputDir)) {
	$OutputDir = Join-Path $TmpDir "chapter3_visual_trace_$RunId"
}

# Legacy chapter_3_animation exe (old engine).
$LegacyWorkDir = Join-Path $LegacyRoot "bin\x32\release"
$LegacyExe = Join-Path $LegacyWorkDir "chapter_3_animation.exe"
$LegacyProcessName = "chapter_3_animation"

# Modern HelloOgre3D exe.
$ModernWorkDir = Join-Path $RepoRoot "bin"
$ModernExe = Join-Path $ModernWorkDir "HelloOgre3D.exe"
$ModernProcessName = "HelloOgre3D"
$ModernLog = Join-Path $ModernWorkDir "Sandbox.log"
$ModernDebugLog = Join-Path $ModernWorkDir "Sandbox_d.log"

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

function Copy-Hashtable {
	param(
		[hashtable]$Source
	)

	$clone = @{}
	if ($null -ne $Source) {
		foreach ($key in $Source.Keys) {
			$clone[$key] = $Source[$key]
		}
	}
	return $clone
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

function Stop-ProcessesByName {
	param(
		[string]$Name
	)

	$existing = @(Get-Process -Name $Name -ErrorAction SilentlyContinue)
	if ($existing.Count -gt 0) {
		$existing | Stop-Process -Force
		foreach ($proc in $existing) {
			try { Wait-Process -Id $proc.Id -Timeout 5 -ErrorAction SilentlyContinue } catch {}
		}
	}
}

function Wait-CaptureDone {
	param(
		[System.Diagnostics.Process]$Process,
		[string]$TracePath,
		[string[]]$ScreenshotPaths,
		[int]$TimeoutSeconds
	)

	$deadline = (Get-Date).AddSeconds($TimeoutSeconds)
	while ((Get-Date) -lt $deadline) {
		Start-Sleep -Milliseconds 500
		if ($Process.HasExited) {
			break
		}

		$allCaptured = $true
		foreach ($filePath in $ScreenshotPaths) {
			if (-not (Test-Path -LiteralPath $filePath)) {
				$allCaptured = $false
				break
			}
		}

		$traceFinished = $false
		if (Test-Path -LiteralPath $TracePath) {
			$traceFinished = $null -ne (Select-String -LiteralPath $TracePath -Pattern '"type":"finish"' -SimpleMatch | Select-Object -First 1)
		}

		if ($allCaptured -and $traceFinished) {
			break
		}
	}
}

function Invoke-CaptureRun {
	param(
		[string]$Label,
		[string]$Exe,
		[string]$WorkDir,
		[string]$ProcessName,
		[hashtable]$Environment,
		[string]$TracePath,
		[string[]]$Captures,
		[string]$CaptureDir,
		[string]$CapturePrefix
	)

	Write-Host "[CH3_VISUAL] captureRun=START label=$Label exe=$Exe"
	Stop-ProcessesByName -Name $ProcessName
	Start-Sleep -Milliseconds 500

	$screenshotPaths = @()
	foreach ($captureTime in $Captures) {
		$screenshotPaths += Join-Path $CaptureDir ("{0}_{1:D5}ms.png" -f $CapturePrefix, [int]$captureTime)
	}

	# Clean stale artefacts for this run.
	Remove-Item -LiteralPath $TracePath -Force -ErrorAction SilentlyContinue
	foreach ($filePath in $screenshotPaths) {
		Remove-Item -LiteralPath $filePath -Force -ErrorAction SilentlyContinue
	}

	# Ogre needs a visible (non-minimized) window for RenderCaptureGate to grab
	# frames; chapter4 capture script always uses Normal. Keep parity here.
	$windowStyle = "Normal"

	$process = $null
	Set-ProcessEnvironment -Values $Environment -Body {
		$script:CapturedProcess = Start-Process -FilePath $Exe -WorkingDirectory $WorkDir -WindowStyle $windowStyle -PassThru
	}
	$process = $script:CapturedProcess
	$script:CapturedProcess = $null
	Write-Host "[CH3_VISUAL] started label=$Label pid=$($process.Id) seconds=$Seconds"

	try {
		Wait-CaptureDone -Process $process -TracePath $TracePath -ScreenshotPaths $screenshotPaths -TimeoutSeconds $Seconds
	}
	finally {
		if ($KeepAlive) {
			Write-Host "[CH3_VISUAL] keepAlive label=$Label pid=$($process.Id)"
		}
		elseif (-not $process.HasExited) {
			Stop-Process -Id $process.Id -Force
			Write-Host "[CH3_VISUAL] stopped label=$Label pid=$($process.Id)"
		}
		elseif ($process.ExitCode -ne 0) {
			Write-Host "[CH3_VISUAL] warn label=$Label exitCode=$($process.ExitCode)"
		}
	}

	if (-not (Test-Path -LiteralPath $TracePath)) {
		throw "[$Label] trace not produced: $TracePath"
	}
	$missing = @()
	foreach ($filePath in $screenshotPaths) {
		if (-not (Test-Path -LiteralPath $filePath)) {
			$missing += $filePath
		}
		else {
			Write-Host "[CH3_VISUAL] captured $filePath"
		}
	}
	if ($missing.Count -gt 0) {
		Write-Host "[CH3_VISUAL] warn label=$Label missingScreenshots=$($missing -join '; ')"
	}
	Write-Host "[CH3_VISUAL] captureRun=DONE label=$Label trace=$TracePath"
}

# --- Pre-flight ---------------------------------------------------------
if (-not (Test-Path -LiteralPath $ModernExe)) {
	throw "Modern HelloOgre3D exe not found: $ModernExe"
}
if (-not (Test-Path -LiteralPath $LegacyExe)) {
	throw "Legacy chapter_3_animation exe not found: $LegacyExe"
}

New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null

$sortedCaptures = @(Convert-CaptureTimes -Values $CaptureMs)
if ($sortedCaptures.Count -eq 0) {
	throw "CaptureMs is empty."
}
$captureText = ($sortedCaptures -join ",")

# --- Common environment shared by both modes/versions -------------------
$commonEnv = @{
	HELLO_SAMPLE_PRESET          = "chapter3_compare"
	HELLO_PARITY_SEED            = "$Seed"
	HELLO_SAMPLE_SEED            = "$Seed"
	HELLO_PARITY_TRACE           = "1"
	HELLO_PARITY_TRACE_DELAY_MS  = "$TraceDelayMs"
	HELLO_RENDER_CAPTURE         = "1"
	HELLO_RENDER_CAPTURE_MS      = $captureText
	HELLO_CH3_COMPARE            = "1"
	HELLO_CH3_HIDE_UI            = "1"
}

# --- Python probe -------------------------------------------------------
$pythonCommand = Get-Command python -ErrorAction SilentlyContinue
$compareScript = Join-Path $ScriptRoot "compare_parity_trace.py"

Write-Host "[CH3_VISUAL] runId=$RunId"
Write-Host "[CH3_VISUAL] outputDir=$OutputDir"
Write-Host "[CH3_VISUAL] mode=$Mode capturesMs=$captureText seed=$Seed seconds=$Seconds simulationFps=$SimulationFps traceDelayMs=$TraceDelayMs visible=$($Visible.IsPresent) keepAlive=$($KeepAlive.IsPresent)"

function Build-VersionEnvironment {
	param(
		[hashtable]$BaseEnv,
		[bool]$IsModern,
		[string]$TracePath,
		[string]$CaptureDir,
		[string]$CapturePrefix
	)

	$env = Copy-Hashtable -Source $BaseEnv
	$env["HELLO_PARITY_TRACE_FILE"] = $TracePath
	$env["HELLO_RENDER_CAPTURE_DIR"] = $CaptureDir
	$env["HELLO_RENDER_CAPTURE_PREFIX"] = $CapturePrefix
	if ($IsModern) {
		$env["HELLO_SANDBOX_SAMPLE"] = "Sandbox3"
		$env["HELLO_SANDBOX_SMOKE_TEST"] = "1"
		$env["HELLO_PARITY_TRACE_LOG"] = "0"
	}
	return $env
}

# --- Mode A: fixed-step parity ------------------------------------------
function Invoke-ModeA {
	Write-Host "[CH3_VISUAL] mode=A type=fixedStepParity"
	$modeDir = Join-Path $OutputDir "modeA"
	$oldDir = Join-Path $modeDir "old"
	$newDir = Join-Path $modeDir "new"
	New-Item -ItemType Directory -Force -Path $oldDir | Out-Null
	New-Item -ItemType Directory -Force -Path $newDir | Out-Null

	$legacyA = Join-Path $modeDir "legacy_trace.jsonl"
	$modernA = Join-Path $modeDir "modern_trace.jsonl"

	$baseEnv = Copy-Hashtable -Source $commonEnv
	$baseEnv["HELLO_SIM_FPS"] = "$SimulationFps"
	$baseEnv["HELLO_PARITY_TRACE_INTERVAL_MS"] = "33"
	$baseEnv["HELLO_PARITY_TRACE_MAX_SAMPLES"] = "140"
	$baseEnv["HELLO_PARITY_TRACE_DELAY_MS"] = "0"

	$legacyEnv = Build-VersionEnvironment -BaseEnv $baseEnv -IsModern $false -TracePath $legacyA -CaptureDir $oldDir -CapturePrefix "old"
	$modernEnv = Build-VersionEnvironment -BaseEnv $baseEnv -IsModern $true -TracePath $modernA -CaptureDir $newDir -CapturePrefix "new"

	Invoke-CaptureRun -Label "modeA-old" -Exe $LegacyExe -WorkDir $LegacyWorkDir -ProcessName $LegacyProcessName `
		-Environment $legacyEnv -TracePath $legacyA -Captures $sortedCaptures -CaptureDir $oldDir -CapturePrefix "old"
	Invoke-CaptureRun -Label "modeA-new" -Exe $ModernExe -WorkDir $ModernWorkDir -ProcessName $ModernProcessName `
		-Environment $modernEnv -TracePath $modernA -Captures $sortedCaptures -CaptureDir $newDir -CapturePrefix "new"

	if ($null -eq $pythonCommand) {
		Write-Host "[CH3_VISUAL] compare=SKIP reason=pythonMissing"
		return
	}

	Write-Host "[CH3_VISUAL] compare=START mode=A"
	& $pythonCommand.Source $compareScript $legacyA $modernA --anim --progress-tolerance 0.05 --time-tolerance-ms 250
	$animExit = $LASTEXITCODE
	Write-Host "[CH3_VISUAL] compare=DONE mode=A exitCode=$animExit"
}

# --- Mode B: real frame-rate diagnostics --------------------------------
function Invoke-ModeB {
	Write-Host "[CH3_VISUAL] mode=B type=realtimeJitter"
	$modeDir = Join-Path $OutputDir "modeB"
	$oldDir = Join-Path $modeDir "old"
	$newDir = Join-Path $modeDir "new"
	New-Item -ItemType Directory -Force -Path $oldDir | Out-Null
	New-Item -ItemType Directory -Force -Path $newDir | Out-Null

	$legacyB = Join-Path $modeDir "legacy_realtime.jsonl"
	$modernB = Join-Path $modeDir "modern_realtime.jsonl"

	$baseEnv = Copy-Hashtable -Source $commonEnv
	# Mode B uses the real frame clock: do NOT pin HELLO_SIM_FPS.
	if ($baseEnv.ContainsKey("HELLO_SIM_FPS")) {
		$baseEnv.Remove("HELLO_SIM_FPS")
	}
	$baseEnv["HELLO_PARITY_TRACE_INTERVAL_MS"] = "0"
	$baseEnv["HELLO_PARITY_TRACE_MAX_SAMPLES"] = "600"
	$baseEnv["HELLO_PARITY_TRACE_DELAY_MS"] = "0"

	$legacyEnv = Build-VersionEnvironment -BaseEnv $baseEnv -IsModern $false -TracePath $legacyB -CaptureDir $oldDir -CapturePrefix "old"
	$modernEnv = Build-VersionEnvironment -BaseEnv $baseEnv -IsModern $true -TracePath $modernB -CaptureDir $newDir -CapturePrefix "new"

	Invoke-CaptureRun -Label "modeB-old" -Exe $LegacyExe -WorkDir $LegacyWorkDir -ProcessName $LegacyProcessName `
		-Environment $legacyEnv -TracePath $legacyB -Captures $sortedCaptures -CaptureDir $oldDir -CapturePrefix "old"
	Invoke-CaptureRun -Label "modeB-new" -Exe $ModernExe -WorkDir $ModernWorkDir -ProcessName $ModernProcessName `
		-Environment $modernEnv -TracePath $modernB -Captures $sortedCaptures -CaptureDir $newDir -CapturePrefix "new"

	if ($null -eq $pythonCommand) {
		Write-Host "[CH3_VISUAL] compare=SKIP reason=pythonMissing"
		return
	}

	Write-Host "[CH3_VISUAL] jitter=START mode=B"
	& $pythonCommand.Source $compareScript $legacyB $modernB --anim-jitter
	Write-Host "[CH3_VISUAL] jitter=DONE mode=B exitCode=$LASTEXITCODE"
}

if ($Mode -eq "A" -or $Mode -eq "Both") {
	Invoke-ModeA
}
if ($Mode -eq "B" -or $Mode -eq "Both") {
	Invoke-ModeB
}

Write-Host "[CH3_VISUAL] status=PASS outputDir=$OutputDir"
