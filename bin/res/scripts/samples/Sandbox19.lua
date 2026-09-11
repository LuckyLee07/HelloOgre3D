-- Relay Outpost: sample orchestration. UI and order state live in dedicated modules.
require("res.scripts.agent.SoldierAgent.lua")
require("res.scripts.agent.BehaviorSoldierAgent.lua")
require("res.scripts.ai.behavior.Sandbox19CommandConditions.lua")
local Scene = require("res.scripts.samples.sandbox19_scene.lua")
local Hud = require("res.scripts.samples.sandbox19_hud.lua")
local Commands = require("res.scripts.samples.sandbox19_commands.lua")
local Observer = require("res.scripts.samples.ai_observer")
local Audio = require("res.scripts.samples.sandbox19_audio.lua")
local _audio = nil
local _observer = Observer.New()
_observer.enabled = false
local _hud, _observerPanel, _anchors, _nav, _player, _commands
local _allyIds, _enemyIds, _selection, _profiles = {}, {}, {}, {}
local _state, _wave, _startedMs, _endedMs, _endReason = "PREPARE", 0, 0, nil, ""
local _hint, _hintKind, _hintUntil = "", "info", 0
local _drag, _dragFrame, _mouse = nil, nil, {x = 0, y = 0, down = false}
local _worldMarks, _targetMark, _goalLabel = {}, nil, nil
local _enemyMarks, _orderMarks, _selectionMarks = {}, {}, {}
local _matchConfig = nil
local _lastEnemy, _restart, _restartStart = 0, false, false
local _paused, _lastProgressMs, _lastAlive = false, 0, 0
local _debug = false
local _test = nil
local _experiment = nil
local _names = {"VEGA", "ROOK"}
local SAMPLE = "Sandbox19"

local function now() return GameManager:getTimeInMillis() end
local function find(id)
	if id == nil or id <= 0 then return nil end
	local agents = ObjectManager:getAllAgents()
	for i = 0, agents:size() - 1 do
		if agents[i]:GetObjId() == id then return agents[i] end
	end
	return nil
end
local function bbOf(agent)
	local ai = agent ~= nil and agent:GetAIComponent() or nil
	return ai ~= nil and ai:GetBlackboard() or nil
end
local function hint(text, kind)
	_hint, _hintKind, _hintUntil = text, kind or "info", now() + (kind == "accepted" and 1800 or 3500)
	if _audio ~= nil and (kind == "accepted" or kind == "failed") then _audio:Play(kind, now()) end
end
local function distance(a, b)
	local dx, dz = a.x - b.x, a.z - b.z
	return math.sqrt(dx * dx + dz * dz)
end
local function living(ids)
	local count = 0
	for _, id in ipairs(ids) do
		local a = find(id)
		if a ~= nil and a:GetHealth() > 0 then count = count + 1 end
	end
	return count
end
local function experimentHealth(ids)
	local values, total = {}, 0
	for _, id in ipairs(ids) do
		local actor = find(id)
		local health = math.max(0, math.floor((actor ~= nil and actor:GetHealth() or 0) + 0.5))
		values[#values + 1] = tostring(health)
		total = total + health
	end
	return #values > 0 and table.concat(values, ",") or "-", total
end
local function recordExperiment(event)
	if _experiment == nil or _player == nil then return end
	local allyHp = experimentHealth(_allyIds)
	local enemyHp = experimentHealth(_enemyIds)
	local stats = _commands ~= nil and _commands.stats or {}
	local simulationMs = now()
	local matchElapsedMs = _state == "PREPARE" and 0 or math.max(0, simulationMs - _startedMs)
	print(string.format("[Sandbox19Experiment] schema=1 runId=%s event=%s simulationMs=%d matchElapsedMs=%d state=%s wave=%d commanderHp=%d allyAlive=%d allyHp=%s enemyAlive=%d enemyHp=%s issued=%d completed=%d failed=%d replaced=%d cancelled=%d active=%d director=none spawnMode=%s aiSchedulerEnabled=%s aiTickMs=%d aiMaxPerFrame=%d commanderMaxHp=%d allyMaxHp=%d enemyMaxHp=%d",
		_experiment.runId, event, simulationMs, matchElapsedMs, _state, _wave,
		math.max(0, math.floor(_player:GetHealth() + 0.5)), living(_allyIds), allyHp,
		living(_enemyIds), enemyHp, stats.issued or 0, stats.completed or 0,
		stats.failed or 0, stats.replaced or 0, stats.cancelled or 0, stats.active or 0,
		_experiment.spawnMode, tostring(_experiment.aiSchedulerEnabled), _experiment.aiTickMs, _experiment.aiMaxPerFrame,
		_experiment.commanderMaxHp, _experiment.allyMaxHp, _experiment.enemyMaxHp))
end
local function initializeExperiment()
	local runId = os.getenv ~= nil and os.getenv("HELLO_EXPERIMENT_RUN_ID") or nil
	if runId == nil or runId == "" then return end
	if #runId > 96 or string.match(runId, "^[%w_.%-]+$") == nil then
		error("[Sandbox19Experiment] invalid HELLO_EXPERIMENT_RUN_ID")
	end
	local horizonMs = tonumber(os.getenv("HELLO_EXPERIMENT_HORIZON_MS"))
	if horizonMs == nil or horizonMs < 1000 or horizonMs > 3500000 then
		error("[Sandbox19Experiment] invalid HELLO_EXPERIMENT_HORIZON_MS")
	end
	local allyAlive, enemyAlive = living(_allyIds), living(_enemyIds)
	local _, allyTotal = experimentHealth(_allyIds)
	local _, enemyTotal = experimentHealth(_enemyIds)
	local preset = ConfigManager:GetSamplePreset(SAMPLE)
	local scheduler = preset.aiScheduler or {}
	_experiment = {runId = runId, horizonMs = math.floor(horizonMs), horizonLogged = false,
		firstContactLogged = false, nextHeartbeatMs = now() + 5000,
		lastPhase = _state .. ":" .. tostring(_wave), lastAllyAlive = allyAlive,
		lastEnemyAlive = enemyAlive, previousHealth = math.max(0, _player:GetHealth()) + allyTotal + enemyTotal,
		spawnMode = tostring(preset.spawnMode), aiSchedulerEnabled = scheduler.enabled == true,
		aiTickMs = tonumber(scheduler.tickMs) or 0,
		aiMaxPerFrame = tonumber(scheduler.maxPerFrame) or 0,
		commanderMaxHp = tonumber(_matchConfig.commanderHealth) or 0,
		allyMaxHp = tonumber(_matchConfig.allyHealth) or 0,
		enemyMaxHp = tonumber(_matchConfig.enemyHealth) or 0}
	recordExperiment("ready")
end
local function updateExperiment()
	if _experiment == nil or _player == nil then return end
	local simulationMs = now()
	local matchElapsedMs = _state == "PREPARE" and 0 or math.max(0, simulationMs - _startedMs)
	local allyAlive, enemyAlive = living(_allyIds), living(_enemyIds)
	local _, allyTotal = experimentHealth(_allyIds)
	local _, enemyTotal = experimentHealth(_enemyIds)
	local totalHealth = math.max(0, _player:GetHealth()) + allyTotal + enemyTotal
	if not _experiment.firstContactLogged and _state ~= "PREPARE"
		and totalHealth < _experiment.previousHealth - 0.1 then
		_experiment.firstContactLogged = true
		recordExperiment("first-contact")
	end
	local phase = _state .. ":" .. tostring(_wave)
	if phase ~= _experiment.lastPhase then
		_experiment.lastPhase = phase
		recordExperiment((_state == "VICTORY" or _state == "DEFEAT") and "terminal" or "phase")
	end
	if allyAlive < _experiment.lastAllyAlive or enemyAlive < _experiment.lastEnemyAlive then
		recordExperiment("casualty")
	end
	if not _experiment.horizonLogged and _state ~= "PREPARE"
		and matchElapsedMs >= _experiment.horizonMs then
		_experiment.horizonLogged = true
		recordExperiment("horizon")
	elseif _state ~= "PREPARE" and _state ~= "VICTORY" and _state ~= "DEFEAT"
		and simulationMs >= _experiment.nextHeartbeatMs then
		recordExperiment("heartbeat")
		_experiment.nextHeartbeatMs = simulationMs + 5000
	end
	_experiment.lastAllyAlive, _experiment.lastEnemyAlive = allyAlive, enemyAlive
	_experiment.previousHealth = totalHealth
end
local function selectAll()
	_selection = {}
	for _, id in ipairs(_allyIds) do
		local a = find(id)
		if a ~= nil and a:GetHealth() > 0 then _selection[id] = true end
	end
end
local function selectedCount()
	local n = 0
	for _ in pairs(_selection) do n = n + 1 end
	return n
end
local function visibleToSquad(id)
	for _, allyId in ipairs(_allyIds) do
		local ally = find(allyId)
		if ally ~= nil and ally:GetHealth() > 0 and ally:GetAIComponent():CanSeeEnemy(id) then return true end
	end
	return false
end
local function visibleToSelection(id)
	for allyId in pairs(_selection) do
		local ally = find(allyId)
		if ally ~= nil and ally:GetHealth() > 0 and ally:GetAIComponent():CanSeeEnemy(id) then return true end
	end
	return false
end
local function isActive()
	return _state ~= "PREPARE" and _state ~= "VICTORY" and _state ~= "DEFEAT" and not _paused
end
local function spawnAgent(slot, team, player)
	local a = Create_SoldierWithProfile(player and "res/scripts/agent/HumanSoldierAgent.lua"
		or "res/scripts/agent/BehaviorSoldierAgent.lua",
		team == 1 and Soldier.AppearanceTypes.LIGHT or Soldier.AppearanceTypes.DARK,
		team, player and "commander_soldier" or "ai_soldier")
	ConfigManager:PlaceAgentOnPresetSpawn(a, SAMPLE, slot, "default")
	a:SetForward(Vector3(0, 0, team == 1 and 1 or -1))
	_profiles[a:GetObjId()] = player and "commander_soldier" or "ai_soldier"
	local hp = player and _matchConfig.commanderHealth or (team == 1 and _matchConfig.allyHealth or _matchConfig.enemyHealth)
	a:GetAttribComponent():SetMaxHealth(hp)
	a:SetHealth(hp)
	local bb = bbOf(a)
	if bb ~= nil then
		bb:SetFloat("maxHealth", hp)
		bb:SetFloat("pursue.reach", 9)
		bb:SetBool("sandbox19.aimedFire", true)
		bb:SetBool("weapon.actionOwnsFire", true)
		bb:SetVec3("sandbox19.anchorPos", a:GetPosition())
	end
	return a
end
local function startWave()
	_wave = _wave + 1
	_state = "WAVE"
	local slots = _matchConfig.waveSpawnIndices[_wave]
	for index = 1, _matchConfig.waveEnemyCounts[_wave] do
		local slot = slots[index]
		local enemy = spawnAgent(slot, 0, false)
		_enemyIds[#_enemyIds + 1] = enemy:GetObjId()
	end
	_lastProgressMs, _lastAlive = now(), 2
	hint(_wave == 1 and "Gate guards active. Choose the main lane or west flank."
		or "Courtyard guards ahead. Clear the relay approach.")
	print("[Sandbox19Match] phase=WAVE wave=" .. _wave .. " enemies=2 director=none elapsedMs=" .. (now() - _startedMs))
end
local function startMission()
	if _state ~= "PREPARE" then return end
	_startedMs = now()
	GameManager:SetSimulationPaused(false)
	_paused = false
	startWave()
	_audio:Play("start", now())
end
local function setPaused(paused)
	if _state == "PREPARE" or _state == "VICTORY" or _state == "DEFEAT" then return end
	_paused = paused
	_drag = nil
	GameManager:SetSimulationPaused(paused)
	_audio:Play("pause", now())
	print("[Sandbox19Pause] paused=" .. tostring(paused) .. " simulationMs=" .. now())
end
local function finish(state, reason)
	_state, _endedMs, _endReason = state, now(), reason
	_commands:Clear("cancelled", "mission-ended")
	_paused = false
	_drag = nil
	GameManager:SetSimulationPaused(true)
	_audio:Play(state == "VICTORY" and "victory" or "defeat", now())
	print("[Sandbox19Match] phase=" .. state .. " reason=" .. reason .. " wave=" .. _wave
		.. " elapsedMs=" .. (_endedMs - _startedMs) .. " director=none")
end
local function issue(kind, targetId, base)
	if not isActive() then return end
	if selectedCount() == 0 then hint("Select VEGA / ROOK, or press Tab.", "failed"); return end
	local accepted, failed, lastReason = 0, 0, ""
	local ordered = {}
	for _, id in ipairs(_allyIds) do if _selection[id] then ordered[#ordered + 1] = id end end
	for index, id in ipairs(ordered) do
		local a = find(id)
		if a ~= nil and a:GetHealth() > 0 then
			local destination = base
			if base ~= nil then
				destination = Vector3(base.x + (index - (#ordered + 1) * 0.5) * 2.5, base.y, base.z)
				destination = SandboxNav:FindClosestPoint("default", destination)
			end
			local ok, reason = _commands:Issue(a, kind, targetId, destination, now())
			if ok then accepted = accepted + 1 else failed, lastReason = failed + 1, reason end
		end
	end
	hint(string.upper(kind) .. " accepted by " .. accepted .. " ally"
		.. (accepted == 1 and "" or " units") .. (failed > 0 and (" | " .. lastReason) or ""),
		accepted > 0 and "accepted" or "failed")
end
local function focus()
	local target = find(_lastEnemy)
	if target == nil or target:GetHealth() <= 0 or not visibleToSelection(_lastEnemy) then
		_lastEnemy = 0
		local best = math.huge
		for _, id in ipairs(_enemyIds) do
			local enemy = find(id)
			if enemy ~= nil and enemy:GetHealth() > 0 then
				for allyId in pairs(_selection) do
					local ally = find(allyId)
					if ally ~= nil and ally:GetAIComponent():CanSeeEnemy(id) then
						local d = distance(_player:GetPosition(), enemy:GetPosition())
						if d < best then _lastEnemy, best = id, d end
					end
				end
			end
		end
	end
	if _lastEnemy == 0 then hint("No visible target. Move the squad into contact.", "failed"); return end
	issue("focus", _lastEnemy, nil)
end
local function cancel()
	for id in pairs(_selection) do
		_commands:Finish(id, "cancelled", "player-cancel")
		local a = find(id)
		local bb = bbOf(a)
		if bb ~= nil then
			bb:Remove("sandbox19.holdPos")
			bb:SetVec3("sandbox19.anchorPos", a:GetPosition())
		end
	end
	hint("Orders cancelled. Squad guards its current position.")
end
local function objective()
	if _state == "PREPARE" then return "SECURE THE RELAY" end
	if _state == "ADVANCE" then return "ADVANCE TO THE COURTYARD" end
	if _state == "REGROUP" then return "REGROUP AT THE RELAY" end
	if _wave == 1 then return "CLEAR THE GATE" end
	return "SECURE THE COURTYARD"
end
local function spawnEncounter()
	GameManager:SetSimulationPaused(false)
	_paused, _state, _wave = false, "PREPARE", 0
	_startedMs, _endedMs, _endReason, _lastEnemy = now(), nil, "", 0
	_allyIds, _enemyIds, _profiles, _selection = {}, {}, {}, {}
	_commands = Commands.New({find = find, hint = hint})
	_player = spawnAgent(1, 1, true)
	for slot = 2, 3 do
		local a = spawnAgent(slot, 1, false)
		_allyIds[#_allyIds + 1] = a:GetObjId()
	end
	selectAll()
	SandboxCamera:ConfigureFollowCamera(6.5, 3.2, 0, 1.5, 5.5, 11)
	SandboxCamera:SetCameraRelativeMovement(true)
	SandboxCamera:SnapFollowTarget(_player:GetPosition(), _player:GetForward())
	-- Pause only after placement and camera setup; no actor can fire in the briefing.
	GameManager:SetSimulationPaused(true)
	hint("Operation Relay Outpost. Ready when you are.")
	print("[Sandbox19] ready playerId=" .. _player:GetObjId() .. " role=commander mission=relay-outpost")
end
local function restartEncounter(start)
	_observer:Release(find)
	_audio:Reset()
	_commands:Clear("cancelled", "restart")
	GameManager:SetSimulationPaused(false)
	_player = nil
	SandboxObjects:ClearProjectiles()
	ObjectManager:clearAllObjects(MGR_OBJ_AGENT)
	if TeamBlackboard ~= nil then TeamBlackboard:Reset() end
	_drag = nil
	spawnEncounter()
	if start then startMission() end
end
local function updateMission()
	if not isActive() then return end
	local allyAlive, enemyAlive = living(_allyIds), living(_enemyIds)
	if _player:GetHealth() <= 0 then finish("DEFEAT", "Commander down."); return end
	if allyAlive == 0 then finish("DEFEAT", "Both squadmates were lost."); return end
	_commands:Update(now())
	if _state == "WAVE" and enemyAlive == 0 then
		if _wave == 1 then
			_state = "ADVANCE"
			hint("Gate secure. Advance to the courtyard; no automatic redeployment.")
		else
			_state = "REGROUP"
			hint("All guards cleared. Regroup at the relay with one surviving ally.")
		end
		print("[Sandbox19Match] phase=" .. _state .. " elapsedMs=" .. (now() - _startedMs))
	elseif _state == "ADVANCE" then
		local entered = _player:GetPosition().z >= _anchors.trigger.z
		for _, id in ipairs(_allyIds) do
			local a = find(id)
			if a ~= nil and a:GetHealth() > 0 and a:GetPosition().z >= _anchors.trigger.z then entered = true end
		end
		if entered then startWave(); return end
	elseif _state == "REGROUP" then
		if distance(_player:GetPosition(), _anchors.goal) <= _anchors.goalRadius then
			for _, id in ipairs(_allyIds) do
				local a = find(id)
				if a ~= nil and a:GetHealth() > 0 and distance(a:GetPosition(), _anchors.goal) <= _anchors.goalRadius then
					finish("VICTORY", "Relay secured. The squad regrouped."); return
				end
			end
		end
	end
	if enemyAlive < _lastAlive then _lastProgressMs, _lastAlive = now(), enemyAlive end
	if _state == "WAVE" and now() - _lastProgressMs > 45000 then
		hint("Contact unresolved. Reposition or use the observer (I).", "info")
		print("[Sandbox19Match] phase=STALEMATE wave=" .. _wave .. " director=none")
		_lastProgressMs = now()
	end
end
local function screen(position)
	local p = SandboxCamera:WorldToScreen(position)
	if p.x < 0 or p.y < 0 or p.x > GameManager:getScreenWidth() or p.y > GameManager:getScreenHeight() then return nil end
	return p
end
local function pick(x, y, enemies)
	local best, bestSq = 0, 42 * 42
	for _, id in ipairs(enemies and _enemyIds or _allyIds) do
		local a = find(id)
		if a ~= nil and a:GetHealth() > 0 and (not enemies or visibleToSquad(id)) then
			local p = a:GetPosition(); p.y = p.y + 1
			local s = screen(p)
			if s ~= nil then
				local ds = (s.x - x)^2 + (s.y - y)^2
				if ds < bestSq then best, bestSq = id, ds end
			end
		end
	end
	return best
end
local function action(name, id)
	if os.getenv("HELLO_INPUT_REPLAY") ~= nil then
		print("[Sandbox19UI] synthetic=true action=" .. name .. " id=" .. tostring(id or -1))
	end
	if name == "start" then startMission()
	elseif name == "pause" then setPaused(true)
	elseif name == "resume" then setPaused(false)
	elseif name == "retry" then _restart, _restartStart = true, true
	elseif name == "menu" then _restart, _restartStart = true, false
	elseif name == "quit" then GameManager:RequestQuit()
	elseif name == "select_all" then selectAll(); _audio:Play("select", now())
	elseif name == "select" then
		local a = find(id)
		if a ~= nil and a:GetHealth() > 0 then _selection = {[id] = true}; _audio:Play("select", now()) end
	elseif name == "focus" then focus()
	elseif name == "rally" then issue("rally", -1, _player:GetPosition())
	elseif name == "retreat" then issue("retreat", -1, _anchors.fallback)
	elseif name == "cancel" then cancel()
	elseif name == "audio_up" then _audio.volume = math.min(1, _audio.volume + 0.1); _audio:Apply(true); _audio:Play("select", now())
	elseif name == "audio_down" then _audio.volume = math.max(0, _audio.volume - 0.1); _audio:Apply(true); _audio:Play("select", now())
	elseif name == "audio_mute" then _audio.muted = not _audio.muted; _audio:Apply(true) end
end
local function updateWorldMarkers()
	local visible = not _paused and _state ~= "PREPARE" and _state ~= "VICTORY" and _state ~= "DEFEAT"
	local slot = 1
	for _, id in ipairs(_allyIds) do
		local a = find(id)
		local mark = _worldMarks[slot]
		local p = a ~= nil and a:GetPosition() or nil
		if p ~= nil then p.y = p.y + 1.85 end
		local s = p ~= nil and screen(p) or nil
		local show = visible and a ~= nil and a:GetHealth() > 0 and s ~= nil
		mark:setVisible(show)
		local foot = a ~= nil and screen(a:GetPosition() - Vector3(0, 0.72, 0)) or nil
		_selectionMarks[slot]:setVisible(show and _selection[id] == true and foot ~= nil)
		if foot ~= nil then _selectionMarks[slot]:setPosition(Vector2(foot.x, foot.y)) end
		if show then
			mark:setPosition(Vector2(s.x, s.y - 8))
			mark:setBackgroundColor(_selection[id] and ColourValue(0.40, 0.93, 0.91, 1.0) or ColourValue(0.31, 0.70, 0.72, 0.92))
		end
		slot = slot + 1
	end
	for index = 1, 4 do
		local enemy = find(_enemyIds[index])
		local p = enemy ~= nil and enemy:GetPosition() or nil
		if p ~= nil then p.y = p.y + 1.4 end
		local ep = p ~= nil and screen(p) or nil
		local show = visible and enemy ~= nil and enemy:GetHealth() > 0 and ep ~= nil and visibleToSquad(enemy:GetObjId())
		local mark = _enemyMarks[index]
		mark.icon:setVisible(show); mark.track:setVisible(show); mark.hp:setVisible(show)
		if show then
			mark.icon:setPosition(Vector2(ep.x, ep.y - 9))
			mark.track:setPosition(Vector2(ep.x - 20, ep.y + 2))
			mark.hp:setPosition(Vector2(ep.x - 20, ep.y + 2))
			mark.hp:setDimension(Vector2(40 * math.max(0, enemy:GetHealth()) / enemy:GetAttribComponent():GetMaxHealth(), 3))
		end
	end
	for index, id in ipairs(_allyIds) do
		local order = _commands.active[id]
		local p = order ~= nil and order.position ~= nil and screen(order.position + Vector3(0, 0.15, 0)) or nil
		_orderMarks[index]:setVisible(visible and p ~= nil)
		if p ~= nil then _orderMarks[index]:setPosition(Vector2(p.x, p.y)) end
	end
	local target = find(_lastEnemy)
	local s = target ~= nil and target:GetHealth() > 0 and visibleToSquad(_lastEnemy)
		and screen(target:GetPosition() + Vector3(0, 1.9, 0)) or nil
	_targetMark:setVisible(visible and s ~= nil)
	if s ~= nil then _targetMark:setPosition(Vector2(s.x - 31, s.y - 10)) end
	local goal = _state == "ADVANCE" and _anchors.trigger.center or _anchors.goal
	local gs = screen(goal + Vector3(0, 2, 0))
	local showGoal = visible and gs ~= nil and (_state == "ADVANCE" or _state == "REGROUP")
	_goalLabel:setVisible(showGoal)
	if gs ~= nil then
		_goalLabel:setPosition(Vector2(gs.x - 43, gs.y - 12))
		_goalLabel:setMarkupText(GUI.MarkupColor.White .. GUI.Markup.Small ..
			(_state == "ADVANCE" and "COURTYARD" or "RELAY") .. "  " .. math.floor(distance(_player:GetPosition(), goal)) .. "m")
	end
	_dragFrame:setVisible(visible and _drag ~= nil)
	if _drag ~= nil then
		_dragFrame:setPosition(Vector2(math.min(_drag.x, _mouse.x), math.min(_drag.y, _mouse.y)))
		_dragFrame:setDimension(Vector2(math.max(1, math.abs(_mouse.x - _drag.x)), math.max(1, math.abs(_mouse.y - _drag.y))))
	end
end
local function updateHud()
	for id in pairs(_selection) do local a = find(id); if a == nil or a:GetHealth() <= 0 then _selection[id] = nil end end
	local allies = {}
	for index, id in ipairs(_allyIds) do
		local a = find(id)
		local bb = bbOf(a)
		local order = _commands.active[id]
		local currentAction = bb ~= nil and bb:GetString("__bt.currentAction") or ""
		allies[index] = {id = id, name = _names[index], hp = a ~= nil and a:GetHealth() or 0, maxHp = _matchConfig.allyHealth,
			alive = a ~= nil and a:GetHealth() > 0, selected = _selection[id] == true,
			command = order ~= nil and string.upper(order.kind) or (bb ~= nil and bb:Has("sandbox19.holdPos") and "HOLD" or "AUTONOMOUS"),
			status = currentAction ~= "" and string.upper(currentAction) or "READY"}
	end
	_hud:Update({state = _state, paused = _paused, objective = objective(),
		phase = _state == "PREPARE" and "BRIEFING" or (_wave == 1 and "GATE APPROACH" or "RELAY COURTYARD"),
		elapsedMs = _state == "PREPARE" and 0 or ((_endedMs or now()) - _startedMs),
		enemyAlive = living(_enemyIds), enemyTotal = 4, enemyKilled = #_enemyIds - living(_enemyIds),
		allyAlive = living(_allyIds), selectedCount = selectedCount(), commanderHp = _player:GetHealth(), commanderMaxHp = _matchConfig.commanderHealth,
		allies = allies, hint = now() <= _hintUntil and _hint or "", hintKind = _hintKind,
		audioVolume = _audio.volume, audioMuted = _audio.muted, audioAvailable = _audio.available,
		mouseX = _mouse.x, mouseY = _mouse.y, mouseDown = _mouse.down, orders = _commands.stats, endReason = _endReason},
		GameManager:getScreenWidth(), GameManager:getScreenHeight())
	_observer:Update(_selection, find, now(), 2147483647, _profiles)
	_observerPanel:setVisible(_observer.enabled)
	if _observer.enabled then
		_observerPanel:setPosition(Vector2(math.max(4, GameManager:getScreenWidth() - 490), 90))
		_observerPanel:setMarkupText(GUI.MarkupColor.White .. GUI.Markup.SmallMono .. table.concat(Observer.Lines(_observer.snapshot), "\n"))
	end
	updateWorldMarkers()
end

function EventHandle_Keyboard(keycode, pressed)
	GUI_HandleKeyEvent(keycode, pressed)
	if not pressed then return false end
	if keycode == OIS.KC_ESCAPE then
		if _state == "PREPARE" then return true end
		setPaused(not _paused); return true
	elseif keycode == OIS.KC_RETURN then
		if _state == "PREPARE" then startMission()
		elseif _state == "VICTORY" or _state == "DEFEAT" then _restart, _restartStart = true, true end
		return true
	elseif keycode == OIS.KC_I then _observer.enabled = not _observer.enabled; return true
	elseif keycode == OIS.KC_O then _observer:Dump(); return true
	elseif keycode == OIS.KC_F3 then
		_debug = not _debug
		_G.HELLO_SUPPRESS_AI_PATH_DRAW = not _debug
		Scene.SetNavigationDebug(_debug)
		return true
	end
	if not isActive() then return false end
	if keycode == OIS.KC_TAB then selectAll(); return true
	elseif keycode == OIS.KC_1 then action("select", _allyIds[1]); return true
	elseif keycode == OIS.KC_2 then action("select", _allyIds[2]); return true
	elseif keycode == OIS.KC_F then focus(); return true
	elseif keycode == OIS.KC_T then action("retreat"); return true
	elseif keycode == OIS.KC_G then action("rally"); return true
	elseif keycode == OIS.KC_X then cancel(); return true end
	return false
end
function EventHandle_Mouse(ctype, x, y, button)
	_mouse.x, _mouse.y = x, y
	local hit, id = nil, nil
	if _hud ~= nil then hit, id = _hud:HitTest(x, y) end
	if ctype == 1 then _mouse.down = true elseif ctype == 2 then _mouse.down = false end
	if hit ~= nil then
		if ctype == 1 and button == 0 then action(hit, id) end
		if ctype == 2 then _drag = nil end
		return true
	end
	if not isActive() then return true end
	if ctype == 1 and button == 0 then _drag = {x = x, y = y}; return true end
	if ctype == 2 and button == 0 and _drag ~= nil then
		if math.abs(x - _drag.x) + math.abs(y - _drag.y) < 8 then
			local ally = pick(x, y, false)
			_selection = ally > 0 and {[ally] = true} or {}
		else
			_selection = {}
			for _, allyId in ipairs(_allyIds) do
				local a = find(allyId)
				local s = a ~= nil and a:GetHealth() > 0 and screen(a:GetPosition()) or nil
				if s ~= nil and s.x >= math.min(x, _drag.x) and s.x <= math.max(x, _drag.x)
					and s.y >= math.min(y, _drag.y) and s.y <= math.max(y, _drag.y) then _selection[allyId] = true end
			end
		end
		_drag = nil
		if os.getenv("HELLO_INPUT_REPLAY") ~= nil then print("[Sandbox19Selection] synthetic=true count=" .. selectedCount()) end
		return true
	end
	if ctype == 1 and button == 1 then
		local enemy = pick(x, y, true)
		if enemy > 0 then _lastEnemy = enemy; focus()
		else
			local p = SandboxCamera:ScreenToGroundPoint(x, y, 0)
			if p.x == p.x and p.z == p.z and p.x >= -23 and p.x <= 23 and p.z >= -23 and p.z <= 39 then
				local projected = SandboxNav:FindClosestPoint("default", p)
				if distance(p, projected) <= 2 then issue("rally", -1, projected)
				else hint("That point is outside the walkable area.", "failed") end
			else hint("Choose a point inside the outpost.", "failed") end
		end
		return true
	end
	return ctype ~= 0
end
function EventHandle_WindowResized(width, height)
	GUI_WindowResized(width, height)
	if _player ~= nil then updateHud() end
end
function Sandbox_Initialize()
	GUI_CreateCameraAndProfileInfo()
	SandboxUI:SetBuildInfoVisible(false)
	GUI_CreateSandboxText(GUI.MarkupColor.White .. GUI.Markup.Medium ..
		"RELAY OUTPOST\nWASD move | Q/E orbit | Wheel zoom\nLMB / drag select | 1/2 / Tab squad\nRMB ground: move | enemy: focus\nF focus | T fallback + hold | G gather\nX cancel | Esc pause | I observer\nF3 paths | F5 performance | Enter start", {w = 450, h = 230}):setVisible(false)
	_G.HELLO_SUPPRESS_AI_PATH_DRAW = true
	SandboxAgentConfig:SetUseCppFsmFlag(true)
	_anchors = Scene.Create()
	_matchConfig = ConfigManager:GetSamplePreset(SAMPLE).commanderMatch
	SandboxScene:UpdateSceneGraph()
	_nav = Scene.CreateNavigation()
	if _G.HELLO_SANDBOX_SMOKE_MODE == true or os.getenv("HELLO_SANDBOX19_PRODUCT_TEST") == "1" then Scene.ValidateNavigation(_anchors) end
	-- Markers precede HUD so modal panels cover them.
	for i = 1, 2 do
		local ring = SandboxUI:CreatePolygon()
		ring:setSides(28); ring:setRadius(17)
		ring:setBackgroundColor(ColourValue(0.3, 0.85, 0.85, 0.10))
		ring:setBorder(2, ColourValue(0.3, 0.85, 0.85, 0.9)); ring:setVisible(false)
		_selectionMarks[i] = ring
		local mark = SandboxUI:CreatePolygon()
		mark:setSides(3); mark:setRadius(7); mark:setAngleDegrees(90); mark:setVisible(false)
		_worldMarks[i] = mark
		local destination = SandboxUI:CreatePolygon()
		destination:setSides(24); destination:setRadius(14)
		destination:setBackgroundColor(ColourValue(0.3, 0.85, 0.85, 0.14))
		destination:setBorder(2, ColourValue(0.3, 0.85, 0.85, 0.95)); destination:setVisible(false)
		_orderMarks[i] = destination
	end
	for i = 1, 4 do
		local icon = SandboxUI:CreatePolygon()
		icon:setSides(4); icon:setRadius(6); icon:setAngleDegrees(45)
		icon:setBackgroundColor(ColourValue(0.95, 0.52, 0.27, 1)); icon:setVisible(false)
		local track = SandboxUI:CreateUIFrame(); track:setDimension(Vector2(40, 3))
		track:setBackgroundColor(ColourValue(0.15, 0.12, 0.10, 0.9)); track:setVisible(false)
		local hp = SandboxUI:CreateUIFrame(); hp:setDimension(Vector2(40, 3))
		hp:setBackgroundColor(ColourValue(0.95, 0.52, 0.27, 1)); hp:setVisible(false)
		_enemyMarks[i] = {icon = icon, track = track, hp = hp}
	end
	_targetMark = SandboxUI:CreateUIFrame()
	_targetMark:setDimension(Vector2(62, 18)); _targetMark:setTextMargin(2, 7)
	_targetMark:setBackgroundColor(ColourValue(0.08, 0.10, 0.10, 0.90))
	_targetMark:setMarkupText(GUI.MarkupColor.White .. GUI.Markup.Small .. "FOCUS")
	_goalLabel = SandboxUI:CreateUIFrame()
	_goalLabel:setDimension(Vector2(86, 22)); _goalLabel:setTextMargin(3, 6)
	_goalLabel:setBackgroundColor(ColourValue(0.28, 0.22, 0.14, 0.88))
	_dragFrame = SandboxUI:CreateUIFrame()
	_dragFrame:setBackgroundColor(ColourValue(0.20, 0.80, 0.85, 0.18))
	_dragFrame:setVisible(false)
	_hud = Hud.New()
	_audio = Audio.New()
	_observerPanel = SandboxUI:CreateUIFrame()
	_observerPanel:setDimension(Vector2(480, 310)); _observerPanel:setTextMargin(8, 8)
	_observerPanel:setBackgroundColor(ColourValue(0.03, 0.06, 0.07, 0.96))
	_observerPanel:setVisible(false)
	spawnEncounter()
	initializeExperiment()
	updateHud()
	if _G.HELLO_SANDBOX_SMOKE_MODE == true or os.getenv("HELLO_SANDBOX19_PRODUCT_TEST") == "1"
		or os.getenv("HELLO_SANDBOX19_OBSERVATION_SELF_TEST") == "1" then
		_test = require("res.scripts.samples.sandbox19_product_selftest.lua").New()
	end
	print(ConfigManager:BuildDebugSummary(SAMPLE))
end
function Sandbox_Update(deltaMs)
	if _hud == nil or _player == nil then return end
	if _restart then local start = _restartStart; _restart = false; restartEncounter(start) end
	updateMission()
	if isActive() then _audio:Observe(ObjectManager:getAllAgents(), now()) end
	updateHud()
	GUI_UpdateCameraInfo()
	GUI_UpdateProfileInfo()
	if _test ~= nil then
		_test:Step({find = find, player = _player, allies = _allyIds, enemies = _enemyIds,
			commands = _commands, anchors = _anchors, state = _state, wave = _wave, selectAll = selectAll,
			start = startMission, issue = issue, pause = setPaused, restart = restartEncounter,
			finish = finish, observer = _observer, selected = _selection}, now())
	end
	updateExperiment()
end
