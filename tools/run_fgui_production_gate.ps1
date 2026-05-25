param(
	[ValidateSet("Fast", "Full", "RuntimeOnly", "BuildOnly")]
	[string]$Mode = "Full",
	[ValidateRange(1, 1000)]
	[int]$LongLoopCount = 5,
	[ValidateRange(1, 1000)]
	[int]$PressurePopupCount = 50,
	[ValidateRange(0, 1000)]
	[int]$PressureListCount = 80,
	[int]$AllSeconds = 0,
	[int]$LongLoopSeconds = 0,
	[int]$PressureSeconds = 0,
	[int]$Tail = 160,
	[ValidateRange(1, 5)]
	[int]$SelfTestRetries = 2,
	[switch]$SkipBuild,
	[switch]$SkipStatic,
	[switch]$SkipSelfTest,
	[switch]$SkipPressure,
	[switch]$SkipGitCheck,
	[switch]$StopExisting,
	[switch]$Visible,
	[switch]$DryRun
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptRoot "..")
$SelfTestScript = Join-Path $ScriptRoot "run_fgui_selftest.ps1"
$StaticCheckScript = Join-Path $ScriptRoot "check_fgui_static.ps1"
$SolutionPath = Join-Path $RepoRoot "build\HelloOgre3D.sln"
$LogPath = Join-Path $RepoRoot "bin\Sandbox.log"

function Write-GateStep {
	param([string]$Title)
	Write-Host ""
	Write-Host "[FGUI-GATE] === $Title ==="
}

function Invoke-CommandStep {
	param(
		[string]$Title,
		[string]$Command,
		[string[]]$Arguments,
		[string]$WorkingDirectory
	)

	Write-GateStep $Title
	if ($DryRun) {
		Write-Host "[FGUI-GATE] dry-run:" $Command ($Arguments -join " ")
		return
	}

	Push-Location $WorkingDirectory
	try {
		& $Command @Arguments
		if ($LASTEXITCODE -ne 0) {
			throw "$Title failed with exit code $LASTEXITCODE"
		}
	} finally {
		Pop-Location
	}
}

function Invoke-StaticCheck {
	param(
		[string]$Title
	)

	Write-GateStep $Title
	if ($DryRun) {
		Write-Host "[FGUI-GATE] dry-run:" $StaticCheckScript
		return
	}

	Push-Location $RepoRoot
	try {
		& $StaticCheckScript
		if ($LASTEXITCODE -ne 0) {
			throw "$Title failed with exit code $LASTEXITCODE"
		}
	} finally {
		Pop-Location
	}
}

function Get-VS2017MSBuild {
	$candidates = @(
		"C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise\MSBuild\15.0\Bin\amd64\MSBuild.exe",
		"C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\MSBuild\15.0\Bin\amd64\MSBuild.exe",
		"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\amd64\MSBuild.exe",
		"C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\MSBuild\15.0\Bin\amd64\MSBuild.exe"
	)
	foreach ($candidate in $candidates) {
		if (Test-Path -LiteralPath $candidate) {
			return $candidate
		}
	}
	throw "VS2017 amd64 MSBuild.exe not found."
}

function Invoke-MSBuildConfig {
	param([string]$Configuration)

	if (-not (Test-Path -LiteralPath $SolutionPath)) {
		throw "Solution not found: $SolutionPath. Run vs2017.bat before the production gate."
	}

	$msbuild = Get-VS2017MSBuild
	Invoke-CommandStep "VS2017 $Configuration|x64 build" $msbuild @(
		$SolutionPath,
		"/t:HelloOgre3D",
		"/p:Configuration=$Configuration",
		"/p:Platform=x64",
		"/m"
	) $RepoRoot
}

function Invoke-SelfTest {
	param(
		[string]$SelfTestMode,
		[int]$Count,
		[int]$Seconds,
		[int]$PopupCount,
		[int]$ListCount
	)

	$params = @{
		Mode = $SelfTestMode
		Count = $Count
		Tail = $Tail
	}
	if ($Seconds -gt 0) {
		$params.Seconds = $Seconds
	}
	if ($StopExisting) {
		$params.StopExisting = $true
	}
	if ($Visible) {
		$params.Visible = $true
	}
	if ($SelfTestMode -eq "Pressure") {
		$params.PressurePopupCount = $PopupCount
		if ($ListCount -gt 0) {
			$params.PressureListCount = $ListCount
		}
	}
	for ($attempt = 1; $attempt -le $SelfTestRetries; $attempt++) {
		try {
			Write-GateStep "FGUI $SelfTestMode selftest attempt $attempt/$SelfTestRetries"
			if ($DryRun) {
				Write-Host "[FGUI-GATE] dry-run:" $SelfTestScript (($params.GetEnumerator() | ForEach-Object { "-$($_.Key) $($_.Value)" }) -join " ")
				return
			}
			Push-Location $RepoRoot
			try {
				& $SelfTestScript @params
				if ($LASTEXITCODE -ne 0) {
					throw "FGUI $SelfTestMode selftest failed with exit code $LASTEXITCODE"
				}
			} finally {
				Pop-Location
			}
			return
		} catch {
			$errorText = $_ | Out-String
			$deviceFailure = $false
			if (($errorText -match "FGUI|selftest|exit code|log contains") -and (Test-Path -LiteralPath $LogPath)) {
				$deviceFailure = [bool](Select-String -LiteralPath $LogPath -Pattern "Cannot create device" -Quiet)
			}
			if ($attempt -lt $SelfTestRetries -and $deviceFailure) {
				Write-Host "[FGUI-GATE] D3D9 device creation failed; retrying selftest after a short delay."
				Start-Sleep -Seconds 5
				continue
			}
			throw
		}
	}
}

function Get-PressureWaitSeconds {
	if ($PressureSeconds -gt 0) {
		return $PressureSeconds
	}
	return [int][Math]::Max(45, 18 + [Math]::Ceiling($PressurePopupCount / 2) + [Math]::Ceiling($PressureListCount / 10))
}

if (-not (Test-Path -LiteralPath $SelfTestScript)) {
	throw "Selftest script not found: $SelfTestScript"
}
if (-not (Test-Path -LiteralPath $StaticCheckScript)) {
	throw "Static check script not found: $StaticCheckScript"
}

$runBuild = (-not $SkipBuild) -and ($Mode -eq "Full" -or $Mode -eq "BuildOnly")
$runStatic = (-not $SkipStatic) -and ($Mode -ne "BuildOnly")
$runSelfTest = (-not $SkipSelfTest) -and ($Mode -ne "BuildOnly")
$runLongLoop = $runSelfTest -and ($Mode -eq "Full" -or $Mode -eq "RuntimeOnly")
$runPressure = $runSelfTest -and (-not $SkipPressure) -and ($Mode -eq "Full")
$runGitCheck = (-not $SkipGitCheck) -and ($Mode -ne "BuildOnly")

Write-Host "[FGUI-GATE] mode=$Mode repo=$RepoRoot"
Write-Host "[FGUI-GATE] build=$runBuild static=$runStatic selftest=$runSelfTest longLoop=$runLongLoop pressure=$runPressure gitCheck=$runGitCheck"

if ($runBuild) {
	Invoke-MSBuildConfig "Debug"
	Invoke-MSBuildConfig "Release"
}

if ($runStatic) {
	Invoke-StaticCheck "FGUI static checks"
}

if ($runSelfTest) {
	Invoke-SelfTest "All" 3 $AllSeconds 0 0
}

if ($runLongLoop) {
	Invoke-SelfTest "LongLoop" $LongLoopCount $LongLoopSeconds 0 0
}

if ($runPressure) {
	Invoke-SelfTest "Pressure" $PressurePopupCount (Get-PressureWaitSeconds) $PressurePopupCount $PressureListCount
}

if ($runGitCheck) {
	Invoke-CommandStep "git diff --check" "git" @("diff", "--check") $RepoRoot
}

Write-Host ""
Write-Host "[FGUI-GATE] production gate passed."
