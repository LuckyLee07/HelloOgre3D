-- Chapter7Conditions.lua
-- 原书 Chapter 7 行为树条件：只消费 KnowledgeSource 写入的 blackboard。

Chapter7Conditions = {}

local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

function Chapter7Conditions.IsNotAlive(agent, bb)
	return agent:GetHealth() <= 0
end

function Chapter7Conditions.IsCriticalHealth(agent, bb)
	return agent:GetHealth() < (bb:GetFloat("maxHealth", 100.0) * 0.2)
end

function Chapter7Conditions.HasEnemy(agent, bb)
	local enemy = bb:GetAgent("enemy")
	return enemy ~= nil and enemy:GetHealth() > 0
end

function Chapter7Conditions.HasNoAmmo(agent, bb)
	return not AgentComponents.HasAmmo(agent)
end

function Chapter7Conditions.CanShootEnemy(agent, bb)
	local enemy = bb:GetAgent("enemy")
	if enemy == nil or enemy:GetHealth() <= 0 then return false end

	local toEnemy = enemy:GetPosition() - agent:GetPosition()
	toEnemy.y = 0
	return toEnemy:length() < 3.0
end

function Chapter7Conditions.HasMovePosition(agent, bb)
	return AgentComponents.HasMovePosition(agent, 1.5)
end

function Chapter7Conditions.RandomChance(agent, bb, cfg)
	return math.random() >= 0.5
end

return Chapter7Conditions
