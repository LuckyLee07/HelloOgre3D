local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")

local _elapsedMsByAgent = {}
local _requestedMoveByAgent = {}
local _requestedIdleByAgent = {}

local function configureBodyAsm(agent)
	if agent == nil then
		return false, "agentMissing"
	end

	local bodyAsm = AgentComponents.GetBodyAsm(agent)
	if bodyAsm == nil then
		return false, "bodyAsmMissing"
	end

	local idle = AgentComponents.GetBodyAnimation(agent, "stand_idle_aim")
	local move = AgentComponents.GetBodyAnimation(agent, "stand_run_forward_aim")
	if idle == nil or move == nil then
		return false, "animationMissing"
	end

	bodyAsm:AddState("probe_idle", idle, true)
	bodyAsm:AddState("probe_move", move, true)
	bodyAsm:AddTransition("probe_idle", "probe_move", 0.0, 0.2, 0.0)
	bodyAsm:AddTransition("probe_move", "probe_idle", 0.0, 0.2, 0.0)
	bodyAsm:RequestState("probe_idle")
	return true, "ok"
end

function Agent_Initialize(agent)
	local ok, reason = configureBodyAsm(agent)
	if agent ~= nil then
		AgentComponents.SetMaxSpeed(agent, 0)
	end
	print("[NonSoldierAnimProbeAgent] initialize", "ok=" .. tostring(ok), "reason=" .. tostring(reason))
end

function Agent_Update(agent, deltaTimeInMillis)
	if agent == nil then
		return
	end

	local bodyAsm = AgentComponents.GetBodyAsm(agent)
	if bodyAsm == nil then
		return
	end

	local elapsedMs = (_elapsedMsByAgent[agent] or 0) + (deltaTimeInMillis or 0)
	_elapsedMsByAgent[agent] = elapsedMs

	if elapsedMs >= 500 and not _requestedMoveByAgent[agent] then
		bodyAsm:RequestState("probe_move")
		_requestedMoveByAgent[agent] = true
		print("[NonSoldierAnimProbeAgent] request", "state=probe_move")
	elseif elapsedMs >= 1500 and not _requestedIdleByAgent[agent] then
		bodyAsm:RequestState("probe_idle")
		_requestedIdleByAgent[agent] = true
		print("[NonSoldierAnimProbeAgent] request", "state=probe_idle")
	end
end

function Agent_EventHandle(agent, keycode)
end
