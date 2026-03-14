-- ShootState --

--local m_pAgent = owner;
AgentState_OnEnter = function(agent)
    agent:RequestState(SSTATE_FIRE);
end

AgentState_OnLeave = function(agent)

end

AgentState_OnUpdate = function(agent, deltaTime)
    if agent:IsMoving() then
        agent:SlowMoving(2.0)
    end

    if not agent:IsAnimReadyForMove() then
        if not agent:HasNextAnim() then
            agent:RequestState(SSTATE_FIRE)
        end
        return "";
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
