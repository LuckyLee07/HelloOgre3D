param([string]$OutputDirectory = "", [switch]$Archive)
$ErrorActionPreference = 'Stop'

function ConvertTo-RelayCrlf([string]$Text) {
	# Here-strings already contain CRLF when this script is saved with CRLF.
	return ($Text -replace "`r`n?", "`n").Replace("`n", "`r`n")
}

function Assert-RelayX64Pe([string]$Path) {
	$relayStream = [IO.File]::OpenRead($Path)
	$relayReader = New-Object IO.BinaryReader($relayStream)
	try {
		if ($relayReader.ReadUInt16() -ne 0x5A4D) { throw "Not a Windows PE image: $Path" }
		$relayStream.Position = 0x3C
		$relayStream.Position = $relayReader.ReadUInt32()
		if ($relayReader.ReadUInt32() -ne 0x00004550 -or $relayReader.ReadUInt16() -ne 0x8664) {
			throw "Expected an x64 Windows PE image: $Path"
		}
	} finally {
		$relayReader.Dispose()
	}
}

$relayRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
	$OutputDirectory = Join-Path $relayRoot ('tmp\RelayOutpost-win64-' + (Get-Date -Format 'yyyyMMdd-HHmmss-fff') + '-' + [Guid]::NewGuid().ToString('N').Substring(0, 8))
}
$relayOutput = [IO.Path]::GetFullPath($OutputDirectory)
$relayOutputPrefix = $relayOutput.TrimEnd([IO.Path]::DirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
$relayRootPrefix = $relayRoot.TrimEnd([IO.Path]::DirectorySeparatorChar) + [IO.Path]::DirectorySeparatorChar
$relayTemporaryRoot = [IO.Path]::GetFullPath((Join-Path $relayRoot 'tmp')) + [IO.Path]::DirectorySeparatorChar
if ($relayOutput.StartsWith($relayRootPrefix, [StringComparison]::OrdinalIgnoreCase) -and
	-not $relayOutput.StartsWith($relayTemporaryRoot, [StringComparison]::OrdinalIgnoreCase)) {
	throw "Inside the checkout, output must be in tmp. An explicit new directory outside the checkout is also supported."
}
if (Test-Path -LiteralPath $relayOutput) { throw 'Output already exists; choose a new directory.' }
if ($Archive -and (Test-Path -LiteralPath ($relayOutput + '.zip'))) { throw 'Archive already exists; choose a new directory.' }
$relayExe = Join-Path $relayRoot 'bin\HelloOgre3D.exe'
if (-not (Test-Path -LiteralPath $relayExe -PathType Leaf)) { throw 'Build Release before packaging.' }
Assert-RelayX64Pe $relayExe

# The current Release import audit found only Windows OS DLLs plus D3DX9_43.
# Ogre/ParticleFX/CRT are linked statically. winmm is a Windows audio component.
# Bundle the optional legacy DirectX helper app-locally; never copy OS DLLs.
$relayD3dx = Join-Path $relayRoot 'bin\d3dx9_43.dll'
if (-not (Test-Path -LiteralPath $relayD3dx -PathType Leaf)) {
	$relaySystemFolder = if ([Environment]::Is64BitProcess) { 'System32' } else { 'Sysnative' }
	$relayD3dx = Join-Path (Join-Path $env:WINDIR $relaySystemFolder) 'd3dx9_43.dll'
}
if (-not (Test-Path -LiteralPath $relayD3dx -PathType Leaf)) {
	throw 'Missing x64 d3dx9_43.dll. Supply it in bin or install the DirectX June 2010 runtime before packaging.'
}
Assert-RelayX64Pe $relayD3dx

# Refuse a resource config that escapes the package, including absolute paths.
$relayResourceConfig = Join-Path $relayRoot 'bin\SandboxResources.cfg'
$relayResourceLocations = @()
foreach ($relayLine in [IO.File]::ReadAllLines($relayResourceConfig)) {
	$relayLine = $relayLine.Trim()
	if ($relayLine -eq '' -or $relayLine -match '^[#;\[]') { continue }
	if ($relayLine -notmatch '^(FileSystem|Obf|Zip)\s*=\s*(.+)$') { throw "Unsupported resource entry: $relayLine" }
	$relayLocation = $Matches[2].Trim()
	if ([IO.Path]::IsPathRooted($relayLocation)) { throw "Resource path must be relative: $relayLocation" }
	$relayPackagedLocation = [IO.Path]::GetFullPath((Join-Path (Join-Path $relayOutput 'bin') $relayLocation))
	if (-not $relayPackagedLocation.StartsWith($relayOutputPrefix, [StringComparison]::OrdinalIgnoreCase)) {
		throw "Resource path escapes the package: $relayLocation"
	}
	$relayResourceLocations += $relayPackagedLocation
}
# Only tracked resources plus this feature's known new files; no local radar,
# settings, logs, credentials, debug symbols or arbitrary untracked assets.
$relayResources = @(& git -c core.quotepath=false -C $relayRoot ls-files -- bin/res media)
if ($LASTEXITCODE -ne 0) { throw 'Could not enumerate tracked resources.' }
$relayNewFiles = @(
	'bin/res/scripts/samples/sandbox19_scene.lua',
	'bin/res/scripts/samples/sandbox19_hud.lua',
	'bin/res/scripts/samples/sandbox19_commands.lua',
	'bin/res/scripts/samples/sandbox19_audio.lua',
	'bin/res/scripts/samples/sandbox19_product_selftest.lua',
	'media/materials/sandbox19_relay.material'
)
$relayNewFiles += Get-ChildItem -LiteralPath (Join-Path $relayRoot 'bin\res\audio\relay') -File | ForEach-Object {
	'bin/res/audio/relay/' + $_.Name
}
$relayFiles = @(($relayResources + $relayNewFiles) | Sort-Object -Unique)
foreach ($relayRelative in $relayFiles) {
	if (-not (Test-Path -LiteralPath (Join-Path $relayRoot $relayRelative) -PathType Leaf)) {
		throw "Package input is missing: $relayRelative"
	}
}

# Finish preflight before writing output. This script never deletes or replaces
# an existing package, including an explicitly selected user temp directory.
New-Item -ItemType Directory -Path (Join-Path $relayOutput 'bin') -Force | Out-Null
Copy-Item -LiteralPath $relayExe -Destination (Join-Path $relayOutput 'bin\HelloOgre3D.exe')
Copy-Item -LiteralPath $relayD3dx -Destination (Join-Path $relayOutput 'bin\d3dx9_43.dll')
Copy-Item -LiteralPath $relayResourceConfig -Destination (Join-Path $relayOutput 'bin\SandboxResources.cfg')
# Device names are machine-specific; Ogre selects the local default adapter.
$relayRenderConfig = [IO.File]::ReadAllText((Join-Path $relayRoot 'bin\Sandbox.cfg')) -replace '(?m)^Rendering Device=.*(?:\r?\n|$)', ''
[IO.File]::WriteAllText((Join-Path $relayOutput 'bin\Sandbox.cfg'), (ConvertTo-RelayCrlf $relayRenderConfig), [Text.Encoding]::ASCII)
foreach ($relayRelative in $relayFiles) {
	$relaySource = Join-Path $relayRoot $relayRelative
	$relayDestination = Join-Path $relayOutput $relayRelative
	New-Item -ItemType Directory -Path (Split-Path -Parent $relayDestination) -Force | Out-Null
	Copy-Item -LiteralPath $relaySource -Destination $relayDestination
}
foreach ($relayLocation in $relayResourceLocations) {
	if (-not (Test-Path -LiteralPath $relayLocation)) { throw "Packaged resource location is missing: $relayLocation" }
}
$relayLauncher = @'
@echo off
setlocal
cd /d "%~dp0bin"
rem Remove inherited automated tests, without clearing graphics/audio settings.
for /f "tokens=1 delims==" %%V in ('set HELLO_ 2^>nul ^| findstr /i /r /c:"^HELLO_[A-Z0-9_]*SELF_TEST[A-Z0-9_]*=" /c:"^HELLO_[A-Z0-9_]*PRODUCT_TEST[A-Z0-9_]*=" /c:"^HELLO_[A-Z0-9_]*SMOKE[A-Z0-9_]*="') do set "%%V="
for %%P in (HELLO_INPUT_REPLAY HELLO_RENDER_CAPTURE HELLO_VISUAL_CAPTURE HELLO_VISUAL_TRACE HELLO_PARITY_TRACE HELLO_PARITY_SEED HELLO_INTENT_TRACE) do call :ClearPrefix %%P
set "HELLO_WINDOW_BACKGROUND="
set "HELLO_WINDOW_WIDTH="
set "HELLO_WINDOW_HEIGHT="
set "HELLO_AUDIO_SILENT="
set "HELLO_SANDBOX_SAMPLE=Sandbox19"
set "HELLO_SAMPLE_PRESET=Sandbox19"
start "" "HelloOgre3D.exe"
exit /b
:ClearPrefix
for /f "tokens=1 delims==" %%V in ('set %~1 2^>nul') do set "%%V="
exit /b
'@
[IO.File]::WriteAllText((Join-Path $relayOutput 'Play.cmd'), (ConvertTo-RelayCrlf $relayLauncher) + "`r`n", [Text.Encoding]::ASCII)
$relayReadme = @'
RELAY OUTPOST - Windows x64 private playtest

Double-click Play.cmd. The whole folder is self-contained; keep bin and media
together. Start in the briefing, then select START MISSION or press Enter.
Play.cmd selects Sandbox19 and its preset, and removes inherited replay,
self-test, smoke and capture settings. Normal rendering settings remain usable.
Use Play.cmd rather than launching the EXE from an arbitrary working directory.

WASD move; Q/E orbit; mouse wheel zoom; Shift sprint.
Left click/drag selects allies; 1/2 select one; Tab selects both.
Right click ground rallies; right click visible enemy focuses.
F focus; G gather at commander; T fall back to the entry and hold; X cancel.
Esc pauses; the pause menu has sound settings, retry and quit.
Clear the gate, advance to the courtyard, clear its guards, then bring the
commander and at least one surviving ally to the amber relay assembly zone.
I opens the AI observer; O writes its evidence; F3 paths; F5 performance.

Settings are written to bin/relay_settings.cfg; runtime log is bin/Sandbox.log.
This package reuses existing project soldier assets subject to their original
DEXSOFT license. Nobiax and other source notices remain in media. The package
uses unchanged TropicalSunnyDay textures from SkyboxSet by Heiko Irrgang,
licensed CC BY-SA 3.0 (media/textures/skybox_set/LICENSE.txt). This package
is for the owner's local playtest; no external distribution grant is implied.
No development workspace, Python, Visual Studio or source checkout is needed.
Requires Windows x64 and a working Direct3D9 graphics driver. The optional legacy
d3dx9_43.dll helper is included beside the EXE; other DLLs are Windows components.
'@
[IO.File]::WriteAllText((Join-Path $relayOutput 'README.txt'), (ConvertTo-RelayCrlf $relayReadme) + "`r`n", [Text.Encoding]::UTF8)
$relayManifest = Get-ChildItem -LiteralPath $relayOutput -File -Recurse | ForEach-Object {
	[pscustomobject]@{ path = $_.FullName.Substring($relayOutput.Length + 1); bytes = $_.Length; sha256 = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash }
}
$relayManifest | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $relayOutput 'manifest.json') -Encoding UTF8
if ($Archive) { Compress-Archive -LiteralPath $relayOutput -DestinationPath ($relayOutput + '.zip') }
Write-Output $relayOutput
