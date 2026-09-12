-- Sample-local audio/settings. Data file contains numbers only, never Lua code.
local Audio = {}
Audio.__index = Audio
local WINDOW_MODES = {
	{width = 1280, height = 720},
	{width = 1600, height = 900},
	{width = 1920, height = 1080},
}

local function Round(value, fallback)
	local number = tonumber(value)
	if number == nil or number ~= number then return fallback or 0 end
	return math.floor(number + 0.5)
end

local function ModeIndex(width, height)
	for index, mode in ipairs(WINDOW_MODES) do
		if mode.width == width and mode.height == height then return index end
	end
	return nil
end

function Audio.New()
	local self = setmetatable({volume = 0.65, muted = false, available = SandboxAudio:IsAvailable(),
		nextCombatMs = 0, samples = {}, lastUiMs = -10000, silent = os.getenv("HELLO_AUDIO_SILENT") == "1",
		windowWidth = Round(GameManager:getScreenWidth(), 1280), windowHeight = Round(GameManager:getScreenHeight(), 720),
		windowLocked = os.getenv("HELLO_WINDOW_WIDTH") ~= nil or os.getenv("HELLO_WINDOW_HEIGHT") ~= nil,
		background = os.getenv("HELLO_WINDOW_BACKGROUND") == "1"}, Audio)
	local file = io.open("relay_settings.cfg", "r")
	if file ~= nil then
		local text = file:read(128); file:close()
		local volume = tonumber(text:match("volume=([%d.]+)"))
		if volume ~= nil then self.volume = math.max(0, math.min(1, volume)) end
		self.muted = text:match("muted=1") ~= nil
		local width, height = text:match("window=(%d+)x(%d+)")
		width, height = Round(width), Round(height)
		if ModeIndex(width, height) ~= nil then
			self.savedWindowWidth, self.savedWindowHeight = width, height
		end
	end
	self:Apply(false)
	if self.savedWindowWidth ~= nil and not self.windowLocked and not self.background
		and (self.windowWidth ~= self.savedWindowWidth or self.windowHeight ~= self.savedWindowHeight)
		and GameManager:RequestWindowSize(self.savedWindowWidth, self.savedWindowHeight) then
		self.windowWidth, self.windowHeight = self.savedWindowWidth, self.savedWindowHeight
	end
	return self
end
function Audio:_Save()
	local file = io.open("relay_settings.cfg", "w")
	if file ~= nil then
		file:write(string.format("volume=%.2f\nmuted=%d\n", self.volume, self.muted and 1 or 0))
		if self.savedWindowWidth ~= nil then
			file:write(string.format("window=%dx%d\n", self.savedWindowWidth, self.savedWindowHeight))
		end
		file:close()
		return true
	end
	print("[Sandbox19Settings] settings write unavailable")
	return false
end
function Audio:Apply(save)
	SandboxAudio:SetVolume((self.muted or self.silent) and 0 or self.volume)
	if save then self:_Save() end
end
function Audio:ObserveWindow(width, height)
	self.windowWidth = math.max(1, Round(width, self.windowWidth))
	self.windowHeight = math.max(1, Round(height, self.windowHeight))
end
function Audio:CycleWindow(direction)
	if self.windowLocked then return false, "Launch resolution override is active." end
	local current = ModeIndex(self.windowWidth, self.windowHeight)
	if current == nil then
		local bestDistance = math.huge
		for index, mode in ipairs(WINDOW_MODES) do
			local distance = math.abs(mode.width - self.windowWidth) + math.abs(mode.height - self.windowHeight)
			if distance < bestDistance then current, bestDistance = index, distance end
		end
	end
	local step = direction < 0 and -1 or 1
	local target = WINDOW_MODES[((current - 1 + step) % #WINDOW_MODES) + 1]
	if not GameManager:RequestWindowSize(target.width, target.height) then
		return false, "Window resizing is unavailable in this launch mode."
	end
	self.windowWidth, self.windowHeight = target.width, target.height
	self.savedWindowWidth, self.savedWindowHeight = target.width, target.height
	self:_Save()
	return true, string.format("Display set to %d x %d.", target.width, target.height)
end
function Audio:Play(name, timeMs, combat)
	if not self.available then return end
	if combat and timeMs - self.lastUiMs < 500 then return end
	local ok = SandboxAudio:Play("res/audio/relay/" .. name .. ".wav")
	if not combat then self.lastUiMs = timeMs end
	if not ok then
		self.available = false
		print("[Sandbox19Audio] playback failed name=" .. name)
	end
end
function Audio:Reset()
	SandboxAudio:StopAll()
	self.samples, self.nextCombatMs = {}, 0
end
function Audio:Observe(agents, timeMs)
	local shot, hit = false, false
	for i = 0, agents:size() - 1 do
		local a = agents[i]
		local weapon = a:GetWeaponComponent()
		local hp, ammo = a:GetHealth(), weapon ~= nil and weapon:GetAmmo() or 0
		local old = self.samples[a:GetObjId()]
		if old ~= nil then
			if ammo < old.ammo then shot = true end
			if hp < old.hp then hit = true end
		end
		self.samples[a:GetObjId()] = {hp = hp, ammo = ammo}
	end
	if timeMs >= self.nextCombatMs and (shot or hit) then
		self:Play(hit and "hit" or "shot", timeMs, true)
		self.nextCombatMs = timeMs + 80
	end
end
return Audio
