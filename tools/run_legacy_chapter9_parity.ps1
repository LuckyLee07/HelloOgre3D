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
	[switch]$DisableFixedSpawn,
	[switch]$Visible,
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
}
foreach ($key in $legacyEnv.Keys) {
	Set-Item -Path "Env:$key" -Value $legacyEnv[$key]
	[Environment]::SetEnvironmentVariable($key, $legacyEnv[$key], "Process")
}

Write-Host "[LEGACY] exe=$exePath"
Write-Host "[LEGACY] trace=$tracePath"
Write-Host "[LEGACY] seconds=$Seconds visible=$($Visible.IsPresent) keepAlive=$($KeepAlive.IsPresent) seed=$Seed fixedSpawn=$(-not $DisableFixedSpawn.IsPresent)"

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

if ($Visible) {
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
