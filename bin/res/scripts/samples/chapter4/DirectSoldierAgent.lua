require("res.scripts.samples.chapter4.SoldierAgent.lua")

function Agent_Initialize(agent)
    local _soldier = Soldier_CreateSoldier(agent)
    local _weapon = Soldier_CreateWeapon(agent)

    Soldier_AttachWeapon(_soldier, _weapon)
end

function Agent_EventHandle(agent)
end

function Agent_Update(agent, deltaTime)
    
end