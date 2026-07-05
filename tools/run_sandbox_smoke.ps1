param(
	[ValidateSet(
		"Default",
		"Sandbox1",
		"Sandbox2",
		"Sandbox3",
		"Sandbox4",
		"Sandbox5",
		"Sandbox6",
		"Sandbox7",
		"Sandbox8",
		"Sandbox9",
		"Sandbox10",
		"Sandbox11",
		"Sandbox12",
		"Sandbox13",
		"Sandbox14",
		"Sandbox15",
		"Sandbox16",
		"Sandbox17",
		"Sandbox18"
	)]
	[string]$Sample = "Sandbox8",
	[int]$Seconds = 28,
	[int]$Tail = 160,
	[switch]$Visible,
	[switch]$KeepAlive,
	[switch]$StopExisting,
	[switch]$IgnoreLogErrors,
	[switch]$NoTail,
	[switch]$RuntimeDiag,
	[switch]$BlackboardSelfTest,
	[switch]$AiEventSelfTest,
	[int]$DiagMaxObjects = 8,
	[int]$DiagMaxResources = 6,
	[int]$DiagMaxEvents = 6,
	[int]$DiagFocusAgentId = 0,
	[string]$Preset = "",
	[int]$Seed = 0,
	[int]$AgentCount = 0,
	[int]$LightTeamCount = 0,
	[string]$SpawnMode = "",
	[switch]$AiScheduler,
	[int]$AiTickMs = 50,
	[int]$AiMaxPerFrame = 8,
	[switch]$VisualTrace,
	[int]$VisualTraceDelayFrames = 15,
	[int]$VisualTraceIntervalFrames = 15,
	[int]$VisualTraceMaxSamples = 8,
	[int]$VisualTraceMaxAgents = 16,
	[switch]$ParityTrace,
	[int]$ParityTraceDelayMs = 1000,
	[int]$ParityTraceIntervalMs = 500,
	[int]$ParityTraceMaxSamples = 16,
	[int]$ParityTraceMaxAgents = 8,
	[switch]$DisableSpatialIndex,
	[switch]$DisablePerceptionSystem,
	[switch]$DisablePerceptionCache,
	[int]$SpatialCellSize = 0,
	[switch]$PerfStallLog,
	[int]$PerfStallThresholdMs = 40,
	[int]$PerfSummaryIntervalMs = 3000,
	[int]$BtTickIntervalMs = 0,
	[switch]$BtTickStagger,
	[switch]$DryRun
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptRoot "..")
$BinDir = Join-Path $RepoRoot "bin"
$ExePath = Join-Path $BinDir "HelloOgre3D.exe"
$LogPath = Join-Path $BinDir "Sandbox.log"
$LogCandidates = @(
	$LogPath,
	(Join-Path $BinDir "Sandbox_d.log")
)

$SelectedSample = $Sample
if ($SelectedSample -eq "Default") {
	$SelectedSample = "Sandbox8"
}
$AiPerfPresetNames = @(
	"ai_perception_pressure",
	"ai_perf_100",
	"ai_perf_500",
	"ai_perf_1000"
)
$Chapter9TacticsCppPresetNames = @(
	"chapter9_tactics_cpp",
	"chapter9_tactics_cpp_pressure"
)
if (($Preset -eq "ai_lastknown_demo" -or $Preset -eq "chapter8_perception") -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox10"
}
if ($Preset -eq "bt_runtime_lod" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox10"
}
if ($Preset -eq "bt_runtime_budget" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox10"
}
if (($Preset -eq "bt_runtime_rebuild" -or $Preset -eq "bt_hot_reload") -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox10"
}
if ($Preset -eq "chapter8_comms" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox11"
}
if ($Preset -eq "team_blackboard" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox12"
}
if ($Preset -eq "influence_map" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox13"
}
if ($Preset -eq "hearing_danger" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox14"
}
if ($Preset -eq "formation_tactics" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox15"
}
if ($Preset -eq "chapter9_tactics_lua" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox17"
}
if ($Preset -eq "chapter9_tactics_legacy_parity" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox17"
}
if ($Preset -eq "chapter9_tactics_agent_motion_probe" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox17"
}
if (($Chapter9TacticsCppPresetNames -contains $Preset) -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox18"
}
if (($AiPerfPresetNames -contains $Preset) -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox16"
}
$BtRuntimeDiagPresetNames = @(
	"bt_runtime_lod",
	"bt_runtime_budget",
	"bt_runtime_rebuild",
	"bt_hot_reload"
)
$RuntimeDiagEnabled = $RuntimeDiag.IsPresent -or $BlackboardSelfTest.IsPresent -or $AiEventSelfTest.IsPresent -or ($BtRuntimeDiagPresetNames -contains $Preset)

$KnownEnvNames = @(
	"HELLO_SANDBOX_SMOKE_TEST",
	"HELLO_SANDBOX_SAMPLE",
	"HELLO_SANDBOX_SMOKE_RUN_ID",
	"HELLO_RUNTIME_DIAGNOSTIC_SELF_TEST",
	"HELLO_AI_BLACKBOARD_SELF_TEST",
	"HELLO_AI_EVENT_SELF_TEST",
	"HELLO_RUNTIME_DIAGNOSTIC_MAX_OBJECTS",
	"HELLO_RUNTIME_DIAGNOSTIC_MAX_RESOURCES",
	"HELLO_RUNTIME_DIAGNOSTIC_MAX_EVENTS",
	"HELLO_RUNTIME_DIAGNOSTIC_FOCUS_AGENT_ID",
	"HELLO_SAMPLE_PRESET",
	"HELLO_SAMPLE_SEED",
	"HELLO_SAMPLE_AGENT_COUNT",
	"HELLO_SAMPLE_LIGHT_COUNT",
	"HELLO_SAMPLE_SPAWN_MODE",
	"HELLO_SAMPLE_AI_SCHEDULER",
	"HELLO_SAMPLE_AI_TICK_MS",
	"HELLO_SAMPLE_AI_MAX_PER_FRAME",
	"HELLO_AI_SCHEDULER_ENABLE",
	"HELLO_AI_SCHEDULER_TICK_MS",
	"HELLO_AI_SCHEDULER_MAX_PER_FRAME",
	"HELLO_VISUAL_TRACE_GATE",
	"HELLO_VISUAL_TRACE_DELAY_FRAMES",
	"HELLO_VISUAL_TRACE_INTERVAL_FRAMES",
	"HELLO_VISUAL_TRACE_MAX_SAMPLES",
	"HELLO_VISUAL_TRACE_MAX_AGENTS",
	"HELLO_PARITY_TRACE",
	"HELLO_PARITY_TRACE_DELAY_MS",
	"HELLO_PARITY_TRACE_INTERVAL_MS",
	"HELLO_PARITY_TRACE_MAX_SAMPLES",
	"HELLO_PARITY_TRACE_MAX_AGENTS",
	"HELLO_PARITY_TRACE_AI_SUMMARY",
	"HELLO_PARITY_TRACE_AI_SUMMARY_MAX_LINES",
	"HELLO_PARITY_TRACE_AI_SUMMARY_MAX_LINE_LENGTH",
	"HELLO_AI_SPATIAL_INDEX_ENABLE",
	"HELLO_AI_PERCEPTION_SYSTEM_ENABLE",
	"HELLO_AI_PERCEPTION_CACHE_ENABLE",
	"HELLO_AI_SPATIAL_INDEX_CELL_SIZE",
	"HELLO_PERF_STALL_LOG",
	"HELLO_PERF_STALL_THRESHOLD_MS",
	"HELLO_PERF_SUMMARY_INTERVAL_MS",
	"HELLO_BT_TICK_INTERVAL_MS",
	"HELLO_BT_TICK_STAGGER",
	"HELLO_BT_DISTANCE_LOD_NEAR",
	"HELLO_BT_DISTANCE_LOD_FAR",
	"HELLO_BT_DISTANCE_LOD_MAX_MULTIPLIER",
	"HELLO_BT_MAX_TREE_TICKS_PER_FRAME"
)

$RunId = "{0:yyyyMMddHHmmssfff}-{1}" -f (Get-Date), $PID
if ($RuntimeDiagEnabled -and -not $PSBoundParameters.ContainsKey("Seconds") -and $Seconds -lt 55) {
	$Seconds = 55
}
$SelectedEnv = [ordered]@{
	"HELLO_SANDBOX_SMOKE_TEST" = "1"
	"HELLO_SANDBOX_SAMPLE" = $SelectedSample
	"HELLO_SANDBOX_SMOKE_RUN_ID" = $RunId
}
if ($RuntimeDiagEnabled) {
	$SelectedEnv["HELLO_RUNTIME_DIAGNOSTIC_SELF_TEST"] = "1"
	$SelectedEnv["HELLO_RUNTIME_DIAGNOSTIC_MAX_OBJECTS"] = [string]$DiagMaxObjects
	$SelectedEnv["HELLO_RUNTIME_DIAGNOSTIC_MAX_RESOURCES"] = [string]$DiagMaxResources
	$SelectedEnv["HELLO_RUNTIME_DIAGNOSTIC_MAX_EVENTS"] = [string]$DiagMaxEvents
	if ($DiagFocusAgentId -gt 0) {
		$SelectedEnv["HELLO_RUNTIME_DIAGNOSTIC_FOCUS_AGENT_ID"] = [string]$DiagFocusAgentId
	}
}
if ($BlackboardSelfTest) {
	$SelectedEnv["HELLO_AI_BLACKBOARD_SELF_TEST"] = "1"
}
if ($AiEventSelfTest) {
	$SelectedEnv["HELLO_AI_EVENT_SELF_TEST"] = "1"
}
if ($Preset -ne "") {
	$SelectedEnv["HELLO_SAMPLE_PRESET"] = $Preset
}
if ($Seed -gt 0) {
	$SelectedEnv["HELLO_SAMPLE_SEED"] = [string]$Seed
}
if ($AgentCount -gt 0) {
	$SelectedEnv["HELLO_SAMPLE_AGENT_COUNT"] = [string]$AgentCount
}
if ($LightTeamCount -gt 0) {
	$SelectedEnv["HELLO_SAMPLE_LIGHT_COUNT"] = [string]$LightTeamCount
}
if ($SpawnMode -ne "") {
	$SelectedEnv["HELLO_SAMPLE_SPAWN_MODE"] = $SpawnMode
}
if ($AiScheduler) {
	$SelectedEnv["HELLO_AI_SCHEDULER_ENABLE"] = "1"
	$SelectedEnv["HELLO_AI_SCHEDULER_TICK_MS"] = [string]$AiTickMs
	$SelectedEnv["HELLO_AI_SCHEDULER_MAX_PER_FRAME"] = [string]$AiMaxPerFrame
}
if ($VisualTrace) {
	$SelectedEnv["HELLO_VISUAL_TRACE_GATE"] = "1"
	$SelectedEnv["HELLO_VISUAL_TRACE_DELAY_FRAMES"] = [string]$VisualTraceDelayFrames
	$SelectedEnv["HELLO_VISUAL_TRACE_INTERVAL_FRAMES"] = [string]$VisualTraceIntervalFrames
	$SelectedEnv["HELLO_VISUAL_TRACE_MAX_SAMPLES"] = [string]$VisualTraceMaxSamples
	$SelectedEnv["HELLO_VISUAL_TRACE_MAX_AGENTS"] = [string]$VisualTraceMaxAgents
}
if ($ParityTrace) {
	$SelectedEnv["HELLO_PARITY_TRACE"] = "1"
	$SelectedEnv["HELLO_PARITY_TRACE_DELAY_MS"] = [string]$ParityTraceDelayMs
	$SelectedEnv["HELLO_PARITY_TRACE_INTERVAL_MS"] = [string]$ParityTraceIntervalMs
	$SelectedEnv["HELLO_PARITY_TRACE_MAX_SAMPLES"] = [string]$ParityTraceMaxSamples
	$SelectedEnv["HELLO_PARITY_TRACE_MAX_AGENTS"] = [string]$ParityTraceMaxAgents
}
if ($DisableSpatialIndex) {
	$SelectedEnv["HELLO_AI_SPATIAL_INDEX_ENABLE"] = "0"
}
if ($DisablePerceptionSystem) {
	$SelectedEnv["HELLO_AI_PERCEPTION_SYSTEM_ENABLE"] = "0"
}
if ($DisablePerceptionCache) {
	$SelectedEnv["HELLO_AI_PERCEPTION_CACHE_ENABLE"] = "0"
}
if ($SpatialCellSize -gt 0) {
	$SelectedEnv["HELLO_AI_SPATIAL_INDEX_CELL_SIZE"] = [string]$SpatialCellSize
}
if ($PerfStallLog -or ($AiPerfPresetNames -contains $Preset)) {
	$SelectedEnv["HELLO_PERF_STALL_LOG"] = "1"
	$SelectedEnv["HELLO_PERF_STALL_THRESHOLD_MS"] = [string]$PerfStallThresholdMs
	$SelectedEnv["HELLO_PERF_SUMMARY_INTERVAL_MS"] = [string]$PerfSummaryIntervalMs
}
if ($BtTickIntervalMs -gt 0) {
	$SelectedEnv["HELLO_BT_TICK_INTERVAL_MS"] = [string]$BtTickIntervalMs
}
if ($BtTickStagger) {
	$SelectedEnv["HELLO_BT_TICK_STAGGER"] = "1"
}
if ($Preset -eq "ai_perf_smoke" -and $Seconds -lt 120) {
	$Seconds = 120
}
if ($Preset -eq "ai_perf_1000" -and $Seconds -lt 120) {
	$Seconds = 120
}
elseif ($Preset -eq "ai_perf_500" -and $Seconds -lt 90) {
	$Seconds = 90
}
elseif (($Preset -like "ai_perf_*") -and $Seconds -lt 35) {
	$Seconds = 35
}
if (($Preset -eq "ai_lastknown_demo" -or $Preset -eq "chapter8_perception") -and $Seconds -lt 70) {
	$Seconds = 70
}
if ($Preset -eq "bt_runtime_lod" -and $Seconds -lt 70) {
	$Seconds = 70
}
if ($Preset -eq "bt_hot_reload" -and $Seconds -lt 70) {
	$Seconds = 70
}
if ($Preset -eq "chapter8_comms" -and $Seconds -lt 70) {
	$Seconds = 70
}
if ($Preset -eq "team_blackboard" -and $Seconds -lt 70) {
	$Seconds = 70
}
if ($Preset -eq "influence_map" -and $Seconds -lt 70) {
	$Seconds = 70
}
if ($Preset -eq "hearing_danger" -and $Seconds -lt 70) {
	$Seconds = 70
}
if ($Preset -eq "formation_tactics" -and $Seconds -lt 70) {
	$Seconds = 70
}
if ($Preset -eq "chapter9_tactics_lua" -and $Seconds -lt 35) {
	$Seconds = 35
}
if ($Preset -eq "chapter9_tactics_legacy_parity" -and $Seconds -lt 35) {
	$Seconds = 35
}
if ($Preset -eq "chapter9_tactics_agent_motion_probe" -and $Seconds -lt 12) {
	$Seconds = 12
}
if (($Chapter9TacticsCppPresetNames -contains $Preset) -and $Seconds -lt 35) {
	$Seconds = 35
}

Write-Host "[SMOKE] sample=$SelectedSample preset=$Preset runId=$RunId runtimeDiag=$RuntimeDiagEnabled blackboardSelfTest=$($BlackboardSelfTest.IsPresent) aiEventSelfTest=$($AiEventSelfTest.IsPresent) aiScheduler=$($AiScheduler.IsPresent) visualTrace=$($VisualTrace.IsPresent) parityTrace=$($ParityTrace.IsPresent) disableSpatialIndex=$($DisableSpatialIndex.IsPresent) disablePerceptionSystem=$($DisablePerceptionSystem.IsPresent) disablePerceptionCache=$($DisablePerceptionCache.IsPresent) perfStallLog=$($SelectedEnv.Contains('HELLO_PERF_STALL_LOG')) btTickIntervalMs=$BtTickIntervalMs btTickStagger=$($BtTickStagger.IsPresent) seconds=$Seconds visible=$($Visible.IsPresent) keepAlive=$($KeepAlive.IsPresent)"
Write-Host "[SMOKE] exe=$ExePath"
foreach ($item in $SelectedEnv.GetEnumerator()) {
	Write-Host "[SMOKE] env $($item.Key)=$($item.Value)"
}

if ($DryRun) {
	Write-Host "[SMOKE] dry run only; process not started."
	exit 0
}

if (-not (Test-Path -LiteralPath $ExePath)) {
	throw "HelloOgre3D.exe not found: $ExePath"
}

$StartLogLineCounts = @{}
$StartLogWriteTimes = @{}
foreach ($candidate in $LogCandidates) {
	if (Test-Path -LiteralPath $candidate) {
		$item = Get-Item -LiteralPath $candidate
		$StartLogLineCounts[$candidate] = (Get-Content -LiteralPath $candidate).Count
		$StartLogWriteTimes[$candidate] = $item.LastWriteTime
	} else {
		$StartLogLineCounts[$candidate] = 0
		$StartLogWriteTimes[$candidate] = [DateTime]::MinValue
	}
}

$OldEnv = @{}
foreach ($name in $KnownEnvNames) {
	$OldEnv[$name] = [Environment]::GetEnvironmentVariable($name, "Process")
	Remove-Item -Path "Env:$name" -ErrorAction SilentlyContinue
}
foreach ($item in $SelectedEnv.GetEnumerator()) {
	Set-Item -Path "Env:$($item.Key)" -Value $item.Value
}

try {
	if ($StopExisting) {
		$existingProcesses = @(Get-Process -Name "HelloOgre3D" -ErrorAction SilentlyContinue)
		if ($existingProcesses.Count -gt 0) {
			$existingProcesses | Stop-Process -Force
			$existingProcesses | ForEach-Object {
				try {
					Wait-Process -Id $_.Id -Timeout 5 -ErrorAction SilentlyContinue
				} catch {
				}
			}
		}
		Start-Sleep -Seconds 2
	}

	if ($Visible) {
		$process = Start-Process -FilePath $ExePath -WorkingDirectory $BinDir -PassThru
	} else {
		$process = Start-Process -FilePath $ExePath -WorkingDirectory $BinDir -WindowStyle Hidden -PassThru
	}

	Write-Host "[SMOKE] started pid=$($process.Id)"
	Start-Sleep -Seconds $Seconds

	if (-not $process.HasExited) {
		if ($KeepAlive) {
			Write-Host "[SMOKE] pid=$($process.Id) still running; kept alive by request."
		} else {
			Stop-Process -Id $process.Id -Force
			try {
				Wait-Process -Id $process.Id -Timeout 5 -ErrorAction SilentlyContinue
			} catch {
			}
			Write-Host "[SMOKE] stopped pid=$($process.Id) after $Seconds seconds."
		}
	} else {
		Write-Host "[SMOKE] process exited code=$($process.ExitCode)."
		if ($process.ExitCode -ne 0) {
			throw "HelloOgre3D exited with code $($process.ExitCode)."
		}
	}

	Start-Sleep -Milliseconds 750

	$NewLogLines = @()
	$TouchedLogLines = @()
	$TailLogPath = $LogPath
	$TailLogWriteTime = [DateTime]::MinValue
	foreach ($candidate in $LogCandidates) {
		if (-not (Test-Path -LiteralPath $candidate)) {
			continue
		}
		$allLogLines = Get-Content -LiteralPath $candidate
		$writeTime = (Get-Item -LiteralPath $candidate).LastWriteTime
		if ($writeTime -gt [DateTime]$StartLogWriteTimes[$candidate]) {
			$TouchedLogLines += $allLogLines
		}
		$startLogLineCount = [int]$StartLogLineCounts[$candidate]
		if ($startLogLineCount -gt $allLogLines.Count) {
			$startLogLineCount = 0
		}
		$newCandidateLines = @($allLogLines | Select-Object -Skip $startLogLineCount)
		if ($newCandidateLines.Count -gt 0) {
			$NewLogLines += $newCandidateLines
		}
		if ($newCandidateLines.Count -eq 0 -and $writeTime -gt [DateTime]$StartLogWriteTimes[$candidate]) {
			$newCandidateLines = @($allLogLines)
			$NewLogLines += $newCandidateLines
		}
		if ($newCandidateLines.Count -gt 0 -and $writeTime -gt $TailLogWriteTime) {
			$TailLogPath = $candidate
			$TailLogWriteTime = $writeTime
		}
	}

	if (-not $NoTail -and (Test-Path -LiteralPath $TailLogPath)) {
		Write-Host "[SMOKE] log tail: $TailLogPath"
		Get-Content -LiteralPath $TailLogPath -Tail $Tail
	}

	$CheckedLogLineCount = $NewLogLines.Count
	if (-not $IgnoreLogErrors) {
		if ($NewLogLines.Count -eq 0 -and $TouchedLogLines.Count -eq 0) {
			throw "Sandbox smoke test produced no new log output."
		}

		$runPattern = "\[SandboxSmoke\] run id:\s+$([regex]::Escape($RunId))"
		$LogLinesForChecks = $NewLogLines
		$runMatches = @()
		if ($TouchedLogLines.Count -gt 0) {
			$runMatches = @($TouchedLogLines | Select-String -Pattern $runPattern)
			if ($runMatches.Count -gt 0) {
				$lastRunLineNumber = $runMatches[$runMatches.Count - 1].LineNumber
				$LogLinesForChecks = @($TouchedLogLines | Select-Object -Skip ($lastRunLineNumber - 1))
			}
		}
		if ($runMatches.Count -eq 0) {
			$runMatches = @($NewLogLines | Select-String -Pattern $runPattern)
		}
		if ($runMatches.Count -eq 0) {
			throw "Sandbox smoke log did not confirm run id: $RunId"
		}

		$samplePattern = "\[SandboxSmoke\] sample selected:\s+$SelectedSample"
		$sampleMatches = @($LogLinesForChecks | Select-String -Pattern $samplePattern)
		if ($sampleMatches.Count -eq 0) {
			throw "Sandbox smoke log did not confirm sample selection: $SelectedSample"
		}

		if ($RuntimeDiagEnabled) {
			$diagMatches = @($LogLinesForChecks | Select-String -Pattern "\[RuntimeDiag\] self test result:\s+true")
			if ($diagMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm runtime diagnostic selftest success."
			}
			if ($DiagFocusAgentId -gt 0) {
				$focusMatches = @($LogLinesForChecks | Select-String -Pattern "focusAgentId=$DiagFocusAgentId.*focusFound=true")
				if ($focusMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm focused AI runtime diagnostics for agent $DiagFocusAgentId."
				}
			}
			if ($AiEventSelfTest) {
				$eventSelfTestMatches = @($LogLinesForChecks | Select-String -Pattern "\[AIEventSelfTest\]\s+result=true")
				if ($eventSelfTestMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm AI event scope selftest success."
				}
			}
			if ($Preset -ne "") {
				$presetMatches = @($LogLinesForChecks | Select-String -Pattern "\[ConfigManager\].*preset=.*$([regex]::Escape($Preset))")
				if ($presetMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm sample preset: $Preset"
				}
			}
			$expectedBtTickIntervalMs = 0
			if ($BtTickIntervalMs -gt 0) {
				$expectedBtTickIntervalMs = $BtTickIntervalMs
			} elseif ($Preset -eq "bt_runtime_lod" -or $Preset -eq "bt_runtime_budget" -or $Preset -eq "bt_runtime_rebuild" -or $Preset -eq "bt_hot_reload") {
				$expectedBtTickIntervalMs = 250
			}
			if ($expectedBtTickIntervalMs -gt 0) {
				$btIntervalPattern = "\[BTStats\].*tickIntervalMs=$expectedBtTickIntervalMs(\.0+)?"
				$btIntervalMatches = @($LogLinesForChecks | Select-String -Pattern $btIntervalPattern)
				if ($btIntervalMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm BehaviorTree runtime tick interval $expectedBtTickIntervalMs ms."
				}
				$btSkippedMatches = @($LogLinesForChecks | Select-String -Pattern "\[BTStats\].*tickSkipped=[1-9][0-9]*")
				if ($btSkippedMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm BehaviorTree runtime tick skipping."
				}
				if ($Preset -eq "bt_runtime_lod") {
					$btCacheMatches = @($LogLinesForChecks | Select-String -Pattern "\[BTStats\].*cacheHits=[1-9][0-9]*")
					if ($btCacheMatches.Count -eq 0) {
						throw "Sandbox smoke log did not confirm BehaviorTree condition result cache hits."
					}
					$btDistanceLodMatches = @($LogLinesForChecks | Select-String -Pattern "\[BTStats\].*distanceLodMultiplier=[2-9]([\. ][0-9]*)?.*distanceLodSkipped=[1-9][0-9]*")
					if ($btDistanceLodMatches.Count -eq 0) {
						throw "Sandbox smoke log did not confirm BehaviorTree distance LOD multiplier and skipping."
					}
				}
				if ($Preset -eq "bt_runtime_budget") {
					$btBudgetMatches = @($LogLinesForChecks | Select-String -Pattern "\[BTStats\].*budgetSkipped=[1-9][0-9]*.*budgetMax=1")
					if ($btBudgetMatches.Count -eq 0) {
						throw "Sandbox smoke log did not confirm BehaviorTree frame budget skipping."
					}
				}
				if ($Preset -eq "bt_runtime_rebuild") {
					$btRebuildMatches = @($LogLinesForChecks | Select-String -Pattern "\[BTStats\].*storageResets=[1-9][0-9]*.*nodeReuses=[1-9][0-9]*.*treeReuses=[1-9][0-9]*")
					if ($btRebuildMatches.Count -eq 0) {
						throw "Sandbox smoke log did not confirm BehaviorTree rebuild storage reuse stats."
					}
				}
				if ($Preset -eq "bt_hot_reload") {
					$btHotReloadMatches = @($LogLinesForChecks | Select-String -Pattern "\[BTHotReloadSelfTest\]\s+PASS")
					if ($btHotReloadMatches.Count -eq 0) {
						throw "Sandbox smoke log did not confirm BehaviorTree hot reload self test."
					}
					$btHotReloadStatsMatches = @($LogLinesForChecks | Select-String -Pattern "\[BTStats\].*treeBuilds=[2-9][0-9]*.*storageResets=[1-9][0-9]*")
					if ($btHotReloadStatsMatches.Count -eq 0) {
						throw "Sandbox smoke log did not confirm BehaviorTree hot reload build/storage stats."
					}
				}
			}
		}

		if ($AiScheduler) {
			$schedulerMatches = @($LogLinesForChecks | Select-String -Pattern "\[AIScheduler\].*enabled=.*true")
			if ($schedulerMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm AI scheduler diagnostics."
			}
			if ($AiPerfPresetNames -contains $Preset) {
				$schedulerStatsMatches = @($LogLinesForChecks | Select-String -Pattern "\[AIScheduler\].*enabled=true.*totalTicked=[1-9][0-9]*.*totalSkipped=[1-9][0-9]*")
				if ($schedulerStatsMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm AI scheduler tick/skip stats for ai_perf preset."
				}
			}
		}

		if ($AiPerfPresetNames -contains $Preset) {
			$luaCallbackMatches = @($LogLinesForChecks | Select-String -Pattern "\[FramePerf\] game .*luaCallbacks=[1-9][0-9]*.*luaCallbackMs=")
			if ($luaCallbackMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm FramePerf Lua callback count for ai_perf preset."
			}
		}

		if ($Preset -eq "chapter8_perception" -or $Preset -eq "ai_lastknown_demo") {
			$schedulerMatches = @($LogLinesForChecks | Select-String -Pattern "\[AIScheduler\].*enabled=.*true")
			if ($schedulerMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm AI scheduler is enabled for perception preset."
			}
		}

		if ($Preset -eq "team_blackboard") {
			$schedulerMatches = @($LogLinesForChecks | Select-String -Pattern "\[AIScheduler\].*enabled=.*true")
			if ($schedulerMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm AI scheduler is enabled for TeamBlackboard preset."
			}
			$teamBlackboardMatches = @($LogLinesForChecks | Select-String -Pattern "\[TeamBlackboardSmoke\]\s+PASS")
			if ($teamBlackboardMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm TeamBlackboard shared target response."
			}
			$teamBlackboardLifecycleMatches = @($LogLinesForChecks | Select-String -Pattern "\[TeamBlackboardLifecycleSelfTest\]\s+PASS")
			if ($teamBlackboardLifecycleMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm TeamBlackboard lifecycle reset self test."
			}
			$teamBlackboardScheduleMatches = @($LogLinesForChecks | Select-String -Pattern "\[TeamBlackboardSmoke\].*scanRuns=.*scanSkips=.*applyRuns=.*applySkips=")
			if ($teamBlackboardScheduleMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm TeamBlackboard scheduled scan/apply stats."
			}
			$teamBlackboardTypedMatches = @($LogLinesForChecks | Select-String -Pattern "\[TeamBlackboardSmoke\].*cppTypedFacts=\s*[1-9][0-9]*")
			if ($teamBlackboardTypedMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm TeamBlackboard typed C++ facts."
			}
			$teamBlackboardFocusMatches = @($LogLinesForChecks | Select-String -Pattern "\[TeamBlackboardSmoke\].*cppFocusApplies=\s*[1-9][0-9]*")
			if ($teamBlackboardFocusMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm TeamBlackboard C++ FocusTarget apply."
			}
			$teamBlackboardGetterMatches = @($LogLinesForChecks | Select-String -Pattern "\[TeamBlackboardSmoke\].*cppGetter=\s*true")
			if ($teamBlackboardGetterMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm TeamBlackboard C++ typed fact getter."
			}
		}

		if ($Preset -eq "influence_map") {
			$schedulerMatches = @($LogLinesForChecks | Select-String -Pattern "\[AIScheduler\].*enabled=.*true")
			if ($schedulerMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm AI scheduler is enabled for InfluenceMap preset."
			}
			$influenceMatches = @($LogLinesForChecks | Select-String -Pattern "\[InfluenceMapSmoke\]\s+PASS")
			if ($influenceMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm InfluenceMap tactical movement."
			}
			$influenceScheduleMatches = @($LogLinesForChecks | Select-String -Pattern "\[InfluenceMapSmoke\].*updates=.*skips=.*intervalMs=")
			if ($influenceScheduleMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm InfluenceMap scheduled update stats."
			}
			$teamBlackboardScheduleMatches = @($LogLinesForChecks | Select-String -Pattern "\[TeamBlackboardSmoke\].*scanRuns=.*scanSkips=.*applyRuns=.*applySkips=")
			if ($teamBlackboardScheduleMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm TeamBlackboard scheduled scan/apply stats for InfluenceMap preset."
			}
			$teamBlackboardTypedMatches = @($LogLinesForChecks | Select-String -Pattern "\[TeamBlackboardSmoke\].*cppTypedFacts=\s*[1-9][0-9]*")
			if ($teamBlackboardTypedMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm TeamBlackboard typed C++ facts for InfluenceMap preset."
			}
			$teamBlackboardFocusMatches = @($LogLinesForChecks | Select-String -Pattern "\[TeamBlackboardSmoke\].*cppFocusApplies=\s*[1-9][0-9]*")
			if ($teamBlackboardFocusMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm TeamBlackboard C++ FocusTarget apply for InfluenceMap preset."
			}
		}

		if ($Preset -eq "hearing_danger") {
			$schedulerMatches = @($LogLinesForChecks | Select-String -Pattern "\[AIScheduler\].*enabled=.*true")
			if ($schedulerMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm AI scheduler is enabled for Hearing/Danger preset."
			}
			$hearingMatches = @($LogLinesForChecks | Select-String -Pattern "\[HearingDangerSmoke\]\s+PASS")
			if ($hearingMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Hearing/Danger sensory response."
			}
			$hearingScheduleMatches = @($LogLinesForChecks | Select-String -Pattern "\[HearingDangerSmoke\].*runs=.*skips=.*agentChecks=")
			if ($hearingScheduleMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Hearing/Danger scheduled scan stats."
			}
			$hearingRetreatMatches = @($LogLinesForChecks | Select-String -Pattern "\[HearingDangerSmoke\].*cppRetreatApplies=\s*[1-9][0-9]*")
			if ($hearingRetreatMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Hearing/Danger C++ RetreatPoint apply."
			}
			$hearingCppSenseMatches = @($LogLinesForChecks | Select-String -Pattern "\[HearingDangerSmoke\].*cppSense=\s*true")
			if ($hearingCppSenseMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Hearing/Danger C++ perception sense."
			}
		}

		if ($Preset -eq "formation_tactics") {
			$schedulerMatches = @($LogLinesForChecks | Select-String -Pattern "\[AIScheduler\].*enabled=.*true")
			if ($schedulerMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm AI scheduler is enabled for Formation preset."
			}
			$formationMatches = @($LogLinesForChecks | Select-String -Pattern "\[FormationSmoke\]\s+PASS")
			if ($formationMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Formation tactical behavior."
			}
			$formationScheduleMatches = @($LogLinesForChecks | Select-String -Pattern "\[FormationSmoke\].*slots=.*ready=.*backupCalls=.*runs=.*skips=")
			if ($formationScheduleMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Formation scheduled update stats."
			}
			$formationTypedMatches = @($LogLinesForChecks | Select-String -Pattern "\[FormationSmoke\].*cppTypedFacts=\s*[1-9][0-9]*")
			if ($formationTypedMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Formation typed C++ facts."
			}
			$formationFocusMatches = @($LogLinesForChecks | Select-String -Pattern "\[FormationSmoke\].*cppFocusApplies=\s*[1-9][0-9]*")
			if ($formationFocusMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Formation C++ FocusTarget apply."
			}
		}

		if ($Preset -eq "chapter9_tactics_lua") {
			$tacticsMatches = @($LogLinesForChecks | Select-String -Pattern "\[Chapter9TacticsSmoke\]\s+PASS")
			if ($tacticsMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Chapter 9 tactics behavior."
			}
			$tacticsScheduleMatches = @($LogLinesForChecks | Select-String -Pattern "\[Chapter9TacticsSmoke\].*dangerCells=.*teamCells=.*dangerUpdates=.*teamUpdates=")
			if ($tacticsScheduleMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Chapter 9 tactics layer update stats."
			}
		}

		if ($Preset -eq "chapter9_tactics_legacy_parity") {
			$legacyBridgeMatches = @($LogLinesForChecks | Select-String -Pattern "\[Chapter9LegacyEventBridgeSelfTest\]\s+PASS")
			if ($legacyBridgeMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Chapter 9 legacy event bridge lifecycle guard."
			}
		}

		if ($Chapter9TacticsCppPresetNames -contains $Preset) {
			$tacticsMatches = @($LogLinesForChecks | Select-String -Pattern "\[Chapter9TacticsCppSmoke\]\s+PASS")
			if ($tacticsMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Chapter 9 C++ tactics behavior."
			}
			$tacticsScheduleMatches = @($LogLinesForChecks | Select-String -Pattern "\[Chapter9TacticsCppSmoke\].*dangerCells=.*teamCells=.*writes=.*queries=")
			if ($tacticsScheduleMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm Chapter 9 C++ tactics influence stats."
			}
			$dirtyRegionMatches = @($LogLinesForChecks | Select-String -Pattern "\[TacticalDirtyRegionSmoke\]\s+PASS")
			if ($dirtyRegionMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm tactical dirty region stats."
			}
			$dirtySummaryMatches = @($LogLinesForChecks | Select-String -Pattern "dirtyRegions=[1-9][0-9]*.*dirtyCells=[1-9][0-9]*.*spreadRegionCells=[1-9][0-9]*.*spreadLimited=true")
			if ($dirtySummaryMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm bounded tactical dirty region spread summary."
			}
			$schemaMatches = @($LogLinesForChecks | Select-String -Pattern "\[TacticalLayerSchemaSmoke\]\s+PASS")
			if ($schemaMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm tactical support/cover/crowd schema."
			}
			$schemaSummaryMatches = @($LogLinesForChecks | Select-String -Pattern "schema=support/cover/crowd.*supportCells=[1-9][0-9]*.*coverCells=[1-9][0-9]*.*crowdCells=[1-9][0-9]*")
			if ($schemaSummaryMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm active tactical support/cover/crowd layers."
			}
			$debugConfigMatches = @($LogLinesForChecks | Select-String -Pattern "\[TacticalDebugConfigSmoke\]\s+PASS")
			if ($debugConfigMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm tactical layer debug config."
			}
			$debugSummaryMatches = @($LogLinesForChecks | Select-String -Pattern "\[TacticalDebugDraw\].*configs=[1-9][0-9]*.*order=.*projectToNav=")
			if ($debugSummaryMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm explicit tactical debug draw layer config summary."
			}
			$debugValueMatches = @($LogLinesForChecks | Select-String -Pattern "\[TacticalDebugDraw\].*danger\(configured=true,order=12,y=0\.18,threshold=0\.1,maxCells=96,neutral=false,projectToNav=true,nav=default,maxProjectionDistance=[0-9.]+,drawn=[0-9]+,projected=[0-9]+,projectionRejected=[0-9]+\)")
			if ($debugValueMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm explicit tactical danger debug config values."
			}
			if ($Preset -eq "chapter9_tactics_cpp_pressure") {
				$candidateLimitMatches = @($LogLinesForChecks | Select-String -Pattern "\[TacticalCandidateLimitSmoke\]\s+PASS")
				if ($candidateLimitMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm tactical query candidate limit."
				}
				$candidateSummaryMatches = @($LogLinesForChecks | Select-String -Pattern "candidateLimit=64.*capped=true")
				if ($candidateSummaryMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm capped tactical query summary."
				}
			}
		}

		if ($Preset -eq "ai_perception_pressure" -or $SelectedSample -eq "Sandbox16") {
			$spatialMatches = @($LogLinesForChecks | Select-String -Pattern "\[AgentSpatialIndex\].*queries=")
			if ($spatialMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm AgentSpatialIndex diagnostics."
			}
			if (-not $DisablePerceptionSystem) {
				$perceptionMatches = @($LogLinesForChecks | Select-String -Pattern "\[AgentPerceptionSystem\].*scans=")
				if ($perceptionMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm AgentPerceptionSystem diagnostics."
				}
			}
			if ($DisablePerceptionCache) {
				$cacheDisabledMatches = @($LogLinesForChecks | Select-String -Pattern "cache enabled=0")
				if ($cacheDisabledMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm disabled perception cache diagnostics."
				}
			}
		}

		$CheckedLogLineCount = $LogLinesForChecks.Count
		$FailurePattern = "OGRE EXCEPTION|PANIC:|lua_dofile path|call_func error|call_string error|lua_pcall|stack traceback|self test result:\s*false|self test case:.*FAIL|\[VisualTraceGate\] result:\s*false"
		$Failures = @($LogLinesForChecks | Select-String -Pattern $FailurePattern)
		if ($Failures.Count -gt 0) {
			Write-Host "[SMOKE] detected log failures:"
			$Failures | Select-Object -First 12 | ForEach-Object { Write-Host $_.Line }
			throw "Sandbox smoke log contains failure patterns."
		}
	}

	Write-Host "[SMOKE] status=PASS lines=$CheckedLogLineCount"
} finally {
	foreach ($name in $KnownEnvNames) {
		if ($null -eq $OldEnv[$name]) {
			Remove-Item -Path "Env:$name" -ErrorAction SilentlyContinue
		} else {
			Set-Item -Path "Env:$name" -Value $OldEnv[$name]
		}
	}
}
