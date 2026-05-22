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

	Write-Host "[FGUI-CHECK] static checks passed."
} finally {
	Pop-Location
}
