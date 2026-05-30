local BehaviorEventRuntime = {}

local function parseEventName(eventName)
	local text = tostring(eventName or "")
	local queryStart = string.find(text, "?", 1, true)
	if queryStart == nil then
		return text
	end
	return string.sub(text, 1, queryStart - 1)
end

local function getBlackboard(agent, blackboard)
	if blackboard ~= nil then
		return blackboard
	end
	if agent == nil then
		return nil
	end
	local getAi = agent.GetAI or agent.GetAIController
	local ai = getAi ~= nil and getAi(agent) or nil
	return ai ~= nil and ai.GetBlackboard ~= nil and ai:GetBlackboard() or nil
end

local function eventPrefix(eventName)
	return "__bt.event." .. parseEventName(eventName)
end

local function readString(value)
	if value == nil then
		return ""
	end
	return tostring(value)
end

local function setObjectId(blackboard, key, value)
	if key == nil then
		return
	end
	if blackboard.SetObjectId ~= nil then
		blackboard:SetObjectId(key, value)
	end
	blackboard:SetInt(key, value)
end

local function addObjectId(blackboard, key, value)
	if key == nil or value < 0 or blackboard.AddObjectIdToArray == nil then
		return
	end
	blackboard:AddObjectIdToArray(key, value)
end

function BehaviorEventRuntime.Push(agent, eventName, event, options)
	local blackboard = getBlackboard(agent)
	if blackboard == nil then
		return false
	end

	options = options or {}
	event = event or {}
	local prefix = eventPrefix(eventName)
	local countKey = prefix .. ".count"
	local count = blackboard:GetInt(countKey, 0) + 1
	local targetId = tonumber(event.targetId) or -1
	local teamId = tonumber(event.teamId) or -1

	blackboard:SetBool(prefix .. ".pending", true)
	blackboard:SetInt(countKey, count)
	blackboard:SetString(prefix .. ".regionId", readString(event.regionId))
	blackboard:SetString(prefix .. ".scope", readString(event.scope))
	setObjectId(blackboard, prefix .. ".targetId", targetId)
	addObjectId(blackboard, prefix .. ".targetIds", targetId)
	blackboard:SetInt(prefix .. ".teamId", teamId)
	if event.position ~= nil then
		blackboard:SetVec3(prefix .. ".position", event.position)
	end
	if event.target ~= nil then
		blackboard:SetAgent(prefix .. ".target", event.target)
	end
	if options.targetKey ~= nil and event.target ~= nil then
		blackboard:SetAgent(options.targetKey, event.target)
	end
	if options.targetIdKey ~= nil then
		setObjectId(blackboard, options.targetIdKey, targetId)
	end
	if options.targetIdArrayKey ~= nil then
		addObjectId(blackboard, options.targetIdArrayKey, targetId)
	end
	if options.setBoolKey ~= nil then
		blackboard:SetBool(options.setBoolKey, true)
	end
	return true
end

function BehaviorEventRuntime.Test(agent, blackboard, eventName, config)
	config = config or {}
	local bb = getBlackboard(agent, blackboard)
	if bb == nil then
		return false
	end

	local prefix = eventPrefix(eventName or config.event or config.name)
	if not bb:GetBool(prefix .. ".pending", false) then
		return false
	end

	if config.regionId ~= nil and bb:GetString(prefix .. ".regionId") ~= tostring(config.regionId) then
		return false
	end

	if config.targetKey ~= nil then
		local target = bb:GetAgent(prefix .. ".target")
		if target ~= nil then
			bb:SetAgent(config.targetKey, target)
		end
	end

	if config.targetIdKey ~= nil then
		local targetId = bb:GetInt(prefix .. ".targetId", -1)
		if bb.GetObjectId ~= nil then
			targetId = bb:GetObjectId(prefix .. ".targetId", targetId)
		end
		setObjectId(bb, config.targetIdKey, targetId)
	end

	if config.setBoolKey ~= nil then
		bb:SetBool(config.setBoolKey, true)
	end

	if config.consume ~= false then
		bb:SetBool(prefix .. ".pending", false)
	end
	return true
end

function BehaviorEventRuntime.Clear(agent, blackboard, eventName)
	local bb = getBlackboard(agent, blackboard)
	if bb == nil then
		return
	end
	bb:SetBool(eventPrefix(eventName) .. ".pending", false)
end

return BehaviorEventRuntime
