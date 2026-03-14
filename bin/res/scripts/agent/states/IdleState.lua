-- IdleState --

--local m_pAgent = owner;
AgentState_OnEnter = function(agent)
    print("Info==========>>>IdleOnEnter")
    agent:RequestState(SSTATE_IDLE_AIM);
end

AgentState_OnLeave = function(agent)
    print("Info==========>>>IdleOnEnter")
end

AgentState_OnUpdate = function(agent, deltaTime)
    if agent:IsMoving() then
        agent:SlowMoving(2.0)
    end
    
    local pInput = agent:GetInput();
    if IsNumKeyDown(pInput, 2) then
        return "toShoot";
    elseif IsNumKeyDown(pInput, 3) then
        return "toMove";
    elseif IsNumKeyDown(pInput, 4) then
        return "toDeath";
    end

    return "";
end
