# Chapter-8 perception parity gate.
# Runs the legacy book demo (HelloOgre3DX chapter_8_perception.exe, fixed spawn +
# ParityTrace埋点) and the modern Sandbox11 (chapter8_perception_parity preset,
# -ParityTrace), then compares逐帧 pos/target/team/alive with tolerances.
#
# Legacy side needs no recompile: chapter_8 loads its Lua from
# src/chapter_8_perception/script at runtime, and ParityTrace.lua is pure Lua.
#
# Usage:
#   tools\run_chapter8_parity_gate.ps1
#   tools\run_chapter8_parity_gate.ps1 -Visible          # 显示两个窗口便于肉眼比对
param(
	[string]$LegacyRoot = "E:\Workspace\HelloOgre3DX",
	[int]$LegacySeconds = 22,
	[int]$Seconds = 22,
	[int]$TraceDelayMs = 1000,
	[int]$TraceIntervalMs = 500,
	[int]$TraceMaxSamples = 16,
	[int]$TraceMaxAgents = 6,
	[int]$Seed = 20260812,
	[double]$PositionTolerance = 1.05,
	[double]$TargetTolerance = 1.5,
	[double]$TimeToleranceMs = 250.0,
	[switch]$Visible,
	[switch]$NoTail
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
$TmpDir = Join-Path $RepoRoot "tmp"
$RunId = "{0:yyyyMMddHHmmssfff}-{1}" -f (Get-Date), $PID

$SmokeScript = Join-Path $ScriptRoot "run_sandbox_smoke.ps1"
$CompareScript = Join-Path $ScriptRoot "compare_parity_trace.py"
$LegacyExe = Join-Path $LegacyRoot "bin\x32\release\chapter_8_perception.exe"
$LegacyWork = Join-Path $LegacyRoot "bin\x32\release"
$LegacyTrace = Join-Path $TmpDir "chapter8_legacy_parity_$RunId.jsonl"
$ModernTrace = Join-Path $TmpDir "chapter8_modern_parity_$RunId.log"
$CompareLog = Join-Path $TmpDir "chapter8_parity_compare_$RunId.txt"

foreach ($path in @($SmokeScript, $CompareScript, $LegacyExe)) {
	if (-not (Test-Path -LiteralPath $path)) {
		throw "Required gate dependency missing: $path"
	}
}

# Resolve a Python launcher (this box's bare `python` shim is broken; prefer `py -3`).
$PyExe = "py"
$PyPre = @("-3")
if (-not (Get-Command $PyExe -ErrorAction SilentlyContinue)) {
	$PyExe = "python"
	$PyPre = @()
}

New-Item -ItemType Directory -Force -Path $TmpDir | Out-Null

Write-Host "[CH8_PARITY] runId=$RunId"
Write-Host "[CH8_PARITY] legacyExe=$LegacyExe"
Write-Host "[CH8_PARITY] legacyTrace=$LegacyTrace"
Write-Host "[CH8_PARITY] modernTrace=$ModernTrace"
Write-Host "[CH8_PARITY] samples=$TraceMaxSamples delayMs=$TraceDelayMs intervalMs=$TraceIntervalMs seed=$Seed"

# ---- Legacy side (HelloOgre3DX chapter_8_perception.exe) ----
Get-Process chapter_8_perception -ErrorAction SilentlyContinue | Stop-Process -Force
Remove-Item -LiteralPath $LegacyTrace -Force -ErrorAction SilentlyContinue

$legacyEnv = @{
	HELLO_PARITY_TRACE = "1"
	HELLO_PARITY_TRACE_FILE = $LegacyTrace
	HELLO_PARITY_FIXED_SPAWN = "1"
	HELLO_PARITY_FIXED_TEAMS = "1"
	HELLO_PARITY_TRACE_DELAY_MS = "$TraceDelayMs"
	HELLO_PARITY_TRACE_INTERVAL_MS = "$TraceIntervalMs"
	HELLO_PARITY_TRACE_MAX_SAMPLES = "$TraceMaxSamples"
	HELLO_PARITY_TRACE_MAX_AGENTS = "$TraceMaxAgents"
	HELLO_PARITY_SEED = "$Seed"
	HELLO_SAMPLE_SEED = "$Seed"
	HELLO_SAMPLE_PRESET = "legacy_chapter8_fixed"
}
foreach ($k in $legacyEnv.Keys) { Set-Item -Path "Env:$k" -Value $legacyEnv[$k] }

$startArgs = @{ FilePath = $LegacyExe; WorkingDirectory = $LegacyWork; PassThru = $true }
if (-not $Visible) { $startArgs.WindowStyle = "Hidden" }
$legacyProc = Start-Process @startArgs
Write-Host "[CH8_PARITY] legacy started pid=$($legacyProc.Id) seconds=$LegacySeconds"
Start-Sleep -Seconds $LegacySeconds
$legacyProc.Refresh()
if (-not $legacyProc.HasExited) { Stop-Process -Id $legacyProc.Id -Force }
foreach ($k in $legacyEnv.Keys) { Remove-Item -Path "Env:$k" -ErrorAction SilentlyContinue }
Start-Sleep -Milliseconds 500

if (-not (Test-Path -LiteralPath $LegacyTrace)) { throw "legacy parity trace missing: $LegacyTrace" }
$legSamples = (Select-String -Path $LegacyTrace -Pattern '"type":"sample"').Count
Write-Host "[CH8_PARITY] legacy samples=$legSamples"

# ---- Modern side (Sandbox11 + chapter8_perception_parity) ----
$env:HELLO_PARITY_TRACE_FILE = $ModernTrace
& $SmokeScript `
	-Sample Sandbox11 `
	-Preset chapter8_perception_parity `
	-ParityTrace `
	-ParityTraceDelayMs $TraceDelayMs `
	-ParityTraceIntervalMs $TraceIntervalMs `
	-ParityTraceMaxSamples $TraceMaxSamples `
	-ParityTraceMaxAgents $TraceMaxAgents `
	-StopExisting `
	-Seconds $Seconds `
	-Visible:$Visible.IsPresent `
	-NoTail:$NoTail.IsPresent
Remove-Item -Path "Env:HELLO_PARITY_TRACE_FILE" -ErrorAction SilentlyContinue

if (-not (Test-Path -LiteralPath $ModernTrace)) {
	$fallback = Join-Path $RepoRoot "bin\Sandbox.log"
	if (Test-Path -LiteralPath $fallback) { Copy-Item -LiteralPath $fallback -Destination $ModernTrace -Force }
	else { throw "modern parity trace missing: $ModernTrace" }
}
$modSamples = (Select-String -Path $ModernTrace -Pattern '"type":"sample"').Count
Write-Host "[CH8_PARITY] modern samples=$modSamples"

# ---- Compare ----
$compareArgs = $PyPre + @(
	$CompareScript, $LegacyTrace, $ModernTrace,
	"--position-tolerance", ([string]$PositionTolerance),
	"--target-tolerance", ([string]$TargetTolerance),
	"--time-tolerance-ms", ([string]$TimeToleranceMs)
)
$compareOutput = & $PyExe @compareArgs 2>&1
$compareExitCode = $LASTEXITCODE
$compareOutput | Tee-Object -FilePath $CompareLog

if ($compareExitCode -ne 0) {
	Write-Host "[CH8_PARITY] status=DIVERGED (expected in phase-1; see $CompareLog)"
	exit $compareExitCode
}
Write-Host "[CH8_PARITY] status=PASS"
