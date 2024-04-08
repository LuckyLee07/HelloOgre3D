
function Sandbox_Initialize(ctype)

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

    --[[
    -- misc meshes
    SandboxUtilities_CreateObject(
        sandbox, "modular_block", Vector.new(-3, 0, 0));
    SandboxUtilities_CreateObject(
        sandbox, "modular_cooling", Vector.new(-3, 0, 3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_roof", Vector.new(-2, 0, -4));

    -- brick meshes
    SandboxUtilities_CreateObject(
        sandbox, "modular_pillar_brick_3", Vector.new(-1, 0, 0));
    SandboxUtilities_CreateObject(
        sandbox, "modular_pillar_brick_2", Vector.new(-1, 0, 3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_pillar_brick_1", Vector.new(-1, 0, 6));
    SandboxUtilities_CreateObject(
        sandbox, "modular_brick_door", Vector.new(1, 0, 0));
    SandboxUtilities_CreateObject(
        sandbox, "modular_brick_double_window", Vector.new(4, 0, 0));
    SandboxUtilities_CreateObject(
        sandbox, "modular_brick_small_double_window", Vector.new(7, 0, 0));
    SandboxUtilities_CreateObject(
        sandbox, "modular_brick_small_window_1", Vector.new(10, 0, 0));
    SandboxUtilities_CreateObject(
        sandbox, "modular_brick_window_1", Vector.new(13, 0, 0));
    SandboxUtilities_CreateObject(
        sandbox, "modular_brick_small_window_2", Vector.new(16, 0, 0));
    SandboxUtilities_CreateObject(
        sandbox, "modular_brick_window_2", Vector.new(18, 0, 0));
    SandboxUtilities_CreateObject(
        sandbox, "modular_wall_brick_1", Vector.new(1, 0, -6));
    SandboxUtilities_CreateObject(
        sandbox, "modular_wall_brick_2", Vector.new(4, 0, -6));
    SandboxUtilities_CreateObject(
        sandbox, "modular_wall_brick_3", Vector.new(6, 0, -6));
    SandboxUtilities_CreateObject(
        sandbox, "modular_wall_brick_4", Vector.new(8, 0, -6));
    SandboxUtilities_CreateObject(
        sandbox, "modular_wall_brick_5", Vector.new(11, 0, -6));
    
    -- concrete meshes
    SandboxUtilities_CreateObject(
        sandbox, "modular_pillar_concrete_3", Vector.new(-2, 0, 0));
    SandboxUtilities_CreateObject(
        sandbox, "modular_pillar_concrete_2", Vector.new(-2, 0, 3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_pillar_concrete_1", Vector.new(-2, 0, 6));
    SandboxUtilities_CreateObject(
        sandbox, "modular_concrete_door", Vector.new(1, 0, 3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_concrete_double_window", Vector.new(4, 0, 3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_concrete_small_double_window", Vector.new(7, 0, 3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_concrete_small_window_1", Vector.new(10, 0, 3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_concrete_window_1", Vector.new(13, 0, 3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_concrete_small_window_2", Vector.new(16, 0, 3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_concrete_window_2", Vector.new(18, 0, 3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_wall_concrete_1", Vector.new(1, 0, -3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_wall_concrete_2", Vector.new(4, 0, -3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_wall_concrete_3", Vector.new(6, 0, -3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_wall_concrete_4", Vector.new(8, 0, -3));
    SandboxUtilities_CreateObject(
        sandbox, "modular_wall_concrete_5", Vector.new(11, 0, -3));
    
    -- metal meshes
    SandboxUtilities_CreateObject(
        sandbox, "modular_hangar_door", Vector.new(1, 0, 6));
    --]]
end
