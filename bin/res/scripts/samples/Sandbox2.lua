
local textSize = {w = 300, h = 180}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
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
            "F7: toggle physics debug";

Shoot_BoxCount = 0
function Sandbox_Initialize(ctype)
    GUI_CreateCameraAndProfileInfo()
    GUI_CreateSandboxText(infoText, textSize)

    -- Initialize the camera position to focus on the soldier.
    local camera = Sandbox:GetCamera();
    camera:setPosition(Vector3(-30, 5, 7));
    camera:setOrientation(Quaternion(-131, -68, -133));

    -- Create The Sky.
    Sandbox:SetSkyBox("ThickCloudsWaterSkyBox", Vector3(0, 180, 0));

    -- Create Lighting.
    Sandbox:SetAmbientLight(Vector3(0.3));

    -- Create a directional light for the sun.
    local directLight = Sandbox:CreateDirectionalLight(Vector3(1, -1, 1));
    directLight:setDiffuseColour(ColourValue(1.8, 1.4, 0.9));
    directLight:setSpecularColour(ColourValue(1.8, 1.4, 0.9));

    -- Create a plane in the physics world
    local plane = Sandbox:CreatePlane(200, 200);
    plane:setOrientation(Quaternion(0, 0, 0));
    plane:setPosition(Vector3(0, 0, 0));
    Sandbox:setMaterial(plane, "Ground2");

    Sandbox:CreateAgent("SeekingAgent.lua", AGENT_OBJ_SEEKING)
    Sandbox:CreateAgent("PursuingAgent.lua", AGENT_OBJ_PURSUING)
    
    local points = std.vector_Ogre__Vector3_();
    points:push_back(Vector3(0, 0, 0))
    points:push_back(Vector3(30, 0, 0))
    points:push_back(Vector3(30, 0, 50))
    points:push_back(Vector3(-30, 0, 0))
    points:push_back(Vector3(-30, 0, 20))
    --[[
    points:push_back(Vector3(0, 0, 0))
    points:push_back(Vector3(10, 0, -20))
    points:push_back(Vector3(10, 0, 20))
    points:push_back(Vector3(-10, 0, 0))
    points:push_back(Vector3(-10, 0, 20))
    --]]
    for index = 1, 20 do
        local agent = Sandbox:CreateAgent("PathingAgent.lua", AGENT_OBJ_PATHING);
        agent:SetPath(points, true);

        -- Randomly vary speeds to allow agents to pass one another.
        local randomSpeed = math.random(
            agent:GetMaxSpeed() * 0.85,
            agent:GetMaxSpeed() * 1.15);

        agent:SetMaxSpeed(randomSpeed);
    end

    for index = 1, 5 do
        Sandbox:CreateAgent("FollowerAgent.lua", AGENT_OBJ_FOLLOWER);
    end
end


function Sandbox_Update(deltaTimeInMillis)
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()

    -- std::vector not lua table
    local allBlocks = ObjectManager:getAllBlocks();
    for index = 1, allBlocks:size()-1 do
        local pObject = allBlocks[index];
        --DebugDrawer:drawSquare(pObject:GetPosition(), pObject:GetRadius(), ColourValue(1, 0, 0))
    end
end


function EventHandle_Keyboard(keycode, pressed)
    GUI_HandleKeyEvent(keycode, pressed)

    if not pressed then return end
    if (keycode == OIS.KC_F1) then
        local camera = Sandbox:GetCamera();
        camera:setPosition(Vector3(7, 5, -18));
        camera:setOrientation(Quaternion(-160, 0, -180));
    elseif (keycode == OIS.KC_F9) then
        ObjectManager:clearAllObjects(MGR_OBJ_BLOCK, false)
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
end

function Sandbox_ShootBox()
    Shoot_BoxCount = Shoot_BoxCount + 1
    local object = CreateBlockObject("modular_block")

    local cameraPosition = Sandbox:GetCameraPosition()
    local cameraForward = Sandbox:GetCameraForward()

    local position = cameraPosition + cameraForward * 2;
    local rotation = Sandbox:GetCameraOrientation();
    object:setPosition(position)
    object:setOrientation(rotation)

    object:SetMass(15);
    local impulse = cameraForward * 15000 * 0.65
    local angularImpulse = Sandbox:GetCameraLeft() * 10
    object:applyImpulse(impulse)
    object:applyAngularImpulse(angularImpulse)
end
