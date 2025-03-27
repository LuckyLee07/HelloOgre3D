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
    if currStateName ~= Soldier.SoldierStates.STAND_IDLE_AIM then
        soldierASM:RequestState(Soldier.SoldierStates.STAND_IDLE_AIM)
    end
end

function Agent_ShootState(agent, deltaTime)
    local soldierASM = agent:getBody():GetObjectASM()
    local currStateName = soldierASM:GetCurrStateName()
    if currStateName ~= Soldier.SoldierStates.STAND_FIRE then
        soldierASM:RequestState(Soldier.SoldierStates.STAND_FIRE)
    end
end

function Soldier_Initialize(agent)
    local height = agent:GetHeight()
    local posoffset = Vector3(0, height/2, 10)
    local position = agent:GetPosition()
    local randomVec = Vector3(math.random(-5, 5), 0, math.random(-5, 5))
    agent:setPosition(position + posoffset + randomVec)
end

local soldierState = nil
function Soldier_EventHandle(agent, keycode)
    if keycode == OIS.KC_2 then
        soldierState = "Fire"
    elseif keycode == OIS.KC_3 then
        soldierState = "Idle"
    end
end

function Soldier_Update(agent, deltaTime)
    if soldierState == "Fire" then
        Agent_ShootState(agent)
    elseif soldierState == "Idle" then
        Agent_IdleState(agent)
    end
end