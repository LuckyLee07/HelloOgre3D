local CreatureDefModuleName = "res.scripts.defs.CreatureDefs"

local CreatureAssembler = {
	defModuleName = CreatureDefModuleName,
}

local function isTable(value)
	return type(value) == "table"
end

local function isArrayTable(value)
	if not isTable(value) then
		return false
	end

	local count = 0
	for key in pairs(value) do
		if type(key) ~= "number" then
			return false
		end
		count = count + 1
	end
	return count > 0
end

local function cloneTable(source, seen)
	if not isTable(source) then
		return source
	end

	seen = seen or {}
	if seen[source] ~= nil then
		return seen[source]
	end

	local target = {}
	seen[source] = target
	for key, value in pairs(source) do
		target[cloneTable(key, seen)] = cloneTable(value, seen)
	end
	return target
end

local function mergeTable(base, patch)
	local result = cloneTable(base or {})
	if not isTable(patch) then
		return result
	end

	for key, value in pairs(patch) do
		if key ~= "inherit" and isTable(value) and isTable(result[key]) and not isArrayTable(value) and not isArrayTable(result[key]) then
			result[key] = mergeTable(result[key], value)
		elseif key ~= "inherit" then
			result[key] = cloneTable(value)
		end
	end
	return result
end

local function requireModule(moduleName)
	local ok, result = pcall(require, moduleName)
	if ok then
		return result
	end
	error("[CreatureAssembler] require failed: " .. tostring(moduleName) .. " " .. tostring(result))
end

local function loadDefModule(moduleName)
	local source = requireModule(moduleName)
	return source
end

local function getCoordinate(value, keyName, index, defaultValue)
	if value == nil then
		return defaultValue or 0
	end
	local named = value[keyName]
	if named ~= nil then
		return named
	end
	local indexed = value[index]
	if indexed ~= nil then
		return indexed
	end
	return defaultValue or 0
end

local function toVector3(value, defaultValue)
	if value == nil then
		return defaultValue
	end
	if type(value) ~= "table" then
		return value
	end
	return Vector3(
		getCoordinate(value, "x", 1, 0),
		getCoordinate(value, "y", 2, 0),
		getCoordinate(value, "z", 3, 0)
	)
end

local function resolveAppearance(value)
	if value == nil then
		return nil
	end
	if Soldier ~= nil and Soldier.AppearanceTypes ~= nil then
		local upperValue = string.upper(tostring(value))
		if Soldier.AppearanceTypes[upperValue] ~= nil then
			return Soldier.AppearanceTypes[upperValue]
		end
	end
	return value
end

local function resolveSoldierMesh(appearance, factory)
	if factory.mesh ~= nil then
		return factory.mesh
	end
	if appearance == "Dark" then
		return "models/futuristic_soldier/futuristic_soldier_dark_anim.mesh"
	end
	return "models/futuristic_soldier/futuristic_soldier_anim.mesh"
end

local function resolveAgentType(value)
	if type(value) == "number" then
		return value
	end
	if value == nil then
		return AGENT_OBJ_SEEKING
	end

	local key = string.lower(tostring(value))
	local names = {
		none = AGENT_OBJ_NONE,
		seeking = AGENT_OBJ_SEEKING,
		follower = AGENT_OBJ_FOLLOWER,
		pathing = AGENT_OBJ_PATHING,
		pursuing = AGENT_OBJ_PURSUING,
	}
	if names[key] ~= nil then
		return names[key]
	end
	if _G[value] ~= nil then
		return _G[value]
	end
	return AGENT_OBJ_SEEKING
end

local function callIfPresent(object, methodName, ...)
	if object == nil then
		return
	end
	local method = object[methodName]
	if method ~= nil then
		method(object, ...)
	end
end

local function createSoldier(def, factory, sandbox)
	if Soldier_InitSoldierAsm == nil or Soldier_InitWeaponAsm == nil then
		requireModule("res.scripts.agent.SoldierAgent.lua")
	end

	local appearance = resolveAppearance(factory.appearance)
	local soldier = sandbox:CreateSoldier(resolveSoldierMesh(appearance, factory), factory.script)
	soldier:SetTeamId(def.teamId or 0)
	Soldier_InitSoldierAsm(soldier)

	if factory.weaponMesh ~= false then
		soldier:initWeapon(factory.weaponMesh or "models/futuristic_soldier/soldier_weapon.mesh")
		Soldier_InitWeaponAsm(soldier)
	end
	return soldier
end

local function createAgent(def, factory, sandbox)
	if factory.script ~= nil then
		return sandbox:CreateAgent(resolveAgentType(factory.agentType or def.agentType), factory.script)
	end
	return sandbox:CreateAgent(resolveAgentType(factory.agentType or def.agentType))
end

local function applyTransform(object, transform)
	if transform == nil then
		return
	end

	local position = toVector3(transform.position)
	if position ~= nil then
		callIfPresent(object, "setPosition", position)
	end

	local rotation = toVector3(transform.rotation)
	if rotation ~= nil then
		callIfPresent(object, "setRotation", rotation)
	end
end

local function applyLocomotion(object, locomotion)
	if locomotion == nil then
		return
	end
	if locomotion.speed ~= nil then
		callIfPresent(object, "SetSpeed", locomotion.speed)
	end

	local target = toVector3(locomotion.target)
	if target ~= nil then
		callIfPresent(object, "SetTarget", target)
	end
	if locomotion.targetRadius ~= nil then
		callIfPresent(object, "SetTargetRadius", locomotion.targetRadius)
	end
end

local function applyAttributes(object, attributes)
	if attributes == nil then
		return
	end
	if attributes.maxHealth ~= nil then
		callIfPresent(object, "SetMaxHealth", attributes.maxHealth)
	end
	if attributes.health ~= nil then
		callIfPresent(object, "SetHealth", attributes.health)
	end
end

local function applyWeapon(object, weapon)
	if weapon == nil then
		return
	end
	if weapon.maxAmmo ~= nil then
		callIfPresent(object, "SetMaxAmmo", weapon.maxAmmo)
	end
	if weapon.ammo ~= nil then
		callIfPresent(object, "SetAmmo", weapon.ammo)
	end
end

local function setBlackboardValue(blackboard, key, value)
	if type(value) == "boolean" then
		blackboard:SetBool(key, value)
	elseif type(value) == "number" then
		blackboard:SetFloat(key, value)
	elseif type(value) == "string" then
		blackboard:SetString(key, value)
	elseif type(value) == "table" then
		blackboard:SetString(key, tostring(value.value or value.id or ""))
	end
end

local function applyBlackboard(object, def)
	if object == nil then
		return
	end

	local getAi = object.GetAI or object.GetAIController
	local ai = getAi ~= nil and getAi(object) or nil
	local blackboard = ai ~= nil and ai.GetBlackboard ~= nil and ai:GetBlackboard() or nil
	if blackboard == nil then
		return
	end

	blackboard:SetString("__creature.defId", tostring(def.id or ""))
	if def.displayName ~= nil then
		blackboard:SetString("__creature.displayName", tostring(def.displayName))
	end
	if def.behaviorTree ~= nil and def.behaviorTree.script ~= nil then
		blackboard:SetString("__bt.config", tostring(def.behaviorTree.script))
	end

	if object.GetMaxHealth ~= nil then
		blackboard:SetFloat("maxHealth", object:GetMaxHealth())
	elseif def.attributes ~= nil and def.attributes.maxHealth ~= nil then
		blackboard:SetFloat("maxHealth", def.attributes.maxHealth)
	end

	if def.ai ~= nil and def.ai.blackboard ~= nil then
		for key, value in pairs(def.ai.blackboard) do
			setBlackboardValue(blackboard, key, value)
		end
	end
end

function CreatureAssembler:ReloadDefs()
	package.loaded[self.defModuleName] = nil
	self.defSource = loadDefModule(self.defModuleName)
	self.defs = self.defSource.defs or self.defSource
	self.resolvedDefs = {}
end

function CreatureAssembler:GetRawDef(defId)
	if self.defs == nil then
		self:ReloadDefs()
	end
	return self.defs[defId]
end

function CreatureAssembler:GetDef(defId, resolving)
	if self.resolvedDefs ~= nil and self.resolvedDefs[defId] ~= nil then
		return cloneTable(self.resolvedDefs[defId])
	end

	local rawDef = self:GetRawDef(defId)
	if rawDef == nil then
		error("[CreatureAssembler] unknown creature def: " .. tostring(defId))
	end

	resolving = resolving or {}
	if resolving[defId] == true then
		error("[CreatureAssembler] circular inherit chain at: " .. tostring(defId))
	end
	resolving[defId] = true

	local resolved = cloneTable(rawDef)
	if rawDef.inherit ~= nil then
		resolved = mergeTable(self:GetDef(rawDef.inherit, resolving), rawDef)
	end
	resolved.id = resolved.id or defId

	self.resolvedDefs = self.resolvedDefs or {}
	self.resolvedDefs[defId] = cloneTable(resolved)
	resolving[defId] = nil
	return cloneTable(resolved)
end

function CreatureAssembler:GetTriggerVolumeDefs()
	if self.defSource == nil then
		self:ReloadDefs()
	end
	return cloneTable(self.defSource.triggerVolumes or {})
end

function CreatureAssembler:GetPendingCreateDef()
	return self.pendingCreateDef
end

function CreatureAssembler:CreateCreature(defId, overrides, sandbox)
	sandbox = sandbox or Sandbox
	if sandbox == nil then
		error("[CreatureAssembler] Sandbox is nil")
	end

	local def = mergeTable(self:GetDef(defId), overrides)
	local factory = def.factory or {}
	local factoryType = factory.type or def.kind
	local object = nil

	self.pendingCreateDef = def
	local ok, result = pcall(function()
		if factoryType == "soldier" then
			return createSoldier(def, factory, sandbox)
		elseif factoryType == "agent" then
			return createAgent(def, factory, sandbox)
		end
		error("[CreatureAssembler] unsupported factory type: " .. tostring(factoryType))
	end)
	self.pendingCreateDef = nil
	if not ok then
		error(result)
	end
	object = result

	if object == nil then
		error("[CreatureAssembler] failed to create creature: " .. tostring(defId))
	end

	if def.teamId ~= nil then
		callIfPresent(object, "SetTeamId", def.teamId)
	end
	applyTransform(object, def.transform)
	applyLocomotion(object, def.locomotion)
	applyAttributes(object, def.attributes)
	applyWeapon(object, def.weapon)
	applyBlackboard(object, def)

	self.instances = self.instances or setmetatable({}, { __mode = "k" })
	self.instances[object] = {
		defId = def.id or defId,
		def = cloneTable(def),
	}
	if TriggerRuntime ~= nil and TriggerRuntime.RegisterCreature ~= nil then
		TriggerRuntime:RegisterCreature(object, def)
	end
	return object, def
end

function CreatureAssembler:CreateCreatures(entries, sandbox)
	local objects = {}
	for index, entry in ipairs(entries or {}) do
		local defId = entry
		local overrides = nil
		if type(entry) == "table" then
			defId = entry.defId or entry.id or entry[1]
			overrides = entry.overrides
			if overrides == nil then
				overrides = cloneTable(entry)
				overrides.defId = nil
				overrides.id = nil
				overrides[1] = nil
			end
		end
		objects[index] = self:CreateCreature(defId, overrides, sandbox)
	end
	return objects
end

function CreatureAssembler:BuildDebugSummary()
	local defCount = 0
	if self.defs ~= nil then
		for _ in pairs(self.defs) do
			defCount = defCount + 1
		end
	end

	local instanceCount = 0
	if self.instances ~= nil then
		for _ in pairs(self.instances) do
			instanceCount = instanceCount + 1
		end
	end

	return string.format("[CreatureAssembler] defs=%d instances=%d", defCount, instanceCount)
end

function CreatureAssembler:GetInstanceData(object)
	if self.instances == nil then
		return nil
	end
	return self.instances[object]
end

CreatureAssembler:ReloadDefs()
_G.CreatureAssembler = CreatureAssembler

return CreatureAssembler
