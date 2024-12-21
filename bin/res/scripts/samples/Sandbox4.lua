
require("res.scripts.samples.chapter4.DirectSoldierAgent.lua")

local textSize = {w = 300, h = 300}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "W/A/S/D: to move" .. GUI.MarkupNewline ..
        "Hold Shift: to accelerate movement" .. GUI.MarkupNewline ..
        "Hold RMB: to look" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "F1: to reset the camera" .. GUI.MarkupNewline ..
        "F2: toggle the menu" .. GUI.MarkupNewline ..
        "F3: toggle skeleton" .. GUI.MarkupNewline ..
        "F5: toggle performance information" .. GUI.MarkupNewline ..
        "F6: toggle camera information" .. GUI.MarkupNewline ..
        "F7: toggle physics debug" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Switch Agents to:" .. GUI.MarkupNewline ..
        "  Num 1: idle state" .. GUI.MarkupNewline ..
        "  Num 2: shooting state" .. GUI.MarkupNewline ..
        "  Num 3: moving state" .. GUI.MarkupNewline ..
        "  Num 4: death state" .. GUI.MarkupNewline ..
        "  Num 5: stand/crouch stance" .. GUI.MarkupNewline;

local weaponState = "sniper"

function EventHandle_Keyboard(keycode, pressed)
    GUI_HandleKeyEvent(keycode, pressed)

    if not pressed then return end
    if (keycode == OIS.KC_F1) then
        local camera = Sandbox:GetCamera();
        camera:setPosition(Vector3(-17, 10, 0));
        camera:setOrientation(GetForward(Vector3(-145, -50, -150)));
    elseif (keycode == OIS.KC_F2) then
        local isShow = TextComponent:isVisible()
        TextComponent:setVisible(not isShow)
    elseif (keycode == OIS.KC_F3) then
        
    elseif (keycode == OIS.KC_F4) then
        
    end
end

function EventHandle_Mouse(ctype)
end

function EventHandle_WindowResized(width, height)
    GUI_WindowResized(width, height)
    
    local dimension = TextComponent:GetDimension()
    local ui_posx = width - dimension.x - 20;
    local ui_posy = height - dimension.y - 35;
    TextComponent:setPosition(Vector2(ui_posx, ui_posy))
end

local TextComponent = nil
function Sandbox_Initialize(ctype)
    GUI_CreateCameraAndProfileInfo()

    TextComponent = GUI_CreateSandboxText(infoText, textSize)

    -- Create The Sky.
    Sandbox:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0));

    -- Create a plane in the physics world
    local plane = Sandbox:CreatePlane(200, 200);
    plane:setOrientation(Quaternion(0, 0, 0));
    plane:setPosition(Vector3(0, 0, 0));
    Sandbox:setMaterial(plane, "Ground2");

    -- Create Lighting.
    Sandbox:SetAmbientLight(Vector3(0.3));
    -- Create a directional light for the sun.
    local directLight = Sandbox:CreateDirectionalLight(Vector3(1, -1, 1));
    directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9));
    directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9));

    -- Initialize the camera position to focus on the soldier.
    local camera = Sandbox:GetCamera();
    camera:setPosition(Vector3(0, 1, -3));
    camera:setOrientation(GetForward(Vector3(0, 0, -1)));

    local soldierPath = "models/futuristic_soldier/futuristic_soldier_dark_anim.mesh"
    local soldier = Sandbox:CreateEntityObject(soldierPath);
    soldier:setPosition(Vector3(0, 0, 0))
    soldier:setRotation(Vector3(0, -90, 0))

    local weaponPath = "models/futuristic_soldier/soldier_weapon.mesh"
    local weapon = Sandbox:CreateEntityObject(weaponPath);
    
    -- Create an animation state machine to handle weapon animations.
    _G.weaponAsm = weapon:GetWeaponASM();

    -- Retrieve and store the weapon animations before attaching the weapon to
    -- the soldier.
    local sniper_idle = weapon:GetAnimation("sniper_idle")
    local smg_idle = weapon:GetAnimation("smg_idle")

    weaponAsm:AddState("smg_idle", smg_idle, true);
    weaponAsm:AddState("sniper_idle", sniper_idle, true);
    weaponAsm:AddState("smg_transform", weapon:GetAnimation("smg_transform"));
    weaponAsm:AddState("sniper_reload", weapon:GetAnimation("sniper_reload"));
    weaponAsm:AddState("sniper_transform", weapon:GetAnimation("sniper_transform"));

    weaponAsm:AddTransition("sniper_idle", "sniper_reload", sniper_idle:GetLength(), 0.2);
    weaponAsm:AddTransition("sniper_idle", "sniper_transform", sniper_idle:GetLength(), 0.2);
    weaponAsm:AddTransition("sniper_reload", "sniper_idle", 0.2, 0.2);
    weaponAsm:AddTransition("sniper_transform", "sniper_idle", 0.2, 0.2);
    weaponAsm:AddTransition("smg_idle", "smg_transform", smg_idle:GetLength(), 0.2);
    weaponAsm:AddTransition("smg_transform", "smg_idle", 0.2, 0.2);

    -- Request a default looping animation to play.
    weaponAsm:RequestState("sniper_idle");

    local idle_anim = soldier:GetAnimation("stand_idle_aim");
    local idleAnimLength = idle_anim:GetLength();

    -- Create an animation state machine to handle soldier animations.
    _G.soldierAsm = soldier:GetMoveASM();

    soldierAsm:AddState("idle_aim", idle_anim, true);
    soldierAsm:AddState("dead", soldier:GetAnimation("stand_dead_2"));
    soldierAsm:AddState("dead_headshot", soldier:GetAnimation("stand_dead_headshot"));
    soldierAsm:AddState("fire", soldier:GetAnimation("stand_fire_one_shot"), true);
    soldierAsm:AddState("melee", soldier:GetAnimation("stand_melee_1_with_weapon"));
    soldierAsm:AddState("reload", soldier:GetAnimation("stand_reload"));
    soldierAsm:AddState("run_backward", soldier:GetAnimation("stand_run_backward_aim"), true);
    soldierAsm:AddState("run_forward", soldier:GetAnimation("stand_run_forward_aim"), true);
    soldierAsm:AddState("smg_transform", soldier:GetAnimation("stand_smg_transform"));
    soldierAsm:AddState("sniper_transform", soldier:GetAnimation("stand_sniper_transform"));

    -- Idle Transitions
    -- All transitions from idle can happen at any time in the idle animation.
    soldierAsm:AddTransition("idle_aim", "dead", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "dead_headshot", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "fire", idleAnimLength, 0.1);
    soldierAsm:AddTransition("idle_aim", "melee", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "reload", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "run_backward", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "run_forward", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "smg_transform", idleAnimLength, 0.2);
    soldierAsm:AddTransition("idle_aim", "sniper_transform", idleAnimLength, 0.2);

    -- Fire Transitions
    soldierAsm:AddTransition("fire", "idle_aim", 0.1, 0.1);
    -- Melee Transitions
    soldierAsm:AddTransition("melee", "idle_aim", 0.2, 0.2);
    -- Reload Transitions
    soldierAsm:AddTransition("reload", "idle_aim", 0.2, 0.2);
    -- Run Backward Transitions
    soldierAsm:AddTransition("run_backward", "idle_aim", 0.2, 0.2);
    -- Run Forward Transitions
    soldierAsm:AddTransition("run_forward", "idle_aim", 0.2, 0.2);
    -- SMG Transitions
    soldierAsm:AddTransition("smg_transform", "idle_aim", 0.2, 0.2);
    -- Sniper Transitions
    soldierAsm:AddTransition("sniper_transform", "idle_aim", 0.2, 0.2);

    soldierAsm:RequestState("idle_aim")

    soldier:AttachToBone("b_RightHand", weapon, Vector3(0.04, 0.05, -0.01), Vector3(98.0, 97.0, 0))
end


function Sandbox_Update(deltaTimeInMillis)
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()

    if soldierAsm:GetCurrStateName() ~= "idle_aim" then
        soldierAsm:RequestState("idle_aim")
    end

    if weaponState == "sniper" then
        if weaponAsm:GetCurrStateName() ~= "sniper_idle" then
            weaponAsm:RequestState("sniper_idle")
        end
    elseif weaponState == "smg" then
        if weaponAsm:GetCurrStateName() ~= "smg_idle" then
            weaponAsm:RequestState("smg_idle")
        end
    end
end