
function Vector2(...)
	return Ogre.Vector2(unpack{...})
end

function Vector3(...)
	return Ogre.Vector3(unpack{...})
end

function Quaternion(xr, yr, zr)
	return QuaternionFromRotationDegrees(xr, yr, zr)
end

NewSandboxUtilities = {
	Objects = {
        modular_block = 	{file = "models/nobiax_modular/modular_block.mesh", mass = 15},
		modular_cooling = 	{file = "models/nobiax_modular/modular_cooling.mesh", mass = 15},
		modular_roof = 		{file = "models/nobiax_modular/modular_roof.mesh", mass = 50},
		modular_pillar_brick_1 = {file = "models/nobiax_modular/modular_pillar_brick_1.mesh", mass = 40},
		modular_pillar_brick_2 = {file = "models/nobiax_modular/modular_pillar_brick_2.mesh", mass = 30},
		modular_pillar_brick_3 = {file = "models/nobiax_modular/modular_pillar_brick_3.mesh", mass = 20},
		modular_brick_door = 	 {file = "models/nobiax_modular/modular_brick_door.mesh", mass = 80},
		modular_brick_double_window = 	{file = "models/nobiax_modular/modular_brick_double_window.mesh", mass = 80},
		modular_brick_small_double_window = {file = "models/nobiax_modular/modular_brick_small_double_window.mesh", mass = 80},
		modular_brick_small_window_1 = 	{file = "models/nobiax_modular/modular_brick_small_window_1.mesh", mass = 80},
		modular_brick_small_window_2 = 	{file = "models/nobiax_modular/modular_brick_small_window_2.mesh", mass = 40},
		modular_brick_window_1 = 	{file = "models/nobiax_modular/modular_brick_window_1.mesh", mass = 40},
		modular_brick_window_2 = 	{file = "models/nobiax_modular/modular_brick_window_2.mesh", mass = 40},
		modular_wall_brick_1 = {file = "models/nobiax_modular/modular_wall_brick_1.mesh", mass = 80},
		modular_wall_brick_2 = {file = "models/nobiax_modular/modular_wall_brick_2.mesh", mass = 40},
		modular_wall_brick_3 = {file = "models/nobiax_modular/modular_wall_brick_3.mesh", mass = 30},
		modular_wall_brick_4 = {file = "models/nobiax_modular/modular_wall_brick_4.mesh", mass = 30},
		modular_wall_brick_5 = {file = "models/nobiax_modular/modular_wall_brick_5.mesh", mass = 20},
		modular_pillar_concrete_3 = {file = "models/nobiax_modular/modular_pillar_concrete_3.mesh", mass = 20},
		modular_pillar_concrete_2 = {file = "models/nobiax_modular/modular_pillar_concrete_2.mesh", mass = 30},
		modular_pillar_concrete_1 = {file = "models/nobiax_modular/modular_pillar_concrete_1.mesh", mass = 40},
		modular_concrete_door = 	{file = "models/nobiax_modular/modular_concrete_door.mesh", mass = 80},
		modular_concrete_double_window = 	{file = "models/nobiax_modular/modular_concrete_double_window.mesh", mass = 80},
		modular_concrete_small_double_window = {file = "models/nobiax_modular/modular_concrete_small_double_window.mesh", mass = 80},
		modular_concrete_small_window_1 = 	{file = "models/nobiax_modular/modular_concrete_small_window_1.mesh", mass = 80},
		modular_concrete_window_1 = 		{file = "models/nobiax_modular/modular_concrete_window_1.mesh", mass = 80},
		modular_concrete_small_window_2 = 	{file = "models/nobiax_modular/modular_concrete_small_window_2.mesh", mass = 40},
		modular_concrete_window_2 = 	{file = "models/nobiax_modular/modular_concrete_window_2.mesh", mass = 40},
		modular_wall_concrete_1 = 	{file = "models/nobiax_modular/modular_wall_concrete_1.mesh", mass = 80},
		modular_wall_concrete_2 = 	{file = "models/nobiax_modular/modular_wall_concrete_2.mesh", mass = 40},
		modular_wall_concrete_3 = 	{file = "models/nobiax_modular/modular_wall_concrete_3.mesh", mass = 30},
		modular_wall_concrete_4 =   {file = "models/nobiax_modular/modular_wall_concrete_4.mesh", mass = 30},
		modular_wall_concrete_5 = 	{file = "models/nobiax_modular/modular_wall_concrete_5.mesh", mass = 20},
		modular_hangar_door = 		{file = "models/nobiax_modular/modular_hangar_door.mesh", mass = 100},
	}
}

function CreateBlockObject(objectName, position, rotation)
    if NewSandboxUtilities.Objects[objectName] == nil then
        return;
    end

    local filePath = NewSandboxUtilities.Objects[objectName].file;
    local fileMass = NewSandboxUtilities.Objects[objectName].mass;

    local object = Sandbox:CreateBlockObject(filePath);
    if fileMass >= 0 then object:SetMass(fileMass); end
    
    if position ~= nil then object:setPosition(position); end
    
    if rotation ~= nil then object:setRotation(rotation); end
    
    return object;
end


function DistanceSquared(pos1, pos2)
	return pos1:squaredDistance(pos2)
end

Vector = Ogre.Vector3
function Vector.Normalize(vec3)
	return vec3:normalisedCopy()
end

function Vector.Length(vec3)
	return vec3:length()
end

function Vector.LengthSquared(vec3)
	return vec3:squaredLength()
end

function Vector.tostring(vec3)
	return string.format('(%.2f, %.2f, %.2f)', vec3.x, vec3.y, vec3.z)
end

function ColourValue(...)
	return Ogre.ColourValue(unpack{...})
end

UtilColors = {
	Black 	= 	ColourValue(0, 0, 0);
	Blue 	= 	ColourValue(0, 0, 1);
	Green 	= 	ColourValue(0, 1, 0);
	Orange 	= 	ColourValue(1, 0.5, 0);
	Red 	= 	ColourValue(1, 0, 0);
	Yellow 	= 	ColourValue(1, 1, 0);
	White 	= 	ColourValue(1, 1, 1);
};


function CreateLevelBox(size, position, rotation)
	local width, height, length = size, size, size
	local uTile, vTile = size * 0.5, size * 0.5
    local object = Sandbox:CreateBlockBox(width, height, length, uTile, vTile);
	object:setPosition(position);
	object:setRotation(rotation);
    object:setMaterial("Ground2");
    object:SetMass(0)
end

function SandboxUtilities_CreateLevel()
    local level = {
        { 20, Vector3(0, -10, 0), Vector3(0, 0, 0) },
        { 20, Vector3(0, -10, 20), Vector3(0, 0, 0) },
        { 20, Vector3(-20, -10, 20), Vector3(0, 0, 0) },
        { 20, Vector3(24, -10, 0), Vector3(0, 0, 0) },
        { 20, Vector3(24, -10, 20), Vector3(0, 0, 0) },
        { 20, Vector3(24, -10, 40), Vector3(0, 0, 0) },
        { 20, Vector3(24, -10, 60), Vector3(0, 0, 0) },
        { 4, Vector3(12, -2, -4), Vector3(0, 0, 0) },
        { 4, Vector3(12, -2, 4), Vector3(0, 0, 0) },
        { 20, Vector3(48, -10, 0), Vector3(0, 0, 0) },
        { 20, Vector3(48, -10, 20), Vector3(0, 0, 0) },
        { 4, Vector3(36, -4, -8), Vector3(0, 0, 0) },
        { 4, Vector3(36, -4, -4), Vector3(0, 0, 0) },
        { 4, Vector3(36, -4, 0), Vector3(0, 0, 0) },
        { 4, Vector3(36, -4, 4), Vector3(0, 0, 0) },
        { 4, Vector3(36, -4, 8), Vector3(0, 0, 0) },
        { 4, Vector3(36, -4, 12), Vector3(0, 0, 0) },
        { 4, Vector3(36, -4, 16), Vector3(0, 0, 0) },
        { 4, Vector3(36, -4, 20), Vector3(0, 0, 0) },
        { 4, Vector3(36, -4, 24), Vector3(0, 0, 0) },
        { 4, Vector3(24, 2, -8), Vector3(0, 0, 0) },
        { 4, Vector3(24, 2, -4), Vector3(0, 0, 0) },
        { 4, Vector3(24, 2, 0), Vector3(0, 0, 0) },
        { 4, Vector3(24, 2, 4), Vector3(0, 0, 0) },
        { 4, Vector3(24, 2, 8), Vector3(0, 0, 0) },
        { 8, Vector3(24, 0, 16), Vector3(0, 0, 0) },
        { 8, Vector3(24, 0, 24), Vector3(0, 0, 0) },
        { 8, Vector3(24, 0, 32), Vector3(0, 0, 0) },
        { 4, Vector3(24, 1.55, 37.41), Vector3(15, 0, 0) },
        { 4, Vector3(24, 0.515, 41.272), Vector3(15, 0, 0) },
        { 4, Vector3(24, -0.52, 45.134), Vector3(15, 0, 0) },
        { 4, Vector3(24, -1.555, 48.996), Vector3(15, 0, 0) },
        { 2, Vector3(20, 1, 15), Vector3(0, 0, 0) },
        { 2, Vector3(17, 1, 15), Vector3(0, 0, 0) },
        { 2, Vector3(15, 1, 15), Vector3(0, 0, 0) },
        { 2, Vector3(19, 3, 15), Vector3(0, 0, 0) },
        { 2, Vector3(17, 3, 15), Vector3(0, 0, 0) },
        { 2, Vector3(15, 3, 15), Vector3(0, 0, 0) },
        { 2, Vector3(17, 0, 19), Vector3(0, 0, 0) },
        { 2, Vector3(19, 0, 19), Vector3(0, 0, 0) },
        { 2, Vector3(17, 1, 23), Vector3(0, 0, 0) },
        { 2, Vector3(15, 1, 23), Vector3(0, 0, 0) },
        { 2, Vector3(17, 1, 27), Vector3(0, 0, 0) },
        { 2, Vector3(19, 1, 27), Vector3(0, 0, 0) },
        { 2, Vector3(9, 3, 27), Vector3(0, 0, 0) },
        { 2, Vector3(11, 3, 27), Vector3(0, 0, 0) },
        { 2, Vector3(13, 3, 27), Vector3(0, 0, 0) },
        { 2, Vector3(15, 3, 27), Vector3(0, 0, 0) },
        { 2, Vector3(17, 3, 27), Vector3(0, 0, 0) },
        { 2, Vector3(19, 3, 27), Vector3(0, 0, 0) },
        { 6, Vector3(5, 1, 25), Vector3(0, 0, 0) },
        { 6, Vector3(-1, 1, 25), Vector3(0, 0, 0) },
        { 6, Vector3(-7, 1, 25), Vector3(0, 0, 0) },
        { 2, Vector3(-10.71, 2.77, 25), Vector3(0, 0, 15) },
        { 2, Vector3(-12.64, 2.252, 25), Vector3(0, 0, 15) },
        { 2, Vector3(-14.57, 1.734, 25), Vector3(0, 0, 15) },
        { 2, Vector3(-16.5, 1.216, 25), Vector3(0, 0, 15) },
        { 2, Vector3(-18.43, 0.698, 25), Vector3(0, 0, 15) },
        { 2, Vector3(-20.36, 0.18, 25), Vector3(0, 0, 15) },
        { 2, Vector3(-22.29, -0.338, 25), Vector3(0, 0, 15) },
        { 2, Vector3(-24.22, -0.856, 25), Vector3(0, 0, 15) },
        { 2, Vector3(-7, 4, 25), Vector3(0, 0, 0) },
        { 2, Vector3(-3, 5, 23), Vector3(0, 0, 0) },
        { 2, Vector3(-1, 5, 23), Vector3(0, 0, 0) },
        { 2, Vector3(1, 5, 23), Vector3(0, 0, 0) },
        { 2, Vector3(5, 4, 25), Vector3(0, 0, 0) },
        { 2, Vector3(27, 3, 9), Vector3(0, 0, 0) },
        { 2, Vector3(29, 3, 9), Vector3(0, 0, 0) },
        { 2, Vector3(31, 3, 9), Vector3(0, 0, 0) },
        { 2, Vector3(33, 3, 9), Vector3(0, 0, 0) },
        { 2, Vector3(35, 3, 9), Vector3(0, 0, 0) },
        { 2, Vector3(37, 3, 9), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 9), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 7), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 11), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 13), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 15), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 17), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 19), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 21), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 23), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 25), Vector3(0, 0, 0) },
        { 2, Vector3(39, 3, 27), Vector3(0, 0, 0) },
        { 2, Vector3(39, 5, 17), Vector3(0, 0, 0) },
        { 2, Vector3(39, 5, 19), Vector3(0, 0, 0) },
        { 2, Vector3(39, 5, 21), Vector3(0, 0, 0) },
        { 2, Vector3(43, 5, 13), Vector3(0, 0, 0) },
        { 2, Vector3(7, 3, 21), Vector3(0, 0, 0) },
        { 2, Vector3(5, 3, 21), Vector3(0, 0, 0) },
        { 2, Vector3(-21, -0.5, 19), Vector3(0, 0, 0) },
        { 2, Vector3(-22.705, -0.725, 19), Vector3(0, 0, 15) },
        { 2, Vector3(-19.295, -0.725, 19), Vector3(0, 0, -15) },
        { 2, Vector3(-21, -0.725, 20.705), Vector3(15, 0, 0) },
        { 2, Vector3(-21, -0.725, 17.295), Vector3(-15, 0, 0) },
        { 6, Vector3(43, 3, 9), Vector3(0, 0, 0) },
        { 6, Vector3(49, 3, 9), Vector3(0, 0, 0) },
        { 8, Vector3(48, 2, 16), Vector3(0, 0, 0) },
        { 8, Vector3(48, 0, 24), Vector3(0, 0, 0) },
        { 4, Vector3(42, 2, 14), Vector3(0, 0, 0) },
        { 4, Vector3(42, 2, 18), Vector3(0, 0, 0) },
        { 4, Vector3(42, 2, 22), Vector3(0, 0, 0) },
        { 4, Vector3(42, 2, 26), Vector3(0, 0, 0) },
        { 4, Vector3(48, 3.268, 20.731), Vector3(30, 0, 0) },
        { 2, Vector3(-13, -0.5, 15), Vector3(0, 0, 0) },
        { 2, Vector3(-14.705, -0.725, 15), Vector3(0, 0, 15) },
        { 2, Vector3(-11.295, -0.725, 15), Vector3(0, 0, -15) },
        { 2, Vector3(-13, -0.725, 16.705), Vector3(15, 0, 0) },
        { 2, Vector3(-13, -0.725, 13.295), Vector3(-15, 0, 0) },
        { 2, Vector3(45, 6, 9), Vector3(0, 0, 0) },
        { 2, Vector3(46.365, 5.635, 9), Vector3(0, 0, -30) },
        { 2, Vector3(43.635, 5.635, 9), Vector3(0, 0, 30) },
        { 2, Vector3(45, 5.635, 10.365), Vector3(30, 0, 0) },
        { 2, Vector3(45, 5.635, 7.635), Vector3(-30, 0, 0) },
    }
    
    for index = 1, #level do
        local block = level[index];
        CreateLevelBox(block[1], block[2], block[3]);
    end
end