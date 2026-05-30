local TriggerVolume = {}
TriggerVolume.__index = TriggerVolume

local function toVector3(value, defaultValue)
	if value == nil then
		return defaultValue
	end
	if type(value) ~= "table" then
		return value
	end
	return Vector3(value.x or value[1] or 0, value.y or value[2] or 0, value.z or value[3] or 0)
end

local function getObjectId(object)
	if object == nil or object.GetObjId == nil then
		return nil
	end
	return object:GetObjId()
end

local function getObjectTeamId(object)
	if object == nil or object.GetTeamId == nil then
		return nil
	end
	return object:GetTeamId()
end

function TriggerVolume.New(config)
	config = config or {}
	local volume = {
		id = config.id or config.regionId or "trigger_volume",
		event = config.event or config.enterEvent or "TRIGGER_ENTER",
		exitEvent = config.exitEvent,
		scope = config.scope or "Global",
		center = toVector3(config.center, Vector3(0, 0, 0)),
		halfExtents = toVector3(config.halfExtents or config.extents, Vector3(1, 1, 1)),
		targetTeamId = config.targetTeamId,
		targetDefId = config.targetDefId,
		drawDebug = config.drawDebug ~= false,
		inside = {},
		enterCount = 0,
		exitCount = 0,
	}
	setmetatable(volume, TriggerVolume)
	return volume
end

function TriggerVolume:SetCenter(center)
	self.center = toVector3(center, self.center)
end

function TriggerVolume:SetHalfExtents(halfExtents)
	self.halfExtents = toVector3(halfExtents, self.halfExtents)
end

function TriggerVolume:ContainsPosition(position)
	if position == nil then
		return false
	end

	return math.abs(position.x - self.center.x) <= self.halfExtents.x
		and math.abs(position.y - self.center.y) <= self.halfExtents.y
		and math.abs(position.z - self.center.z) <= self.halfExtents.z
end

function TriggerVolume:MatchesTarget(object)
	if object == nil then
		return false
	end
	if self.targetTeamId ~= nil and getObjectTeamId(object) ~= self.targetTeamId then
		return false
	end
	return true
end

function TriggerVolume:BuildPayload(object, eventName)
	local position = object ~= nil and object.GetPosition ~= nil and object:GetPosition() or self.center
	return {
		eventName = eventName,
		regionId = self.id,
		volumeId = self.id,
		scope = self.scope,
		target = object,
		targetId = getObjectId(object) or -1,
		teamId = getObjectTeamId(object) or -1,
		position = position,
		positionX = position.x,
		positionY = position.y,
		positionZ = position.z,
	}
end

function TriggerVolume:UpdateObject(object, runtime)
	if not self:MatchesTarget(object) then
		return
	end

	local objectId = getObjectId(object)
	if objectId == nil then
		return
	end

	local position = object.GetPosition ~= nil and object:GetPosition() or nil
	local isInside = self:ContainsPosition(position)
	local wasInside = self.inside[objectId] == true

	if isInside and not wasInside then
		self.inside[objectId] = true
		self.enterCount = self.enterCount + 1
		runtime:Emit(self.event, self:BuildPayload(object, self.event))
	elseif not isInside and wasInside then
		self.inside[objectId] = nil
		self.exitCount = self.exitCount + 1
		if self.exitEvent ~= nil then
			runtime:Emit(self.exitEvent, self:BuildPayload(object, self.exitEvent))
		end
	end
end

function TriggerVolume:DrawDebug()
	if not self.drawDebug or DebugDrawer == nil then
		return
	end

	local length = math.max(self.halfExtents.x, self.halfExtents.z) * 2.0
	local color = self.enterCount > 0 and UtilColors.Green or UtilColors.Yellow
	DebugDrawer:drawSquare(self.center, length, color, false)
end

function TriggerVolume:BuildDebugSummary()
	return string.format(
		"[TriggerVolume] id=%s event=%s inside=%d enters=%d exits=%d",
		tostring(self.id),
		tostring(self.event),
		self:GetInsideCount(),
		self.enterCount,
		self.exitCount
	)
end

function TriggerVolume:GetInsideCount()
	local count = 0
	for _ in pairs(self.inside) do
		count = count + 1
	end
	return count
end

return TriggerVolume
