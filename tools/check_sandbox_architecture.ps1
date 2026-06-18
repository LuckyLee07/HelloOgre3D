param(
	[switch]$ListBaseline
)

$ErrorActionPreference = "Stop"

$ScriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = (Resolve-Path (Join-Path $ScriptRoot "..")).ProviderPath

$SourceRoots = @(
	"src\HelloOgre3D\sandbox",
	"src\HelloOgre3D\runtime"
)

$AllowedIncludes = @{
	"src\HelloOgre3D\sandbox\systems\input\InputManagerMac.mm|ClientManager.h" = 1
	"src\HelloOgre3D\sandbox\systems\input\InputManager.cpp|ClientManager.h" = 1
}

$AllowedGlobals = @{
	"src\HelloOgre3D\sandbox\systems\manager\SandboxMgr.h|g_SandboxMgr" = 1
	"src\HelloOgre3D\sandbox\systems\manager\SandboxMgr.cpp|g_SandboxMgr" = 3
	"src\HelloOgre3D\sandbox\systems\manager\ObjectManager.h|g_ObjectManager" = 1
	"src\HelloOgre3D\sandbox\systems\manager\ObjectManager.cpp|g_ObjectManager" = 2
}

function Get-SourceFiles {
	$extensions = @(".h", ".hpp", ".cpp", ".cc", ".cxx", ".mm")
	$files = @()
	foreach ($root in $SourceRoots) {
		$absoluteRoot = Join-Path $RepoRoot $root
		if (-not (Test-Path -LiteralPath $absoluteRoot)) {
			continue
		}

		$files += Get-ChildItem -LiteralPath $absoluteRoot -Recurse -File |
			Where-Object { $extensions -contains $_.Extension.ToLowerInvariant() }
	}
	return $files
}

function Get-RelativePath {
	param([string]$Path)

	$fullPath = (Resolve-Path -LiteralPath $Path).ProviderPath
	if (-not $fullPath.StartsWith($RepoRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
		return $fullPath
	}
	return $fullPath.Substring($RepoRoot.Length).TrimStart("\") -replace "/", "\"
}

function Add-Count {
	param(
		[hashtable]$Counts,
		[string]$Key,
		[int]$Count
	)

	if ($Count -le 0) {
		return
	}
	if (-not $Counts.ContainsKey($Key)) {
		$Counts[$Key] = 0
	}
	$Counts[$Key] += $Count
}

function Test-CountsAgainstBaseline {
	param(
		[string]$Title,
		[hashtable]$Actual,
		[hashtable]$Allowed
	)

	$violations = @()
	foreach ($key in ($Actual.Keys | Sort-Object)) {
		$count = [int]$Actual[$key]
		$allowedCount = 0
		if ($Allowed.ContainsKey($key)) {
			$allowedCount = [int]$Allowed[$key]
		}

		if ($count -gt $allowedCount) {
			$violations += "$key count=$count allowed=$allowedCount"
		}
	}

	if ($violations.Count -gt 0) {
		throw "$Title found new sandbox architecture violations:`n$($violations -join "`n")"
	}
}

Write-Host "[SANDBOX-ARCH] Scanning sandbox/runtime architecture guards"

$includeCounts = @{}
$globalCounts = @{}
$globalTokens = @("g_GameManager", "g_ObjectManager", "g_SandboxMgr")

foreach ($file in Get-SourceFiles) {
	$relativePath = Get-RelativePath $file.FullName
	$text = Get-Content -LiteralPath $file.FullName -Raw

	$gameManagerIncludeCount = ([regex]::Matches($text, '#include\s+[<"](?:game/)?GameManager\.h[>"]')).Count
	Add-Count $includeCounts "$relativePath|GameManager.h" $gameManagerIncludeCount

	$clientManagerIncludeCount = ([regex]::Matches($text, '#include\s+[<"](?:client/)?ClientManager\.h[>"]')).Count
	Add-Count $includeCounts "$relativePath|ClientManager.h" $clientManagerIncludeCount

	foreach ($token in $globalTokens) {
		$count = ([regex]::Matches($text, "(?<![A-Za-z0-9_])$token(?![A-Za-z0-9_])")).Count
		Add-Count $globalCounts "$relativePath|$token" $count
	}
}

if ($ListBaseline) {
	Write-Host "[SANDBOX-ARCH] Include baseline"
	foreach ($key in ($includeCounts.Keys | Sort-Object)) {
		Write-Host "`"$key`" = $($includeCounts[$key])"
	}
	Write-Host "[SANDBOX-ARCH] Global baseline"
	foreach ($key in ($globalCounts.Keys | Sort-Object)) {
		Write-Host "`"$key`" = $($globalCounts[$key])"
	}
}

Test-CountsAgainstBaseline "Include guard" $includeCounts $AllowedIncludes
Test-CountsAgainstBaseline "Global singleton guard" $globalCounts $AllowedGlobals

Write-Host "[SANDBOX-ARCH] static checks passed."
