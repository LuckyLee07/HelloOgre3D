-- Chapter7Knowledge.lua
-- 把原书 Chapter 7 的 KnowledgeSource 思想接到当前 C++ Blackboard。

local KnowledgeSource = require("res.scripts.ai.knowledge.KnowledgeSource.lua")

local Chapter7Knowledge = {}

local function _DistanceSq(left, right)
	local delta = left - right
	delta.y = 0
	return delta:squaredLength()
end

local function _HasPath(fromPos, toPos)
	local path = std.vector_Ogre__Vector3_()
	return SandboxNav:FindPath("default", fromPos, toPos, path) and path:size() > 0
end

local function _EvaluateNearestEnemy(context)
	local owner = context.owner
	if owner == nil or owner:GetHealth() <= 0 then
		return { evaluation = nil, confidence = 0.0 }
	end

	local agents = ObjectManager:getAllAgents()
	local ownerPos = owner:GetPosition()
	local ownerId = owner:GetObjId()
	local bestEnemy = nil
	local bestDistanceSq = nil

	for i = 0, agents:size() - 1 do
		local candidate = agents[i]
		if candidate ~= nil
			and candidate:GetObjId() ~= ownerId
			and candidate:GetHealth() > 0 then
			local candidatePos = candidate:GetPosition()
			local distanceSq = _DistanceSq(ownerPos, candidatePos)
			if (bestDistanceSq == nil or distanceSq < bestDistanceSq)
				and _HasPath(ownerPos, candidatePos) then
				bestEnemy = candidate
				bestDistanceSq = distanceSq
			end
		end
	end

	if bestEnemy == nil then
		return { evaluation = nil, confidence = 0.0 }
	end

	local distance = math.sqrt(bestDistanceSq or 0)
	return {
		evaluation = {
			agent = bestEnemy,
			id = bestEnemy:GetObjId(),
			position = bestEnemy:GetPosition(),
			distance = distance,
		},
		confidence = 1.0,
	}
end

local function _EvaluateBestFleePosition(context)
	local owner = context.owner
	if owner == nil or owner:GetHealth() <= 0 then
		return { evaluation = nil, confidence = 0.0 }
	end

	local bb = context.blackboard
	local enemy = bb ~= nil and bb:GetAgent("enemy") or nil
	if enemy == nil or enemy:GetHealth() <= 0 then
		local randomPoint = SandboxNav:RandomPoint("default")
		return { evaluation = randomPoint, confidence = 0.25 }
	end

	local enemyPos = enemy:GetPosition()
	local bestPoint = nil
	local bestScore = nil
	for i = 1, 32 do
		local point = SandboxNav:RandomPoint("default")
		local score = _DistanceSq(point, enemyPos)
		if bestScore == nil or score > bestScore then
			bestPoint = point
			bestScore = score
		end
	end

	if bestPoint == nil then
		bestPoint = SandboxNav:RandomPoint("default")
		bestScore = _DistanceSq(bestPoint, enemyPos)
	end

	return { evaluation = bestPoint, confidence = 1.0 }
end

local function _AddAttribute(context, attribute)
	if context.sources[attribute] == nil then
		context.sources[attribute] = {}
		table.insert(context.orderedAttributes, attribute)
	end
end

local function _SelectBestResult(context, attribute)
	local best = nil
	local sources = context.sources[attribute] or {}
	for _, source in ipairs(sources) do
		local result = source:Evaluate(context, context.timeMs)
		if best == nil
			or result.confidence > best.confidence
			or (result.confidence == best.confidence and result.evaluateCount > best.evaluateCount) then
			best = result
		end
	end
	return best
end

local function _WriteEnemy(context, result)
	local bb = context.blackboard
	if bb == nil then return end

	local value = result ~= nil and result.evaluation or nil
	if value == nil or value.agent == nil or value.agent:GetHealth() <= 0 then
		bb:Remove("enemy")
		bb:SetObjectId("knowledge.enemyId", -1)
		bb:SetFloat("knowledge.enemyDistance", -1.0)
		bb:SetFloat("knowledge.enemyConfidence", 0.0)
		bb:SetString("knowledge.enemySource", result ~= nil and result.source or "none")
		return
	end

	bb:SetAgent("enemy", value.agent)
	bb:SetObjectId("knowledge.enemyId", value.id or value.agent:GetObjId())
	bb:SetVec3("knowledge.enemyPosition", value.position or value.agent:GetPosition())
	bb:SetFloat("knowledge.enemyDistance", value.distance or 0.0)
	bb:SetFloat("knowledge.enemyConfidence", result.confidence or 0.0)
	bb:SetString("knowledge.enemySource", result.source or "unknown")
	bb:SetInt("knowledge.enemyEvalCount", result.evaluateCount or 0)
end

local function _WriteFleePosition(context, result)
	local bb = context.blackboard
	if bb == nil then return end

	local value = result ~= nil and result.evaluation or nil
	if value == nil then
		bb:Remove("knowledge.bestFleePosition")
		bb:SetFloat("knowledge.bestFleeConfidence", 0.0)
		bb:SetString("knowledge.bestFleeSource", result ~= nil and result.source or "none")
		return
	end

	bb:SetVec3("knowledge.bestFleePosition", value)
	bb:SetFloat("knowledge.bestFleeConfidence", result.confidence or 0.0)
	bb:SetString("knowledge.bestFleeSource", result.source or "unknown")
	bb:SetInt("knowledge.fleeEvalCount", result.evaluateCount or 0)
end

function Chapter7Knowledge.CreateContext(owner, blackboard)
	return {
		owner = owner,
		blackboard = blackboard,
		timeMs = 0,
		sources = {},
		orderedAttributes = {},
	}
end

function Chapter7Knowledge.AddSource(context, attribute, source)
	if context == nil or attribute == nil or source == nil then return end
	_AddAttribute(context, attribute)
	table.insert(context.sources[attribute], source)
end

function Chapter7Knowledge.InstallDefaultSources(context)
	Chapter7Knowledge.AddSource(context, "enemy", KnowledgeSource.New(
		"NearestPathableEnemy",
		_EvaluateNearestEnemy,
		250))
	Chapter7Knowledge.AddSource(context, "bestFleePosition", KnowledgeSource.New(
		"FarthestReachableFleePoint",
		_EvaluateBestFleePosition,
		1000))
end

function Chapter7Knowledge.Update(context, deltaMs)
	if context == nil then return end
	context.timeMs = context.timeMs + (deltaMs or 0)

	for _, attribute in ipairs(context.orderedAttributes) do
		local result = _SelectBestResult(context, attribute)
		if attribute == "enemy" then
			_WriteEnemy(context, result)
		elseif attribute == "bestFleePosition" then
			_WriteFleePosition(context, result)
		end
	end

	local bb = context.blackboard
	if bb ~= nil then
		bb:SetInt("knowledge.timeMs", context.timeMs)
		bb:SetString("knowledge.summary", string.format(
			"enemy=%d dist=%.1f fleeEval=%d",
			bb:GetObjectId("knowledge.enemyId", -1),
			bb:GetFloat("knowledge.enemyDistance", -1.0),
			bb:GetInt("knowledge.fleeEvalCount", 0)))
	end
end

_G.Chapter7Knowledge = Chapter7Knowledge
return Chapter7Knowledge
