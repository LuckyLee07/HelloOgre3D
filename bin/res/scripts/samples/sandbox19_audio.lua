-- Sample-local audio/settings. Data file contains numbers only, never Lua code.
local Audio = {}
Audio.__index = Audio
function Audio.New()
	local self = setmetatable({volume = 0.65, muted = false, available = SandboxAudio:IsAvailable(),
		nextCombatMs = 0, samples = {}, lastUiMs = -10000, silent = os.getenv("HELLO_AUDIO_SILENT") == "1"}, Audio)
	local file = io.open("relay_settings.cfg", "r")
	if file ~= nil then
		local text = file:read(128); file:close()
		local volume = tonumber(text:match("volume=([%d.]+)"))
		if volume ~= nil then self.volume = math.max(0, math.min(1, volume)) end
		self.muted = text:match("muted=1") ~= nil
	end
	self:Apply(false)
	return self
end
function Audio:Apply(save)
	SandboxAudio:SetVolume((self.muted or self.silent) and 0 or self.volume)
	if not save then return end
	local file = io.open("relay_settings.cfg", "w")
	if file ~= nil then
		file:write(string.format("volume=%.2f\nmuted=%d\n", self.volume, self.muted and 1 or 0)); file:close()
	else print("[Sandbox19Audio] settings write unavailable") end
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
