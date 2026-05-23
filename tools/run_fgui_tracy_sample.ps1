param(
	[ValidateRange(1, 1000)]
	[int]$Count = 8,
	[ValidateRange(1, 1000)]
	[int]$ListCount = 50,
	[int]$Seconds = 180,
	[int]$Tail = 260,
	[string]$OutputDir = "docs\perf",
	[switch]$Visible,
	[switch]$KeepAlive,
	[switch]$NoReport
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptRoot "..")
$SelfTestScript = Join-Path $ScriptRoot "run_fgui_selftest.ps1"
$ResolvedOutputDir = Join-Path $RepoRoot $OutputDir

if (-not (Test-Path -LiteralPath $SelfTestScript)) {
	throw "FGUI selftest script not found: $SelfTestScript"
}

if (-not (Test-Path -LiteralPath $ResolvedOutputDir)) {
	New-Item -ItemType Directory -Path $ResolvedOutputDir | Out-Null
}

$env:HELLO_FGUI_PRESSURE_LIST_COUNT = [string]$ListCount
try {
	$selfTestArgs = @{
		Mode = "Pressure"
		Count = $Count
		PressurePopupCount = $Count
		PressureListCount = $ListCount
		Seconds = $Seconds
		Tail = $Tail
		StopExisting = $true
	}
	if ($Visible) {
		$selfTestArgs["Visible"] = $true
	}
	if ($KeepAlive) {
		$selfTestArgs["KeepAlive"] = $true
	}

	Write-Host "[FGUI] Tracy sample pressure count=$Count listCount=$ListCount seconds=$Seconds"
	$output = & $SelfTestScript @selfTestArgs 2>&1
	$output | ForEach-Object { Write-Host $_ }

	if ($NoReport) {
		return
	}

	$timestamp = Get-Date -Format "yyyyMMdd-HHmmss"
	$reportSuffix = ""
	if ($Visible) {
		$reportSuffix = "-visible"
	}
	$reportPath = Join-Path $ResolvedOutputDir "fgui-tracy-sample-$timestamp$reportSuffix.md"
	$outputText = $output -join "`n"
	$passed = $outputText -match "pressure self test result:\s*true" -and $outputText -notmatch "OGRE EXCEPTION|PANIC|self test result:\s*false|self test case:.*FAIL"
	$resultText = if ($passed) { "PASS" } else { "CHECK_LOG" }
	$runMode = if ($Visible) { "visible window" } else { "hidden window" }
	$commandLine = "tools\run_fgui_tracy_sample.ps1 -Count $Count -ListCount $ListCount -Seconds $Seconds -Tail $Tail"
	if ($Visible) {
		$commandLine = "$commandLine -Visible"
	}
	if ($KeepAlive) {
		$commandLine = "$commandLine -KeepAlive"
	}
	$interesting = @($output | Select-String -Pattern "pressure stats|pressure case|pressure self test end|pressure self test result|RenderFrameDetail|RenderStats|PerfStat|Health|OGRE EXCEPTION|PANIC|self test result:\s*false")
	$lines = New-Object System.Collections.Generic.List[string]
	$title = "FGUI Tracy Sample $timestamp"
	if ($Visible) {
		$title = "$title Visible"
	}
	$lines.Add("# $title")
	$lines.Add("")
	$lines.Add("- Mode: Pressure")
	$lines.Add("- Count: $Count")
	$lines.Add("- ListCount: $ListCount")
	$lines.Add("- Seconds: $Seconds")
	$lines.Add("- Run mode: $runMode")
	$lines.Add("- Result: $resultText")
	$lines.Add("- Command: ``$commandLine``")
	$lines.Add("- Tracy viewer: tools\tracy-viewer")
	$lines.Add("")
	$lines.Add("## Extract")
	if ($interesting.Count -eq 0) {
		$lines.Add("")
		$lines.Add("No matching pressure/render/perf lines were found in this run.")
	} else {
		$lines.Add("")
		$lines.Add('```text')
		foreach ($line in $interesting) {
			$lines.Add($line.Line.TrimEnd())
		}
		$lines.Add('```')
	}
	Set-Content -LiteralPath $reportPath -Value $lines -Encoding UTF8
	Write-Host "[FGUI] Tracy sample report: $reportPath"
} finally {
	Remove-Item -Path "Env:HELLO_FGUI_PRESSURE_LIST_COUNT" -ErrorAction SilentlyContinue
}
