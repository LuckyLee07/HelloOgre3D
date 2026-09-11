-- Relay Outpost presentation. All game state is supplied as plain Lua values.
-- New() allocates once; Update() reuses manager-owned Gorilla primitives.
-- HitTest returns action, optional ally id; "block" consumes non-action UI hits.
-- Hide/Destroy only hide primitives: UIManager owns their lifetime. Reuse this
-- instance for retries; create a new instance only after the UI scene changes.
local Hud = {}
Hud.__index = Hud

local COMMAND_LABELS = {FOCUS = "FOCUS", RETREAT = "FALL BACK", RALLY = "RALLY", HOLD = "HOLD", AUTONOMOUS = "AUTONOMOUS"}
local STATUS_LABELS = {SHOOT = "ENGAGING", PURSUE = "CLOSING", MOVE = "MOVING", RELOAD = "RELOADING", IDLE = "READY", PATROL = "GUARDING"}

local function Number(value, fallback)
	local n = tonumber(value)
	if n == nil or n ~= n or n == math.huge or n == -math.huge then return fallback or 0 end
	return n
end

local function Clamp(value, low, high)
	return math.max(low, math.min(high, value))
end

local function Count(value)
	return math.max(0, math.floor(Number(value)))
end

local function Plain(value)
	return tostring(value or ""):gsub("[^ -~\n]", ""):gsub("%%", "%%%%")
end

local function Clock(ms)
	local seconds = math.floor(math.max(0, Number(ms)) / 1000)
	return string.format("%02d:%02d", math.floor(seconds / 60), seconds % 60)
end

-- Advance widths from the bundled media/fonts/dejavu/dejavu.gorilla (ASCII
-- 32..126). UIFrame exposes no measurement API. Keep wrapping aligned with
-- this atlas instead of guessing character counts or changing global colors.
local ADVANCES = {
	[9] = "3,4,5,10,7,11,9,3,4,4,6,10,3,4,3,4,7,7,7,7,7,7,7,7,7,7,4,4,10,10,10,6,12,8,8,8,9,7,6,9,9,3,3,7,6,10,9,9,7,9,8,7,7,8,8,11,8,7,8,4,4,4,10,6,6,7,7,6,7,7,4,7,7,3,3,6,3,11,7,7,7,7,4,6,4,7,7,9,7,7,6,7,4,7,10",
	[14] = "6,7,8,16,12,18,14,5,7,7,9,16,6,6,6,6,12,12,12,12,12,12,12,12,12,12,6,6,16,16,16,10,19,13,13,13,14,12,11,14,14,5,5,12,10,16,14,15,11,15,13,12,11,14,13,18,13,11,13,7,6,7,16,9,9,11,12,10,12,11,6,12,12,5,5,11,5,18,12,11,12,12,7,9,7,12,11,15,11,11,10,12,6,12,16",
}
for font, csv in pairs(ADVANCES) do
	local widths, code = {}, 32
	for advance in csv:gmatch("%d+") do widths[code], code = tonumber(advance), code + 1 end
	ADVANCES[font] = widths
end

local function TextWidth(text, font)
	local width, widths = 0, ADVANCES[font]
	for i = 1, #text do width = width + (widths[text:byte(i)] or 0) end
	return width
end

local function FitLine(text, width, font)
	if TextWidth(text, font) <= width then return text end
	while #text > 0 and TextWidth(text .. "...", font) > width do text = text:sub(1, -2) end
	return text .. "..."
end

local function Inside(x, y, bounds)
	return x >= bounds.x and y >= bounds.y and x < bounds.x + bounds.w and y < bounds.y + bounds.h
end

local FRAME_KEYS = {
	"mission", "mission_accent", "mission_kicker", "mission_title", "mission_detail",
	"clock", "clock_text", "pause", "pause_accent", "context", "focus", "focus_accent",
	"retreat", "retreat_accent", "rally", "rally_accent", "cancel", "cancel_accent",
	"commander", "commander_title", "commander_value", "commander_track", "commander_hp",
	"select_all", "select_all_accent", "banner", "banner_accent", "banner_text",
	"shade", "modal", "modal_accent", "modal_kicker", "modal_title", "modal_subtitle",
	"modal_line", "modal_line2", "modal_detail1", "modal_detail2", "modal_detail3",
	"modal_detail4", "modal_detail5", "modal_detail6", "modal_detail7", "modal_detail8",
	"stat1_label", "stat1_value", "stat2_label", "stat2_value", "stat3_label", "stat3_value",
	"primary", "primary_accent", "secondary", "secondary_accent", "tertiary", "tertiary_accent",
	"audio_label", "audio_down", "audio_down_accent", "audio_up", "audio_up_accent",
	"audio_mute", "audio_mute_accent", "modal_footer",
}

local function IsModal(key)
	return key:find("^modal") or key:find("^stat") or key:find("^audio") or
		key:find("^primary") or key:find("^secondary") or key:find("^tertiary")
end

function Hud.New()
	local self = setmetatable({ frames = {}, polygons = {}, hitRegions = {}, wrapCache = {}, visible = false }, Hud)
	self.colors = {
		panel = ColourValue(0.090, 0.141, 0.169, 0.93),
		modal = ColourValue(0.070, 0.110, 0.129, 0.98),
		shade = ColourValue(0.015, 0.024, 0.029, 0.48),
		cyan = ColourValue(0.396, 0.820, 0.812, 1.0),
		amber = ColourValue(0.906, 0.682, 0.380, 1.0),
		danger = ColourValue(0.914, 0.467, 0.314, 1.0),
		muted = ColourValue(0.310, 0.396, 0.416, 0.90),
		track = ColourValue(0.040, 0.071, 0.086, 0.95),
		selected = ColourValue(0.110, 0.267, 0.282, 0.97),
		hover = ColourValue(0.157, 0.318, 0.329, 0.98),
		pressed = ColourValue(0.204, 0.416, 0.416, 1.0),
		disabled = ColourValue(0.082, 0.114, 0.129, 0.91),
		clear = ColourValue(0, 0, 0, 0),
	}
	local function Frame(key, layer)
		local frame = SandboxUI:CreateUIFrame(layer)
		if frame ~= nil then
			frame:setTextMargin(0, 0)
			frame:setVisible(false)
		end
		self.frames[key] = { object = frame, visible = false }
	end
	for _, key in ipairs(FRAME_KEYS) do
		Frame(key, key == "shade" and 8 or (IsModal(key) and 10 or 5))
	end
	for i = 1, 2 do
		for _, suffix in ipairs({ "panel", "accent", "key", "name", "hp_value", "track", "hp", "status" }) do
			Frame("ally" .. i .. "_" .. suffix, 5)
		end
		local polygon = SandboxUI:CreatePolygon(5)
		if polygon ~= nil then
			polygon:setSides(3)
			polygon:setAngleDegrees(-90)
			polygon:setVisible(false)
		end
		self.polygons[i] = { object = polygon, visible = false }
	end
	return self
end

function Hud:_Frame(key, x, y, w, h, color, text, font, textColor)
	local node = self.frames[key]
	if node == nil or node.object == nil then return end
	local frame = node.object
	local s = self.scale
	x, y, w, h = math.floor(x * s), math.floor(y * s), math.max(1, math.floor(w * s)), math.max(1, math.floor(h * s))
	if node.x ~= x or node.y ~= y then frame:setPosition(Vector2(x, y)); node.x, node.y = x, y end
	if node.w ~= w or node.h ~= h then frame:setDimension(Vector2(w, h)); node.w, node.h = w, h end
	if node.color ~= color then frame:setBackgroundColor(self.colors[color or "clear"]); node.color = color end
	local markup = text and ((textColor or "%0") .. "%@" .. tostring(font or 14) .. "%" .. Plain(text)) or ""
	if node.text ~= markup then frame:setMarkupText(markup); node.text = markup end
	if not node.visible then frame:setVisible(true); node.visible = true end
	node.used = true
end

function Hud:_Text(key, x, y, w, h, text, font, textColor)
	self:_Frame(key, x, y, w, h, "clear", text, font, textColor)
end

function Hud:_Wrap(key, value, width, font, maxLines)
	local text = tostring(value or ""):gsub("[^ -~]", " ")
	local pixels = math.floor(width * self.scale) - 2
	local cached = self.wrapCache[key]
	if cached and cached.text == text and cached.pixels == pixels and cached.font == font then return cached.value, cached.lines end
	local lines, line = {}, ""
	for word in text:gmatch("%S+") do
		local candidate = line == "" and word or line .. " " .. word
		if line ~= "" and TextWidth(candidate, font) > pixels then
			lines[#lines + 1], line = line, word
		else line = candidate end
	end
	if line ~= "" then lines[#lines + 1] = line end
	if #lines == 0 then lines[1] = "" end
	if #lines > maxLines then
		lines[maxLines] = table.concat(lines, " ", maxLines)
		for i = #lines, maxLines + 1, -1 do lines[i] = nil end
	end
	for i, content in ipairs(lines) do lines[i] = FitLine(content, pixels, font) end
	local valueText = table.concat(lines, "\n")
	self.wrapCache[key] = {text = text, pixels = pixels, font = font, value = valueText, lines = #lines}
	return valueText, #lines
end

function Hud:_Region(x, y, w, h, action, id)
	local s = self.scale
	table.insert(self.hitRegions, { x = x * s, y = y * s, w = w * s, h = h * s, action = action or "block", id = id })
end

function Hud:_Button(key, x, y, w, h, label, action, enabled, primary, id, reason)
	local mouseX, mouseY = Number(self.model.mouseX, -1) / self.scale, Number(self.model.mouseY, -1) / self.scale
	local hovered = Inside(mouseX, mouseY, { x = x, y = y, w = w, h = h })
	local color = primary and "selected" or "panel"
	if enabled == false then color = "disabled"
	elseif hovered then color = self.model.mouseDown and "pressed" or "hover" end
	self:_Frame(key, x, y, w, h, color, label, 14, enabled == false and "%8" or "%0")
	self:_Frame(key .. "_accent", x, y + h - 2, w, 2, enabled == false and "muted" or (primary and "cyan" or "muted"))
	-- The same frame supplies its caption; only markup is used because the
	-- legacy UIFrame::setVisible does not hide ordinary Caption text.
	local node = self.frames[key]
	if node and node.object then
		local pad, top = math.floor(12 * self.scale), math.floor((h * self.scale - 22) * 0.5)
		if node.pad ~= pad or node.top ~= top then
			node.object:setTextMargin(top, pad)
			node.pad, node.top = pad, top
		end
	end
	self:_Region(x, y, w, h, enabled == false and "block" or action, id)
	if hovered and enabled == false and reason then self.disabledReason = reason end
end

function Hud:_Ally(index, ally, y)
	ally = ally or {}
	local hp, maxHp = math.max(0, Number(ally.hp)), math.max(1, Number(ally.maxHp, 100))
	local alive = ally.alive ~= false and hp > 0
	local prefix = "ally" .. index .. "_"
	local selected = alive and ally.selected == true
	local hovered = alive and Inside(Number(self.model.mouseX, -1) / self.scale, Number(self.model.mouseY, -1) / self.scale,
		{ x = 24, y = y, w = 264, h = 56 })
	local color = hovered and (self.model.mouseDown and "pressed" or "hover") or (selected and "selected" or "panel")
	self:_Frame(prefix .. "panel", 24, y, 264, 56, color)
	self:_Frame(prefix .. "accent", 24, y, 3, 56, selected and "cyan" or (alive and "muted" or "danger"))
	self:_Text(prefix .. "key", 38, y + 27, 22, 22, tostring(index), 14, alive and "%0" or "%8")
	self:_Text(prefix .. "name", 67, y + 3, 136, 23, ally.name or (index == 1 and "ALPHA" or "BRAVO"), 14)
	self:_Text(prefix .. "hp_value", 217, y + 3, 55, 23, tostring(math.floor(hp)), 14, alive and "%0" or "%8")
	self:_Frame(prefix .. "track", 68, y + 27, 203, 4, "track")
	if alive then self:_Frame(prefix .. "hp", 68, y + 27, 203 * Clamp(hp / maxHp, 0, 1), 4, hp <= maxHp * 0.3 and "danger" or "cyan") end
	local command = tostring(ally.command or ""):upper()
	local activity = tostring(ally.status or ""):upper()
	command, activity = COMMAND_LABELS[command] or command, STATUS_LABELS[activity] or activity
	local status = "DOWN"
	if alive then
		status = command ~= "" and command or (activity ~= "" and activity or "AUTONOMOUS")
		if command ~= "" and activity ~= "" and activity ~= command then status = status .. " / " .. activity end
	end
	status = self:_Wrap(prefix .. "status", tostring(status):upper(), 202, self.smallFont, 1)
	self:_Text(prefix .. "status", 68, y + 34, 202, 19, status, self.smallFont, alive and "%0" or "%8")
	self:_Region(24, y, 264, 56, alive and ally.id ~= nil and "select" or "block", ally.id)
	local mark = self.polygons[index]
	if mark and mark.object then
		mark.object:setPosition(Vector2(47 * self.scale, (y + 18) * self.scale))
		mark.object:setRadius(7 * self.scale)
		mark.object:setBackgroundColor(self.colors[alive and "cyan" or "muted"])
		if not mark.visible then mark.object:setVisible(true); mark.visible = true end
		mark.used = true
	end
end

function Hud:_Combat(model, width, height, state)
	local bottom = height - 24
	local hp, maxHp = math.max(0, Number(model.commanderHp, 100)), math.max(1, Number(model.commanderMaxHp, 100))
	local selected = Count(model.selectedCount)
	local objective, objectiveLines = self:_Wrap("objective", model.objective or "SECURE THE COURTYARD", 307, 14, 2)
	local objectiveStep = 24 / self.scale
	local detailY = 54 + objectiveLines * objectiveStep + 5
	local missionHeight = detailY - 24 + 27
	self:_Frame("mission", 24, 24, 340, missionHeight, "panel")
	self:_Frame("mission_accent", 24, 24, 3, missionHeight, "amber")
	self:_Text("mission_kicker", 39, 30, 309, 22, "RELAY OUTPOST", 14)
	self:_Text("mission_title", 39, 54, 307, objectiveLines * objectiveStep + 2, objective, 14)
	local detail = model.phase and tostring(model.phase) .. "  /  " or ""
	detail = detail .. "Hostiles remaining  " .. tostring(Count(model.enemyAlive))
	self:_Text("mission_detail", 39, detailY, 307, 21, detail, self.smallFont)
	self:_Region(24, 24, 340, missionHeight)
	self:_Frame("clock", width - 184, 24, 98, 44, "panel")
	self:_Text("clock_text", width - 172, 34, 87, 29, Clock(model.elapsedMs), 14)
	self:_Region(width - 184, 24, 98, 44)
	self:_Button("pause", width - 80, 24, 56, 44, "II", "pause", state == "ACTIVE", false)
	self:_Ally(1, model.allies and model.allies[1], bottom - 120)
	self:_Ally(2, model.allies and model.allies[2], bottom - 56)
	local available = selected > 0 and state == "ACTIVE"
	local defs = {
		{ key = "focus", label = "F  FOCUS", w = 106 },
		{ key = "retreat", label = "T  FALL BACK", w = 120 },
		{ key = "rally", label = "G  RALLY", w = 106 },
		{ key = "cancel", label = "X  CANCEL", w = 96 },
	}
	local commandWidth = 0
	for index, def in ipairs(defs) do commandWidth = commandWidth + def.w + (index > 1 and 8 or 0) end
	local commandX = (width - commandWidth) * 0.5
	for _, def in ipairs(defs) do
		local cfg = model.commands and model.commands[def.key] or {}
		local enabled = available and cfg.enabled ~= false
		self:_Button(def.key, commandX, bottom - 42, def.w, 42, def.label, def.key, enabled, false, nil,
			cfg.reason or (selected == 0 and "Select a squadmate with 1 / 2 / Tab" or "Order unavailable"))
		commandX = commandX + def.w + 8
	end
	local context = self.disabledReason or model.context or (selected > 0 and (tostring(selected) .. " SELECTED  /  Right-click target or ground") or "1 / 2 select   |   Tab selects squad")
	local contextText, contextLines = self:_Wrap("context", context, commandWidth, self.smallFont, 2)
	local contextHeight = contextLines * (self.smallFont == 14 and 22 or 14) / self.scale + 2
	self:_Text("context", (width - commandWidth) * 0.5, bottom - 53 - contextHeight, commandWidth, contextHeight, contextText, self.smallFont)
	local commanderX = width - 280
	self:_Frame("commander", commanderX, bottom - 94, 256, 46, "panel")
	self:_Text("commander_title", commanderX + 12, bottom - 91, 174, 22, "COMMANDER", 14)
	self:_Text("commander_value", commanderX + 209, bottom - 91, 36, 22, tostring(math.floor(hp)), 14)
	self:_Frame("commander_track", commanderX + 12, bottom - 62, 232, 4, "track")
	if hp > 0 then self:_Frame("commander_hp", commanderX + 12, bottom - 62, 232 * Clamp(hp / maxHp, 0, 1), 4, hp <= maxHp * 0.3 and "danger" or "cyan") end
	self:_Region(commanderX, bottom - 94, 256, 46)
	self:_Button("select_all", commanderX, bottom - 38, 256, 38, "TAB   SELECT SQUAD", "select_all", state == "ACTIVE", false)
	if model.hint ~= nil and tostring(model.hint) ~= "" and state == "ACTIVE" then
		local bannerWidth = math.min(420, width - 48)
		local bx = (width - bannerWidth) * 0.5
		local bannerText, bannerLines = self:_Wrap("banner", model.hint, bannerWidth - 30, 14, 2)
		local by = math.max(145, 24 + missionHeight + 12)
		local bannerHeight = 18 + bannerLines * 24 / self.scale
		self:_Frame("banner", bx, by, bannerWidth, bannerHeight, "panel")
		self:_Frame("banner_accent", bx, by, 3, bannerHeight, model.hintKind == "failed" and "danger" or "cyan")
		self:_Text("banner_text", bx + 15, by + 8, bannerWidth - 30, bannerHeight - 14, bannerText, 14)
		self:_Region(bx, by, bannerWidth, bannerHeight)
	end
end

function Hud:_Modal(width, height, h, accent, title, subtitle)
	local x, y, w = (width - 640) * 0.5, (height - h) * 0.5, 640
	self:_Frame("shade", 0, 0, width, height, "shade")
	self:_Frame("modal", x, y, w, h, "modal")
	self:_Frame("modal_accent", x, y, w, 3, accent)
	self:_Text("modal_kicker", x + 32, y + 24, w - 64, 25, "RELAY OUTPOST", 14)
	self:_Text("modal_title", x + 32, y + 53, w - 64, 47, title, 24)
	self:_Text("modal_subtitle", x + 32, y + 104, w - 64, 27, subtitle, 14)
	self:_Frame("modal_line", x + 32, y + 146, w - 64, 1, "muted")
	self.hitRegions = {}
	self:_Region(0, 0, width, height)
	return x, y, w
end

function Hud:_Prepare(model, width, height)
	local x, y = self:_Modal(width, height, 452, "amber", "TAKE THE RELAY", "Lead your squad. Clear the courtyard. Bring them home.")
	self:_Text("modal_detail1", x + 32, y + 165, 576, 25, "01   CLEAR THE GUARDS", 14)
	self:_Text("modal_detail2", x + 69, y + 195, 539, 23, "Use the direct lane or covered side route. Both reach the relay.", self.smallFont)
	self:_Text("modal_detail3", x + 32, y + 227, 576, 25, "02   REGROUP AT THE RELAY", 14)
	self:_Text("modal_detail4", x + 69, y + 257, 539, 23, "Reach the marked zone with at least one surviving squadmate.", self.smallFont)
	self:_Frame("modal_line2", x + 32, y + 292, 576, 1, "muted")
	self:_Text("modal_detail5", x + 32, y + 306, 576, 24, model.controls or "WASD move  |  Q/E orbit  |  Wheel zoom  |  1 / 2 / Tab select", self.smallFont)
	self:_Text("modal_detail6", x + 32, y + 332, 576, 24, "Right-click to order  |  F focus  |  T fall back  |  G rally", self.smallFont)
	self:_Text("modal_detail7", x + 32, y + 358, 576, 24, "X cancel  |  Esc pause", self.smallFont)
	self:_Button("primary", x + 32, y + 384, 366, 44, "START MISSION", "start", true, true)
	self:_Button("secondary", x + 410, y + 384, 198, 44, "QUIT", "quit", true, false)
end

function Hud:_Pause(model, width, height)
	local x, y = self:_Modal(width, height, 392, "cyan", "MISSION PAUSED", "Your squad is holding. Resume when you are ready.")
	local audioAvailable = model.audioAvailable ~= false
	local volume = math.floor(Clamp(Number(model.audioVolume, 1), 0, 1) * 100 + 0.5)
	local audioLabel = audioAvailable and ("AUDIO  " .. (model.audioMuted and "MUTED" or tostring(volume) .. "%")) or "AUDIO UNAVAILABLE"
	self:_Text("audio_label", x + 32, y + 172, 255, 28, audioLabel, 14)
	self:_Button("audio_down", x + 298, y + 166, 52, 42, "-", "audio_down", audioAvailable, false)
	self:_Button("audio_up", x + 360, y + 166, 52, 42, "+", "audio_up", audioAvailable, false)
	self:_Button("audio_mute", x + 424, y + 166, 184, 42, model.audioMuted and "UNMUTE" or "MUTE", "audio_mute", audioAvailable, false)
	self:_Text("modal_detail1", x + 32, y + 228, 576, 25, "Mission time  " .. Clock(model.elapsedMs) .. "    |    Esc to resume", 14)
	self:_Button("primary", x + 32, y + 275, 576, 46, "RESUME MISSION", "resume", true, true)
	self:_Button("secondary", x + 32, y + 333, 282, 38, "RETRY", "retry", true, false)
	self:_Button("tertiary", x + 326, y + 333, 282, 38, "QUIT", "quit", true, false)
end

function Hud:_Result(model, width, height, victory)
	local reason = model.endReason or (victory and "Courtyard secured. Squad regrouped." or "Your squad could not secure the relay.")
	local x, y = self:_Modal(width, height, 468, victory and "cyan" or "danger", victory and "MISSION COMPLETE" or "MISSION FAILED", reason)
	local stats = {
		{ "TIME", Clock(model.elapsedMs) },
		{ "SQUAD SAFE", tostring(Count(model.allyAlive)) .. " / 2" },
		{ "HOSTILES", tostring(Count(model.enemyKilled)) .. " / " .. tostring(Count(model.enemyTotal)) },
	}
	for i, stat in ipairs(stats) do
		local sx = x + 32 + (i - 1) * 194
		self:_Text("stat" .. i .. "_label", sx, y + 165, 182, 26, stat[1], 14)
		self:_Text("stat" .. i .. "_value", sx, y + 198, 182, 43, stat[2], 24)
	end
	self:_Frame("modal_line2", x + 32, y + 255, 576, 1, "muted")
	local orders = model.orders or {}
	self:_Text("modal_detail1", x + 32, y + 274, 576, 25, "ALLY ORDERS", 14)
	self:_Text("modal_detail2", x + 32, y + 308, 576, 25,
		string.format("%d issued   /   %d completed   /   %d failed", Count(orders.issued), Count(orders.completed), Count(orders.failed)), 14)
	self:_Text("modal_detail3", x + 32, y + 341, 576, 25,
		string.format("%d replaced   /   %d cancelled   /   %d active", Count(orders.replaced), Count(orders.cancelled), Count(orders.active)), 14)
	self:_Text("modal_footer", x + 32, y + 371, 576, 21,
		"One order per selected ally; rejected requests are not issued.", self.smallFont)
	self:_Button("primary", x + 32, y + 396, 236, 46, "PLAY AGAIN", "retry", true, true)
	self:_Button("secondary", x + 280, y + 396, 190, 46, "BRIEFING", "menu", true, false)
	self:_Button("tertiary", x + 482, y + 396, 126, 46, "QUIT", "quit", true, false)
end

function Hud:Update(model, width, height)
	self.model = model or {}
	width, height = math.max(640, Number(width, 1280)), math.max(360, Number(height, 720))
	-- Geometry scales with the viewport; text stays on crisp native atlas sizes.
	self.scale = math.min(width / 1280, height / 720)
	self.smallFont = self.scale >= 1.25 and 14 or 9
	self.hitRegions, self.disabledReason = {}, nil
	self.visible = true
	for _, node in pairs(self.frames) do node.used = false end
	for _, node in ipairs(self.polygons) do node.used = false end
	local state = tostring(self.model.state or "ACTIVE"):upper()
	if self.model.paused then state = "PAUSED" end
	if state == "WAVE" or state == "INTERMISSION" or state == "REGROUP" or state == "ADVANCE" then state = "ACTIVE" end
	local logicalWidth, logicalHeight = width / self.scale, height / self.scale
	self:_Combat(self.model, logicalWidth, logicalHeight, state)
	if state == "PREPARE" or state == "PREP" or state == "MENU" then
		self:_Prepare(self.model, logicalWidth, logicalHeight)
	elseif state == "PAUSED" then
		self:_Pause(self.model, logicalWidth, logicalHeight)
	elseif state == "VICTORY" or state == "DEFEAT" or state == "WON" or state == "LOST" then
		self:_Result(self.model, logicalWidth, logicalHeight, state == "VICTORY" or state == "WON")
	end
	for _, node in pairs(self.frames) do
		if not node.used and node.visible and node.object then node.object:setVisible(false); node.visible = false end
	end
	for _, node in ipairs(self.polygons) do
		if not node.used and node.visible and node.object then node.object:setVisible(false); node.visible = false end
	end
end

function Hud:HitTest(x, y)
	if not self.visible then return nil end
	for i = #self.hitRegions, 1, -1 do
		local region = self.hitRegions[i]
		if Inside(Number(x, -1), Number(y, -1), region) then return region.action, region.id end
	end
	return nil
end

function Hud:Hide()
	for _, node in pairs(self.frames) do
		if node.object and node.visible then node.object:setVisible(false); node.visible = false end
	end
	for _, node in ipairs(self.polygons) do
		if node.object and node.visible then node.object:setVisible(false); node.visible = false end
	end
	self.visible, self.hitRegions = false, {}
end

Hud.Destroy = Hud.Hide
return Hud
