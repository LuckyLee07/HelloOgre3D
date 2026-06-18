-- PerceptionPressureAgent.lua
-- Minimal AI script for Sandbox16. It keeps soldiers stationary and only drives
-- the C++ VisionSensor path through AIController::TickAI.

local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

function Agent_Initialize(agent)
	if agent == nil then return end

	agent:SetMaxSpeed(0)
	agent:SetVelocity(Vector3(0, 0, 0))

	local ai = AgentComponents.GetAI(agent)
	local bb = ai ~= nil and ai:GetBlackboard() or nil
	if bb == nil then
		print("Error: AIController blackboard not available")
		return
	end

	bb:SetFloat("maxHealth", AgentComponents.GetMaxHealth(agent))
	bb:SetFloat("perception.visionRange", 18.0)
	bb:SetFloat("perception.fieldOfViewDegrees", 360.0)
	bb:SetInt("perception.visionIntervalMs", 1)
	bb:SetBool("perception.requirePath", false)
	bb:SetString("debug.demo", "ai_perception_pressure")

	local sampleName = _G.HELLO_SANDBOX_SAMPLE_NAME or "Sandbox16"
	if ConfigManager ~= nil and ConfigManager.ApplyAiBlackboardDefaults ~= nil then
		ConfigManager:ApplyAiBlackboardDefaults(bb, sampleName)
	end
end

function Agent_Update(agent, deltaTimeInMillis)
end

function Agent_EventHandle(agent, keycode)
end
