-- DeathState --

--local m_pAgent = owner;
AgentState_OnEnter = function(agent)
    agent:RequestState(SSTATE_DEAD);
end

AgentState_OnLeave = function(agent)

end

AgentState_OnUpdate = function(agent, deltaTime)
    if agent:IsMoving() then
        agent:SlowMoving(2.0)
    else
        if agent:GetCurStateId() ~= SSTATE_DEAD then
            agent:RequestState(SSTATE_DEAD)
        else
            agent:SetHealth(0.0)
        end
    end

    return "";
end
