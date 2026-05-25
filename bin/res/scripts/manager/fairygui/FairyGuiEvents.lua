local NativeApi = require("res.scripts.manager.fairygui.FairyGuiNativeApi")

local FairyGuiEvents = Class("FairyGuiEvents")

local EVENT_TYPES = {
	Enter = 0,
	Exit = 1,
	Changed = 2,
	Submit = 3,
	TouchBegin = 10,
	TouchMove = 11,
	TouchEnd = 12,
	Click = 13,
	MouseWheel = 16,
	RightClick = 17,
	KeyDown = 30,
	KeyUp = 31,
	ClickItem = 50,
	RightClickItem = 53,
	DragStart = 60,
	DragMove = 61,
	DragEnd = 62,
}

local EVENT_NAMES = {}
for eventName, eventType in pairs(EVENT_TYPES) do
	EVENT_NAMES[eventType] = eventName
end

local EVENT_ALIASES = {
	Drag = "DragStart",
}

local function tableCount(source)
	local count = 0
	if type(source) ~= "table" then
		return count
	end
	for _, _ in pairs(source) do
		count = count + 1
	end
	return count
end

local function nowMs()
	return os.clock and os.clock() * 1000 or 0
end

local function formatMs(value)
	return string.format("%.3f", tonumber(value) or 0)
end

local function getEventType(eventType)
	if type(eventType) == "number" then
		return eventType
	end
	if type(eventType) ~= "string" then
		return nil
	end

	local resolvedName = EVENT_ALIASES[eventType] or eventType
	return EVENT_TYPES[resolvedName]
end

function FairyGuiEvents:Init(owner)
	self.owner = owner
	self.callbacks = owner ~= nil and owner.callbacks or self.callbacks or {}
	self.bindings = owner ~= nil and owner.bindings or self.bindings or {}
	self.bindingsByHandle = owner ~= nil and owner.bindingsByHandle or self.bindingsByHandle or {}
	self.transitionCallbacks = owner ~= nil and owner.transitionCallbacks or self.transitionCallbacks or {}
	self.transitionCallbacksByHandle = owner ~= nil and owner.transitionCallbacksByHandle or self.transitionCallbacksByHandle or {}
	self.timers = owner ~= nil and owner.timers or self.timers or {}
	self.timersByKey = owner ~= nil and owner.timersByKey or self.timersByKey or {}
	self.eventStats = owner ~= nil and owner.eventStats or self.eventStats or {}
	self.eventDispatchTotal = owner ~= nil and owner.eventDispatchTotal or self.eventDispatchTotal or 0
	self.lastEvent = owner ~= nil and owner.lastEvent or self.lastEvent
	self.nextCallbackId = owner ~= nil and owner.nextCallbackId or self.nextCallbackId or 1
	if owner ~= nil then
		owner.callbacks = self.callbacks
		owner.bindings = self.bindings
		owner.bindingsByHandle = self.bindingsByHandle
		owner.transitionCallbacks = self.transitionCallbacks
		owner.transitionCallbacksByHandle = self.transitionCallbacksByHandle
		owner.timers = self.timers
		owner.timersByKey = self.timersByKey
		owner.eventStats = self.eventStats
		owner.eventDispatchTotal = self.eventDispatchTotal
		owner.lastEvent = self.lastEvent
		owner.nextCallbackId = self.nextCallbackId
	end
end

function FairyGuiEvents:DebugInjectMouseMove(x, y)
	if NativeApi == nil or NativeApi.injectFairyGuiMouseMove == nil then
		return false
	end
	return NativeApi:injectFairyGuiMouseMove(tonumber(x) or 0, tonumber(y) or 0)
end

function FairyGuiEvents:DebugInjectMouseDown(x, y, button)
	if NativeApi == nil or NativeApi.injectFairyGuiMouseDown == nil then
		return false
	end
	return NativeApi:injectFairyGuiMouseDown(tonumber(x) or 0, tonumber(y) or 0, tonumber(button) or 0)
end

function FairyGuiEvents:DebugInjectMouseUp(x, y, button)
	if NativeApi == nil or NativeApi.injectFairyGuiMouseUp == nil then
		return false
	end
	return NativeApi:injectFairyGuiMouseUp(tonumber(x) or 0, tonumber(y) or 0, tonumber(button) or 0)
end

function FairyGuiEvents:DebugInjectMouseWheel(x, y, wheelDelta)
	if NativeApi == nil or NativeApi.injectFairyGuiMouseWheel == nil then
		return false
	end
	return NativeApi:injectFairyGuiMouseWheel(tonumber(x) or 0, tonumber(y) or 0, tonumber(wheelDelta) or 0)
end

function FairyGuiEvents:DebugInjectClick(x, y, button)
	if NativeApi == nil or NativeApi.injectFairyGuiClick == nil then
		return false
	end
	return NativeApi:injectFairyGuiClick(tonumber(x) or 0, tonumber(y) or 0, tonumber(button) or 0)
end

function FairyGuiEvents:DebugInjectKeyPressed(keyCode, keyText)
	if NativeApi == nil or NativeApi.injectFairyGuiKeyPressed == nil then
		return false
	end
	return NativeApi:injectFairyGuiKeyPressed(tonumber(keyCode) or 0, tonumber(keyText) or 0)
end

function FairyGuiEvents:DebugInjectKeyReleased(keyCode, keyText)
	if NativeApi == nil or NativeApi.injectFairyGuiKeyReleased == nil then
		return false
	end
	return NativeApi:injectFairyGuiKeyReleased(tonumber(keyCode) or 0, tonumber(keyText) or 0)
end

function FairyGuiEvents:RegisterTransitionCallback(handle, transitionName, callback)
	local self = self.owner
	if self == nil or handle == nil or type(callback) ~= "function" then
		return 0
	end

	local callbackId = self.nextCallbackId
	self.nextCallbackId = self.nextCallbackId + 1
	self.callbacks[callbackId] = callback
	self.transitionCallbacks[callbackId] = {
		callbackId = callbackId,
		handle = handle,
		transitionName = transitionName or "",
	}

	local callbackList = self.transitionCallbacksByHandle[handle]
	if callbackList == nil then
		callbackList = {}
		self.transitionCallbacksByHandle[handle] = callbackList
	end
	callbackList[callbackId] = true
	return callbackId
end

function FairyGuiEvents:RemoveTransitionCallback(callbackId)
	local self = self.owner
	if self == nil then
		return false
	end

	local callbackInfo = self.transitionCallbacks[callbackId]
	if callbackInfo == nil then
		return false
	end

	self.callbacks[callbackId] = nil
	self.transitionCallbacks[callbackId] = nil
	local callbackList = self.transitionCallbacksByHandle[callbackInfo.handle]
	if callbackList ~= nil then
		callbackList[callbackId] = nil
		if tableCount(callbackList) <= 0 then
			self.transitionCallbacksByHandle[callbackInfo.handle] = nil
		end
	end
	return true
end

function FairyGuiEvents:ClearTransitionCallbacksForHandle(handle, transitionName)
	local self = self.owner
	if self == nil then
		return
	end

	local callbackList = self.transitionCallbacksByHandle[handle]
	if callbackList == nil then
		return
	end

	local callbackIds = {}
	for callbackId, _ in pairs(callbackList) do
		local callbackInfo = self.transitionCallbacks[callbackId]
		if transitionName == nil or transitionName == "" or callbackInfo == nil or callbackInfo.transitionName == transitionName then
			table.insert(callbackIds, callbackId)
		end
	end
	for _, callbackId in ipairs(callbackIds) do
		self:RemoveTransitionCallback(callbackId)
	end
end

function FairyGuiEvents:PlayTransition(handle, transitionName, times, delay, callback)
	local self = self.owner
	if self == nil then
		return false
	end

	if type(transitionName) == "table" then
		local options = transitionName
		transitionName = options.name or options.transitionName or ""
		times = options.times
		delay = options.delay
		callback = options.onComplete or options.callback
	elseif type(times) == "function" and callback == nil then
		callback = times
		times = nil
		delay = nil
	elseif type(delay) == "function" and callback == nil then
		callback = delay
		delay = nil
	end

	if NativeApi == nil or NativeApi.playFairyGuiTransition == nil or handle == nil then
		return false
	end

	transitionName = transitionName or ""
	local callbackId = self:RegisterTransitionCallback(handle, transitionName, callback)
	local played = NativeApi:playFairyGuiTransition(handle, transitionName, times or 1, delay or 0, callbackId or 0)
	if not played and callbackId ~= nil and callbackId > 0 then
		self:RemoveTransitionCallback(callbackId)
	end
	return played
end

function FairyGuiEvents:StopTransition(handle, transitionName, setToComplete, processCallback)
	local self = self.owner
	if self == nil then
		return false
	end

	if type(transitionName) == "table" then
		local options = transitionName
		transitionName = options.name or options.transitionName or ""
		setToComplete = options.setToComplete
		processCallback = options.processCallback
	end
	if NativeApi == nil or NativeApi.stopFairyGuiTransition == nil or handle == nil then
		return false
	end

	transitionName = transitionName or ""
	if processCallback == nil then
		processCallback = true
	end
	local stopped = NativeApi:stopFairyGuiTransition(handle, transitionName, setToComplete == true, processCallback == true)
	if stopped and processCallback ~= true then
		self:ClearTransitionCallbacksForHandle(handle, transitionName)
	end
	return stopped
end

function FairyGuiEvents:_DispatchTransition(callbackId, objectHandle, transitionName)
	local self = self.owner
	if self == nil then
		return false
	end

	local callback = self.callbacks[callbackId]
	self:RemoveTransitionCallback(callbackId)
	if callback == nil then
		return false
	end

	local ok, err = pcall(callback, {
		callbackId = callbackId,
		objectHandle = objectHandle,
		transitionName = transitionName or "",
	})
	if not ok then
		print("[FGUI] transition callback error:", err)
		return false
	end
	return true
end

function FairyGuiEvents:AddEvent(handle, childPath, eventType, callback)
	local self = self.owner
	if self == nil or NativeApi == nil or handle == nil or type(callback) ~= "function" then
		return nil
	end

	local resolvedEventType = getEventType(eventType)
	if resolvedEventType == nil then
		return nil
	end

	local callbackId = self.nextCallbackId
	self.nextCallbackId = self.nextCallbackId + 1
	self.callbacks[callbackId] = callback

	local bindingId = nil
	if NativeApi.addFairyGuiEventListener ~= nil then
		bindingId = NativeApi:addFairyGuiEventListener(handle, childPath or "", resolvedEventType, callbackId)
	elseif resolvedEventType == EVENT_TYPES.Click then
		bindingId = NativeApi:addFairyGuiClickListener(handle, childPath or "", callbackId)
	end
	if bindingId == nil or bindingId <= 0 then
		self.callbacks[callbackId] = nil
		local objectInfo = self:GetObjectInfo(handle)
		self:RecordResourceFallback("missingEventTarget", {
			handle = handle,
			key = objectInfo ~= nil and objectInfo.key or nil,
			uiName = objectInfo ~= nil and objectInfo.uiName or nil,
			packageName = objectInfo ~= nil and objectInfo.packageName or nil,
			packagePath = objectInfo ~= nil and objectInfo.packagePath or nil,
			objectName = objectInfo ~= nil and objectInfo.objectName or nil,
			childPath = childPath or "",
			eventType = eventType,
		}, "addFairyGuiEventListener returned empty binding")
		return nil
	end

	self.bindings[bindingId] = {
		bindingId = bindingId,
		callbackId = callbackId,
		handle = handle,
		childPath = childPath or "",
		eventType = resolvedEventType,
	}

	local bindingList = self.bindingsByHandle[handle]
	if bindingList == nil then
		bindingList = {}
		self.bindingsByHandle[handle] = bindingList
	end
	bindingList[bindingId] = true
	return bindingId
end

function FairyGuiEvents:AddClick(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "Click", callback)
end

function FairyGuiEvents:AddChanged(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "Changed", callback)
end

function FairyGuiEvents:AddFocusIn(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "Enter", callback)
end

function FairyGuiEvents:AddFocusOut(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "Exit", callback)
end

function FairyGuiEvents:AddControllerChanged(handle, controllerName, callback)
	local self = self.owner
	if self == nil or NativeApi == nil or NativeApi.addFairyGuiControllerChangedListener == nil or handle == nil or type(callback) ~= "function" then
		return nil
	end

	local callbackId = self.nextCallbackId
	self.nextCallbackId = self.nextCallbackId + 1
	self.callbacks[callbackId] = callback

	local bindingId = NativeApi:addFairyGuiControllerChangedListener(handle, controllerName or "", callbackId)
	if bindingId == nil or bindingId <= 0 then
		self.callbacks[callbackId] = nil
		return nil
	end

	self.bindings[bindingId] = {
		bindingId = bindingId,
		callbackId = callbackId,
		handle = handle,
		childPath = "",
		eventType = "ControllerChanged",
		controllerName = controllerName or "",
	}

	local bindingList = self.bindingsByHandle[handle]
	if bindingList == nil then
		bindingList = {}
		self.bindingsByHandle[handle] = bindingList
	end
	bindingList[bindingId] = true
	return bindingId
end

function FairyGuiEvents:AddClickItem(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "ClickItem", callback)
end

function FairyGuiEvents:AddRightClick(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "RightClick", callback)
end

function FairyGuiEvents:AddMouseWheel(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "MouseWheel", callback)
end

function FairyGuiEvents:AddKeyDown(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "KeyDown", callback)
end

function FairyGuiEvents:AddKeyUp(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "KeyUp", callback)
end

function FairyGuiEvents:AddSubmit(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "Submit", callback)
end

function FairyGuiEvents:AddTouchBegin(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "TouchBegin", callback)
end

function FairyGuiEvents:AddTouchEnd(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "TouchEnd", callback)
end

function FairyGuiEvents:AddDragStart(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "DragStart", callback)
end

function FairyGuiEvents:AddDragMove(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "DragMove", callback)
end

function FairyGuiEvents:AddDragEnd(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "DragEnd", callback)
end

function FairyGuiEvents:RemoveBinding(bindingId)
	local self = self.owner
	if self == nil then
		return false
	end

	local binding = self.bindings[bindingId]
	if binding == nil then
		return false
	end

	if NativeApi ~= nil then
		NativeApi:removeFairyGuiListener(bindingId)
	end
	self.callbacks[binding.callbackId] = nil
	self.bindings[bindingId] = nil

	local bindingList = self.bindingsByHandle[binding.handle]
	if bindingList ~= nil then
		bindingList[bindingId] = nil
	end
	return true
end

function FairyGuiEvents:ClearBindingsForHandle(handle)
	local self = self.owner
	if self == nil then
		return
	end

	local bindingList = self.bindingsByHandle[handle]
	if bindingList == nil then
		return
	end

	local bindingIds = {}
	for bindingId, _ in pairs(bindingList) do
		table.insert(bindingIds, bindingId)
	end
	for _, bindingId in ipairs(bindingIds) do
		self:RemoveBinding(bindingId)
	end
	self.bindingsByHandle[handle] = nil
end

function FairyGuiEvents:_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId, wheelDelta, dragDeltaX, dragDeltaY)
	local self = self.owner
	if self == nil then
		return false
	end

	local callback = self.callbacks[callbackId]
	if callback == nil then
		return false
	end

	local binding = self.bindings[bindingId]
	local itemIndex = rawItemIndex ~= nil and rawItemIndex >= 0 and rawItemIndex + 1 or nil
	local eventData = nil
	local eventListHandle = nil
	if binding ~= nil and itemIndex ~= nil then
		eventListHandle = self:GetTargetHandle(binding.handle, binding.childPath)
		local listData = eventListHandle ~= nil and self.listDataByHandle[eventListHandle] or nil
		eventData = listData ~= nil and listData[itemIndex] or nil
	end

	local eventName = EVENT_NAMES[eventType] or eventType
	if binding ~= nil and binding.eventType == "ControllerChanged" then
		eventName = "ControllerChanged"
	end
	self.eventDispatchTotal = (self.eventDispatchTotal or 0) + 1
	self.eventStats[eventName] = (self.eventStats[eventName] or 0) + 1
	self.lastEvent = {
		callbackId = callbackId,
		rootHandle = rootHandle,
		senderHandle = senderHandle,
		itemHandle = itemHandle,
		itemIndex = itemIndex,
		x = x,
		y = y,
		button = button,
		touchId = touchId,
		wheelDelta = wheelDelta or 0,
		dragDeltaX = dragDeltaX or 0,
		dragDeltaY = dragDeltaY or 0,
		eventType = eventName,
		eventTypeId = eventType,
		bindingId = bindingId,
	}

	local eventPayload = {
		callbackId = callbackId,
		rootHandle = rootHandle,
		senderHandle = senderHandle,
		itemHandle = itemHandle,
		itemIndex = itemIndex,
		rawItemIndex = rawItemIndex,
		itemData = eventData,
		x = x,
		y = y,
		button = button,
		touchId = touchId,
		wheelDelta = wheelDelta or 0,
		dragDeltaX = dragDeltaX or 0,
		dragDeltaY = dragDeltaY or 0,
		eventType = eventName,
		eventTypeId = eventType,
		bindingId = bindingId,
	}
	if eventListHandle ~= nil and self.treeDataByHandle[eventListHandle] ~= nil and type(eventData) == "table" then
		eventPayload.treeNode = eventData
		eventPayload.treeSource = eventData.source
		eventPayload.treeKey = eventData.key
		eventPayload.treeParentKey = eventData.parentKey
		eventPayload.treeDepth = eventData.depth or 0
		eventPayload.treeExpanded = eventData.expanded == true
		eventPayload.treeHasChildren = eventData.hasChildren == true
		eventPayload.treeSelected = eventData.selected == true
		self.lastEvent.treeKey = eventPayload.treeKey
		self.lastEvent.treeDepth = eventPayload.treeDepth
	end
	if binding ~= nil and binding.eventType == "ControllerChanged" then
		eventPayload.controllerName = binding.controllerName or ""
		eventPayload.selectedIndex = self:GetControllerIndex(binding.handle, eventPayload.controllerName)
		eventPayload.selectedPage = self:GetControllerPage(binding.handle, eventPayload.controllerName)
		eventPayload.selectedPageId = self:GetControllerPageId(binding.handle, eventPayload.controllerName)
		eventPayload.pageCount = self:GetControllerPageCount(binding.handle, eventPayload.controllerName)
	end

	local startMs = nowMs()
	local ok, err = pcall(callback, eventPayload)
	local elapsedMs = nowMs() - startMs
	self:RecordPerf("event", elapsedMs, eventName, ok)
	self.lastEvent.elapsedMs = elapsedMs
	if not ok then
		print("[FGUI] event callback error:", err)
		return false
	end
	return true
end

function FairyGuiEvents:DumpBindings()
	local self = self.owner
	if self == nil then
		return
	end

	local count = 0
	for _, _ in pairs(self.bindings) do
		count = count + 1
	end
	print("[FGUI] DumpBindings count=", count)
	for bindingId, binding in pairs(self.bindings) do
		print("[FGUI] Binding", bindingId, "handle=", binding.handle, "child=", binding.childPath, "eventType=", binding.eventType)
	end
end

function FairyGuiEvents:GetEventStats()
	local self = self.owner
	if self == nil then
		return {
			total = 0,
			events = {},
			lastEvent = nil,
		}
	end

	return {
		total = self.eventDispatchTotal or 0,
		events = self.eventStats or {},
		lastEvent = self.lastEvent,
	}
end

function FairyGuiEvents:DumpEventStats()
	local stats = self:GetEventStats()
	local owner = self.owner
	local perf = owner ~= nil and owner:GetPerfStat("event") or {}
	print("[FGUI] DumpEventStats total=", stats.total, "avgMs=", formatMs(perf.avgMs), "maxMs=", formatMs(perf.maxMs), "lastMs=", formatMs(perf.lastMs), "last=", perf.lastName or "")
	for eventType, count in pairs(stats.events) do
		print("[FGUI] EventStat", eventType, count)
	end
	if stats.lastEvent ~= nil then
		print("[FGUI] LastEvent", stats.lastEvent.eventType, "root=", stats.lastEvent.rootHandle, "sender=", stats.lastEvent.senderHandle, "item=", stats.lastEvent.itemHandle, "itemIndex=", stats.lastEvent.itemIndex, "x=", stats.lastEvent.x, "y=", stats.lastEvent.y, "button=", stats.lastEvent.button, "wheel=", stats.lastEvent.wheelDelta, "dragDelta=", stats.lastEvent.dragDeltaX, stats.lastEvent.dragDeltaY, "elapsedMs=", formatMs(stats.lastEvent.elapsedMs))
	end
end

return FairyGuiEvents
