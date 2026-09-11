-- Sandbox19's single-level relay station. ObjectManager owns every static block.
-- Nobiax meshes/textures retain their CC0 notices under media/{models,textures}.
local Scene = {}

local PILLAR_MESH = "models/nobiax_modular/modular_pillar_concrete_1.mesh"
local ROOF_MESH = "models/nobiax_modular/modular_roof.mesh"
local BLOCK_MESH = "models/nobiax_modular/modular_block.mesh"
local COOLING_MESH = "models/nobiax_modular/modular_cooling.mesh"
local HANGAR_MESH = "models/nobiax_modular/modular_hangar_door.mesh"
local _navMesh = nil
local _debugVisible = false
local _sideWallIds = {}
local _floorIds = {}
local _coverIds = {}

local function _Box(width, height, length, x, y, z, yaw, material)
	-- Procedural geometry and Bullet receive the same dimensions. Unequal boxes
	-- are reserved for simple slabs and housings whose visual alignment is checked
	-- in the product capture; detailed props continue to use authored meshes.
	local block = SandboxObjects:CreateBlockBox(width, height, length, width * 0.25, length * 0.25)
	block:setPosition(Vector3(x, y, z))
	block:setRotation(Vector3(0, yaw or 0, 0))
	block:setMaterial(material)
	block:SetMass(0)
	return block
end

local function _Cube(size, x, y, z, material)
	local block = _Box(size, size, size, x, y, z, 0, material)
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

local function _PaintCorners(cx, cz, halfWidth, halfDepth, material)
	-- Sparse flush corner tiles keep the gameplay zone discoverable without
	-- turning a large luminous outline into the dominant foreground shape.
	for _, corner in ipairs({
		{ -halfWidth, -halfDepth }, { -halfWidth + 0.6, -halfDepth }, { -halfWidth, -halfDepth + 0.6 },
		{ halfWidth, -halfDepth }, { halfWidth - 0.6, -halfDepth }, { halfWidth, -halfDepth + 0.6 },
		{ -halfWidth, halfDepth }, { -halfWidth + 0.6, halfDepth }, { -halfWidth, halfDepth - 0.6 },
		{ halfWidth, halfDepth }, { halfWidth - 0.6, halfDepth }, { halfWidth, halfDepth - 0.6 },
	}) do
		_Cube(0.35, cx + corner[1], -0.165, cz + corner[2], material)
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
		navbounds = { min = Vector3(-24.32, -16, -24), max = Vector3(24.32, 13.5, 40.7) },
		routes = {
			direct = { Vector3(0, 0, -7), Vector3(0, 0, 9), Vector3(0, 0, 23) },
			side = { Vector3(-18, 0, -12), Vector3(-18, 0, 10), Vector3(-18, 0, 22), Vector3(-7, 0, 26) },
		},
	}
end

function Scene.Create()
	_G.SandboxLevelBoxes = {}
	_sideWallIds, _floorIds, _coverIds = {}, {}, {}
	_navMesh, _debugVisible = nil, false

	-- Pitch raises the generated mountain belt behind the relay silhouette.
	SandboxScene:SetSkyBox("Relay/Sky", Vector3(-12, 180, 0))
	SandboxScene:SetAmbientLight(Vector3(0.32, 0.30, 0.27))
	local sunlight = SandboxScene:CreateDirectionalLight(Vector3(-0.55, -1, -0.20))
	sunlight:setDiffuseColour(ColourValue(1.16, 1.02, 0.82))
	sunlight:setSpecularColour(ColourValue(0.16, 0.14, 0.11))

	-- 48 x 64 m floor; all visible surfaces have matching static Bullet bodies.
	for _, x in ipairs({ -16, 0, 16 }) do
		for _, z in ipairs({ -16, 0, 16, 32 }) do
			local material = x == 0 and "Relay/MainRoute" or "Relay/Ground"
			local floor = _Cube(16, x, -8, z, material)
			_floorIds[floor:GetObjId()] = true
		end
	end

	-- Continuous low perimeter slabs replace the former fence of repeated panels.
	-- Their 2.4 m top reveals the generated desert horizon from the follow camera.
	_Box(0.55, 2.4, 64, -23.72, 1.2, 8, 0, "Relay/ConcreteShade")
	_Box(0.55, 2.4, 64, 23.72, 1.2, 8, 0, "Relay/Concrete")
	_Box(48, 2.4, 0.55, 0, 1.2, -23.72, 0, "Relay/ConcreteShade")
	_Box(15.5, 2.4, 0.55, -16.25, 1.2, 39.72, 0, "Relay/Concrete")
	_Box(15.5, 2.4, 0.55, 16.25, 1.2, 39.72, 0, "Relay/ConcreteShade")
	for _, z in ipairs({ 0, 16, 32 }) do
		_Module(PILLAR_MESH, -23.35, 1.28, z, 0, "Relay/Metal")
		_Module(PILLAR_MESH, 23.35, 1.28, z, 0, "Relay/Metal")
	end

	-- The western bypass is a real line-of-sight break, open at both ends.
	-- Main route remains x=-8..8; both join the north yard before the relay.
	local bypassWall = _Box(0.55, 2.6, 25.6, -12, 1.3, 4.8, 0, "Relay/ConcreteShade")
	_sideWallIds[bypassWall:GetObjId()] = true
	-- Service bay to the east and two pillars identify the courtyard threshold.
	_Box(0.55, 2.6, 12.8, 13, 1.3, 10.24, 0, "Relay/Concrete")
	for _, x in ipairs({ -8, 8 }) do
		_Module(PILLAR_MESH, x, 1.28, 20, 0, "Relay/Metal")
	end
	_Box(10.24, 2.6, 0.55, 17.92, 1.3, 20, 0, "Relay/ConcreteShade")
	-- Two solid service units sit behind the eastern bay wall. Their independent
	-- 3.17 m roof panels are inaccessible and each is below the nav region cutoff.
	-- The validated main lane and western bypass keep their original geometry.
	for _, z in ipairs({ 9, 15 }) do
		_Cube(2, 18, 1, z, "Relay/Cover")
		_Module(ROOF_MESH, 18, 2.14, z, 0, "Relay/Equipment")
		_Module(COOLING_MESH, 18, 1.15, z - 1.05, 180, "Relay/Equipment")
	end
	for _, z in ipairs({ 10.8, 11.7 }) do
		_Module(BLOCK_MESH, 20.6, 0.32, z, 0, "Relay/Equipment")
	end

	-- Long low barriers replace clusters of identical cubes. They stay outside
	-- the x=-8..8 main lane and leave the x=-18 bypass open.
	for _, cover in ipairs({
		{ -10.6, -8.5, -4 }, { 10.6, -8.5, 4 },
		{ -10.3, 5.5, 3 }, { 10.3, 5.5, -3 },
		{ -10.1, 23.5, -5 }, { 10.1, 23.5, 5 },
	}) do
		local barrier = _Box(4.2, 1.25, 1.15, cover[1], 0.625, cover[2], cover[3], "Relay/Cover")
		_coverIds[barrier:GetObjId()] = true
	end

	-- Open service canopies frame the near lane. Their posts stay outside both
	-- validated routes; the roof collision remains above actor height.
	for _, x in ipairs({ -18.0 }) do
		for _, z in ipairs({ -7.0, -3.8, -0.6 }) do
			_Module(ROOF_MESH, x, 2.68, z, 0, "Relay/Equipment")
		end
		for _, px in ipairs({ x - 2.4, x + 2.4 }) do
			for _, pz in ipairs({ -8.6, 1.0 }) do
				_Module(PILLAR_MESH, px, 1.28, pz, 0, "Relay/Metal")
			end
		end
	end
	for _, prop in ipairs({
		{ -20.2, -10.2, 0 }, { -20.2, -9.2, 90 }, { -15.8, -5.0, 0 },
		{ 20.2, -10.2, 0 }, { 20.2, -9.2, 90 }, { 15.8, -5.0, 0 },
		{ -17.6, 12.5, 0 }, { 17.6, 12.5, 0 },
	}) do
		_Module(BLOCK_MESH, prop[1], 0.32, prop[2], prop[3], "Relay/Equipment")
	end

	-- A stepped relay building gives the destination a readable silhouette:
	-- low wings, central gatehouse, rooftop plant and a narrow mast.
	_Box(6.2, 3.4, 3.0, -5.7, 1.7, 39.0, 0, "Relay/Concrete")
	_Box(6.2, 3.4, 3.0, 5.7, 1.7, 39.0, 0, "Relay/ConcreteShade")
	_Box(5.4, 4.8, 3.4, 0, 2.4, 39.1, 0, "Relay/Concrete")
	_Box(4.4, 2.4, 3.0, 0, 6.0, 39.25, 0, "Relay/ConcreteShade")
	_Box(5.6, 0.20, 0.18, 0, 3.65, 37.36, 0, "Relay/Accent")
	_Module(HANGAR_MESH, 0, 1.28, 37.38, 0, "Relay/Equipment")
	for _, x in ipairs({ -5.8, 5.8 }) do
		_Module(COOLING_MESH, x, 1.20, 37.42, 180, "Relay/Metal")
		_Module(ROOF_MESH, x, 3.54, 39.0, 0, "Relay/Equipment")
	end
	for _, x in ipairs({ -1.45, 1.45 }) do
		_Module(COOLING_MESH, x, 5.15, 39.1, 0, "Relay/Equipment")
	end
	for _, y in ipairs({ 7.2, 9.7 }) do
		_Module(PILLAR_MESH, 0, y, 39.25, 0, "Relay/Metal")
	end

	_PaintCorners(0, -16, 5, 4, "Relay/SafeMark")
	_PaintCorners(0, 33.5, 5, 3, "Relay/GoalMark")
	-- Short inset dashes lead through the main entrance without visual debug lines.
	for _, z in ipairs({ -7, -3, 1, 5, 9, 13, 17, 21, 25, 29 }) do
		_Cube(0.5, 0, -0.24, z, "Relay/RouteMark")
	end

	SandboxScene:UpdateSceneGraph()
	print("[Sandbox19Scene] relay-station size=48x64 routes=2 floor=0 side-wall=solid composition=desert-layered")
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
	local coverHit = SandboxRaycast:RayCastObjectId(Vector3(-14, 0.8, -8.5), Vector3(-7.5, 0.8, -8.5))
	local coverSolid = _coverIds[coverHit] == true
	print("[Sandbox19ArenaSelfTest] " .. (coverSolid and "PASS" or "FAIL") .. " rectangular-cover-ray")
	pass = coverSolid and pass
	local floorHit = SandboxRaycast:RayCastObjectId(Vector3(2, 3, 2), Vector3(2, -2, 2))
	local floorSolid = _floorIds[floorHit] == true
	print("[Sandbox19ArenaSelfTest] " .. (floorSolid and "PASS" or "FAIL") .. " floor-collision")
	pass = floorSolid and pass
	print("[Sandbox19ArenaSelfTest] " .. (pass and "PASS" or "FAIL") .. " relay-static-collision")
	return pass
end

return Scene
