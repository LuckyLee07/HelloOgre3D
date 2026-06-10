param(
	[string]$LegacyRoot = "E:\Workspace\HelloOgre3DX",
	[int]$LegacySeconds = 35,
	[int]$Seconds = 45,
	[int]$TraceDelayMs = 1000,
	[int]$TraceIntervalMs = 500,
	[int]$TraceMaxSamples = 16,
	[int]$TraceMaxAgents = 8,
	[int]$Seed = 20260608,
	[int]$FirstAgentId = 115,
	[int]$LightCount = 3,
	[double]$PositionTolerance = 1.05,
	[double]$TargetTolerance = 1.5,
	[double]$TimeToleranceMs = 250.0,
	[int]$CellTolerance = 8,
	[string]$Python = "python",
	[switch]$Visible,
	[switch]$NoTail
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).Path
$TmpDir = Join-Path $RepoRoot "tmp"
$RunId = "{0:yyyyMMddHHmmssfff}-{1}" -f (Get-Date), $PID

$LegacyScript = Join-Path $ScriptRoot "run_legacy_chapter9_parity.ps1"
$SmokeScript = Join-Path $ScriptRoot "run_sandbox_smoke.ps1"
$CompareScript = Join-Path $ScriptRoot "compare_parity_trace.py"
$ModernLog = Join-Path $RepoRoot "bin\Sandbox.log"
$ModernDebugLog = Join-Path $RepoRoot "bin\Sandbox_d.log"
$LegacyTrace = Join-Path $TmpDir "chapter9_legacy_parity_$RunId.jsonl"
$ModernTrace = Join-Path $TmpDir "chapter9_modern_parity_$RunId.log"
$CompareLog = Join-Path $TmpDir "chapter9_parity_compare_$RunId.txt"

foreach ($path in @($LegacyScript, $SmokeScript, $CompareScript)) {
	if (-not (Test-Path -LiteralPath $path)) {
		throw "Required gate dependency missing: $path"
	}
}

New-Item -ItemType Directory -Force -Path $TmpDir | Out-Null
Remove-Item -LiteralPath $ModernLog -Force -ErrorAction SilentlyContinue
Remove-Item -LiteralPath $ModernDebugLog -Force -ErrorAction SilentlyContinue

Write-Host "[CH9_PARITY] runId=$RunId"
Write-Host "[CH9_PARITY] legacyRoot=$LegacyRoot"
Write-Host "[CH9_PARITY] legacyTrace=$LegacyTrace"
Write-Host "[CH9_PARITY] modernTrace=$ModernTrace"
Write-Host "[CH9_PARITY] compareLog=$CompareLog"
Write-Host "[CH9_PARITY] samples=$TraceMaxSamples delayMs=$TraceDelayMs intervalMs=$TraceIntervalMs seed=$Seed"

& $LegacyScript `
	-LegacyRoot $LegacyRoot `
	-TracePath $LegacyTrace `
	-Seconds $LegacySeconds `
	-TraceDelayMs $TraceDelayMs `
	-TraceIntervalMs $TraceIntervalMs `
	-TraceMaxSamples $TraceMaxSamples `
	-TraceMaxAgents $TraceMaxAgents `
	-Seed $Seed `
	-FirstAgentId $FirstAgentId `
	-LightCount $LightCount `
	-Visible:$Visible.IsPresent `
	-NoTail:$NoTail.IsPresent

& $SmokeScript `
	-Preset chapter9_tactics_legacy_parity `
	-ParityTrace `
	-ParityTraceDelayMs $TraceDelayMs `
	-ParityTraceIntervalMs $TraceIntervalMs `
	-ParityTraceMaxSamples $TraceMaxSamples `
	-ParityTraceMaxAgents $TraceMaxAgents `
	-StopExisting `
	-Seconds $Seconds `
	-Visible:$Visible.IsPresent `
	-NoTail:$NoTail.IsPresent

$sourceModernLog = $ModernLog
if (-not (Test-Path -LiteralPath $sourceModernLog) -and (Test-Path -LiteralPath $ModernDebugLog)) {
	$sourceModernLog = $ModernDebugLog
}
if (-not (Test-Path -LiteralPath $sourceModernLog)) {
	throw "Modern parity log missing: $ModernLog"
}
Copy-Item -LiteralPath $sourceModernLog -Destination $ModernTrace -Force

$compareArgs = @(
	$CompareScript,
	$LegacyTrace,
	$ModernTrace,
	"--position-tolerance",
	([string]$PositionTolerance),
	"--target-tolerance",
	([string]$TargetTolerance),
	"--time-tolerance-ms",
	([string]$TimeToleranceMs),
	"--cell-tolerance",
	([string]$CellTolerance)
)
$compareOutput = & $Python @compareArgs 2>&1
$compareExitCode = $LASTEXITCODE
$compareOutput | Tee-Object -FilePath $CompareLog

if ($compareExitCode -ne 0) {
	Write-Host "[CH9_PARITY] status=FAIL"
	exit $compareExitCode
}

Write-Host "[CH9_PARITY] status=PASS"
