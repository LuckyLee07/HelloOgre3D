param(
	[switch]$SkipLua,
	[switch]$SkipPython,
	[switch]$SkipAutoGen
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Resolve-Path (Join-Path $ScriptRoot "..")

function Invoke-Checked {
	param(
		[string]$Title,
		[string]$Command,
		[string[]]$Arguments
	)

	Write-Host "[FGUI-CHECK] $Title"
	& $Command @Arguments
	if ($LASTEXITCODE -ne 0) {
		throw "$Title failed with exit code $LASTEXITCODE"
	}
}

function Test-FairyGuiNativeEntryGuard {
	Write-Host "[FGUI-CHECK] Native entry guard"

	$fguiLuaRoot = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui"
	$nativeApiPath = Join-Path $fguiLuaRoot "FairyGuiNativeApi.lua"
	if (-not (Test-Path -LiteralPath $nativeApiPath)) {
		throw "FairyGuiNativeApi.lua not found"
	}

	$nativeApiText = Get-Content -LiteralPath $nativeApiPath -Raw
	$runtimeIndex = $nativeApiText.IndexOf('"FairyGuiRuntime"')
	$gameManagerIndex = $nativeApiText.IndexOf('"GameManager"')
	if ($runtimeIndex -lt 0 -or $gameManagerIndex -lt 0 -or $runtimeIndex -gt $gameManagerIndex) {
		throw "FairyGuiNativeApi.lua must resolve FairyGuiRuntime before GameManager"
	}

	$violations = @()
	$fguiLuaFiles = Get-ChildItem -Recurse -File $fguiLuaRoot -Filter "*.lua"
	foreach ($file in $fguiLuaFiles) {
		if ($file.Name -eq "FairyGuiNativeApi.lua") {
			continue
		}

		$matches = Select-String -LiteralPath $file.FullName -Pattern "GameManager" -SimpleMatch
		foreach ($match in $matches) {
			$violations += "$($file.FullName):$($match.LineNumber): $($match.Line.Trim())"
		}
	}

	if ($violations.Count -gt 0) {
		throw "FGUI manager modules must use FairyGuiNativeApi instead of GameManager directly:`n$($violations -join "`n")"
	}
}

Push-Location $RepoRoot
try {
	if (-not $SkipLua) {
		$luaFiles = Get-ChildItem -Recurse -File "bin\res\scripts" -Filter "*.lua" | ForEach-Object { $_.FullName }
		if ($luaFiles.Count -le 0) {
			throw "No Lua files found under bin\res\scripts"
		}
		$luaArgs = @("-p")
		$luaArgs += $luaFiles
		Invoke-Checked "Lua syntax" "luac" $luaArgs
	}

	if (-not $SkipPython) {
		Invoke-Checked "Python compile" "python" @(
			"-B",
			"-c",
			"import pathlib, sys; [compile(pathlib.Path(path).read_text(encoding='utf-8-sig'), path, 'exec') for path in sys.argv[1:]]",
			"tools\fgui_autogen\fairygui_asset_manifest.py",
			"tools\fgui_autogen\fairygui_autogen.py",
			"tools\fgui_autogen\fairygui_batch_generate.py",
			"tools\fgui_autogen\fairygui_generate_ui.py",
			"tools\codex_fileop.py"
		)
	}

	if (-not $SkipAutoGen) {
		Invoke-Checked "AutoGen batch check" "python" @(
			"-B",
			"tools\fgui_autogen\fairygui_batch_generate.py",
			"--check",
			"--strict"
		)
	}

	Test-FairyGuiNativeEntryGuard

	Write-Host "[FGUI-CHECK] static checks passed."
} finally {
	Pop-Location
}
