require("res.scripts.agent.SoldierAgent.lua")
local Access = require("res.scripts.agent.AgentComponentAccess.lua")
local actor, anim, asm, weapon
local time, shots, failures, pauseTicks = 0, 0, 0, 0
local sent = {}
local mesh = 'models/futuristic_soldier/futuristic_soldier_dark_anim.mesh'
local function check(name, value, detail)
    if not value then failures = failures + 1 end
    print(string.format('[AnimCheck] t=%d case=%s result=%s detail=%s', time, name, value and 'PASS' or 'FAIL', tostring(detail or '')))
end
local function at(ms, key, callback)
    if time >= ms and not sent[key] then sent[key] = true; callback() end
end
local function snapshot(label)
    print(string.format('[AnimCase] t=%d label=%s current=%s next=%s clip=%.5f shots=%d ammo=%d', time, label, asm:GetCurrStateName(), asm:GetNextStateName(), asm:GetCurrStateTime(), shots, weapon:GetAmmo()))
end
local function rebuild()
    actor:initBody(mesh)
    Soldier_InitSoldierAsm(actor)
    actor:initWeapon('models/futuristic_soldier/soldier_weapon.mesh')
    Soldier_InitWeaponAsm(actor)
    anim, asm, weapon = Access.GetAnim(actor), Access.GetBodyAsm(actor), actor:GetWeaponComponent()
end
function Agent_Initialize(agent) end
function Agent_Update(agent, dt) end
function Agent_EventHandle(agent, key) end
function EventHandle_Keyboard(key, pressed) end
function EventHandle_Mouse(ctype, x, y, button) return false end
function EventHandle_WindowResized(w, h) end
function Sandbox_Initialize()
    SandboxAgentConfig:SetUseCppFsmFlag(true)
    SandboxScene:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0,180,0))
    SandboxScene:SetAmbientLight(Vector3(.6))
    local light = SandboxScene:CreateDirectionalLight(Vector3(1,-1,1))
    light:setDiffuseColour(ColourValue(1.8,1.4,.9))
    local plane = SandboxObjects:CreatePlane(30,30)
    plane:setPosition(Vector3(0,0,0)); SandboxScene:setMaterial(plane,'Ground2')
    actor = SandboxObjects:CreateSoldier(mesh)
    actor:GetAIComponent():SetDriverByType('dt') -- Empty tree: deterministic intent sequence.
    Soldier_InitSoldierAsm(actor)
    actor:initWeapon('models/futuristic_soldier/soldier_weapon.mesh'); Soldier_InitWeaponAsm(actor)
    actor:setPosition(Vector3(0, actor:GetLocomotionComponent():GetHeight() / 2, 0))
    actor:SetForward(Vector3(0,0,1))
    anim, asm, weapon = Access.GetAnim(actor), Access.GetBodyAsm(actor), actor:GetWeaponComponent()
    weapon:SetAmmo(10)
    SandboxCamera:GetCamera():setPosition(Vector3(-2,1.2,-3.5))
    SandboxCamera:GetCamera():setOrientation(GetForward(Vector3(-2,0,-3.5)))
    print(string.format('[AnimProbe] actor=%d simHz=30', actor:GetObjId()))
end
function Sandbox_Update(dt)
    if GameManager:IsSimulationPaused() then
        pauseTicks = pauseTicks + 1
        if pauseTicks == 6 then
            check('pause_no_shot', shots == 4, shots)
            GameManager:SetSimulationPaused(false)
        end
        return
    end
    time = time + dt
    if actor and anim:ConsumeShootExecution() then
        shots = shots + 1
        check('notify_single_consumption_' .. shots, not anim:ConsumeShootExecution())
        weapon:ConsumeAmmo(1)
        snapshot('shot')
    end
    if actor then
        if time >= 3465 and time < 4257 then actor:SetVelocity(Vector3(1.6,actor:GetVelocity().y,0)) end
        if time >= 4257 and time < 4752 then actor:SetVelocity(Vector3(-1.6,actor:GetVelocity().y,0)) end
    end
    -- Pin the idle phase after warm-up; startup's partial first tick is not a test variable.
    at(330, 'reload', function() Access.GetBodyAnimation(actor, 'stand_idle_aim'):Init(0); anim:EnterReloadIntent(); snapshot('reload') end)
    at(363, 'cancel', function() anim:EnterIdleIntent(); snapshot('cancel') end)
    at(660, 'cancelled', function() check('cancel_reload', asm:GetCurrStateName() == 'idle_aim' and not asm:IsTransitioning(), asm:GetCurrStateName()) end)
    at(990, 'fire', function() anim:EnterShootIntent(); snapshot('fire') end)
    at(1353, 'fired', function() check('fire_after_cancel', shots == 1, shots) end)
    at(1485, 'reload2', function() anim:EnterReloadIntent() end)
    at(1518, 'replace', function() anim:EnterShootIntent(); snapshot('replace_reload_with_fire') end)
    at(1914, 'replaced', function() check('replace_reload', shots == 2, shots) end)
    at(1980, 'cancelShot', function() anim:EnterShootIntent(); anim:EnterIdleIntent() end)
    at(2310, 'cancelShotDone', function() check('cancel_before_notify', shots == 2, shots) end)
    at(2376, 'reload3', function() anim:EnterReloadIntent() end)
    at(2409, 'death', function() anim:EnterDeathIntent() end)
    at(2442, 'deathLocked', function() anim:EnterIdleIntent(); check('death_rejects_shot', not anim:EnterShootIntent()) end)
    at(2739, 'deathDone', function() check('death_terminal', asm:GetCurrStateName() == 'dead' and shots == 2, asm:GetCurrStateName()) end)
    at(2805, 'rebuild', rebuild)
    at(2970, 'rebuiltShot', function() anim:EnterShootIntent(); snapshot('rebuilt_fire') end)
    at(3400, 'rebuiltDone', function() check('body_weapon_rebuild', shots == 3, shots); local foot=actor:GetBonePosition('b_LeftFoot'); check('rebuild_keeps_ground_offset', foot.y > -.1 and foot.y < .3, foot.y) end)
    at(3465, 'move', function() anim:EnterMoveIntent() end)
    at(3729, 'movingShot', function() anim:EnterShootIntent() end)
    at(4224, 'movingShotDone', function() check('moving_shot', shots == 4, shots); anim:EnterMoveIntent() end)
    at(4752, 'stop', function() actor:SetVelocity(Vector3(0,actor:GetVelocity().y,0)); anim:EnterIdleIntent() end)
    at(4851, 'crouch', function() actor:changeStanceType(SOLDIER_CROUCH) end)
    at(5214, 'crouched', function() check('crouch_transition', actor:getStanceType() == SOLDIER_CROUCH, actor:getStanceType()); actor:changeStanceType(SOLDIER_STAND) end)
    at(5610, 'stood', function() check('stand_transition', actor:getStanceType() == SOLDIER_STAND, actor:getStanceType()) end)
    at(5709, 'elevated', function()
        actor:SetForward(Vector3(0,0,1))
        local ready = actor:GetLocomotionComponent():FaceDirection(Vector3(0,30,-10), 33)
        local f = actor:GetForward(); local yaw = math.abs(math.atan2(f.x,f.z) * 180 / math.pi)
        check('elevated_target_upright', math.abs(f.y) < 0.00001, f.y)
        check('turn_speed_limit', yaw <= 17.821 and yaw > 17.8 and not ready, yaw)
        actor:SetForward(Vector3(math.sin(math.rad(179)),0,math.cos(math.rad(179))))
        actor:GetLocomotionComponent():FaceDirection(Vector3(math.sin(math.rad(-179)),0,math.cos(math.rad(-179))), 33)
        f=actor:GetForward(); yaw=math.atan2(f.x,f.z) * 180/math.pi
        check('shortest_angle_wrap', yaw > 179 or yaw < -179, yaw)
    end)
    at(5907, 'uprightPhysics', function() local f=actor:GetForward(); check('physics_stays_upright', math.abs(f.y)<0.00001,f.y) end)
    at(6501, 'pause', function() GameManager:SetSimulationPaused(true) end)
    at(6765, 'ammo', function() check('ammo_matches_shots', weapon:GetAmmo() == 10 - shots, weapon:GetAmmo()); snapshot('done') end)
    at(6930, 'destroy', function() SandboxObjects:RequestDestroyAgent(actor:GetObjId()); actor=nil; anim=nil; asm=nil; weapon=nil end)
    at(7161, 'quit', function() print(string.format('[AnimProbe] result=%s failures=%d shots=%d', failures==0 and 'PASS' or 'FAIL',failures,shots)); GameManager:RequestQuit() end)
end
