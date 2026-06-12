param(
	[string]$LegacyRoot = "E:\Workspace\HelloOgre3DX",
	[string]$TracePath = "",
	[int]$Seconds = 35,
	[int]$TraceDelayMs = 1000,
	[int]$TraceIntervalMs = 500,
	[int]$TraceMaxSamples = 16,
	[int]$TraceMaxAgents = 8,
	[int]$Seed = 20260608,
	[int]$FirstAgentId = 115,
	[int]$LightCount = 3,
	[int]$SimulationFps = 30,
	[switch]$AgentOnly,
	[switch]$MotionProbe,
	[double]$AgentSpawnX = -8.233,
	[double]$AgentSpawnY = 0.000,
	[double]$AgentSpawnZ = -5.396,
	[double]$MotionTargetX = 1.490,
	[double]$MotionTargetY = 4.050,
	[double]$MotionTargetZ = 25.958,
	[int]$MotionStartDelayMs = 1000,
	[int]$MotionDurationMs = 4000,
	[switch]$DisableFixedSpawn,
	[switch]$Visible,
	[switch]$Activate,
	[switch]$KeepAlive,
	[switch]$NoTail
)

$ErrorActionPreference = "Stop"

$workDir = Join-Path $LegacyRoot "bin\x32\release"
$exePath = Join-Path $workDir "chapter_9_tactics.exe"
$logPath = Join-Path $workDir "Sandbox.log"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
if ([string]::IsNullOrWhiteSpace($TracePath)) {
	$tracePath = Join-Path (Join-Path $repoRoot "tmp") "legacy_chapter9_parity_trace.jsonl"
}
else {
	$tracePath = $TracePath
}
$traceDir = Split-Path -Parent $tracePath

if (-not (Test-Path $exePath)) {
	throw "chapter_9_tactics.exe not found: $exePath"
}

New-Item -ItemType Directory -Force -Path $traceDir | Out-Null
Get-Process chapter_9_tactics -ErrorAction SilentlyContinue | Stop-Process -Force
Remove-Item -LiteralPath $logPath -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $tracePath -Force -ErrorAction SilentlyContinue

$legacyEnv = @{
	HELLO_PARITY_TRACE = "1"
	HELLO_PARITY_FIXED_SPAWN = $(if ($DisableFixedSpawn) { "0" } else { "1" })
	HELLO_PARITY_FIXED_TEAMS = "1"
	HELLO_PARITY_LIGHT_COUNT = "$LightCount"
	HELLO_PARITY_FIRST_AGENT_ID = "$FirstAgentId"
	HELLO_SAMPLE_PRESET = "legacy_chapter9_fixed"
	HELLO_PARITY_SEED = "$Seed"
	HELLO_SAMPLE_SEED = "$Seed"
	HELLO_PARITY_TRACE_DELAY_MS = "$TraceDelayMs"
	HELLO_PARITY_TRACE_INTERVAL_MS = "$TraceIntervalMs"
	HELLO_PARITY_TRACE_MAX_SAMPLES = "$TraceMaxSamples"
	HELLO_PARITY_TRACE_MAX_AGENTS = "$TraceMaxAgents"
	HELLO_PARITY_TRACE_FILE = $tracePath
	HELLO_SIM_FPS = "$SimulationFps"
}
if ($AgentOnly) {
	$legacyEnv["HELLO_CH9_HIDE_UI"] = "1"
	$legacyEnv["HELLO_CH9_HIDE_INFLUENCE"] = "1"
	$legacyEnv["HELLO_CH9_SUPPRESS_DEBUG_DRAW"] = "1"
	$legacyEnv["HELLO_CH9_AGENT_COUNT"] = "1"
	$legacyEnv["HELLO_PARITY_LIGHT_COUNT"] = "1"
	$legacyEnv["HELLO_CH9_AGENT_SPAWN_OVERRIDE"] = "1"
	$legacyEnv["HELLO_CH9_AGENT_SPAWN_X"] = ([string]$AgentSpawnX)
	$legacyEnv["HELLO_CH9_AGENT_SPAWN_Y"] = ([string]$AgentSpawnY)
	$legacyEnv["HELLO_CH9_AGENT_SPAWN_Z"] = ([string]$AgentSpawnZ)
}
if ($MotionProbe) {
	$legacyEnv["HELLO_CH9_MOTION_PROBE"] = "1"
	$legacyEnv["HELLO_CH9_MOTION_TARGET_X"] = ([string]$MotionTargetX)
	$legacyEnv["HELLO_CH9_MOTION_TARGET_Y"] = ([string]$MotionTargetY)
	$legacyEnv["HELLO_CH9_MOTION_TARGET_Z"] = ([string]$MotionTargetZ)
	$legacyEnv["HELLO_CH9_MOTION_START_DELAY_MS"] = "$MotionStartDelayMs"
	$legacyEnv["HELLO_CH9_MOTION_DURATION_MS"] = "$MotionDurationMs"
}
foreach ($key in $legacyEnv.Keys) {
	Set-Item -Path "Env:$key" -Value $legacyEnv[$key]
	[Environment]::SetEnvironmentVariable($key, $legacyEnv[$key], "Process")
}

Write-Host "[LEGACY] exe=$exePath"
Write-Host "[LEGACY] trace=$tracePath"
Write-Host "[LEGACY] seconds=$Seconds visible=$($Visible.IsPresent) activate=$($Activate.IsPresent) keepAlive=$($KeepAlive.IsPresent) seed=$Seed fixedSpawn=$(-not $DisableFixedSpawn.IsPresent) agentOnly=$($AgentOnly.IsPresent) motionProbe=$($MotionProbe.IsPresent) simulationFps=$SimulationFps"

$startArgs = @{
	FilePath = $exePath
	WorkingDirectory = $workDir
	PassThru = $true
}
if (-not $Visible) {
	$startArgs.WindowStyle = "Hidden"
}

$process = Start-Process @startArgs
Write-Host "[LEGACY] started pid=$($process.Id)"

if ($Visible -and $Activate) {
	Start-Sleep -Milliseconds 1000
	try {
		$wshell = New-Object -ComObject WScript.Shell
		[void]$wshell.AppActivate($process.Id)
		Write-Host "[LEGACY] activated pid=$($process.Id)"
	}
	catch {
		Write-Host "[LEGACY] activation skipped: $($_.Exception.Message)"
	}
}

Start-Sleep -Seconds $Seconds

$process.Refresh()
if ($process.HasExited) {
	Write-Host "[LEGACY] exited pid=$($process.Id) exitCode=$($process.ExitCode)"
}

if (-not $KeepAlive -and -not $process.HasExited) {
	Stop-Process -Id $process.Id -Force
	Write-Host "[LEGACY] stopped pid=$($process.Id) after $Seconds seconds."
}

Start-Sleep -Milliseconds 500

if (-not (Test-Path $tracePath)) {
	throw "legacy parity trace missing: $tracePath"
}

$traceLines = Select-String -Path $tracePath -Pattern "\[ParityTrace\]"
$sampleLines = Select-String -Path $tracePath -Pattern '"type":"sample"'
$finishLines = Select-String -Path $tracePath -Pattern '"type":"finish"'
$errorLines = Select-String -Path $tracePath -Pattern '"type":"error"'

Write-Host "[LEGACY] parityTraceLines=$($traceLines.Count) samples=$($sampleLines.Count) finish=$($finishLines.Count) errors=$($errorLines.Count)"

if (-not $NoTail) {
	Get-Content -Path $tracePath -Tail 3
}

if ($errorLines.Count -gt 0) {
	exit 2
}
if ($sampleLines.Count -lt $TraceMaxSamples) {
	exit 3
}
if ($finishLines.Count -lt 1) {
	exit 4
}
