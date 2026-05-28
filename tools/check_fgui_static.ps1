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
	if ($runtimeIndex -lt 0) {
		throw "FairyGuiNativeApi.lua must resolve FairyGuiRuntime"
	}
	if ($nativeApiText.IndexOf("GameManager") -ge 0) {
		throw "FairyGuiNativeApi.lua must not use GameManager as a FairyGUI native backend"
	}

	$violations = @()
	$fguiLuaFiles = Get-ChildItem -Recurse -File $fguiLuaRoot -Filter "*.lua"
	foreach ($file in $fguiLuaFiles) {
		$matches = Select-String -LiteralPath $file.FullName -Pattern "GameManager" -SimpleMatch
		foreach ($match in $matches) {
			$violations += "$($file.FullName):$($match.LineNumber): $($match.Line.Trim())"
		}
	}

	if ($violations.Count -gt 0) {
		throw "FGUI manager modules must use FairyGuiNativeApi instead of GameManager directly:`n$($violations -join "`n")"
	}
}

function Test-FairyGuiGameManagerLegacyBindingGuard {
	Write-Host "[FGUI-CHECK] GameManager legacy binding guard"

	$gameManagerHeaderPath = Join-Path $RepoRoot "src\HelloOgre3D\game\GameManager.h"
	$gameToLuaPath = Join-Path $RepoRoot "src\HelloOgre3D\game\GameToLua.cpp"

	$headerText = Get-Content -LiteralPath $gameManagerHeaderPath -Raw
	$beginIndex = $headerText.IndexOf("//tolua_begin")
	$endIndex = $headerText.IndexOf("//tolua_end", $beginIndex)
	if ($beginIndex -lt 0 -or $endIndex -lt 0 -or $endIndex -lt $beginIndex) {
		throw "GameManager.h tolua export block not found"
	}

	$exportBlock = $headerText.Substring($beginIndex, $endIndex - $beginIndex)
	if ($exportBlock.IndexOf("FairyGui") -ge 0) {
		throw "GameManager tolua export block must not expose FairyGUI APIs; use RuntimeToLua/FairyGuiRuntime"
	}

	if (-not (Test-Path -LiteralPath $gameToLuaPath)) {
		throw "GameToLua.cpp not found"
	}

	$bindingViolations = Select-String -LiteralPath $gameToLuaPath -Pattern 'tolua_function\(tolua_S,".*FairyGui'
	if ($bindingViolations.Count -gt 0) {
		$lines = $bindingViolations | ForEach-Object { "$($_.Path):$($_.LineNumber): $($_.Line.Trim())" }
		throw "GameToLua.cpp must not register FairyGUI APIs on GameManager:`n$($lines -join "`n")"
	}
}

function Test-FairyGuiRuntimeOwnershipGuard {
	Write-Host "[FGUI-CHECK] Runtime ownership guard"

	$runtimeFairyGuiRoot = Join-Path $RepoRoot "src\HelloOgre3D\runtime\ui\fairygui"
	$runtimeToLuaPkgPath = Join-Path $RepoRoot "src\HelloOgre3D\runtime\RuntimeToLua.pkg"
	$violations = @()

	$runtimeFiles = Get-ChildItem -Recurse -File $runtimeFairyGuiRoot -Include "*.h", "*.cpp", "*.pkg"
	if (Test-Path -LiteralPath $runtimeToLuaPkgPath) {
		$runtimeFiles += Get-Item -LiteralPath $runtimeToLuaPkgPath
	}

	foreach ($file in $runtimeFiles) {
		$matches = Select-String -LiteralPath $file.FullName -Pattern "GameManager", "ClientManager" -SimpleMatch
		foreach ($match in $matches) {
			$violations += "$($file.FullName):$($match.LineNumber): $($match.Line.Trim())"
		}
	}

	if ($violations.Count -gt 0) {
		throw "FairyGUI runtime code must not depend on game/client manager layers:`n$($violations -join "`n")"
	}
}

function Test-FairyGuiPublicHeaderFacadeGuard {
	Write-Host "[FGUI-CHECK] Public header facade guard"

	$headerPath = Join-Path $RepoRoot "src\HelloOgre3D\runtime\ui\fairygui\FairyGuiSystem.h"
	if (-not (Test-Path -LiteralPath $headerPath)) {
		throw "FairyGuiSystem.h not found"
	}

	$headerText = Get-Content -LiteralPath $headerPath -Raw
	$forbiddenPatterns = @(
		"namespace Ogre",
		"Ogre::",
		"#include ""Ogre",
		"#include <Ogre",
		"cocos2d",
		"fairygui::",
		"RenderCommandSink",
		"GObject",
		"GRoot"
	)

	$violations = @()
	foreach ($pattern in $forbiddenPatterns) {
		if ($headerText.IndexOf($pattern) -ge 0) {
			$violations += $pattern
		}
	}

	if ($violations.Count -gt 0) {
		throw "FairyGuiSystem.h must stay a runtime facade and not expose engine/UI implementation details: $($violations -join ', ')"
	}
	if ($headerText.IndexOf("struct FairyGuiSystemStartupContext") -lt 0) {
		throw "FairyGuiSystem.h must expose FairyGuiSystemStartupContext as the opaque startup boundary"
	}
	if ($headerText.IndexOf("bool Initialize(const FairyGuiSystemStartupContext& context)") -lt 0) {
		throw "FairyGuiSystem.h must initialize through FairyGuiSystemStartupContext"
	}
	if ($headerText.IndexOf("FairyGuiSystemImpl* m_impl") -lt 0) {
		throw "FairyGuiSystem.h must keep implementation details behind FairyGuiSystemImpl"
	}
}

function Test-FairyGuiInternalHeaderGuard {
	Write-Host "[FGUI-CHECK] Internal header split guard"

	$fguiRuntimeRoot = Join-Path $RepoRoot "src\HelloOgre3D\runtime\ui\fairygui"
	$implPath = Join-Path $fguiRuntimeRoot "FairyGuiSystemImpl.h"
	$internalPath = Join-Path $fguiRuntimeRoot "FairyGuiSystemInternal.h"
	$commonHelpersPath = Join-Path $fguiRuntimeRoot "FairyGuiSystemCommonHelpers.h"
	$implText = Get-Content -LiteralPath $implPath -Raw
	$internalText = Get-Content -LiteralPath $internalPath -Raw
	$commonHelpersText = Get-Content -LiteralPath $commonHelpersPath -Raw

	$implForbiddenPatterns = @(
		"FairyGuiSystemFairyIncludes.h",
		"#include ""Ogre",
		"#include <Ogre",
		"#include <windows.h>",
		"#include <imm.h>",
		"#include ""GObject.h""",
		"#include ""GRoot.h""",
		"#include ""UIPackage.h"""
	)
	$violations = @()
	foreach ($pattern in $implForbiddenPatterns) {
		if ($implText.IndexOf($pattern) -ge 0) {
			$violations += "FairyGuiSystemImpl.h contains $pattern"
		}
	}
	if ($internalText.IndexOf("FairyGuiSystemFairyIncludes.h") -ge 0 -or $internalText.IndexOf("FairyGuiSystemInternalHelpers.h") -ge 0) {
		$violations += "FairyGuiSystemInternal.h must stay a thin compatibility include for FairyGuiSystemImpl.h only"
	}
	$commonForbiddenPatterns = @(
		"FairyGuiSystemFairyIncludes.h",
		"#include <windows.h>",
		"#include <imm.h>",
		"#include ""GObject.h""",
		"#include ""GRoot.h""",
		"#include ""UIPackage.h"""
	)
	foreach ($pattern in $commonForbiddenPatterns) {
		if ($commonHelpersText.IndexOf($pattern) -ge 0) {
			$violations += "FairyGuiSystemCommonHelpers.h must stay light and not include $pattern"
		}
	}

	$cppFiles = Get-ChildItem -LiteralPath $fguiRuntimeRoot -File -Filter "FairyGuiSystem*.cpp"
	foreach ($file in $cppFiles) {
		$text = Get-Content -LiteralPath $file.FullName -Raw
		if ($text.IndexOf("FairyGuiSystemInternal.h") -ge 0 -or $text.IndexOf("FairyGuiSystemInternalHelpers.h") -ge 0) {
			$violations += "$($file.Name) must include the specific impl/helper header it needs, not the internal aggregate header"
		}
	}

	$requiredIncludes = @{
		"FairyGuiSystem.cpp" = "FairyGuiSystemCommonHelpers.h"
		"FairyGuiSystemRender.cpp" = "FairyGuiSystemRenderHelpers.h"
		"FairyGuiSystemObjects.cpp" = "FairyGuiSystemObjectHelpers.h"
		"FairyGuiSystemInput.cpp" = "FairyGuiSystemInputHelpers.h"
		"FairyGuiSystemEvents.cpp" = "FairyGuiSystemFairyIncludes.h"
	}
	foreach ($fileName in $requiredIncludes.Keys) {
		$filePath = Join-Path $fguiRuntimeRoot $fileName
		$text = Get-Content -LiteralPath $filePath -Raw
		if ($text.IndexOf($requiredIncludes[$fileName]) -lt 0) {
			$violations += "$fileName must include $($requiredIncludes[$fileName])"
		}
	}
	$helperRequiredIncludes = @{
		"FairyGuiSystemInputHelpers.h" = "FairyGuiSystemFairyIncludes.h"
		"FairyGuiSystemObjectHelpers.h" = "FairyGuiSystemFairyIncludes.h"
		"FairyGuiSystemRenderHelpers.h" = "FairyGuiSystemFairyIncludes.h"
	}
	foreach ($fileName in $helperRequiredIncludes.Keys) {
		$filePath = Join-Path $fguiRuntimeRoot $fileName
		$text = Get-Content -LiteralPath $filePath -Raw
		if ($text.IndexOf($helperRequiredIncludes[$fileName]) -lt 0) {
			$violations += "$fileName must include $($helperRequiredIncludes[$fileName]) directly instead of relying on CommonHelpers"
		}
	}

	if ($violations.Count -gt 0) {
		throw "FairyGUI internal header split regressed:`n$($violations -join "`n")"
	}
}

function Test-FairyGuiLuaFrameworkGuard {
	Write-Host "[FGUI-CHECK] Lua framework facade guard"

	$compatManagerPath = Join-Path $RepoRoot "bin\res\scripts\manager\FairyGuiManager.lua"
	$fguiLuaRoot = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui"
	$uiRoot = Join-Path $RepoRoot "bin\res\scripts\ui"

	if (-not (Test-Path -LiteralPath $compatManagerPath)) {
		throw "Compatibility FairyGuiManager.lua not found"
	}
	$compatText = (Get-Content -LiteralPath $compatManagerPath -Raw).Trim()
	if ($compatText -ne 'return require("res.scripts.manager.fairygui.FairyGuiManager")') {
		throw "bin/res/scripts/manager/FairyGuiManager.lua must remain a compatibility facade"
	}

	$shortRequireViolations = @()
	$scriptFiles = Get-ChildItem -Recurse -File (Join-Path $RepoRoot "bin\res\scripts") -Filter "*.lua"
	foreach ($file in $scriptFiles) {
		if ($file.FullName -eq $compatManagerPath) {
			continue
		}
		$matches = Select-String -LiteralPath $file.FullName -Pattern 'res.scripts.manager.FairyGuiManager' -SimpleMatch
		foreach ($match in $matches) {
			$shortRequireViolations += "$($file.FullName):$($match.LineNumber): $($match.Line.Trim())"
		}
	}
	if ($shortRequireViolations.Count -gt 0) {
		throw "New code must require res.scripts.manager.fairygui.FairyGuiManager directly; the short path is only a compatibility shim:`n$($shortRequireViolations -join "`n")"
	}

	$managerFacadePath = Join-Path $fguiLuaRoot "FairyGuiManager.lua"
	$managerText = Get-Content -LiteralPath $managerFacadePath -Raw
	$requiredModules = @(
		"FairyGuiStore",
		"FairyGuiProfiler",
		"FairyGuiLifecycle",
		"FairyGuiPackage",
		"FairyGuiServices",
		"FairyGuiLayers",
		"FairyGuiEvents",
		"FairyGuiLists",
		"FairyGuiControls",
		"FairyGuiProbes"
	)
	foreach ($module in $requiredModules) {
		$requireText = "require(""res.scripts.manager.fairygui.$module"")"
		if ($managerText.IndexOf($requireText) -lt 0) {
			throw "FairyGuiManager.lua must load $module through the central facade"
		}
	}

	$selfOwnerViolations = @()
	$fguiLuaFiles = Get-ChildItem -Recurse -File $fguiLuaRoot -Filter "*.lua"
	foreach ($file in $fguiLuaFiles) {
		$matches = Select-String -LiteralPath $file.FullName -Pattern 'local self = self.owner' -SimpleMatch
		foreach ($match in $matches) {
			$selfOwnerViolations += "$($file.FullName):$($match.LineNumber): $($match.Line.Trim())"
		}
	}
	if ($selfOwnerViolations.Count -gt 0) {
		throw "FGUI submodules must not shadow themselves with owner; use an explicit local owner variable:`n$($selfOwnerViolations -join "`n")"
	}

	$uiViolations = @()
	if (Test-Path -LiteralPath $uiRoot) {
		$uiFiles = Get-ChildItem -Recurse -File $uiRoot -Filter "*.lua"
		foreach ($file in $uiFiles) {
			$matches = Select-String -LiteralPath $file.FullName -Pattern "FairyGuiNativeApi", "NativeApi", "FairyGuiRuntime", "GameManager" -SimpleMatch
			foreach ($match in $matches) {
				$uiViolations += "$($file.FullName):$($match.LineNumber): $($match.Line.Trim())"
			}
		}
	}
	if ($uiViolations.Count -gt 0) {
		throw "Business UI scripts must not call native backends directly; use FairyGuiManager/Ctrl/View/Model:`n$($uiViolations -join "`n")"
	}

	$viewRoot = Join-Path $uiRoot "views"
	$viewViolations = @()
	if (Test-Path -LiteralPath $viewRoot) {
		$viewFiles = Get-ChildItem -Recurse -File $viewRoot -Filter "*.lua"
		foreach ($file in $viewFiles) {
			$matches = Select-String -LiteralPath $file.FullName -Pattern "FairyGuiManager:CreateObject", "FairyGuiManager.CreateObject", "FairyGuiManager:LoadPackage", "FairyGuiManager.LoadPackage" -SimpleMatch
			foreach ($match in $matches) {
				$viewViolations += "$($file.FullName):$($match.LineNumber): $($match.Line.Trim())"
			}
		}
	}
	if ($viewViolations.Count -gt 0) {
		throw "Business view scripts must enter through registry/Open/Ctrl/View flow instead of manual package/object creation:`n$($viewViolations -join "`n")"
	}

	$profilerPath = Join-Path $fguiLuaRoot "FairyGuiProfiler.lua"
	$profilerText = Get-Content -LiteralPath $profilerPath -Raw
	$requiredSnapshotFields = @(
		"health = health",
		"perf = perf",
		"render = render",
		"eventStats = eventStats",
		"resourceWarnings = warnings",
		"resourceFallbacks = fallbacks",
		"layerSummary = self:CollectLayerSummary()",
		"bindingSummary = self:CollectBindingSummary",
		"packageSummary = self:CollectPackageSummary",
		"function FairyGuiProfiler:BuildAiDebugPanelLines",
		"function FairyGuiProfiler:ShowAiDebugPanel"
	)
	foreach ($field in $requiredSnapshotFields) {
		if ($profilerText.IndexOf($field) -lt 0) {
			throw "FairyGuiProfiler.lua debug snapshot/panel is missing required production diagnostic field: $field"
		}
	}
}

function Assert-TextContains {
	param(
		[string]$Title,
		[string]$Text,
		[string[]]$Needles
	)

	foreach ($needle in $Needles) {
		if ($Text.IndexOf($needle) -lt 0) {
			throw "$Title missing required marker: $needle"
		}
	}
}

function Test-FairyGuiProductionFeatureGuard {
	Write-Host "[FGUI-CHECK] Production feature coverage guard"

	$selfTestPath = Join-Path $RepoRoot "tools\run_fgui_selftest.ps1"
	$samplePath = Join-Path $RepoRoot "bin\res\scripts\samples\fgui_init.lua"
	$managerPath = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui\FairyGuiManager.lua"
	$controlsPath = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui\FairyGuiControls.lua"
	$eventsPath = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui\FairyGuiEvents.lua"
	$listsPath = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui\FairyGuiLists.lua"
	$baseViewPath = Join-Path $RepoRoot "bin\res\scripts\ui\BaseFairyGuiView.lua"
	$baseCtrlPath = Join-Path $RepoRoot "bin\res\scripts\ui\FairyGuiBaseCtrl.lua"
	$servicesPath = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui\FairyGuiServices.lua"
	$servicesCorePath = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui\FairyGuiServiceCore.lua"
	$layersPath = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui\FairyGuiLayers.lua"
	$packagePath = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui\FairyGuiPackage.lua"
	$profilerPath = Join-Path $RepoRoot "bin\res\scripts\manager\fairygui\FairyGuiProfiler.lua"
	$autogenScriptPath = Join-Path $RepoRoot "tools\fgui_autogen\fairygui_autogen.py"
	$renderPath = Join-Path $RepoRoot "src\HelloOgre3D\runtime\ui\fairygui\FairyGuiSystemRender.cpp"

	$selfTestText = Get-Content -LiteralPath $selfTestPath -Raw
	Assert-TextContains "run_fgui_selftest.ps1" $selfTestText @(
		'"BusinessFlow"',
		'"ComplexControls"',
		'"VirtualList"',
		'"Tree"',
		'"ResourcePolicy"',
		'"ResourceFallback"',
		'"TextInputPolicy"',
		'"Pressure"',
		'"LayerBoundary"',
		'"Mask"',
		'"BusinessBenchmark"',
		'"HELLO_FGUI_BUSINESS_FLOW_SELF_TEST"',
		'"HELLO_FGUI_BUSINESS_BENCHMARK_SELF_TEST"',
		'"HELLO_FGUI_COMPLEX_CONTROLS_SELF_TEST"',
		'"HELLO_FGUI_VIRTUAL_LIST_SELF_TEST"',
		'"HELLO_FGUI_TREE_SELF_TEST"',
		'"HELLO_FGUI_RESOURCE_POLICY_SELF_TEST"',
		'"HELLO_FGUI_RESOURCE_FALLBACK_SELF_TEST"',
		'"HELLO_FGUI_TEXT_INPUT_POLICY_SELF_TEST"',
		'"HELLO_FGUI_PRESSURE_SELF_TEST"',
		'"HELLO_FGUI_LAYER_BOUNDARY_SELF_TEST"',
		'"HELLO_FGUI_MASK_SELF_TEST"'
	)

	$sampleText = Get-Content -LiteralPath $samplePath -Raw
	Assert-TextContains "fgui_init.lua" $sampleText @(
		"function FGUI_RunComplexControlsSelfTest",
		"function FGUI_RunVirtualListSelfTest",
		"function FGUI_RunTreeSelfTest",
		"function FGUI_RunBusinessFlowSelfTest",
		"function FGUI_RunBusinessBenchmarkSelfTest",
		"GetDebugPanelSnapshot",
		"ShowGuideMask",
		"ConfigureTextInput",
		"CloseTopPopup",
		"function FGUI_RunResourcePolicySelfTest",
		"function FGUI_RunResourceFallbackSelfTest",
		"function FGUI_RunTextInputPolicySelfTest",
		"GetTextInputDisplayText",
		"AddFocusIn",
		"AddFocusOut",
		"function FGUI_RunPressureSelfTest",
		"function FGUI_RunLayerBoundarySelfTest",
		"FGUI_RunDebugPanelSelfTest",
		"FGUI_RunAiDebugPanelSelfTest",
		"cpuClipSourceTriangleCount",
		"stencilClipPolygonCount",
		"stencilBackend"
	)

	$baseCtrlText = Get-Content -LiteralPath $baseCtrlPath -Raw
	Assert-TextContains "FairyGuiBaseCtrl.lua" $baseCtrlText @(
		"function FairyGuiBaseCtrl:RequireControlPath",
		"function FairyGuiBaseCtrl:RequireControlType",
		"function FairyGuiBaseCtrl:GetListItemDefine",
		"function FairyGuiBaseCtrl:RequireListItemDefine",
		"function FairyGuiBaseCtrl:GetListItemControlPath",
		"function FairyGuiBaseCtrl:RequireListItemControlPath",
		"function FairyGuiBaseCtrl:BindListItemControls",
		"function FairyGuiBaseCtrl:GetComponentDefine",
		"function FairyGuiBaseCtrl:RequireComponentDefine",
		"function FairyGuiBaseCtrl:GetComponentControlPath",
		"function FairyGuiBaseCtrl:RequireComponentControlPath",
		"function FairyGuiBaseCtrl:ValidateAutoGenContract",
		"function FairyGuiBaseCtrl:ConfigureTextInput",
		"function FairyGuiBaseCtrl:AddFocusIn",
		"function FairyGuiBaseCtrl:AddFocusOut"
	)

	$baseViewText = Get-Content -LiteralPath $baseViewPath -Raw
	Assert-TextContains "BaseFairyGuiView.lua" $baseViewText @(
		"function BaseFairyGuiView:ConfigureTextInput",
		"function BaseFairyGuiView:GetTextInputDisplayText",
		"function BaseFairyGuiView:GetTextInputDebugInfo",
		"function BaseFairyGuiView:AddFocusIn",
		"function BaseFairyGuiView:AddFocusOut"
	)

	$autogenScriptText = Get-Content -LiteralPath $autogenScriptPath -Raw
	Assert-TextContains "fairygui_autogen.py" $autogenScriptText @(
		"def append_lua_define_table",
		"RequireControlPath",
		"RequireListItemControlPath",
		"RequireComponentControlPath",
		"ValidateContract"
	)

	$managerText = Get-Content -LiteralPath $managerPath -Raw
	Assert-TextContains "FairyGuiManager.lua" $managerText @(
		"function FairyGuiManager:Open",
		"function FairyGuiManager:OpenMvcUI",
		"function FairyGuiManager:OpenView",
		"function FairyGuiManager:CloseByFilter",
		"function FairyGuiManager:CloseScene",
		"function FairyGuiManager:SetCurrentScene",
		"function FairyGuiManager:PreloadScene",
		"function FairyGuiManager:UnloadPackageGroup",
		"function FairyGuiManager:UnloadPackageTag",
		"function FairyGuiManager:SetCachePolicy",
		"function FairyGuiManager:RecordResourceFallback",
		"function FairyGuiManager:GetResourceFallbacks",
		"function FairyGuiManager:SetTextInputPolicy",
		"function FairyGuiManager:ConfigureTextInput",
		"function FairyGuiManager:GetTextInputDisplayText",
		"function FairyGuiManager:GetTextInputDebugInfo",
		"function FairyGuiManager:RegisterFocusOrder",
		"function FairyGuiManager:FocusNext",
		"function FairyGuiManager:AddFocusIn",
		"function FairyGuiManager:AddFocusOut",
		"function FairyGuiManager:GetGuideMaskRects",
		"function FairyGuiManager:RegisterServiceSkin",
		"function FairyGuiManager:GetServiceSkin",
		"function FairyGuiManager:ResolveServiceSkin",
		"function FairyGuiManager:ShowHoverTip",
		"function FairyGuiManager:HideTip",
		"function FairyGuiManager:ShowDebugPanel",
		"function FairyGuiManager:ShowAiDebugPanel"
	)

	$servicesText = Get-Content -LiteralPath $servicesPath -Raw
	$servicesCoreText = Get-Content -LiteralPath $servicesCorePath -Raw
	$servicesCombinedText = $servicesText + "`n" + $servicesCoreText
	Assert-TextContains "FairyGuiServices/Core" $servicesCombinedText @(
		"DEFAULT_SERVICE_SKINS",
		"function FairyGuiServices:RegisterServiceSkin",
		"function FairyGuiServices:ResolveServiceSkin",
		"serviceSkinName",
		"serviceResource",
		"function FairyGuiServices:ShowHoverTip",
		"function FairyGuiServices:HideTip",
		"popupMenuAnchorRect",
		"popupMenuLayoutRect",
		"tipAnchorRect",
		"tipLayoutRect"
	)

	$layersText = Get-Content -LiteralPath $layersPath -Raw
	Assert-TextContains "FairyGuiLayers.lua" $layersText @(
		"function FairyGuiLayers:GetGuideMaskRects",
		"function FairyGuiLayers:NormalizeGuideMaskRect",
		"objectInfo.guideMaskRects",
		"mergeIntervals"
	)

	$controlsText = Get-Content -LiteralPath $controlsPath -Raw
	Assert-TextContains "FairyGuiControls.lua" $controlsText @(
		"function FairyGuiControls:SetSliderValue",
		"function FairyGuiControls:SetProgressBarValue",
		"function FairyGuiControls:SetComboBoxSelectedIndex",
		"function FairyGuiControls:SetComboBoxValue",
		"function FairyGuiControls:SetTextInputPolicy",
		"function FairyGuiControls:ConfigureTextInput",
		"function FairyGuiControls:GetTextInputDisplayText",
		"function FairyGuiControls:GetTextInputDebugInfo",
		"placeholder",
		"password",
		"maskChar",
		"function FairyGuiControls:RegisterFocusOrder",
		"function FairyGuiControls:FocusNext"
	)

	$eventsText = Get-Content -LiteralPath $eventsPath -Raw
	Assert-TextContains "FairyGuiEvents.lua" $eventsText @(
		"function FairyGuiEvents:AddFocusIn",
		"function FairyGuiEvents:AddFocusOut",
		'"Enter"',
		'"Exit"'
	)

	$listsText = Get-Content -LiteralPath $listsPath -Raw
	Assert-TextContains "FairyGuiLists.lua" $listsText @(
		"function FairyGuiLists:SetVirtualListData",
		"function FairyGuiLists:SetTreeData",
		"function FairyGuiLists:GetListDebugStats",
		"function LIST_ITEM_METHODS:GetChild",
		"function LIST_ITEM_METHODS:BindControls"
	)

	$packageText = Get-Content -LiteralPath $packagePath -Raw
	Assert-TextContains "FairyGuiPackage.lua" $packageText @(
		"function FairyGuiPackage:PreloadScene",
		"function FairyGuiPackage:UnloadPackageGroup",
		"function FairyGuiPackage:UnloadPackageTag",
		"function FairyGuiPackage:RecordResourceFallback"
	)

	$profilerText = Get-Content -LiteralPath $profilerPath -Raw
	Assert-TextContains "FairyGuiProfiler.lua" $profilerText @(
		"stencilBackend",
		"hardwareStencilSupported",
		"cpuClipSourceTriangleCount",
		"stencilClipPolygonCount",
		"function FairyGuiProfiler:BuildDebugPanelLines",
		"function FairyGuiProfiler:BuildAiDebugPanelLines"
	)

	$renderText = Get-Content -LiteralPath $renderPath -Raw
	Assert-TextContains "FairyGuiSystemRender.cpp" $renderText @(
		"RecordCpuClipWork",
		"BuildFrameRenderDetailString",
		"GetStencilBackendString",
		"IsHardwareStencilSupported"
	)
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
		Invoke-Checked "AutoGen BusinessBenchmark check" "python" @(
			"-B",
			"tools\fgui_autogen\fairygui_generate_ui.py",
			"--asset-dir",
			"bin\res\assets\act_38",
			"--package",
			"act_38_test",
			"--component",
			"QingLuanActMain",
			"--ui-name",
			"BusinessBenchmark",
			"--manifest-output",
			"bin\res\assets\fairygui_manifests\business_benchmark.json",
			"--registry-output",
			"bin\res\assets\fairygui_manifests\business_benchmark.registry.lua",
			"--registry-aggregate-output",
			"bin\res\scripts\ui\GeneratedUIRegistry.lua",
			"--layer",
			"Popup",
			"--group",
			"Benchmark",
			"--scene",
			"BusinessBenchmark",
			"--cache",
			"false",
			"--full-screen",
			"--close-on-escape",
			"--check",
			"--strict"
		)
	}

	Test-FairyGuiNativeEntryGuard
	Test-FairyGuiGameManagerLegacyBindingGuard
	Test-FairyGuiRuntimeOwnershipGuard
	Test-FairyGuiPublicHeaderFacadeGuard
	Test-FairyGuiInternalHeaderGuard
	Test-FairyGuiLuaFrameworkGuard
	Test-FairyGuiProductionFeatureGuard

	Write-Host "[FGUI-CHECK] static checks passed."
} finally {
	Pop-Location
}
