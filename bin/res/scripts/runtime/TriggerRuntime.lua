local TriggerVolume = require("res.scripts.runtime.TriggerVolume")
local BehaviorEventRuntime = require("res.scripts.runtime.BehaviorEventRuntime")

local TriggerRuntime = {
	volumes = {},
	listeners = {},
	recentEvents = {},
	eventCounts = {},
	creatureBindings = {},
	maxRecentEvents = 32,
}

local function parseEventName(eventName)
	local text = tostring(eventName or "")
	local queryStart = string.find(text, "?", 1, true)
	local baseName = queryStart == nil and text or string.sub(text, 1, queryStart - 1)
	local params = {}
	if queryStart ~= nil then
		local query = string.sub(text, queryStart + 1)
		for part in string.gmatch(query, "([^&]+)") do
			local equals = string.find(part, "=", 1, true)
			if equals ~= nil then
				params[string.sub(part, 1, equals - 1)] = string.sub(part, equals + 1)
			else
				params[part] = "true"
			end
		end
	end
	return baseName, params
end

local function mergeParams(event, params)
	for key, value in pairs(params or {}) do
		if tonumber(value) ~= nil then
			event[key] = tonumber(value)
		else
			event[key] = value
		end
	end
end

local function matchesParams(event, params)
	for key, expected in pairs(params or {}) do
		if tostring(event[key]) ~= tostring(expected) then
			return false
		end
	end
	return true
end

local function getAgentCount()
	if ObjectManager ~= nil and ObjectManager.getAiAgentCount ~= nil then
		return ObjectManager:getAiAgentCount()
	end
	return 0
end

function TriggerRuntime:Clear()
	self.volumes = {}
	self.listeners = {}
	self.recentEvents = {}
	self.eventCounts = {}
	self.creatureBindings = {}
end

function TriggerRuntime:CreateVolume(config)
	local volume = TriggerVolume.New(config)
	table.insert(self.volumes, volume)
	return volume
end

function TriggerRuntime:RegisterVolumesFromDefs(volumeDefs)
	for _, volumeDef in pairs(volumeDefs or {}) do
		self:CreateVolume(volumeDef)
	end
end

function TriggerRuntime:Subscribe(eventName, callback, filter)
	if callback == nil then
		return nil
	end

	local baseName, params = parseEventName(eventName)
	self.listeners[baseName] = self.listeners[baseName] or {}
	local token = {
		eventName = eventName,
		baseName = baseName,
		params = params,
		callback = callback,
		filter = filter,
	}
	table.insert(self.listeners[baseName], token)
	return token
end

function TriggerRuntime:Emit(eventName, payload)
	local baseName, params = parseEventName(eventName)
	local event = payload or {}
	mergeParams(event, params)
	event.eventName = eventName
	event.baseName = baseName
	event.eventType = event.eventType or baseName

	self.eventCounts[baseName] = (self.eventCounts[baseName] or 0) + 1
	print("[TriggerRuntime] emit", baseName, "region=", tostring(event.regionId), "target=", tostring(event.targetId))
	table.insert(self.recentEvents, {
		eventName = eventName,
		baseName = baseName,
		regionId = event.regionId,
		targetId = event.targetId,
	})
	while #self.recentEvents > self.maxRecentEvents do
		table.remove(self.recentEvents, 1)
	end

	local listeners = self.listeners[baseName] or {}
	for _, listener in ipairs(listeners) do
		if matchesParams(event, listener.params)
			and (listener.filter == nil or listener.filter(event) == true) then
			listener.callback(event)
		end
	end
end

function TriggerRuntime:RegisterCreature(object, def)
	if object == nil or def == nil or def.triggers == nil then
		return
	end

	for _, triggerDef in ipairs(def.triggers) do
		local eventName = triggerDef.event
		if eventName ~= nil then
			local token = self:Subscribe(eventName, function(event)
				if triggerDef.action == "wake_behavior_tree" then
					BehaviorEventRuntime.Push(object, event.baseName, event, triggerDef)
					print("[TriggerRuntime] wake behavior tree", tostring(def.id), tostring(event.baseName), tostring(event.regionId))
				elseif type(triggerDef.action) == "function" then
					triggerDef.action(object, event, triggerDef)
				end
			end, triggerDef.filter)
			table.insert(self.creatureBindings, {
				object = object,
				defId = def.id,
				token = token,
			})
		end
	end
end

function TriggerRuntime:Update(deltaMs)
	local agents = ObjectManager ~= nil and ObjectManager.getAllAgents ~= nil and ObjectManager:getAllAgents() or nil
	if agents == nil then
		return
	end

	local agentCount = getAgentCount()
	for _, volume in ipairs(self.volumes) do
		for index = 0, agentCount - 1 do
			local agent = agents[index]
			if agent ~= nil then
				volume:UpdateObject(agent, self)
			end
		end
		volume:DrawDebug()
	end
end

function TriggerRuntime:BuildDebugSummary()
	local eventParts = {}
	for eventName, count in pairs(self.eventCounts) do
		table.insert(eventParts, eventName .. ":" .. tostring(count))
	end
	table.sort(eventParts)
	return string.format(
		"[TriggerRuntime] volumes=%d listeners=%d bindings=%d events={%s}",
		#self.volumes,
		self:GetListenerCount(),
		#self.creatureBindings,
		table.concat(eventParts, ",")
	)
end

function TriggerRuntime:GetListenerCount()
	local count = 0
	for _, listeners in pairs(self.listeners) do
		count = count + #listeners
	end
	return count
end

_G.TriggerRuntime = TriggerRuntime

return TriggerRuntime
