local TextComponent = nil

function CreateSandboxText()
    local ui_width, ui_height = 300, 180;
    local screenWidth = Sandbox:GetScreenWidth()
    local screenHeight = Sandbox:GetScreenHeight()

    local uiComponent = Sandbox:CreateUIComponent()
    local ui_posx = screenWidth - ui_width - 20;
    local ui_posy = screenHeight - ui_height - 35;
    uiComponent:setPosition(Vector2(ui_posx, ui_posy))
    uiComponent:setDimension(Vector2(ui_width, ui_height))
    uiComponent:setTextMargin(10, 10);

    local startColor = GUI.Palette.DarkBlueGradient
    local endenColor = GUI.Palette.DarkBlackGradient
    uiComponent:setGradientColor(Gorilla.Gradient_NorthSouth, startColor, endenColor)

    local markupText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "W/A/S/D: to move" .. GUI.MarkupNewline ..
        "Hold Shift: to accelerate movement" .. GUI.MarkupNewline ..
        "Hold RMB: to look" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "Space: shoot block" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "F1: to reset the camera" .. GUI.MarkupNewline ..
        "F2: toggle the menu" .. GUI.MarkupNewline ..
        "F5: toggle performance information" .. GUI.MarkupNewline ..
        "F6: toggle camera information" .. GUI.MarkupNewline ..
        "F7: toggle physics debug"
    uiComponent:setMarkupText(markupText)

    return uiComponent
end


function Sandbox_Initialize(ctype)
    GUI_CreateCommonsUI()

    TextComponent = CreateSandboxText()

    -- Initialize the camera position to focus on the soldier.
    local camera = Sandbox:GetCamera();
    camera:setPosition(Vector3(7, 5, -18));
    camera:setOrientation(Quaternion(-160, 0, -180));

    -- Create The Sky.
    Sandbox:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0));

    -- Create Lighting.
    Sandbox:SetAmbientLight(Vector3(0.3));

    local directLight = Sandbox:CreateDirectionalLight(Vector3(1, -1, 1));
    directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9));
    directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9));


    local plane = Sandbox:CreatePlane(200, 200);
    plane:setOrientation(Quaternion(0, 0, 0));
    plane:setPosition(Vector3(0, 0, 0));
    Sandbox:setMaterial(plane, "Ground2");

    -- misc meshes
    CreateSandboxObject("modular_block", Vector3(-3, 0, 0));
    CreateSandboxObject("modular_cooling", Vector3(-3, 0, 3));
    CreateSandboxObject("modular_roof", Vector3(-2, 0, -4));

    -- brick meshes
    CreateSandboxObject("modular_pillar_brick_3", Vector3(-1, 0, 0));
    CreateSandboxObject("modular_pillar_brick_2", Vector3(-1, 0, 3));
    CreateSandboxObject("modular_pillar_brick_1", Vector3(-1, 0, 6));
    CreateSandboxObject("modular_brick_door", Vector3(1, 0, 0));
    CreateSandboxObject("modular_brick_double_window", Vector3(4, 0, 0));
    CreateSandboxObject("modular_brick_small_double_window", Vector3(7, 0, 0));
    CreateSandboxObject("modular_brick_small_window_1", Vector3(10, 0, 0));
    CreateSandboxObject("modular_brick_window_1", Vector3(13, 0, 0));
    CreateSandboxObject("modular_brick_small_window_2", Vector3(16, 0, 0));
    CreateSandboxObject("modular_brick_window_2", Vector3(18, 0, 0));
    CreateSandboxObject("modular_wall_brick_1", Vector3(1, 0, -6));
    CreateSandboxObject("modular_wall_brick_2", Vector3(4, 0, -6));
    CreateSandboxObject("modular_wall_brick_3", Vector3(6, 0, -6));
    CreateSandboxObject("modular_wall_brick_4", Vector3(8, 0, -6));
    CreateSandboxObject("modular_wall_brick_5", Vector3(11, 0, -6));
    
    -- concrete meshes
    CreateSandboxObject("modular_pillar_concrete_3", Vector3(-2, 0, 0));
    CreateSandboxObject("modular_pillar_concrete_2", Vector3(-2, 0, 3));
    CreateSandboxObject("modular_pillar_concrete_1", Vector3(-2, 0, 6));
    CreateSandboxObject("modular_concrete_door", Vector3(1, 0, 3));
    CreateSandboxObject("modular_concrete_double_window", Vector3(4, 0, 3));
    CreateSandboxObject("modular_concrete_small_double_window", Vector3(7, 0, 3));
    CreateSandboxObject("modular_concrete_small_window_1", Vector3(10, 0, 3));
    CreateSandboxObject("modular_concrete_window_1", Vector3(13, 0, 3));
    CreateSandboxObject("modular_concrete_small_window_2", Vector3(16, 0, 3));
    CreateSandboxObject("modular_concrete_window_2", Vector3(18, 0, 3));
    CreateSandboxObject("modular_wall_concrete_1", Vector3(1, 0, -3));
    CreateSandboxObject("modular_wall_concrete_2", Vector3(4, 0, -3));
    CreateSandboxObject("modular_wall_concrete_3", Vector3(6, 0, -3));
    CreateSandboxObject("modular_wall_concrete_4", Vector3(8, 0, -3));
    CreateSandboxObject("modular_wall_concrete_5", Vector3(11, 0, -3));
    
    -- metal meshes
    CreateSandboxObject("modular_hangar_door", Vector3(1, 0, 6));
    --]]
end


function Sandbox_Update()
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()
end


function EventHandle_Keyboard(keycode, pressed)
    GUI_HandleKeyEvent(keycode, pressed)

    if not pressed then return end
    if (keycode == OIS.KC_F1) then
        local camera = Sandbox:GetCamera();
        camera:setPosition(Vector3(7, 5, -18));
        camera:setOrientation(Quaternion(-160, 0, -180));
    elseif (keycode == OIS.KC_F2) then
        local isShow = TextComponent:isVisible()
        TextComponent:setVisible(not isShow)
    elseif (keycode == OIS.KC_SPACE) then
        Sandbox_ShootBox()
    end
end

function EventHandle_Mouse(ctype)

end

_G.Shoot_BoxCount = 0
function Sandbox_ShootBox()
    Shoot_BoxCount = Shoot_BoxCount + 1
    local object = CreateSandboxObject("modular_block")

    local cameraPosition = Sandbox:GetCameraPosition()
    local cameraForward = Sandbox:GetCameraForward()

    local position = cameraPosition + cameraForward * 2;
    local rotation = Sandbox:GetCameraOrientation();
    object:setPosition(position)
    object:setOrientation(rotation)


    local impulse = cameraForward * 15000
    local angularImpulse = Sandbox:GetCameraLeft() * 10
    object:applyImpulse(impulse)
    object:applyAngularImpulse(angularImpulse)
end