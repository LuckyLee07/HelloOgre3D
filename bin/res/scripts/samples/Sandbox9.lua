-- Sandbox9.lua
-- 第一个数据驱动垂直切片：
--   CreatureDef -> CreatureAssembler -> TriggerVolume -> TriggerRuntime
--   -> BehaviorEventRuntime -> BT Event node -> guard attack.

local textSize = {w = 360, h = 250}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
		"[Sandbox9 - Data Slice]" .. GUI.MarkupNewline ..
		"Data-defined player enters trigger region" .. GUI.MarkupNewline ..
		"Guard wakes through BT event node" .. GUI.MarkupNewline ..
		GUI.MarkupNewline ..
		"F1: reset camera" .. GUI.MarkupNewline ..
		"F8: hot reload Def / Trigger / BT" .. GUI.MarkupNewline

local _sliceObjects = {}

local function printBtTrace(object)
	if object == nil then return end
	local getAi = object.GetAI or object.GetAIController
	local ai = getAi ~= nil and getAi(object) or nil
	local driver = ai ~= nil and ai.GetBehaviorTreeDriver ~= nil and ai:GetBehaviorTreeDriver() or nil
	if driver ~= nil and driver.GetLastDebugTrace ~= nil then
		print("[Sandbox9] bt trace:", driver:GetLastDebugTrace())
	end
end

local function scheduleSmokeDiagnostics()
	if _G.HELLO_SANDBOX_SMOKE_MODE ~= true or threadpool == nil then
		return
	end

	threadpool:delay(6, function()
		print(TriggerRuntime:BuildDebugSummary())
		if ObjectManager ~= nil and ObjectManager.buildAiDebugSummary ~= nil then
			print(ObjectManager:buildAiDebugSummary(4))
		end
		for _, object in ipairs(_sliceObjects) do
			printBtTrace(object)
		end
	end)
end

local function reloadRuntimeModules()
	local modules = {
		"res.scripts.defs.CreatureDefs",
		"res.scripts.runtime.BehaviorEventRuntime",
		"res.scripts.runtime.TriggerVolume",
		"res.scripts.runtime.TriggerRuntime",
		"res.scripts.runtime.CreatureAssembler",
		"res.scripts.ai.behavior.config.SliceGuardBT",
	}
	for _, moduleName in ipairs(modules) do
		package.loaded[moduleName] = nil
	end
	_G.CreatureAssembler = require("res.scripts.runtime.CreatureAssembler")
	_G.TriggerRuntime = require("res.scripts.runtime.TriggerRuntime")
end

local function spawnSlice()
	_sliceObjects = {}
	TriggerRuntime:Clear()
	CreatureAssembler:ReloadDefs()
	TriggerRuntime:RegisterVolumesFromDefs(CreatureAssembler:GetTriggerVolumeDefs())

	local player, playerDef = CreatureAssembler:CreateCreature("slice_player")
	local guard, guardDef = CreatureAssembler:CreateCreature("slice_guard")
	table.insert(_sliceObjects, player)
	table.insert(_sliceObjects, guard)

	print("[Sandbox9] spawned data slice:", playerDef.id, guardDef.id)
	print(CreatureAssembler:BuildDebugSummary())
	print(TriggerRuntime:BuildDebugSummary())
	scheduleSmokeDiagnostics()
end

function Sandbox9_ReloadSlice()
	print("[Sandbox9] hot reload begin")
	if ObjectManager ~= nil then
		ObjectManager:clearAllObjects(MGR_OBJ_AGENT, true)
	end
	reloadRuntimeModules()
	spawnSlice()
	print("[Sandbox9] hot reload complete")
end

function EventHandle_Keyboard(keycode, pressed)
	GUI_HandleKeyEvent(keycode, pressed)

	if not pressed then return end
	if keycode == OIS.KC_F1 then
		local camera = Sandbox:GetCamera()
		camera:setPosition(Vector3(-18, 18, -22))
		camera:setOrientation(Quaternion(-140, -35, -150))
	elseif keycode == OIS.KC_F8 then
		Sandbox9_ReloadSlice()
	end
end

function EventHandle_Mouse(ctype)
end

function EventHandle_WindowResized(width, height)
	GUI_WindowResized(width, height)
end

function Sandbox_Initialize()
	GUI_CreateCameraAndProfileInfo()
	GUI_CreateSandboxText(infoText, textSize)

	Sandbox:SetUseCppFsmFlag(true)

	local camera = Sandbox:GetCamera()
	camera:setPosition(Vector3(-18, 18, -22))
	camera:setOrientation(Quaternion(-140, -35, -150))

	Sandbox:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0))

	local plane = Sandbox:CreatePlane(120, 120)
	plane:setPosition(Vector3(0, -10, 0))
	plane:setMaterial("Ground2")

	Sandbox:SetAmbientLight(Vector3(0.35))
	local directLight = Sandbox:CreateDirectionalLight(Vector3(1, -1, 1))
	directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9))
	directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9))

	Sandbox:UpdateSceneGraph()

	local navMeshConfig = rcConfig()
	Sandbox:DefaultConfig(navMeshConfig)
	Sandbox:ApplySettingConfig(navMeshConfig, 0.0, 0.4, 0.2)
	navMeshConfig.minRegionArea = math.pow(80, 2)
	navMeshConfig.walkableSlopeAngle = 45

	local navMesh = Sandbox:CreateNavigationMesh(navMeshConfig, "default")
	if navMesh ~= nil then navMesh:SetDebugVisible(true) end

	spawnSlice()
end

function Sandbox_Update(deltaTimeInMillis)
	GUI_UpdateCameraInfo()
	GUI_UpdateProfileInfo()
	TriggerRuntime:Update(deltaTimeInMillis)
end
