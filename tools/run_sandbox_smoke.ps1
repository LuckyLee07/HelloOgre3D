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
		"Sandbox11"
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
	[int]$DiagMaxObjects = 8,
	[int]$DiagMaxResources = 6,
	[int]$DiagMaxEvents = 6,
	[string]$Preset = "",
	[int]$Seed = 0,
	[int]$AgentCount = 0,
	[int]$LightTeamCount = 0,
	[string]$SpawnMode = "",
	[switch]$AiScheduler,
	[int]$AiTickMs = 50,
	[int]$AiMaxPerFrame = 8,
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
if (($Preset -eq "ai_lastknown_demo" -or $Preset -eq "chapter8_perception") -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox10"
}
if ($Preset -eq "chapter8_comms" -and -not $PSBoundParameters.ContainsKey("Sample")) {
	$SelectedSample = "Sandbox11"
}
$RuntimeDiagEnabled = $RuntimeDiag.IsPresent -or $BlackboardSelfTest.IsPresent

$KnownEnvNames = @(
	"HELLO_SANDBOX_SMOKE_TEST",
	"HELLO_SANDBOX_SAMPLE",
	"HELLO_SANDBOX_SMOKE_RUN_ID",
	"HELLO_RUNTIME_DIAGNOSTIC_SELF_TEST",
	"HELLO_AI_BLACKBOARD_SELF_TEST",
	"HELLO_RUNTIME_DIAGNOSTIC_MAX_OBJECTS",
	"HELLO_RUNTIME_DIAGNOSTIC_MAX_RESOURCES",
	"HELLO_RUNTIME_DIAGNOSTIC_MAX_EVENTS",
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
	"HELLO_AI_SCHEDULER_MAX_PER_FRAME"
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
}
if ($BlackboardSelfTest) {
	$SelectedEnv["HELLO_AI_BLACKBOARD_SELF_TEST"] = "1"
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
if ($Preset -eq "ai_perf_smoke" -and $Seconds -lt 120) {
	$Seconds = 120
}
if (($Preset -eq "ai_lastknown_demo" -or $Preset -eq "chapter8_perception") -and $Seconds -lt 70) {
	$Seconds = 70
}
if ($Preset -eq "chapter8_comms" -and $Seconds -lt 70) {
	$Seconds = 70
}

Write-Host "[SMOKE] sample=$SelectedSample preset=$Preset runId=$RunId runtimeDiag=$RuntimeDiagEnabled blackboardSelfTest=$($BlackboardSelfTest.IsPresent) aiScheduler=$($AiScheduler.IsPresent) seconds=$Seconds visible=$($Visible.IsPresent) keepAlive=$($KeepAlive.IsPresent)"
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
			if ($Preset -ne "") {
				$presetMatches = @($LogLinesForChecks | Select-String -Pattern "\[ConfigManager\].*preset=.*$([regex]::Escape($Preset))")
				if ($presetMatches.Count -eq 0) {
					throw "Sandbox smoke log did not confirm sample preset: $Preset"
				}
			}
		}

		if ($AiScheduler) {
			$schedulerMatches = @($LogLinesForChecks | Select-String -Pattern "\[AIScheduler\].*enabled=.*true")
			if ($schedulerMatches.Count -eq 0) {
				throw "Sandbox smoke log did not confirm AI scheduler diagnostics."
			}
		}

		$CheckedLogLineCount = $LogLinesForChecks.Count
		$FailurePattern = "OGRE EXCEPTION|PANIC:|lua_dofile path|call_func error|call_string error|lua_pcall|stack traceback|self test result:\s*false|self test case:.*FAIL"
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
