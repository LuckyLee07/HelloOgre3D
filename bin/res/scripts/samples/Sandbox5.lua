
require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.samples.chapter4.DirectSoldierAgent.lua")

local textSize = {w = 300, h = 260}
local infoText = GUI.MarkupColor.White .. GUI.Markup.SmallMono ..
        "W/A/S/D: to move" .. GUI.MarkupNewline ..
        "Hold Shift: to accelerate movement" .. GUI.MarkupNewline ..
        "Hold RMB: to look" .. GUI.MarkupNewline ..
        GUI.MarkupNewline ..
        "F1: to reset the camera" .. GUI.MarkupNewline ..
        "F2: toggle the menu" .. GUI.MarkupNewline ..
        "F3: toggle the navigation mesh" .. GUI.MarkupNewline ..
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


local _agents = {}

local function _GetFilePath(luafile)
    return "res/scripts/samples/chapter4/".. luafile;
end

local function _DrawPaths()
    for index, agent in pairs(_agents) do
        -- Draw the agent's cyclic path, offset slightly above the level
        -- geometry.
        DebugDrawer:drawPath(agent:GetPath(), UtilColors.Red, false, Vector3(0.0, 0.02, 0.0))
        DebugDrawer:drawSquare(agent:GetTarget(), 0.1, UtilColors.Red, true);
    end
end

local function _UpdatePaths()
    for index, agent in pairs(_agents) do
        local navPosition = Sandbox:FindClosestPoint("default", agent:GetPosition());
        local targetRadiusSquared = agent:GetTargetRadius() * agent:GetTargetRadius();
        local distanceSquared = DistanceSquared(navPosition, agent:GetTarget());
        -- Determine if the agent is within the target radius to their
        -- target position.
        if (distanceSquared < targetRadiusSquared) then
            local endPoint;
            local path = std.vector_Ogre__Vector3_();
            
            -- Randomly try and pathfind to a new navmesh point, keep trying
            -- until a valid path is found.
            while path:size() == 0 do
                endPoint = Sandbox:RandomPoint("default");
                result = Sandbox:FindPath("default", agent:GetPosition(), endPoint, path);
            end
            -- Assign a new path and target position.
            agent:SetPath(path, false);
            agent:SetTarget(endPoint);
        end
    end
end

function EventHandle_Keyboard(keycode, pressed)
    -- OIS.KC_F2 Event 已处理
    GUI_HandleKeyEvent(keycode, pressed)

    if not pressed then return end
    if (keycode == OIS.KC_F1) then
        local camera = Sandbox:GetCamera();
        camera:setPosition(Vector3(15, 65, 15));
        camera:setOrientation(Quaternion(-90, 0, -180));
    elseif (keycode == OIS.KC_F3) then
        Create_LightSoldier("DirectSoldierAgent.lua")
    end
end

function EventHandle_Mouse(ctype)
end

function EventHandle_WindowResized(width, height)
    GUI_WindowResized(width, height)
end

function Sandbox_Initialize()
    GUI_CreateCameraAndProfileInfo()
    GUI_CreateSandboxText(infoText, textSize)

    -- Initialize the camera position to focus on the soldier.
    local camera = Sandbox:GetCamera();
    camera:setPosition(Vector3(15, 65, 15));
    camera:setOrientation(Quaternion(-90, 0, 180));

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
    
    -- 强制刷新下场景图 防止getWorldAABB()时拿不到有效包围盒
    Sandbox:UpdateSceneGraph()

    -- Create default navigation mesh
    local navMeshConfig = rcConfig();
    Sandbox:DefaultConfig(navMeshConfig)
    navMeshConfig.minRegionArea = math.pow(100, 2)
    navMeshConfig.walkableRadius = math.ceil(0.4 / navMeshConfig.cs)
    navMeshConfig.walkableClimb = math.ceil(0.2 / navMeshConfig.ch)
    navMeshConfig.walkableSlopeAngle = 45
    local navMesh = Sandbox:CreateNavigationMesh(navMeshConfig, 'default')
    if navMesh ~= nil then navMesh:SetDebugVisible(true) end

    -- Create agents and randomly place them on the navmesh.
    local agentLuafile = _GetFilePath("DirectSoldierAgent.lua")
    local points = {
        Vector3(-2.48, 4.05, 26.2),
        Vector3(49.4, 0.05, 3.78),
        Vector3(5.47, 0.05, 2.44),
        Vector3(14.5, 0.05, 13.36),
        Vector3(20.56, 0.05, 3.47),
    }
    for i=1, 5 do
        local agent = Create_Soldier(agentLuafile)
        table.insert(_agents, agent);

        local randomPoint = points[i]--Sandbox:RandomPoint("default");
        agent:setPosition(randomPoint);
        
        -- Use the Agent's closest point to the navmesh as their target position.
        local navPosition = Sandbox:FindClosestPoint("default", agent:GetPosition());
        agent:SetTarget(navPosition);

        -- Increase the target radius to prevent agents from slowing to reach
        -- their target position.
        agent:SetTargetRadius(1);
    end
end

function Sandbox_Update(deltaTimeInMillis)
    GUI_UpdateCameraInfo()
    GUI_UpdateProfileInfo()

    _UpdatePaths()

    _DrawPaths()
end