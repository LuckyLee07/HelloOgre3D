
--require "SandboxUtilities"

function Sandbox_Cleanup(sandbox)
end

function Sandbox_HandleEvent(sandbox, event)
    if (event.source == "keyboard" and event.pressed) then
        if ( event.key == "space_key" ) then
            --SandboxUtilities_ShootBox(sandbox);
        end
    end
end

function Sandbox_Initialize(sandbox)
    -- Initialize the camera position to focus on the soldier.
    Sandbox.SetCameraPosition(sandbox, Vector.new(7, 5, -18));
    Sandbox.SetCameraOrientation(sandbox, Vector.new(-160, 0, -180));

    Sandbox.CreateSkyBox(sandbox, "ThickCloudsWaterSkyBox", Vector.new(0, 180, 0));

    -- Create Lighting.
    Sandbox.SetAmbientLight(sandbox, Vector.new(0.3));

    local directional = Core.CreateDirectionalLight(sandbox, Vector.new(1, -1, 1));
    Core.SetLightDiffuse(directional, Vector.new(1.8, 1.4, 0.9));
    Core.SetLightSpecular(directional, Vector.new(1.8, 1.4, 0.9));

    local plane = Sandbox.CreatePlane(sandbox, 200, 200);
    Core.SetRotation(plane, Vector.new(0, 0, 0));
    Core.SetPosition(plane, Vector.new(0, 0, 0));
    Core.SetMaterial(plane, "Ground2");
    --[[
    -- misc meshes
    SandboxUtilities_CreateObject(sandbox, "modular_block", Vector.new(-3, 0, 0));
    SandboxUtilities_CreateObject(sandbox, "modular_cooling", Vector.new(-3, 0, 3));
    SandboxUtilities_CreateObject(sandbox, "modular_roof", Vector.new(-2, 0, -4));

    -- brick meshes
    SandboxUtilities_CreateObject(sandbox, "modular_pillar_brick_3", Vector.new(-1, 0, 0));
    SandboxUtilities_CreateObject(sandbox, "modular_pillar_brick_2", Vector.new(-1, 0, 3));
    SandboxUtilities_CreateObject(sandbox, "modular_pillar_brick_1", Vector.new(-1, 0, 6));
    SandboxUtilities_CreateObject(sandbox, "modular_brick_door", Vector.new(1, 0, 0));
    SandboxUtilities_CreateObject(sandbox, "modular_brick_double_window", Vector.new(4, 0, 0));
    SandboxUtilities_CreateObject(sandbox, "modular_brick_small_double_window", Vector.new(7, 0, 0));
    SandboxUtilities_CreateObject(sandbox, "modular_brick_small_window_1", Vector.new(10, 0, 0));
    SandboxUtilities_CreateObject(sandbox, "modular_brick_window_1", Vector.new(13, 0, 0));
    SandboxUtilities_CreateObject(sandbox, "modular_brick_small_window_2", Vector.new(16, 0, 0));
    SandboxUtilities_CreateObject(sandbox, "modular_brick_window_2", Vector.new(18, 0, 0));
    SandboxUtilities_CreateObject(sandbox, "modular_wall_brick_1", Vector.new(1, 0, -6));
    SandboxUtilities_CreateObject(sandbox, "modular_wall_brick_2", Vector.new(4, 0, -6));
    SandboxUtilities_CreateObject(sandbox, "modular_wall_brick_3", Vector.new(6, 0, -6));
    SandboxUtilities_CreateObject(sandbox, "modular_wall_brick_4", Vector.new(8, 0, -6));
    SandboxUtilities_CreateObject(sandbox, "modular_wall_brick_5", Vector.new(11, 0, -6));
    
    -- concrete meshes
    SandboxUtilities_CreateObject(sandbox, "modular_pillar_concrete_3", Vector.new(-2, 0, 0));
    SandboxUtilities_CreateObject(sandbox, "modular_pillar_concrete_2", Vector.new(-2, 0, 3));
    SandboxUtilities_CreateObject(sandbox, "modular_pillar_concrete_1", Vector.new(-2, 0, 6));
    SandboxUtilities_CreateObject(sandbox, "modular_concrete_door", Vector.new(1, 0, 3));
    SandboxUtilities_CreateObject(sandbox, "modular_concrete_double_window", Vector.new(4, 0, 3));
    SandboxUtilities_CreateObject(sandbox, "modular_concrete_small_double_window", Vector.new(7, 0, 3));
    SandboxUtilities_CreateObject(sandbox, "modular_concrete_small_window_1", Vector.new(10, 0, 3));
    SandboxUtilities_CreateObject(sandbox, "modular_concrete_window_1", Vector.new(13, 0, 3));
    SandboxUtilities_CreateObject(sandbox, "modular_concrete_small_window_2", Vector.new(16, 0, 3));
    SandboxUtilities_CreateObject(sandbox, "modular_concrete_window_2", Vector.new(18, 0, 3));
    SandboxUtilities_CreateObject(sandbox, "modular_wall_concrete_1", Vector.new(1, 0, -3));
    SandboxUtilities_CreateObject(sandbox, "modular_wall_concrete_2", Vector.new(4, 0, -3));
    SandboxUtilities_CreateObject(sandbox, "modular_wall_concrete_3", Vector.new(6, 0, -3));
    SandboxUtilities_CreateObject(sandbox, "modular_wall_concrete_4", Vector.new(8, 0, -3));
    SandboxUtilities_CreateObject(sandbox, "modular_wall_concrete_5", Vector.new(11, 0, -3));
    
    -- metal meshes
    SandboxUtilities_CreateObject(sandbox, "modular_hangar_door", Vector.new(1, 0, 6));
    --]]
end

function Sandbox_Update(sandbox, deltaTimeInMillis)
    --GUI_UpdateUI(sandbox);
end
