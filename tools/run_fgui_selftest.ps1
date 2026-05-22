param(
	[ValidateSet(
		"DebugPanel",
		"LongLoop",
		"All",
		"Input",
		"Key",
		"TextInput",
		"Lifecycle",
		"Cache",
		"CommonService",
		"CommonServiceDemo",
		"PopupRule",
		"GuideMask",
		"EventPayload",
		"ScreenAdapt",
		"ScreenAdaptDemo",
		"BusinessFlow",
		"Layer",
		"LayerClose",
		"Act38",
		"Wheel",
		"Mask",
		"Cleanup",
		"None"
	)]
	[string]$Mode = "DebugPanel",
	[ValidateRange(1, 1000)]
	[int]$Count = 3,
	[int]$Seconds = 0,
	[int]$Tail = 220,
	[switch]$Visible,
	[switch]$KeepAlive,
	[switch]$StopExisting,
	[switch]$IgnoreLogErrors,
	[switch]$NoTail,
	[switch]$DryRun
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptRoot "..")
$BinDir = Join-Path $RepoRoot "bin"
$ExePath = Join-Path $BinDir "HelloOgre3D.exe"
$LogPath = Join-Path $BinDir "Sandbox.log"

function Get-DefaultWaitSeconds {
	param(
		[string]$SelfTestMode,
		[int]$LoopCount
	)

	switch ($SelfTestMode) {
		"All" { return [Math]::Max(42, 28 + $LoopCount * 4) }
		"LongLoop" { return [Math]::Max(28, 18 + $LoopCount * 4) }
		"DebugPanel" { return 24 }
		"CommonServiceDemo" { return 28 }
		"ScreenAdaptDemo" { return 24 }
		"BusinessFlow" { return 32 }
		"EventPayload" { return 32 }
		"Input" { return 18 }
		"Mask" { return 18 }
		default { return 28 }
	}
}

function Get-FairyGuiEnv {
	param(
		[string]$SelfTestMode,
		[int]$LoopCount
	)

	$values = [ordered]@{}
	switch ($SelfTestMode) {
		"DebugPanel" { $values["HELLO_FGUI_DEBUG_PANEL_DEMO"] = "1" }
		"LongLoop" {
			$values["HELLO_FGUI_LONG_LOOP_SELF_TEST"] = "1"
			$values["HELLO_FGUI_LONG_LOOP_COUNT"] = [string]$LoopCount
		}
		"All" {
			$values["HELLO_FGUI_SELF_TEST_ALL"] = "1"
			$values["HELLO_FGUI_LONG_LOOP_COUNT"] = [string]$LoopCount
		}
		"Input" {
			$values["HELLO_FGUI_INPUT_SELF_TEST"] = "1"
			$values["HELLO_FGUI_INPUT_DEBUG"] = "1"
		}
		"Key" { $values["HELLO_FGUI_KEY_SELF_TEST"] = "1" }
		"TextInput" { $values["HELLO_FGUI_TEXT_INPUT_SELF_TEST"] = "1" }
		"Lifecycle" { $values["HELLO_FGUI_LIFECYCLE_SELF_TEST"] = "1" }
		"Cache" { $values["HELLO_FGUI_CACHE_SELF_TEST"] = "1" }
		"CommonService" { $values["HELLO_FGUI_COMMON_SERVICE_SELF_TEST"] = "1" }
		"CommonServiceDemo" { $values["HELLO_FGUI_COMMON_SERVICE_DEMO"] = "1" }
		"PopupRule" { $values["HELLO_FGUI_POPUP_RULE_SELF_TEST"] = "1" }
		"GuideMask" { $values["HELLO_FGUI_GUIDE_MASK_SELF_TEST"] = "1" }
		"EventPayload" { $values["HELLO_FGUI_EVENT_PAYLOAD_SELF_TEST"] = "1" }
		"ScreenAdapt" { $values["HELLO_FGUI_SCREEN_ADAPT_SELF_TEST"] = "1" }
		"ScreenAdaptDemo" { $values["HELLO_FGUI_SCREEN_ADAPT_DEMO"] = "1" }
		"BusinessFlow" { $values["HELLO_FGUI_BUSINESS_FLOW_SELF_TEST"] = "1" }
		"Layer" { $values["HELLO_FGUI_LAYER_SELF_TEST"] = "1" }
		"LayerClose" { $values["HELLO_FGUI_LAYER_CLOSE_SELF_TEST"] = "1" }
		"Act38" { $values["HELLO_FGUI_ACT38_SELF_TEST"] = "1" }
		"Wheel" { $values["HELLO_FGUI_WHEEL_SELF_TEST"] = "1" }
		"Mask" { $values["HELLO_FGUI_MASK_SELF_TEST"] = "1" }
		"Cleanup" { $values["HELLO_FGUI_CLEANUP_SELF_TEST"] = "1" }
		"None" {}
	}
	return $values
}

$KnownEnvNames = @(
	"HELLO_FGUI_DEBUG_PANEL_DEMO",
	"HELLO_FGUI_LONG_LOOP_SELF_TEST",
	"HELLO_FGUI_LONG_LOOP_COUNT",
	"HELLO_FGUI_SELF_TEST_ALL",
	"HELLO_FGUI_INPUT_SELF_TEST",
	"HELLO_FGUI_INPUT_DEBUG",
	"HELLO_FGUI_KEY_SELF_TEST",
	"HELLO_FGUI_TEXT_INPUT_SELF_TEST",
	"HELLO_FGUI_LIFECYCLE_SELF_TEST",
	"HELLO_FGUI_CACHE_SELF_TEST",
	"HELLO_FGUI_COMMON_SERVICE_SELF_TEST",
	"HELLO_FGUI_COMMON_SERVICE_DEMO",
	"HELLO_FGUI_POPUP_RULE_SELF_TEST",
	"HELLO_FGUI_GUIDE_MASK_SELF_TEST",
	"HELLO_FGUI_EVENT_PAYLOAD_SELF_TEST",
	"HELLO_FGUI_SCREEN_ADAPT_SELF_TEST",
	"HELLO_FGUI_SCREEN_ADAPT_DEMO",
	"HELLO_FGUI_BUSINESS_FLOW_SELF_TEST",
	"HELLO_FGUI_LAYER_SELF_TEST",
	"HELLO_FGUI_LAYER_CLOSE_SELF_TEST",
	"HELLO_FGUI_ACT38_SELF_TEST",
	"HELLO_FGUI_WHEEL_SELF_TEST",
	"HELLO_FGUI_MASK_SELF_TEST",
	"HELLO_FGUI_CLEANUP_SELF_TEST"
)

if ($Seconds -le 0) {
	$Seconds = Get-DefaultWaitSeconds -SelfTestMode $Mode -LoopCount $Count
}

$SelectedEnv = Get-FairyGuiEnv -SelfTestMode $Mode -LoopCount $Count

Write-Host "[FGUI] mode=$Mode count=$Count seconds=$Seconds visible=$($Visible.IsPresent) keepAlive=$($KeepAlive.IsPresent)"
Write-Host "[FGUI] exe=$ExePath"
if ($SelectedEnv.Count -gt 0) {
	foreach ($item in $SelectedEnv.GetEnumerator()) {
		Write-Host "[FGUI] env $($item.Key)=$($item.Value)"
	}
} else {
	Write-Host "[FGUI] env <none>"
}

if ($DryRun) {
	Write-Host "[FGUI] dry run only; process not started."
	exit 0
}

if (-not (Test-Path -LiteralPath $ExePath)) {
	throw "HelloOgre3D.exe not found: $ExePath"
}

$StartLogLineCount = 0
if (Test-Path -LiteralPath $LogPath) {
	$StartLogLineCount = (Get-Content -LiteralPath $LogPath).Count
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
		Get-Process -Name "HelloOgre3D" -ErrorAction SilentlyContinue | Stop-Process -Force
		Start-Sleep -Seconds 1
	}

	if ($Visible) {
		$process = Start-Process -FilePath $ExePath -WorkingDirectory $BinDir -PassThru
	} else {
		$process = Start-Process -FilePath $ExePath -WorkingDirectory $BinDir -WindowStyle Hidden -PassThru
	}

	Write-Host "[FGUI] started pid=$($process.Id)"
	Start-Sleep -Seconds $Seconds

	if (-not $process.HasExited) {
		if ($KeepAlive) {
			Write-Host "[FGUI] pid=$($process.Id) still running; kept alive by request."
		} else {
			Stop-Process -Id $process.Id -Force
			Write-Host "[FGUI] stopped pid=$($process.Id) after $Seconds seconds."
		}
	} else {
		Write-Host "[FGUI] process exited code=$($process.ExitCode)."
		if ($process.ExitCode -ne 0) {
			throw "HelloOgre3D exited with code $($process.ExitCode)."
		}
	}

	$NewLogLines = @()
	if (Test-Path -LiteralPath $LogPath) {
		$AllLogLines = Get-Content -LiteralPath $LogPath
		if ($StartLogLineCount -gt $AllLogLines.Count) {
			$StartLogLineCount = 0
		}
		$NewLogLines = $AllLogLines | Select-Object -Skip $StartLogLineCount
	}

	if (-not $NoTail -and (Test-Path -LiteralPath $LogPath)) {
		Write-Host "[FGUI] log tail: $LogPath"
		Get-Content -LiteralPath $LogPath -Tail $Tail
	}

	if (-not $IgnoreLogErrors) {
		$FailurePattern = "OGRE EXCEPTION|PANIC:|call_func error|self test result:\s*false"
		$Failures = @($NewLogLines | Select-String -Pattern $FailurePattern)
		if ($Failures.Count -gt 0) {
			Write-Host "[FGUI] detected log failures:"
			$Failures | Select-Object -First 12 | ForEach-Object { Write-Host $_.Line }
			throw "FGUI selftest log contains failure patterns."
		}
	}
} finally {
	foreach ($name in $KnownEnvNames) {
		if ($null -eq $OldEnv[$name]) {
			Remove-Item -Path "Env:$name" -ErrorAction SilentlyContinue
		} else {
			Set-Item -Path "Env:$name" -Value $OldEnv[$name]
		}
	}
}
