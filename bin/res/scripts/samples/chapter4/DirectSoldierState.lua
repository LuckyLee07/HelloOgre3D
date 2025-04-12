require("res.scripts.samples.chapter4.SoldierAgent.lua")

function Agent_DeathState(agent, deltaTime)
    local soldierASM = agent:getBody():GetObjectASM()
    local currStateName = soldierASM:GetCurrStateName()
    if currStateName ~= Soldier.SoldierStates.STAND_DEAD then
        soldierASM:RequestState(Soldier.SoldierStates.STAND_DEAD)
    end
end

function Agent_FallingState(agent, deltaTime)
    local soldierASM = agent:getBody():GetObjectASM()
    local currStateName = soldierASM:GetCurrStateName()
    if currStateName ~= Soldier.SoldierStates.STAND_IDLE_AIM then
        soldierASM:RequestState(Soldier.SoldierStates.STAND_IDLE_AIM)
    end
end

function Agent_IdleState(agent, deltaTime)
    local soldierASM = agent:getBody():GetObjectASM()
    local currStateName = soldierASM:GetCurrStateName()
    if currStateName ~= Soldier.SoldierStates.STAND_IDLE_AIM then
        soldierASM:RequestState(Soldier.SoldierStates.STAND_IDLE_AIM)
    end
end

function Agent_MovingState(agent, deltaTime)
    local soldierASM = agent:getBody():GetObjectASM()
    local currStateName = soldierASM:GetCurrStateName()
    if currStateName ~= Soldier.SoldierStates.STAND_RUN_FORWARD then
        soldierASM:RequestState(Soldier.SoldierStates.STAND_RUN_FORWARD)
    end
end

function Agent_ShootState(agent, deltaTime)
    local soldierASM = agent:getBody():GetObjectASM()
    local currStateName = soldierASM:GetCurrStateName()
    if currStateName ~= Soldier.SoldierStates.STAND_FIRE then
        soldierASM:RequestState(Soldier.SoldierStates.STAND_FIRE)
    end
end

