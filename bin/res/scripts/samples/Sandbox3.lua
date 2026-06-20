require("res.scripts.agent.SoldierAgent.lua")
local AgentComponents = require("res.scripts.agent.AgentComponentAccess.lua")
local ParityTrace = require("res.scripts.samples.parity_trace")

local _parityTrace = nil
local _ch3Compare = false

function Agent_Initialize(agent)
end

local function _IsTruthy(value)
    if value == nil then return false end
    value = string.lower(tostring(value))
    return value == "1" or value == "true" or value == "yes" or value == "on"
end

local function _IsAnimAsmReady()
    return soldierAsm ~= nil and weaponAsm ~= nil
end

local function _GetSeed()
    return tonumber(os.getenv("HELLO_PARITY_SEED") or os.getenv("HELLO_SAMPLE_SEED") or "") or 0
end

local function _BuildAnimSnapshot(state)
    local extra = {}
    if state ~= nil and state.lastDeltaMs ~= nil then
        extra.deltaTimeMs = state.lastDeltaMs
    end
    if not _IsAnimAsmReady() then
        extra.notReady = true
        return { agents = {} }
    end
    return {
        agents = { ParityTrace.AnimSnapshot(soldierAsm, 1, extra) },
    }
end

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
        "Num1: melee animation" .. GUI.MarkupNewline ..
        "Num2: sniper reload animation" .. GUI.MarkupNewline ..
        "Num3: sniper transform to smg animation" .. GUI.MarkupNewline ..
        "Num4: smg transform to sniper animation" .. GUI.MarkupNewline ..
        "Num5: fire animation" .. GUI.MarkupNewline ..
        "Num6: run forward animation" .. GUI.MarkupNewline ..
        "Num7: run backward animation" .. GUI.MarkupNewline ..
        "Num8: death animation" .. GUI.MarkupNewline ..
        "Num9: headshot animation" .. GUI.MarkupNewline ;

local weaponState = "sniper"

function EventHandle_Keyboard(keycode, pressed)
    GUI_HandleKeyEvent(keycode, pressed)

    if not pressed then return end
    if (keycode == OIS.KC_F1) then
        local camera = SandboxCamera:GetCamera();
        camera:setPosition(Vector3(0, 1, -3));
        camera:setOrientation(GetForward(Vector3(0, 0, -1)));
    elseif (keycode == OIS.KC_F12) then
        SandboxScript:CallFile("res/scripts/gui.lua")
        SandboxScript:CallFile("res/scripts/agent.lua")

    elseif (IsNumKey(keycode, 1)) then
        soldierAsm:RequestState("melee")
    elseif (IsNumKey(keycode, 2)) then
        if weaponState == "sniper" and soldierAsm:RequestState("reload") then
            weaponAsm:RequestState("sniper_reload")
        end
    elseif (IsNumKey(keycode, 3)) then
        if weaponState == "sniper" then
            if soldierAsm:RequestState("smg_transform") then
                weaponAsm:RequestState("smg_transform")
                weaponState = "smg"
            end
        end
    elseif (IsNumKey(keycode, 4)) then
        if weaponState == "smg" then
            if soldierAsm:RequestState("sniper_transform") then
                weaponAsm:RequestState("sniper_transform")
                weaponState = "sniper"
            end
        end
    elseif (IsNumKey(keycode, 5)) then
        soldierAsm:RequestState("fire")
    elseif (IsNumKey(keycode, 6)) then
        soldierAsm:RequestState("run_forward")
    elseif (IsNumKey(keycode, 7)) then
        soldierAsm:RequestState("run_backward")
    elseif (IsNumKey(keycode, 8)) then
        soldierAsm:RequestState("dead")
    elseif (IsNumKey(keycode, 9)) then
        soldierAsm:RequestState("dead_headshot")
    elseif (keycode == OIS.KC_SPACE) then
    end
end

function EventHandle_Mouse(ctype)

end

function EventHandle_WindowResized(width, height)
    GUI_WindowResized(width, height)
end

function Sandbox_Initialize(ctype)
    if not _IsTruthy(os.getenv("HELLO_CH3_HIDE_UI")) then
        GUI_CreateCameraAndProfileInfo()
        GUI_CreateSandboxText(infoText, textSize)
    end

    -- Create The Sky.
    SandboxScene:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0));

    -- Create a plane in the physics world
    local plane = SandboxObjects:CreatePlane(200, 200);
    plane:setOrientation(Quaternion(0, 0, 0));
    plane:setPosition(Vector3(0, 0, 0));
    SandboxScene:setMaterial(plane, "Ground2");

    -- Create Lighting.
    SandboxScene:SetAmbientLight(Vector3(0.3));
    -- Create a directional light for the sun.
    local directLight = SandboxScene:CreateDirectionalLight(Vector3(1, -1, 1));
    directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9));
    directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9));

    -- Initialize the camera position to focus on the soldier.
    local camera = SandboxCamera:GetCamera();
    camera:setPosition(Vector3(0, 1, -3));
    camera:setOrientation(GetForward(Vector3(0, 0, -1)));

    -- 使用lua端的update驱动
    SandboxAgentConfig:SetUseCppFsmFlag(false)
    
    local soldierPath = "models/futuristic_soldier/futuristic_soldier_dark_anim.mesh"
    local soldierAgent = SandboxObjects:CreateSoldier(soldierPath)
    Soldier_InitSoldierAsm(soldierAgent)

    soldierAgent:setPosition(Vector3(0, 0, 0))
    soldierAgent:setRotation(Vector3(0, -90, 0))

    local weaponPath = "models/futuristic_soldier/soldier_weapon.mesh"
    soldierAgent:initWeapon(weaponPath)
    Soldier_InitWeaponAsm(soldierAgent)

    local weapon = soldierAgent:GetWeaponComponent()
    _G.weaponAsm = weapon:GetObjectASM();

    _G.soldierAsm = AgentComponents.GetBodyAsm(soldierAgent);

    _ch3Compare = _IsTruthy(os.getenv("HELLO_CH3_COMPARE"))
    if _ch3Compare then
        soldierAsm:RequestState("run_forward")
        _parityTrace = ParityTrace.Start({
            sample = "Sandbox3",
            preset = os.getenv("HELLO_SAMPLE_PRESET") or "chapter3_compare",
            seed = _GetSeed(),
        })
    end
end

function Sandbox_Update(deltaTimeInMillis)
    if not _IsAnimAsmReady() then
        return
    end

    if _ch3Compare then
        if _parityTrace ~= nil then
            _parityTrace.lastDeltaMs = math.floor(deltaTimeInMillis + 0.5)
        end
        ParityTrace.Tick(_parityTrace, deltaTimeInMillis, _BuildAnimSnapshot)
        if soldierAsm:GetCurrStateName() ~= "run_forward" then
            soldierAsm:RequestState("run_forward")
        end
        return
    end

    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()

    if soldierAsm:GetCurrStateName() ~= "idle_aim" then
        soldierAsm:RequestState("idle_aim")
    end

    local currStateName = weaponAsm:GetCurrStateName()
    if weaponState == "sniper" then
        if currStateName ~= "sniper_idle" then
            weaponAsm:RequestState("sniper_idle")
        end
    elseif weaponState == "smg" then
        if currStateName ~= "smg_idle" then
            weaponAsm:RequestState("smg_idle")
        end
    end
end


function Agent_EventHandle(agent, keycode)
end

function Agent_Update(agent, deltaTimeInMillis)
end
