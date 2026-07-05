param(
	[switch]$Full,
	[switch]$StopExisting,
	[switch]$NoTail,
	[switch]$DryRun
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptRoot "..")
$SmokeScript = Join-Path $ScriptRoot "run_sandbox_smoke.ps1"
$ArchScript = Join-Path $ScriptRoot "check_sandbox_architecture.ps1"

function Invoke-GateStep {
	param(
		[string]$Name,
		[string[]]$CommandArgs
	)

	Write-Host "[AI_REFACTOR_GATE] step=$Name args=$($CommandArgs -join ' ')"
	if ($DryRun) {
		return
	}
	& $SmokeScript @CommandArgs
}

Push-Location $RepoRoot
try {
	Write-Host "[AI_REFACTOR_GATE] repo=$RepoRoot full=$($Full.IsPresent) dryRun=$($DryRun.IsPresent)"

	Write-Host "[AI_REFACTOR_GATE] step=diff-check"
	if (-not $DryRun) {
		git diff --check
	}

	if (Test-Path $ArchScript) {
		Write-Host "[AI_REFACTOR_GATE] step=sandbox-architecture"
		if (-not $DryRun) {
			& $ArchScript
		}
	}

	$CommonArgs = @()
	if ($StopExisting) {
		$CommonArgs += "-StopExisting"
	}
	if ($NoTail) {
		$CommonArgs += "-NoTail"
	}

	$Cases = @(
		@{ Name = "Sandbox3_LuaFSM_Anim"; Args = @("-Sample", "Sandbox3", "-Seconds", "24", "-ParityTrace", "-ParityTraceMaxSamples", "8") },
		@{ Name = "Sandbox6_FSM"; Args = @("-Sample", "Sandbox6", "-Seconds", "35", "-RuntimeDiag") },
		@{ Name = "Sandbox7_DecisionTree"; Args = @("-Sample", "Sandbox7", "-Seconds", "35", "-RuntimeDiag") },
		@{ Name = "Sandbox8_BehaviorTree"; Args = @("-Sample", "Sandbox8", "-Seconds", "35", "-RuntimeDiag", "-ParityTrace", "-ParityTraceMaxSamples", "8") },
		@{ Name = "Sandbox16_PerceptionPressure"; Args = @("-Preset", "ai_perf_100", "-Seconds", "55", "-RuntimeDiag", "-ParityTrace", "-ParityTraceMaxSamples", "8") },
		@{ Name = "Sandbox18_TacticsCpp"; Args = @("-Preset", "chapter9_tactics_cpp", "-Seconds", "45", "-RuntimeDiag", "-ParityTrace", "-ParityTraceMaxSamples", "8") }
	)

	if ($Full) {
		$Cases += @(
			@{ Name = "Sandbox14_HearingDanger"; Args = @("-Preset", "hearing_danger", "-Seconds", "55", "-RuntimeDiag", "-AiScheduler") },
			@{ Name = "Sandbox18_TacticsPressure"; Args = @("-Preset", "chapter9_tactics_cpp_pressure", "-Seconds", "65", "-RuntimeDiag", "-ParityTrace", "-ParityTraceMaxSamples", "10") },
			@{ Name = "Sandbox17_LegacyParity"; Args = @("-Preset", "chapter9_tactics_legacy_parity", "-Seconds", "45", "-RuntimeDiag", "-ParityTrace", "-ParityTraceMaxSamples", "8") }
		)
	}

	foreach ($Case in $Cases) {
		Invoke-GateStep -Name $Case.Name -CommandArgs ($CommonArgs + $Case.Args)
	}

	Write-Host "[AI_REFACTOR_GATE] result=PASS cases=$($Cases.Count)"
}
finally {
	Pop-Location
}
