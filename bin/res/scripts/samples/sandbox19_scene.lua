-- Sandbox19's single-level relay station. ObjectManager owns every static block.
-- Nobiax meshes/textures retain their CC0 notices under media/{models,textures}.
local Scene = {}

local WALL_MESH = "models/nobiax_modular/modular_wall_concrete_1.mesh"
local PILLAR_MESH = "models/nobiax_modular/modular_pillar_concrete_1.mesh"
local _navMesh = nil
local _debugVisible = false
local _sideWallIds = {}
local _floorIds = {}

local function _Cube(size, x, y, z, material)
	-- Keep the proven equal-sided procedural path; unequal dimensions currently
	-- have a render/physics mismatch. Submerged cubes provide a continuous floor.
	local block = SandboxObjects:CreateBlockBox(size, size, size, size * 0.25, size * 0.25)
	block:setPosition(Vector3(x, y, z))
	block:setRotation(Vector3(0, 0, 0))
	block:setMaterial(material)
	block:SetMass(0)
	table.insert(_G.SandboxLevelBoxes, {
		size = size, position = Vector3(x, y, z), rotation = Vector3(0, 0, 0),
	})
	return block
end

local function _Module(mesh, x, y, z, yaw, material)
	-- CreateBlockObject derives a Bullet convex hull from this same mesh.
	-- Only solid panels/pillars are used: a convex hull would close a door opening.
	local block = SandboxObjects:CreateBlockObject(mesh)
	block:setPosition(Vector3(x, y, z))
	block:setRotation(Vector3(0, yaw or 0, 0))
	block:setMaterial(material or "Relay/Concrete")
	block:SetMass(0)
	return block
end

local function _Wall(x, z, yaw)
	-- Measured mesh bounds: x=-1.277934..1.282066, y=-1.268740..1.291260,
	-- z=-0.16..0.16. Its lower edge is sunk 1 cm; no walkable wall-top survives
	-- the 0.4 m navmesh erosion radius.
	return _Module(WALL_MESH, x, 1.25874, z, yaw, "Relay/Concrete")
end

local function _PaintSquare(cx, cz, halfWidth, halfDepth, material)
	-- Flush inlaid paint tiles: 1 cm rise is below the navigation cell height and
	-- does not create a step or pretend to implement cover/interaction mechanics.
	for x = -halfWidth, halfWidth do
		_Cube(1, cx + x, -0.49, cz - halfDepth, material)
		_Cube(1, cx + x, -0.49, cz + halfDepth, material)
	end
	for z = -halfDepth + 1, halfDepth - 1 do
		_Cube(1, cx - halfWidth, -0.49, cz + z, material)
		_Cube(1, cx + halfWidth, -0.49, cz + z, material)
	end
end

local function _Anchors()
	return {
		commander = Vector3(0, 0, -18),
		allies = { Vector3(-3, 0, -14), Vector3(3, 0, -14) },
		spawnPoints = {
			Vector3(0, 0, -18), Vector3(-3, 0, -14), Vector3(3, 0, -14),
			Vector3(-4, 0, 6), Vector3(4, 0, 6),
			Vector3(-4, 0, 32), Vector3(4, 0, 32),
		},
		fallback = Vector3(0, 0, -16),
		goal = Vector3(0, 0, 35),
		goalRadius = 5,
		trigger = { center = Vector3(0, 0, 19), z = 19 },
		navbounds = { min = Vector3(-24.32, -16, -24), max = Vector3(24.32, 5.5, 40.7) },
		routes = {
			direct = { Vector3(0, 0, -7), Vector3(0, 0, 9), Vector3(0, 0, 23) },
			side = { Vector3(-18, 0, -12), Vector3(-18, 0, 10), Vector3(-18, 0, 22), Vector3(-7, 0, 26) },
		},
	}
end

function Scene.Create()
	_G.SandboxLevelBoxes = {}
	_sideWallIds, _floorIds = {}, {}
	_navMesh, _debugVisible = nil, false

	SandboxScene:SetSkyBox("Relay/Sky", Vector3(0, 180, 0))
	SandboxScene:SetAmbientLight(Vector3(0.56, 0.58, 0.60))
	local sunlight = SandboxScene:CreateDirectionalLight(Vector3(-0.45, -1, 0.35))
	sunlight:setDiffuseColour(ColourValue(0.86, 0.80, 0.69))
	sunlight:setSpecularColour(ColourValue(0.12, 0.12, 0.11))

	-- 48 x 64 m floor; all visible surfaces have matching static Bullet bodies.
	for _, x in ipairs({ -16, 0, 16 }) do
		for _, z in ipairs({ -16, 0, 16, 32 }) do
			local material = x == 0 and "Relay/MainRoute" or "Relay/Ground"
			local floor = _Cube(16, x, -8, z, material)
			_floorIds[floor:GetObjId()] = true
		end
	end

	-- An enclosed yard grounds the scene visually; no floating platform edges.
	for index = 0, 24 do
		local z = -22.72 + index * 2.56
		_Wall(-23.60, z, 90)
		_Wall(23.60, z, 90)
	end
	for index = 0, 18 do
		local x = -23.04 + index * 2.56
		_Wall(x, -23.70, 0)
		_Wall(x, 39.70, 0)
	end

	-- The western bypass is a real line-of-sight break, open at both ends.
	-- Main route remains x=-8..8; both join the north yard before the relay.
	for index = 0, 9 do
		local wall = _Wall(-12, -6.72 + index * 2.56, 90)
		_sideWallIds[wall:GetObjId()] = true
	end
	-- Service bay to the east and two pillars identify the courtyard threshold.
	for index = 0, 4 do _Wall(13, 5.12 + index * 2.56, 90) end
	for _, x in ipairs({ -8, 8 }) do
		_Module(PILLAR_MESH, x, 1.28, 20, 0, "Relay/Metal")
	end
	for index = 0, 3 do _Wall(14.08 + index * 2.56, 20, 0) end
	-- Two solid service units sit behind the eastern bay wall. Their independent
	-- 3.17 m roof panels are inaccessible and each is below the nav region cutoff.
	-- The validated main lane and western bypass keep their original geometry.
	for _, z in ipairs({ 9, 15 }) do
		_Cube(2, 18, 1, z, "Relay/Concrete")
		_Module("models/nobiax_modular/modular_roof.mesh", 18, 2.14, z, 0, "Relay/Equipment")
		_Module("models/nobiax_modular/modular_cooling.mesh", 18, 1.15, z - 1.05, 180, "Relay/Equipment")
	end
	for _, z in ipairs({ 10.8, 11.7 }) do
		_Module("models/nobiax_modular/modular_block.mesh", 20.6, 0.32, z, 0, "Relay/Equipment")
	end

	-- The relay facade marks a destination, not an enterable fake doorway.
	-- Its front edge is beyond the goal centre, leaving a full-sized assembly pad.
	for _, x in ipairs({ -5.12, -2.56, 0, 2.56, 5.12 }) do
		_Wall(x, 38.1, 0)
	end
	for _, x in ipairs({ -6.7, 6.7 }) do
		_Module(PILLAR_MESH, x, 1.28, 38.1, 0, "Relay/Metal")
		_Module(PILLAR_MESH, x, 3.84, 38.1, 0, "Relay/Metal")
	end
	-- Solid relay housings begin at z=38.1, beyond the assembly pad. Separate
	-- roof caps retain a low-complexity skyline without a second walking level.
	for _, x in ipairs({ -4, 0, 4 }) do
		_Cube(2, x, 1, 39.1, "Relay/Concrete")
		_Cube(2, x, 3, 39.1, "Relay/Concrete")
		_Module("models/nobiax_modular/modular_roof.mesh", x, 4.14, 39.1, 0, "Relay/Equipment")
	end
	-- A visibly closed sage panel belongs to the solid facade; it is not a portal.
	_Module("models/nobiax_modular/modular_hangar_door.mesh", 0, 1.28, 37.90, 0, "Relay/Equipment")
	_Module("models/nobiax_modular/modular_cooling.mesh", -5.1, 1.20, 37.65, 180, "Relay/Metal")
	_Module("models/nobiax_modular/modular_cooling.mesh", 5.1, 1.20, 37.65, 180, "Relay/Metal")

	_PaintSquare(0, -16, 5, 4, "Relay/SafeMark")
	_PaintSquare(0, 33.5, 5, 3, "Relay/GoalMark")
	-- Short inset dashes lead through the main entrance without visual debug lines.
	for _, z in ipairs({ -7, -3, 1, 5, 9, 13, 17, 21, 25, 29 }) do
		_Cube(0.5, 0, -0.24, z, "Relay/RouteMark")
	end

	SandboxScene:UpdateSceneGraph()
	print("[Sandbox19Scene] relay-station size=48x64 routes=2 floor=0 side-wall=solid")
	return _Anchors()
end

function Scene.CreateNavigation()
	local config = rcConfig()
	SandboxNav:DefaultConfig(config)
	SandboxNav:ApplySettingConfig(config, 0.0, 0.4, 0.2)
	config.minRegionArea = math.pow(50, 2)
	config.walkableSlopeAngle = 45
	-- NavBuilder fits actual mesh bounds, including the floor and static walls.
	_navMesh = SandboxNav:CreateNavigationMesh(config, "default")
	_debugVisible = false
	if _navMesh ~= nil then _navMesh:SetDebugVisible(false) end
	return _navMesh
end

function Scene.ToggleNavigationDebug()
	_debugVisible = not _debugVisible
	if _navMesh ~= nil then _navMesh:SetDebugVisible(_debugVisible) end
	return _debugVisible
end

function Scene.SetNavigationDebug(visible)
	_debugVisible = visible == true
	if _navMesh ~= nil then _navMesh:SetDebugVisible(_debugVisible) end
end

local function _CheckPath(label, start, target)
	local projected = SandboxNav:FindClosestPoint("default", target)
	local path = std.vector_Ogre__Vector3_()
	local found = SandboxNav:FindPath("default", start, target, path)
	local pass = (projected - target):squaredLength() < 1
		and found and path:size() > 0
		and (path[path:size() - 1] - target):squaredLength() < 1
	print("[Sandbox19ArenaSelfTest] " .. (pass and "PASS" or "FAIL") .. " " .. label)
	return pass
end

function Scene.ValidateNavigation(anchors)
	local a = anchors or _Anchors()
	local pass = true
	for index, target in ipairs(a.spawnPoints) do
		pass = _CheckPath("spawn=" .. index, a.commander, target) and pass
	end
	pass = _CheckPath("goal", a.commander, a.goal) and pass
	pass = _CheckPath("fallback", a.goal, a.fallback) and pass
	for name, route in pairs(a.routes) do
		local previous = a.commander
		for index, target in ipairs(route) do
			pass = _CheckPath("route=" .. name .. " segment=" .. index, previous, target) and pass
			previous = target
		end
		pass = _CheckPath("route=" .. name .. " goal", previous, a.goal) and pass
	end
	print("[Sandbox19ArenaSelfTest] " .. (pass and "PASS" or "FAIL") .. " all-spawns-connected relay-routes")
	return pass
end

function Scene.ValidateCollision()
	local pass = true
	-- Real Bullet rays at two eye/muzzle heights distinguish blocking from paint.
	-- Run after the first physics step has refreshed static bodies' broadphase
	-- bounds. Exact block ids ensure actors cannot satisfy wall/floor assertions.
	for _, height in ipairs({ 0.8, 1.5 }) do
		local hit = SandboxRaycast:RayCastObjectId(Vector3(-18, height, 4), Vector3(-6, height, 4))
		local blocked = _sideWallIds[hit] == true
		print("[Sandbox19ArenaSelfTest] " .. (blocked and "PASS" or "FAIL") .. " side-wall-ray height=" .. height)
		pass = blocked and pass
	end
	local directHit = SandboxRaycast:RayCastObjectId(Vector3(1, 1.5, -12), Vector3(1, 1.5, 24))
	local directClear = directHit == 0
	print("[Sandbox19ArenaSelfTest] " .. (directClear and "PASS" or "FAIL") .. " direct-line-clear")
	pass = directClear and pass
	local floorHit = SandboxRaycast:RayCastObjectId(Vector3(2, 3, 2), Vector3(2, -2, 2))
	local floorSolid = _floorIds[floorHit] == true
	print("[Sandbox19ArenaSelfTest] " .. (floorSolid and "PASS" or "FAIL") .. " floor-collision")
	pass = floorSolid and pass
	print("[Sandbox19ArenaSelfTest] " .. (pass and "PASS" or "FAIL") .. " relay-static-collision")
	return pass
end

return Scene
