
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
		modular_block = 	{file = "models/nobiax_modular/modular_block.mesh", mass = 15},
		modular_block = 	{file = "models/nobiax_modular/modular_block.mesh", mass = 15},
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
    if fileMass >= 0 then object:setMass(fileMass); end
    
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
