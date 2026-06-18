-- KnowledgeSoldierAgent.lua
-- Chapter 7 sample：Lua KnowledgeSource 定期评估，写入 C++ Blackboard，BT 只消费黑板。

require("res.scripts.ai.knowledge.Chapter7Knowledge.lua")
require("res.scripts.ai.behavior.BehaviorTreeLoader.lua")
local Chapter7Conditions = require("res.scripts.ai.behavior.Chapter7Conditions.lua")
local Chapter7BTConfig = require("res.scripts.ai.behavior.config.Chapter7BT.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local _knowledgeByAgentId = {}

function Agent_Initialize(agent)
	if agent == nil then return end

	agent:SetMaxSpeed(SOLDIER_STAND_SPEED)

	local ai = AgentComponents.GetAI(agent)
	if ai == nil then
		print("Error: AIController not available")
		return
	end

	ai:SetDriverByType("bt")
	local driver = ai:GetBehaviorTreeDriver()
	if driver == nil then
		print("Error: BehaviorTreeDriver not available after SetDriverByType(bt)")
		return
	end

	local bb = driver:GetBlackboard()
	bb:SetFloat("maxHealth", AgentComponents.GetMaxHealth(agent))
	bb:SetString("chapter", "chapter7-knowledge")

	local knowledge = Chapter7Knowledge.CreateContext(agent, bb)
	Chapter7Knowledge.InstallDefaultSources(knowledge)
	_knowledgeByAgentId[agent:GetObjId()] = knowledge

	local tree = BehaviorTreeLoader.Build(Chapter7BTConfig, agent, driver, bb, Chapter7Conditions)
	if tree == nil then
		print("Error: failed to build Chapter7 behavior tree from config")
		return
	end
	driver:SetTree(tree)
end

function Agent_Update(agent, deltaTimeInMillis)
	if agent == nil then return end

	local knowledge = _knowledgeByAgentId[agent:GetObjId()]
	if knowledge ~= nil then
		Chapter7Knowledge.Update(knowledge, deltaTimeInMillis)
	end
end
