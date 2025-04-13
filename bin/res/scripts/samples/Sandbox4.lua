
require("res.scripts.samples.chapter4.DirectSoldierAgent.lua")

local textSize = {w = 300, h = 300}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "W/A/S/D: to move" .. GUI.MarkupNewline ..
        "Hold Shift: to accelerate movement" .. GUI.MarkupNewline ..
        "Hold RMB: to look" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "F1: to reset the camera" .. GUI.MarkupNewline ..
        "F2: toggle the menu" .. GUI.MarkupNewline ..
        "F3: spawn a direct controlled soldier" .. GUI.MarkupNewline ..
        "F4: spawn a indirect controlled soldier" .. GUI.MarkupNewline ..
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


function GetFilePath(luafile)
    return "res/scripts/samples/chapter4/".. luafile;
end

function Create_LightSoldier(luafile)
    local soldierPath = "models/futuristic_soldier/futuristic_soldier_anim.mesh"
    local soldierAgent = Sandbox:CreateSoldier(soldierPath, GetFilePath(luafile))
    Soldier_InitSoldierAsm(soldierAgent)

    local weaponPath = "models/futuristic_soldier/soldier_weapon.mesh"
    soldierAgent:initWeapon(weaponPath)
    Soldier_InitWeaponAsm(soldierAgent)
end

function Create_DarkSoldier(luafile)
    local soldierPath = "models/futuristic_soldier/futuristic_soldier_dark_anim.mesh"
    local soldierAgent = Sandbox:CreateSoldier(soldierPath, GetFilePath(luafile))
    Soldier_InitSoldierAsm(soldierAgent)

    local weaponPath = "models/futuristic_soldier/soldier_weapon.mesh"
    soldierAgent:initWeapon(weaponPath)
    Soldier_InitWeaponAsm(soldierAgent)
end

function EventHandle_Keyboard(keycode, pressed)
    GUI_HandleKeyEvent(keycode, pressed)

    if not pressed then return end
    if (keycode == OIS.KC_F1) then
        local camera = Sandbox:GetCamera();
        camera:setPosition(Vector3(-17, 10, 0));
        camera:setOrientation(Quaternion(-145, -50, -150));
    elseif (keycode == OIS.KC_F3) then
        Create_LightSoldier("DirectSoldierAgent.lua")
    elseif (keycode == OIS.KC_F4) then
        Create_DarkSoldier("DirectSoldierAgent.lua")
    end
end

function EventHandle_Mouse(ctype)
end

function EventHandle_WindowResized(width, height)
    GUI_WindowResized(width, height)
end

function Sandbox_Initialize(ctype)
    GUI_CreateCameraAndProfileInfo()
    GUI_CreateSandboxText(infoText, textSize)

    -- Initialize the camera position to focus on the soldier.
    local camera = Sandbox:GetCamera();
    camera:setPosition(Vector3(-17, 10, 0));
    camera:setOrientation(Quaternion(-145, -50, -150));

    -- Create The Sky.
    Sandbox:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0));

    -- Create a plane in the physics world
    local plane = Sandbox:CreatePlane(200, 200);
    plane:setPosition(Vector3(0, -10, 0));
    plane:setMaterial("Ground2");

    -- Create Lighting.
    Sandbox:SetAmbientLight(Vector3(0.3));
    -- Create a directional light for the sun.
    local directLight = Sandbox:CreateDirectionalLight(Vector3(1, -1, 1));
    directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9));
    directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9));

	-- Create the sandbox level
    SandboxUtilities_CreateLevel()
end


function Sandbox_Update(deltaTimeInMillis)
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()

    -- Draw the agent's cyclic path, offset slightly above the level geometry.
    local agentPath = SandboxUtilities_GetLevelPath()
    DebugDrawer:drawPath(agentPath, UtilColors.Blue, true, Vector3(0.0, 0.02, 0.0))
end