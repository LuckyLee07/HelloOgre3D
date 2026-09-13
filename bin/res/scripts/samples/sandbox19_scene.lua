-- Sandbox19's single-level relay station. ObjectManager owns every static block.
-- Nobiax meshes/textures retain their CC0 notices under media/{models,textures}.
local Scene = {}

local PILLAR_MESH = "models/nobiax_modular/modular_pillar_concrete_1.mesh"
local ROOF_MESH = "models/nobiax_modular/modular_roof.mesh"
local BLOCK_MESH = "models/nobiax_modular/modular_block.mesh"
local COOLING_MESH = "models/nobiax_modular/modular_cooling.mesh"
local HANGAR_MESH = "models/nobiax_modular/modular_hangar_door.mesh"
local WINDOW_MESH = "models/nobiax_modular/modular_concrete_small_window_1.mesh"
local _navMesh = nil
local _debugVisible = false
local _sideWallIds = {}
local _floorIds = {}
local _coverIds = {}
local _supplyIds = {}
local _relayFeedback = {}

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

local function _Asset(mesh, x, y, z, yaw)
	-- Preserve the authored submesh materials; CreateBlockObject derives its
	-- solid Bullet hull from these same vertices and ObjectManager owns it.
	local block = SandboxObjects:CreateBlockObject("models/sandbox19/" .. mesh)
	block:setPosition(Vector3(x, y, z))
	block:setRotation(Vector3(0, yaw or 0, 0))
	block:SetMass(0)
	return block
end

local function _VisualPlane(width, height, x, y, z, pitch, yaw, material)
	local plane = SandboxObjects:CreateVisualPlane(width, height)
	plane:setPosition(Vector3(x, y, z))
	plane:setRotation(Vector3(pitch or 0, yaw or 0, 0))
	plane:setMaterial(material)
	return plane
end

local function _TrackFeedback(group, object)
	local items = _relayFeedback[group]
	items[#items + 1] = {object = object, material = nil}
	return object
end

local function _SetFeedbackMaterial(group, material)
	for _, item in ipairs(_relayFeedback[group] or {}) do
		if item.material ~= material then
			item.object:setMaterial(material)
			item.material = material
		end
	end
end

local function _GroundLayer(width, length, x, z, yaw, material)
	-- Kept a few millimetres above the floor. These are render-only planes, so
	-- they cannot alter Bullet contacts, navmesh rasterisation, or weapon rays.
	return _VisualPlane(width, length, x, 0.014, z, 0, yaw, material)
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
	_sideWallIds, _floorIds, _coverIds, _supplyIds = {}, {}, {}, {}
	_relayFeedback = {beacons = {}, strips = {}, beam = {}, door = {}, goal = {}, threshold = {}, phase = "", mode = "off"}
	_navMesh, _debugVisible = nil, false

	-- Pitch raises the generated mountain belt behind the relay silhouette.
	SandboxScene:SetSkyBox("Relay/Sky", Vector3(-12, 180, 0))
	SandboxScene:SetAmbientLight(Vector3(0.31, 0.32, 0.34))
	-- Light travels toward the relay so the front elevation and combatants keep
	-- readable form instead of becoming silhouettes against the bright sky.
	local sunlight = SandboxScene:CreateDirectionalLight(Vector3(0.42, -1, 0.52))
	sunlight:setDiffuseColour(ColourValue(1.05, 1.03, 0.98))
	sunlight:setSpecularColour(ColourValue(0.18, 0.16, 0.13))
	local shadowOverride = os.getenv and os.getenv("HELLO_RENDER_SHADOWS")
	local shadowEnabled = shadowOverride ~= "0" and shadowOverride ~= "false"
		and shadowOverride ~= "off" and shadowOverride ~= "no"
	local shadowConfigured = SandboxScene:ConfigureDirectionalShadows(sunlight, shadowEnabled)
	print("[Sandbox19Scene] directional-shadows=" .. tostring(shadowEnabled and shadowConfigured))

	-- 48 x 64 m floor; all visible surfaces have matching static Bullet bodies.
	for _, x in ipairs({ -16, 0, 16 }) do
		for _, z in ipairs({ -16, 0, 16, 32 }) do
			local material = x == 0 and "Relay/MainRoute" or "Relay/Ground"
			local floor = _Cube(16, x, -8, z, material)
			_floorIds[floor:GetObjId()] = true
		end
	end
	-- Broad, softly feathered dust/scuff layers break the 16 m floor grid without
	-- pretending to be geometry or changing the two validated navigation routes.
	for _, patch in ipairs({
		{ 10.5, 13.0, -0.5, -15.0, 4 }, { 8.0, 10.0, -13.5, -10.0, -18 },
		{ 7.0, 11.0, 14.2, -5.5, 22 }, { 10.0, 12.0, -1.5, 2.5, -8 },
		{ 8.5, 9.5, -14.8, 12.0, 14 }, { 8.0, 11.0, 14.5, 17.0, -24 },
		{ 11.0, 10.0, 0.8, 27.5, 7 }, { 7.0, 7.0, -17.5, 31.0, -15 },
		{ 7.0, 7.0, 17.0, 32.0, 18 },
	}) do
		_GroundLayer(patch[1], patch[2], patch[3], patch[4], patch[5], "Relay/GroundDust")
	end

	-- Continuous low perimeter slabs replace the former fence of repeated panels.
	-- Their 2.4 m top reveals the generated desert horizon from the follow camera.
	_Box(0.55, 2.4, 64, -23.72, 1.2, 8, 0, "Relay/ConcreteShade")
	_Box(0.55, 2.4, 64, 23.72, 1.2, 8, 0, "Relay/Concrete")
	_Box(48, 2.4, 0.55, 0, 1.2, -23.72, 0, "Relay/ConcreteShade")
	-- The north perimeter rises behind the occupied relay frontage. It is still
	-- the same 0.55 m deep boundary, so neither navigation lane is narrowed.
	_Box(15.5, 4.2, 0.55, -16.25, 2.1, 39.72, 0, "Relay/Concrete")
	_Box(15.5, 4.2, 0.55, 16.25, 2.1, 39.72, 0, "Relay/Concrete")
	-- Dark caps and sparse piers give the perimeter a designed edge while all
	-- added collision remains embedded in the existing wall volume.
	_Box(0.76, 0.14, 64, -23.72, 2.47, 8, 0, "Relay/Trim")
	_Box(0.76, 0.14, 64, 23.72, 2.47, 8, 0, "Relay/Trim")
	_Box(48, 0.14, 0.76, 0, 2.47, -23.72, 0, "Relay/Trim")
	_Box(15.5, 0.14, 0.76, -16.25, 4.27, 39.72, 0, "Relay/Trim")
	_Box(15.5, 0.14, 0.76, 16.25, 4.27, 39.72, 0, "Relay/Trim")
	for _, z in ipairs({ 0, 16, 32 }) do
		_Module(PILLAR_MESH, -23.35, 1.28, z, 0, "Relay/Metal")
		_Module(PILLAR_MESH, 23.35, 1.28, z, 0, "Relay/Metal")
	end

	-- The western bypass is a real line-of-sight break, open at both ends.
	-- Main route remains x=-8..8; both join the north yard before the relay.
	local bypassWall = _Box(0.55, 2.6, 25.6, -12, 1.3, 4.8, 0, "Relay/BypassWall")
	_sideWallIds[bypassWall:GetObjId()] = true
	_Box(0.75, 0.14, 25.6, -12, 2.67, 4.8, 0, "Relay/Trim")
	-- Service bay to the east and two pillars identify the courtyard threshold.
	_Box(0.55, 2.6, 12.8, 13, 1.3, 10.24, 0, "Relay/Concrete")
	_Box(0.75, 0.14, 12.8, 13, 2.67, 10.24, 0, "Relay/Trim")
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

	-- Long low barriers now frame a 10 m central assault lane, close enough to
	-- read from the follow camera while leaving the x=-18 bypass untouched.
	for _, cover in ipairs({
		{ -8.3, -8.5, -4 }, { 8.3, -8.5, 4 },
		{ -7.9, 5.5, 3 }, { 7.9, 5.5, -3 },
		{ -7.7, 23.5, -5 }, { 7.7, 23.5, 5 },
	}) do
		local barrier = _Asset("relay_barrier.mesh", cover[1], 0.625, cover[2], cover[3])
		_coverIds[barrier:GetObjId()] = true
		_GroundLayer(4.7, 1.7, cover[1] + 0.12, cover[2] - 0.08, cover[3], "Relay/ContactShadow")
	end

	-- Short low barriers sit behind the courtyard guards. They close neither
	-- the direct x=0 path nor the western bypass, and leave the relay door open.
	for _, x in ipairs({ -6.2, 6.2 }) do
		local barrier = _Asset("relay_barrier_short.mesh", x, 0.625, 33.9, x < 0 and -4 or 4)
		_coverIds[barrier:GetObjId()] = true
		_GroundLayer(2.8, 1.6, x, 33.9, 0, "Relay/ContactShadow")
	end

	-- Supply clusters attach to the outer shoulders of existing cover. The
	-- central lane (x=-5..5) and the x=-18 bypass stay clear at every stage.
	for _, supply in ipairs({
		{-10.9, -6.9, -4}, {10.9, -6.9, 4},
		{-10.5, 6.1, 3}, {10.5, 6.1, -3},
		{-10.3, 24.1, -5}, {10.3, 24.1, 5},
		{-8.6, 34.6, -7},
	}) do
		local crate = _Asset("relay_supply_crate_tall.mesh", supply[1], 0.825, supply[2], supply[3])
		_supplyIds[crate:GetObjId()] = true
		_GroundLayer(2.0, 1.45, supply[1], supply[2], supply[3], "Relay/ContactShadow")
	end
	for _, supply in ipairs({
		{-7.0, -11.0, -7}, {7.6, -11.6, 8},
		{-9.9, 2.0, 0}, {10.0, 2.2, 90},
		{-8.8, 29.0, 0}, {9.0, 30.1, 12},
		{8.5, 34.7, 8},
	}) do
		local crate = _Asset("relay_supply_crate.mesh", supply[1], 0.525, supply[2], supply[3])
		_supplyIds[crate:GetObjId()] = true
		_GroundLayer(2.0, 1.45, supply[1], supply[2], supply[3], "Relay/ContactShadow")
	end

	-- A connected, full-width canopy gives the first encounter an overhead edge.
	-- Posts remain outside the central corridor and x=-18 bypass; the roof is
	-- above standing actors and each native panel stays below the nav cutoff.
	for _, x in ipairs({ -11.60, -8.44 }) do
		for _, z in ipairs({ -0.70, 2.46 }) do
			_Module(ROOF_MESH, x, 3.65, z, 0, "Relay/Canopy")
		end
	end
	for _, x in ipairs({ -12.90, -7.14 }) do
		for _, z in ipairs({ -2.15, 3.45 }) do
			_Box(0.24, 3.49, 0.24, x, 1.745, z, 0, "Relay/Trim")
		end
		_Box(0.20, 0.20, 6.40, x, 3.43, 0.88, 0, "Relay/Trim")
	end
	for _, z in ipairs({ -2.15, 3.45 }) do
		_Box(6.20, 0.20, 0.20, -10.02, 3.43, z, 0, "Relay/Trim")
	end
	for _, x in ipairs({ -10.2, -9.35, -8.5 }) do
		_Module(BLOCK_MESH, x, 0.32, 2.5, 0, "Relay/Equipment")
	end
	_Module(BLOCK_MESH, -9.35, 0.96, 2.5, 0, "Relay/Equipment")
	for _, prop in ipairs({
		{ -20.2, -10.2, 0 }, { -20.2, -9.2, 90 }, { -15.8, -5.0, 0 },
		{ 20.2, -10.2, 0 }, { 20.2, -9.2, 90 }, { 15.8, -5.0, 0 },
		{ -20.2, 5.5, 90 }, { 19.8, 3.8, 0 },
		{ -17.6, 12.5, 0 }, { 17.6, 12.5, 0 },
		{ -20.1, 27.0, 0 }, { 19.6, 27.8, 90 },
	}) do
		_Module(BLOCK_MESH, prop[1], 0.32, prop[2], prop[3], "Relay/Equipment")
		_GroundLayer(1.35, 1.15, prop[1], prop[2], prop[3], "Relay/ContactShadow")
	end

	-- The courtyard terminates in one compound frontage rather than an isolated
	-- gatehouse. Outer wings sit inside the existing north boundary footprint and
	-- leave the central approach and western bypass fully open.
	for _, x in ipairs({ -16.0, 16.0 }) do
		_Box(11.6, 3.6, 2.0, x, 1.8, 38.15, 0, "Relay/Concrete")
		_Box(11.9, 0.24, 2.2, x, 3.72, 38.15, 0, "Relay/ConcreteShade")
		_Box(11.2, 0.11, 0.12, x, 1.15, 37.04, 0, "Relay/BuildingStripe")
		_Box(2.3, 1.15, 1.5, x, 4.22, 38.2, 0, "Relay/EquipmentBox")
		_Box(2.5, 0.12, 1.7, x, 4.86, 38.2, 0, "Relay/Trim")
	end
	for _, x in ipairs({ -19.0, -13.0, 13.0, 19.0 }) do
		_Module(WINDOW_MESH, x, 1.8, 36.99, 0, "Relay/ServiceWindow")
	end
	-- A solid service intake breaks up the repeated windows opposite sector A1.
	_Asset("relay_service_bay.mesh", -16.0, 1.8, 36.99, 0)
	-- Replace one repeated window with the compound's painted sector code.
	-- It sits just forward of the existing solid wing, without a new collider.
	_VisualPlane(1.8, 3.6, 16.0, 2.22, 37.0, 90, 0, "Relay/ZoneStencil")
	for _, x in ipairs({ -10.0, 10.0 }) do
		_Box(0.72, 4.1, 1.4, x, 2.05, 37.55, 0, "Relay/ConcreteShade")
		_Box(0.88, 0.15, 1.55, x, 4.17, 37.55, 0, "Relay/Trim")
	end
	-- A stepped relay building gives the destination a readable silhouette:
	-- pale side volumes, a darker instrumented gatehouse and a narrow mast.
	_Box(6.2, 3.4, 3.0, -5.7, 1.7, 39.0, 0, "Relay/Concrete")
	_Box(6.2, 3.4, 3.0, 5.7, 1.7, 39.0, 0, "Relay/Concrete")
	-- Shallow solid crowns give the two connecting volumes a visible roof edge.
	-- Their lowest point stays above the approach and central doorway.
	for _, x in ipairs({ -5.7, 5.7 }) do
		_Asset("relay_frontage_hood.mesh", x, 3.57, 37.16, 0)
	end
	_Box(5.4, 4.8, 3.4, 0, 2.4, 39.1, 0, "Relay/Facade")
	_GroundLayer(6.8, 3.8, -5.7, 37.9, 0, "Relay/ContactShadow")
	_GroundLayer(6.8, 3.8, 5.7, 37.9, 0, "Relay/ContactShadow")
	_GroundLayer(5.8, 4.1, 0, 37.8, 0, "Relay/ContactShadow")
	_Box(4.4, 2.4, 3.0, 0, 6.0, 39.25, 0, "Relay/ConcreteShade")
	-- A mounted front kit turns the composite boxes into one authored facade.
	-- It is entirely inside the non-walkable relay footprint.
	_Box(17.6, 0.36, 0.24, 0, 0.18, 37.34, 0, "Relay/Trim")
	for _, x in ipairs({ -8.55, -2.70, 2.70, 8.55 }) do
		_Box(0.26, 3.55, 0.24, x, 1.78, 37.30, 0, "Relay/Trim")
	end
	_Box(5.75, 0.30, 0.28, 0, 4.35, 37.27, 0, "Relay/Trim")
	_TrackFeedback("strips", _Box(5.45, 0.14, 0.28, -5.75, 2.67, 37.25, 0, "Relay/StateOff"))
	_TrackFeedback("strips", _Box(5.45, 0.14, 0.28, 5.75, 2.67, 37.25, 0, "Relay/StateOff"))
	for _, x in ipairs({ -7.65, 7.65 }) do
		_TrackFeedback("beacons", _Box(0.22, 0.46, 0.20, x, 2.18, 37.18, 0, "Relay/StateOff"))
	end
	for _, x in ipairs({ -2.15, 2.15 }) do
		_TrackFeedback("beacons", _Box(0.20, 0.34, 0.20, x, 3.58, 37.15, 0, "Relay/StateOff"))
	end
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
	_Box(3.8, 0.14, 0.18, 0, 7.72, 39.25, 0, "Relay/Trim")
	_Box(2.7, 0.12, 0.18, 0, 9.02, 39.25, 0, "Relay/Trim")
	_TrackFeedback("beacons", _Box(0.32, 0.32, 0.32, -1.25, 9.02, 39.12, 0, "Relay/StateOff"))
	_TrackFeedback("beacons", _Box(0.32, 0.32, 0.32, 1.25, 9.02, 39.12, 0, "Relay/StateOff"))

	-- Soft, state-driven signal layers make the mission phase readable in the
	-- world. They are render-only and share the project-owned radial mask.
	_TrackFeedback("beam", _VisualPlane(0.72, 3.6, 0, 10.25, 39.1, 90, 0, "Relay/SignalOff"))
	_TrackFeedback("beam", _VisualPlane(0.72, 3.6, 0, 10.25, 39.1, 90, 90, "Relay/SignalOff"))
	_TrackFeedback("door", _VisualPlane(5.2, 3.3, 0, 1.72, 37.02, 90, 0, "Relay/SignalOff"))
	_TrackFeedback("threshold", _GroundLayer(8.0, 3.4, 0, 19.0, 0, "Relay/SignalOff"))
	_TrackFeedback("goal", _GroundLayer(9.0, 6.5, 0, 35.0, 0, "Relay/SignalOff"))

	_PaintCorners(0, -16, 5, 4, "Relay/SafeMark")
	_PaintCorners(0, 33.5, 5, 3, "Relay/GoalMark")
	-- Short inset dashes lead through the main entrance without visual debug lines.
	for _, z in ipairs({ -7, -3, 1, 5, 9, 13, 17, 21, 25, 29 }) do
		_Cube(0.5, 0, -0.24, z, "Relay/RouteMark")
	end

	SandboxScene:UpdateSceneGraph()
	print("[Sandbox19Scene] relay-station size=48x64 routes=2 floor=0 side-wall=solid composition=p5-authored-props vegetation=0")
	return _Anchors()
end

local function _FeedbackProfile(state, wave)
	if state == "PREPARE" then
		return {mode = "standby", colour = "Amber", stepMs = 700, maxLevel = 2, door = true}
	elseif state == "WAVE" then
		return {mode = "contested", colour = "Amber", stepMs = wave == 1 and 240 or 140,
			maxLevel = 3, beam = true, door = true}
	elseif state == "ADVANCE" then
		return {mode = "advance", colour = "Amber", stepMs = 260, maxLevel = 3,
			beam = true, door = true, threshold = true}
	elseif state == "REGROUP" then
		return {mode = "regroup", colour = "Cyan", stepMs = 240, maxLevel = 3,
			beam = true, door = true, goal = true}
	elseif state == "VICTORY" then
		return {mode = "secured", colour = "Cyan", steady = 3,
			beam = true, door = true, goal = true}
	end
	return {mode = state == "DEFEAT" and "failed" or "off"}
end

local function _FeedbackMaterial(prefix, colour, level)
	if colour == nil or level <= 0 then return "Relay/" .. prefix .. "Off" end
	local suffix = level == 1 and "Dim" or (level == 2 and "Mid" or "")
	return "Relay/" .. prefix .. colour .. suffix
end

function Scene.UpdateRelayFeedback(state, wave, timeMs)
	local profile = _FeedbackProfile(tostring(state or ""), tonumber(wave) or 0)
	local pulse = {1, 2, 3, 2}
	local tick = math.floor(math.max(0, tonumber(timeMs) or 0) / (profile.stepMs or 1000))
	local level = profile.steady or math.min(profile.maxLevel or 0, pulse[(tick % #pulse) + 1])
	local softLevel = math.max(1, level - 1)
	local signalOff = "Relay/SignalOff"

	_SetFeedbackMaterial("beacons", _FeedbackMaterial("State", profile.colour, level))
	_SetFeedbackMaterial("strips", _FeedbackMaterial("State", profile.colour, level))
	_SetFeedbackMaterial("beam", profile.beam and _FeedbackMaterial("Signal", profile.colour, softLevel) or signalOff)
	_SetFeedbackMaterial("door", profile.door and _FeedbackMaterial("Signal", profile.colour, softLevel) or signalOff)
	_SetFeedbackMaterial("threshold", profile.threshold and _FeedbackMaterial("Signal", profile.colour, level) or signalOff)
	_SetFeedbackMaterial("goal", profile.goal and _FeedbackMaterial("Signal", profile.colour, level) or signalOff)

	local phase = tostring(state) .. ":" .. tostring(wave)
	_relayFeedback.mode = profile.mode
	if _relayFeedback.phase ~= phase then
		_relayFeedback.phase = phase
		local ground = profile.threshold and "threshold" or (profile.goal and "goal" or "off")
		print("[Sandbox19RelayFeedback] phase=" .. phase .. " mode=" .. profile.mode
			.. " colour=" .. tostring(profile.colour or "off") .. " ground=" .. ground)
	end
	return profile.mode
end

function Scene.GetRelayFeedbackState()
	return _relayFeedback.mode or "off"
end

function Scene.CreateActorShadow()
	return _VisualPlane(1.20, 0.72, 0, -10, 0, 0, 0, "Relay/ContactShadow")
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
	local courtyardCoverHit = SandboxRaycast:RayCastObjectId(Vector3(-6.2, 0.8, 31), Vector3(-6.2, 0.8, 35.5))
	local courtyardCoverSolid = _coverIds[courtyardCoverHit] == true
	print("[Sandbox19ArenaSelfTest] " .. (courtyardCoverSolid and "PASS" or "FAIL") .. " courtyard-short-cover-ray")
	pass = courtyardCoverSolid and pass
	local doorApproachHit = SandboxRaycast:RayCastObjectId(Vector3(0, 0.8, 24), Vector3(0, 0.8, 36))
	local doorApproachClear = doorApproachHit == 0
	print("[Sandbox19ArenaSelfTest] " .. (doorApproachClear and "PASS" or "FAIL") .. " courtyard-door-approach-clear")
	pass = doorApproachClear and pass
	local crateHit = SandboxRaycast:RayCastObjectId(Vector3(-7, 0.6, -13), Vector3(-7, 0.6, -10))
	local crateSolid = _supplyIds[crateHit] == true
	local overCrate = SandboxRaycast:RayCastObjectId(Vector3(-7, 1.3, -13), Vector3(-7, 1.3, -10))
	local crateHeight = overCrate == 0
	local tallHit = SandboxRaycast:RayCastObjectId(Vector3(-10.9, 1.4, -7.6), Vector3(-10.9, 1.4, -6.2))
	local tallSolid = _supplyIds[tallHit] == true
	local supplyPass = crateSolid and crateHeight and tallSolid
	print("[Sandbox19ArenaSelfTest] " .. (supplyPass and "PASS" or "FAIL") .. " supply-mesh-hulls"
		.. " short=" .. tostring(crateSolid) .. " above-short=" .. tostring(crateHeight)
		.. " tall=" .. tostring(tallSolid))
	pass = supplyPass and pass

	local floorHit = SandboxRaycast:RayCastObjectId(Vector3(2, 3, 2), Vector3(2, -2, 2))
	local floorSolid = _floorIds[floorHit] == true
	print("[Sandbox19ArenaSelfTest] " .. (floorSolid and "PASS" or "FAIL") .. " floor-collision")
	pass = floorSolid and pass
	print("[Sandbox19ArenaSelfTest] " .. (pass and "PASS" or "FAIL") .. " relay-static-collision")
	return pass
end

return Scene
