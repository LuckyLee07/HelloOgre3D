-- InfluenceMap.lua
-- Lightweight grid influence map for tactical AI samples.

local InfluenceMap = {}
InfluenceMap.__index = InfluenceMap

local function _Clamp(value, minValue, maxValue)
	if value < minValue then return minValue end
	if value > maxValue then return maxValue end
	return value
end

local function _DistanceSq2D(a, b)
	local dx = a.x - b.x
	local dz = a.z - b.z
	return dx * dx + dz * dz
end

local function _CloneVec3(pos)
	if pos == nil then
		return nil
	end
	return Vector3(pos.x, pos.y, pos.z)
end

local function _ReadNumber(config, key, defaultValue)
	local value = config ~= nil and tonumber(config[key]) or nil
	if value == nil then
		return defaultValue
	end
	return value
end

function InfluenceMap.New(config)
	local map = {
		config = config or {},
		minX = _ReadNumber(config, "minX", -40.0),
		maxX = _ReadNumber(config, "maxX", 60.0),
		minZ = _ReadNumber(config, "minZ", -20.0),
		maxZ = _ReadNumber(config, "maxZ", 70.0),
		y = _ReadNumber(config, "y", 0.15),
		cellSize = _ReadNumber(config, "cellSize", 5.0),
		layers = {},
	}
	map.width = math.max(1, math.floor((map.maxX - map.minX) / map.cellSize) + 1)
	map.height = math.max(1, math.floor((map.maxZ - map.minZ) / map.cellSize) + 1)
	setmetatable(map, InfluenceMap)
	return map
end

function InfluenceMap:Reset()
	self.layers = {}
end

function InfluenceMap:ClearLayer(layerName)
	layerName = tostring(layerName or "default")
	self.layers[layerName] = {}
end

function InfluenceMap:_Layer(layerName)
	layerName = tostring(layerName or "default")
	if self.layers[layerName] == nil then
		self.layers[layerName] = {}
	end
	return self.layers[layerName]
end

function InfluenceMap:_CellKey(ix, iz)
	return tostring(ix) .. ":" .. tostring(iz)
end

function InfluenceMap:_CellCenter(ix, iz)
	return Vector3(self.minX + (ix + 0.5) * self.cellSize, self.y, self.minZ + (iz + 0.5) * self.cellSize)
end

function InfluenceMap:WorldToCell(pos)
	if pos == nil then
		return nil
	end
	local ix = _Clamp(math.floor((pos.x - self.minX) / self.cellSize), 0, self.width - 1)
	local iz = _Clamp(math.floor((pos.z - self.minZ) / self.cellSize), 0, self.height - 1)
	return ix, iz
end

function InfluenceMap:AddRadialSource(layerName, center, strength, radius)
	if center == nil then
		return 0
	end
	strength = tonumber(strength) or 1.0
	radius = tonumber(radius) or self.cellSize
	if radius <= 0.0 or strength == 0.0 then
		return 0
	end

	local layer = self:_Layer(layerName)
	local minIx = _Clamp(math.floor((center.x - radius - self.minX) / self.cellSize), 0, self.width - 1)
	local maxIx = _Clamp(math.floor((center.x + radius - self.minX) / self.cellSize), 0, self.width - 1)
	local minIz = _Clamp(math.floor((center.z - radius - self.minZ) / self.cellSize), 0, self.height - 1)
	local maxIz = _Clamp(math.floor((center.z + radius - self.minZ) / self.cellSize), 0, self.height - 1)
	local radiusSq = radius * radius
	local written = 0

	for ix = minIx, maxIx do
		for iz = minIz, maxIz do
			local cellCenter = self:_CellCenter(ix, iz)
			local distanceSq = _DistanceSq2D(cellCenter, center)
			if distanceSq <= radiusSq then
				local distance = math.sqrt(distanceSq)
				local value = strength * (1.0 - (distance / radius))
				local key = self:_CellKey(ix, iz)
				local cell = layer[key]
				if cell == nil then
					cell = { ix = ix, iz = iz, position = cellCenter, value = 0.0 }
					layer[key] = cell
				end
				cell.value = cell.value + value
				written = written + 1
			end
		end
	end
	return written
end

function InfluenceMap:AddRingSources(layerName, center, strength, ringRadius, sourceRadius, sourceCount)
	if center == nil then
		return 0
	end
	ringRadius = tonumber(ringRadius) or 6.0
	sourceRadius = tonumber(sourceRadius) or self.cellSize
	sourceCount = math.max(1, tonumber(sourceCount) or 8)
	local written = 0

	for i = 0, sourceCount - 1 do
		local angle = (i / sourceCount) * math.pi * 2.0
		local sourcePos = center + Vector3(math.cos(angle) * ringRadius, 0, math.sin(angle) * ringRadius)
		written = written + self:AddRadialSource(layerName, sourcePos, strength, sourceRadius)
	end
	return written
end

function InfluenceMap:Sample(layerName, pos)
	local ix, iz = self:WorldToCell(pos)
	if ix == nil then
		return 0.0
	end
	local layer = self:_Layer(layerName)
	local cell = layer[self:_CellKey(ix, iz)]
	return cell ~= nil and cell.value or 0.0
end

function InfluenceMap:ScorePosition(pos, weights)
	weights = weights or {}
	local danger = self:Sample("danger", pos)
	local support = self:Sample("support", pos)
	return {
		position = _CloneVec3(pos),
		danger = danger,
		support = support,
		score = support * (tonumber(weights.support) or 1.0) - danger * (tonumber(weights.danger) or 2.0),
	}
end

function InfluenceMap:FindBestSupportPosition(targetPos, config)
	if targetPos == nil then
		return nil
	end
	config = config or {}
	local candidateCount = math.max(4, tonumber(config.candidateCount) or 12)
	local radii = config.radii or { tonumber(config.radius) or 6.0 }
	local weights = config.weights or { danger = 2.0, support = 1.0 }
	local best = nil

	for _, radius in ipairs(radii) do
		radius = tonumber(radius) or 0.0
		if radius > 0.0 then
			for i = 0, candidateCount - 1 do
				local angle = (i / candidateCount) * math.pi * 2.0
				local pos = targetPos + Vector3(math.cos(angle) * radius, 0, math.sin(angle) * radius)
				local scored = self:ScorePosition(pos, weights)
				scored.radius = radius
				if best == nil
					or scored.score > best.score
					or (scored.score == best.score and scored.danger < best.danger) then
					best = scored
				end
			end
		end
	end
	return best
end

function InfluenceMap:GetActiveCells(layerName, minAbsValue)
	local layer = self:_Layer(layerName)
	local cells = {}
	minAbsValue = tonumber(minAbsValue) or 0.0
	for _, cell in pairs(layer) do
		if math.abs(cell.value) >= minAbsValue then
			table.insert(cells, cell)
		end
	end
	return cells
end

function InfluenceMap:CountActiveCells(minAbsValue)
	local count = 0
	minAbsValue = tonumber(minAbsValue) or 0.01
	for _, layer in pairs(self.layers) do
		for _, cell in pairs(layer) do
			if math.abs(cell.value) >= minAbsValue then
				count = count + 1
			end
		end
	end
	return count
end

_G.InfluenceMap = InfluenceMap
return InfluenceMap
