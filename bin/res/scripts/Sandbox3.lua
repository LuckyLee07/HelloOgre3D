local TextComponent = nil

local function IsNumKey(key, numKey)
    return true
end

local runforwardAnim = nil
function Sandbox_Initialize(ctype)
    GUI_CreateCommonsUI()
    TextComponent = GUI_CreateSandbox3Text()

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

    soldier:AttachToBone("b_RightHand", weapon, Vector3(0.04, 0.05, -0.01), Vector3(98.0, 97.0, 0))

    runforwardAnim = soldier:GetAnimation("stand_run_forward_weapon")
    if runforwardAnim ~= nil then runforwardAnim:SetEnabled(true) end
end


function Sandbox_Update(deltaTimeInMillis)
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()

    runforwardAnim:AddTime(deltaTimeInMillis/1000.0)
end


function EventHandle_Keyboard(keycode, pressed)
    GUI_HandleKeyEvent(keycode, pressed)

    if not pressed then return end
    if (keycode == OIS.KC_F1) then
        local camera = Sandbox:GetCamera();
        camera:setPosition(Vector3(0, 1, -3));
        camera:setOrientation(GetForward(Vector3(0, 0, -1)));
    elseif (keycode == OIS.KC_F2) then
        local isShow = TextComponent:isVisible()
        TextComponent:setVisible(not isShow)
    elseif (keycode == OIS.KC_F12) then
        Sandbox:CallFile("res/scripts/gui.lua")
        Sandbox:CallFile("res/scripts/agent.lua")
    elseif (keycode == OIS.KC_SPACE) then
        Sandbox_ShootBox()
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


