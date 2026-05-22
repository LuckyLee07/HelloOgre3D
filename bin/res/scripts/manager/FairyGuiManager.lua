require("res.scripts.manager.FairyGuiProfiler")
require("res.scripts.manager.FairyGuiPackage")
require("res.scripts.manager.FairyGuiServices")
require("res.scripts.manager.FairyGuiLayers")
require("res.scripts.manager.FairyGuiEvents")
require("res.scripts.manager.FairyGuiLists")

local FairyGuiManager = Class("FairyGuiManager")

local instance = nil

local function isBlank(value)
	return value == nil or value == ""
end

local function getViewClassName(viewClass)
	if type(viewClass) == "table" then
		return viewClass.className
	end
	if type(viewClass) ~= "string" then
		return nil
	end
	return string.match(viewClass, "([^%.]+)$") or viewClass
end

local function loadViewClass(viewClass)
	if type(viewClass) == "table" then
		return viewClass
	end

	if type(viewClass) ~= "string" then
		return nil
	end

	local className = getViewClassName(viewClass)
	local viewClassInfo = ClassList[className]
	if viewClassInfo ~= nil then
		return viewClassInfo
	end

	local ok, result = pcall(require, viewClass)
	if ok and type(result) == "table" then
		return result
	end
	return ClassList[className]
end

local function loadClass(classInfo)
	return loadViewClass(classInfo)
end

local function isAutoGenClass(classInfo)
	if type(classInfo) == "table" then
		return classInfo.className ~= nil and string.match(classInfo.className, "AutoGen$") ~= nil
	end
	if type(classInfo) ~= "string" then
		return false
	end
	local className = getViewClassName(classInfo)
	return className ~= nil and string.match(className, "AutoGen$") ~= nil
end

local function callView(view, functionName, ...)
	local func = view and view[functionName]
	if func ~= nil then
		return func(view, ...)
	end
	return nil
end

local function copyTable(source, target)
	target = target or {}
	if type(source) ~= "table" then
		return target
	end

	for name, value in pairs(source) do
		target[name] = value
	end
	return target
end

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

local function isEnvEnabled(name)
	local value = os.getenv and os.getenv(name) or nil
	return value == "1" or value == "true" or value == "TRUE" or value == "True"
end

local function pushUniqueHandle(handles, handleSet, handle)
	if type(handle) ~= "number" or handle <= 0 or handleSet[handle] == true then
		return
	end
	handleSet[handle] = true
	table.insert(handles, handle)
end

local function containsHandle(handles, targetHandle)
	if type(handles) ~= "table" or targetHandle == nil then
		return false
	end

	for _, handle in ipairs(handles) do
		if handle == targetHandle then
			return true
		end
	end
	return false
end

local DEFAULT_LAYER_ORDER = {
	Background = 1000,
	Normal = 2000,
	Popup = 3000,
	Guide = 4000,
	Top = 5000,
	Toast = 6000,
}

local function callLifecycle(view, functionName, ...)
	local func = view and view[functionName]
	if func ~= nil then
		return func(view, ...)
	end
	return nil
end

local function normalizeCloseArgs(forceDestroy, reason)
	if type(forceDestroy) == "table" then
		return forceDestroy.forceDestroy == true, forceDestroy.reason or reason or "close"
	end
	if type(forceDestroy) == "string" and reason == nil then
		return false, forceDestroy
	end
	return forceDestroy == true, reason or (forceDestroy == true and "destroy" or "close")
end

local function detachView(manager, objectInfo, reason)
	if objectInfo == nil or objectInfo.view == nil then
		return
	end

	local view = objectInfo.view
	callLifecycle(view, "OnHide", reason)
	callLifecycle(view, "OnRemove", reason)
	callLifecycle(view, "OnDestroy", reason)
	if view._Detach ~= nil then
		view:_Detach()
	else
		view.handle = nil
	end

	if manager.views ~= nil then
		manager.views[objectInfo.key] = nil
	end
	if manager.viewsByHandle ~= nil then
		manager.viewsByHandle[objectInfo.handle] = nil
	end
	if manager.controllers ~= nil then
		manager.controllers[objectInfo.key] = nil
	end
	if manager.controllersByHandle ~= nil then
		manager.controllersByHandle[objectInfo.handle] = nil
	end
	objectInfo.ctrl = nil
	objectInfo.view = nil
end

function FairyGuiManager:GetInst()
	if instance == nil then
		instance = ClassList.FairyGuiManager.instance()
	end
	return instance
end

function FairyGuiManager:Init()
	self.packageRoot = "res/fuires"
	self.packages = {}
	self.packagesByName = {}
	self.objects = {}
	self.objectsByHandle = {}
	self.uiRegistry = {}
	self.uiNameToKey = {}
	self.hiddenObjects = {}
	self.childrenByHandle = {}
	self.listItemHandlesByHandle = {}
	self.listDataByHandle = {}
	self.listRenderersByHandle = {}
	self.views = {}
	self.viewsByHandle = {}
	self.controllers = {}
	self.controllersByHandle = {}
	self.currentSceneName = "Default"
	self.designWidth = nil
	self.designHeight = nil
	self.scaleMode = "stretch"
	self.layers = copyTable(DEFAULT_LAYER_ORDER)
	self.layerNextOrder = {}
	self.layerObjects = {}
	self.layerRoots = {}
	for layerName, _ in pairs(self.layers) do
		self.layerNextOrder[layerName] = 0
		self.layerObjects[layerName] = {}
	end
	self.uiStack = {}
	self.popupStack = {}
	self.stackEntriesByKey = {}
	self.nextStackSerial = 0
	self.callbacks = {}
	self.bindings = {}
	self.bindingsByHandle = {}
	self.transitionCallbacks = {}
	self.transitionCallbacksByHandle = {}
	self.timers = {}
	self.timersByKey = {}
	self.eventStats = {}
	self.eventDispatchTotal = 0
	self.lastEvent = nil
	self.perfStats = {}
	self.profiler = ClassList.FairyGuiProfiler.new(self)
	self.packageManager = ClassList.FairyGuiPackage.new(self)
	self.services = ClassList.FairyGuiServices.new(self)
	self.layersManager = ClassList.FairyGuiLayers.new(self)
	self.events = ClassList.FairyGuiEvents.new(self)
	self.lists = ClassList.FairyGuiLists.new(self)
	self.toastQueue = {}
	self.toastActive = nil
	self.toastSerial = 0
	self.toastDedupe = {}
	self.loadingRefs = {}
	self.loadingRefTotal = 0
	self.nextCallbackId = 1
end

function FairyGuiManager:GetProfiler()
	if self.profiler == nil then
		self.profiler = ClassList.FairyGuiProfiler.new(self)
	end
	return self.profiler
end

function FairyGuiManager:GetPackageManager()
	if self.packageManager == nil then
		self.packageManager = ClassList.FairyGuiPackage.new(self)
	end
	return self.packageManager
end

function FairyGuiManager:GetServices()
	if self.services == nil then
		self.services = ClassList.FairyGuiServices.new(self)
	end
	return self.services
end

function FairyGuiManager:GetLayers()
	if self.layersManager == nil then
		self.layersManager = ClassList.FairyGuiLayers.new(self)
	end
	return self.layersManager
end

function FairyGuiManager:GetEvents()
	if self.events == nil then
		self.events = ClassList.FairyGuiEvents.new(self)
	end
	return self.events
end

function FairyGuiManager:GetLists()
	if self.lists == nil then
		self.lists = ClassList.FairyGuiLists.new(self)
	end
	return self.lists
end

function FairyGuiManager:IsAvailable()
	if GameManager == nil then
		return false
	end
	return GameManager:isFairyGuiAvailable()
end

function FairyGuiManager:GetPackageNameByPath(packagePath)
	return self:GetPackageManager():GetPackageNameByPath(packagePath)
end

function FairyGuiManager:SetPackageRoot(packageRoot)
	return self:GetPackageManager():SetPackageRoot(packageRoot)
end

function FairyGuiManager:ResolvePackagePath(packagePath)
	return self:GetPackageManager():ResolvePackagePath(packagePath)
end

function FairyGuiManager:AddPackage(packageList)
	return self:GetPackageManager():AddPackage(packageList)
end

function FairyGuiManager:LoadPackage(packagePath, packageName)
	return self:GetPackageManager():LoadPackage(packagePath, packageName)
end

function FairyGuiManager:RetainPackage(packagePath, packageName)
	return self:GetPackageManager():RetainPackage(packagePath, packageName)
end

function FairyGuiManager:ReleasePackage(packagePath, packageName, unloadWhenZero)
	return self:GetPackageManager():ReleasePackage(packagePath, packageName, unloadWhenZero)
end

function FairyGuiManager:PreloadPackage(packagePath, packageName)
	return self:GetPackageManager():PreloadPackage(packagePath, packageName)
end

function FairyGuiManager:CreateObject(packageName, objectName)
	return self:GetPackageManager():CreateObject(packageName, objectName)
end

function FairyGuiManager:CreateContainer(name, ownerHandle)
	if not self:IsAvailable() then
		return 0
	end
	if ownerHandle ~= nil and GameManager.createFairyGuiChildContainer ~= nil then
		return GameManager:createFairyGuiChildContainer(ownerHandle, name or "")
	end
	if GameManager.createFairyGuiContainer == nil then
		return 0
	end
	return GameManager:createFairyGuiContainer(name or "")
end

function FairyGuiManager:CreateLoader(ownerHandle, name, url)
	if not self:IsAvailable() or GameManager.createFairyGuiLoader == nil then
		return 0
	end
	return GameManager:createFairyGuiLoader(ownerHandle or 0, name or "", url or "")
end

function FairyGuiManager:CreateText(ownerHandle, name, text, fontSize, red, green, blue)
	if not self:IsAvailable() or GameManager.createFairyGuiText == nil then
		return 0
	end
	return GameManager:createFairyGuiText(ownerHandle or 0, name or "", text or "", fontSize or 18, red or 255, green or 255, blue or 255)
end

function FairyGuiManager:CreateTextInput(ownerHandle, name, text, fontSize, red, green, blue)
	if not self:IsAvailable() or GameManager.createFairyGuiTextInput == nil then
		return 0
	end
	return GameManager:createFairyGuiTextInput(ownerHandle or 0, name or "", text or "", fontSize or 18, red or 255, green or 255, blue or 255)
end

function FairyGuiManager:GetRenderStats()
	return self:GetProfiler():GetRenderStats()
end

function FairyGuiManager:DumpRenderStats()
	return self:GetProfiler():DumpRenderStats()
end

function FairyGuiManager:RecordPerf(category, elapsedMs, name, success)
	return self:GetProfiler():RecordPerf(category, elapsedMs, name, success)
end

function FairyGuiManager:GetPerfStat(category)
	return self:GetProfiler():GetPerfStat(category)
end

function FairyGuiManager:GetPerfStats()
	return self:GetProfiler():GetPerfStats()
end

function FairyGuiManager:DumpPerfStats()
	return self:GetProfiler():DumpPerfStats()
end

function FairyGuiManager:GetDebugStats()
	return self:GetProfiler():GetDebugStats()
end

function FairyGuiManager:DumpDebugStats()
	return self:GetProfiler():DumpDebugStats()
end

function FairyGuiManager:GetHealthStats()
	return self:GetProfiler():GetHealthStats()
end

function FairyGuiManager:DumpHealth(verbose)
	return self:GetProfiler():DumpHealth(verbose)
end

function FairyGuiManager:SetStrictLifecycle(enabled)
	self.strictLifecycle = enabled == true
end

function FairyGuiManager:IsStrictLifecycleEnabled()
	if self.strictLifecycle ~= nil then
		return self.strictLifecycle == true
	end
	return isEnvEnabled("HELLO_FGUI_STRICT_LIFECYCLE")
end

function FairyGuiManager:CollectOwnedHandles(objectInfo)
	local handles = {}
	local handleSet = {}
	if objectInfo == nil then
		return handles, handleSet
	end

	pushUniqueHandle(handles, handleSet, objectInfo.handle)
	if type(objectInfo.ownedHandles) == "table" then
		for _, ownedHandle in pairs(objectInfo.ownedHandles) do
			pushUniqueHandle(handles, handleSet, ownedHandle)
		end
	end
	for fieldName, value in pairs(objectInfo) do
		if type(fieldName) == "string" and type(value) == "number" and string.match(fieldName, "Handle$") ~= nil then
			pushUniqueHandle(handles, handleSet, value)
		elseif type(fieldName) == "string" and type(value) == "table" and string.match(fieldName, "Handles$") ~= nil then
			for _, ownedHandle in pairs(value) do
				pushUniqueHandle(handles, handleSet, ownedHandle)
			end
		end
	end

	local index = 1
	while index <= #handles do
		local handle = handles[index]
		local childHandles = self.childrenByHandle[handle]
		if childHandles ~= nil then
			for _, childHandle in pairs(childHandles) do
				pushUniqueHandle(handles, handleSet, childHandle)
			end
		end

		local itemHandles = self.listItemHandlesByHandle[handle]
		if itemHandles ~= nil then
			for _, itemHandle in pairs(itemHandles) do
				pushUniqueHandle(handles, handleSet, itemHandle)
			end
		end
		index = index + 1
	end
	return handles, handleSet
end

function FairyGuiManager:CreateCloseSnapshot(objectInfo, ownedHandles)
	if objectInfo == nil then
		return nil
	end

	local snapshot = {
		key = objectInfo.key,
		handle = objectInfo.handle,
		uiName = objectInfo.uiName,
		layer = objectInfo.layer,
		modalMaskHandle = objectInfo.modalMaskHandle,
		viewRef = objectInfo.view,
		ctrlRef = objectInfo.ctrl,
		ownedHandles = {},
	}
	if ownedHandles == nil then
		ownedHandles = self:CollectOwnedHandles(objectInfo)
	end
	for index, handle in ipairs(ownedHandles) do
		snapshot.ownedHandles[index] = handle
	end
	return snapshot
end

function FairyGuiManager:CaptureCloseSnapshot(keyOrHandle)
	local objectInfo = self:GetObjectInfo(keyOrHandle)
	if objectInfo == nil then
		return nil
	end
	local ownedHandles = self:CollectOwnedHandles(objectInfo)
	return self:CreateCloseSnapshot(objectInfo, ownedHandles)
end

function FairyGuiManager:GetCloseResidue(objectInfo, ownedHandles)
	local issues = {}
	if objectInfo == nil then
		table.insert(issues, "objectInfo=nil")
		return issues
	end

	local key = objectInfo.key
	local handle = objectInfo.handle
	local handleSet = {}
	local handles = ownedHandles or objectInfo.ownedHandles or {}
	for _, ownedHandle in ipairs(handles) do
		if type(ownedHandle) == "number" and ownedHandle > 0 then
			handleSet[ownedHandle] = true
		end
	end
	if type(handle) == "number" and handle > 0 then
		handleSet[handle] = true
	end
	if type(objectInfo.modalMaskHandle) == "number" and objectInfo.modalMaskHandle > 0 then
		handleSet[objectInfo.modalMaskHandle] = true
	end

	if key ~= nil and self.objects[key] ~= nil then
		table.insert(issues, "objects[" .. tostring(key) .. "]")
	end
	if key ~= nil and self.hiddenObjects[key] ~= nil then
		table.insert(issues, "hiddenObjects[" .. tostring(key) .. "]")
	end
	if key ~= nil and self.views[key] ~= nil then
		table.insert(issues, "views[" .. tostring(key) .. "]")
	end
	if key ~= nil and self.controllers[key] ~= nil then
		table.insert(issues, "controllers[" .. tostring(key) .. "]")
	end
	if key ~= nil and self.stackEntriesByKey[key] ~= nil then
		table.insert(issues, "stackEntriesByKey[" .. tostring(key) .. "]")
	end
	if key ~= nil and self.timersByKey[key] ~= nil and tableCount(self.timersByKey[key]) > 0 then
		table.insert(issues, "timersByKey[" .. tostring(key) .. "]")
	end
	if objectInfo.uiName ~= nil and self.uiNameToKey[objectInfo.uiName] == key then
		table.insert(issues, "uiNameToKey[" .. tostring(objectInfo.uiName) .. "]")
	end
	if objectInfo.viewRef ~= nil and objectInfo.viewRef.GetTimerCount ~= nil and objectInfo.viewRef:GetTimerCount() > 0 then
		table.insert(issues, "viewTimer[" .. tostring(objectInfo.viewRef:GetTimerCount()) .. "]")
	end
	if objectInfo.ctrlRef ~= nil and objectInfo.ctrlRef.GetTimerCount ~= nil and objectInfo.ctrlRef:GetTimerCount() > 0 then
		table.insert(issues, "ctrlTimer[" .. tostring(objectInfo.ctrlRef:GetTimerCount()) .. "]")
	end

	for ownedHandle, _ in pairs(handleSet) do
		if self.objectsByHandle[ownedHandle] ~= nil then
			table.insert(issues, "objectsByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if self.viewsByHandle[ownedHandle] ~= nil then
			table.insert(issues, "viewsByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if self.controllersByHandle[ownedHandle] ~= nil then
			table.insert(issues, "controllersByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if self.bindingsByHandle[ownedHandle] ~= nil and tableCount(self.bindingsByHandle[ownedHandle]) > 0 then
			table.insert(issues, "bindingsByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if self.transitionCallbacksByHandle[ownedHandle] ~= nil and tableCount(self.transitionCallbacksByHandle[ownedHandle]) > 0 then
			table.insert(issues, "transitionCallbacksByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if self.childrenByHandle[ownedHandle] ~= nil and tableCount(self.childrenByHandle[ownedHandle]) > 0 then
			table.insert(issues, "childrenByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if self.listItemHandlesByHandle[ownedHandle] ~= nil then
			table.insert(issues, "listItemHandlesByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if self.listDataByHandle[ownedHandle] ~= nil then
			table.insert(issues, "listDataByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if self.listRenderersByHandle[ownedHandle] ~= nil then
			table.insert(issues, "listRenderersByHandle[" .. tostring(ownedHandle) .. "]")
		end
	end

	for bindingId, binding in pairs(self.bindings) do
		if binding ~= nil and binding.handle ~= nil and handleSet[binding.handle] == true then
			table.insert(issues, "bindings[" .. tostring(bindingId) .. "]")
		end
	end
	for callbackId, callbackInfo in pairs(self.transitionCallbacks) do
		if callbackInfo ~= nil and callbackInfo.handle ~= nil and handleSet[callbackInfo.handle] == true then
			table.insert(issues, "transitionCallbacks[" .. tostring(callbackId) .. "]")
		end
	end
	for timerId, timerInfo in pairs(self.timers) do
		if timerInfo ~= nil and timerInfo.key == key then
			table.insert(issues, "timers[" .. tostring(timerId) .. "]")
		end
	end
	for parentHandle, childHandles in pairs(self.childrenByHandle) do
		if handleSet[parentHandle] == true then
			table.insert(issues, "childrenByHandleParent[" .. tostring(parentHandle) .. "]")
		elseif type(childHandles) == "table" then
			for childPath, childHandle in pairs(childHandles) do
				if childHandle ~= nil and handleSet[childHandle] == true then
					table.insert(issues, "childrenByHandleRef[" .. tostring(parentHandle) .. ":" .. tostring(childPath) .. "]")
				end
			end
		end
	end
	for listHandle, itemHandles in pairs(self.listItemHandlesByHandle) do
		if handleSet[listHandle] == true then
			table.insert(issues, "listItemHandlesParent[" .. tostring(listHandle) .. "]")
		elseif type(itemHandles) == "table" then
			for itemIndex, itemHandle in pairs(itemHandles) do
				if itemHandle ~= nil and handleSet[itemHandle] == true then
					table.insert(issues, "listItemHandlesRef[" .. tostring(listHandle) .. ":" .. tostring(itemIndex) .. "]")
				end
			end
		end
	end
	for layerName, layerObjects in pairs(self.layerObjects) do
		for ownedHandle, _ in pairs(handleSet) do
			if layerObjects[ownedHandle] ~= nil then
				table.insert(issues, "layerObjects[" .. tostring(layerName) .. ":" .. tostring(ownedHandle) .. "]")
			end
		end
	end
	for index, entry in ipairs(self.uiStack) do
		if entry ~= nil and entry.key == key then
			table.insert(issues, "uiStack[" .. tostring(index) .. "]")
		end
	end
	for index, entry in ipairs(self.popupStack) do
		if entry ~= nil and entry.key == key then
			table.insert(issues, "popupStack[" .. tostring(index) .. "]")
		end
	end

	local focusedHandle = self:GetFocusedHandle()
	if focusedHandle ~= nil and handleSet[focusedHandle] == true then
		table.insert(issues, "focusedHandle[" .. tostring(focusedHandle) .. "]")
	end
	return issues
end

function FairyGuiManager:ValidateClosedObject(objectInfo, ownedHandles, label, printWhenClean)
	local issues = self:GetCloseResidue(objectInfo, ownedHandles)
	if #issues <= 0 then
		if printWhenClean == true then
			print("[FGUI] close residue ok:", label or "", "key=", objectInfo and objectInfo.key)
		end
		return true, issues
	end

	local message = table.concat(issues, "; ")
	print("[FGUI] close residue warning:", label or "", "key=", objectInfo and objectInfo.key, message)
	if self:IsStrictLifecycleEnabled() then
		error("[FGUI] close residue: " .. message)
	end
	return false, issues
end

function FairyGuiManager:ClearFocusForHandles(ownedHandles)
	local focusedHandle = self:GetFocusedHandle()
	if focusedHandle == nil or focusedHandle <= 0 then
		return false
	end
	if not containsHandle(ownedHandles, focusedHandle) then
		return false
	end
	return self:ClearFocus()
end

function FairyGuiManager:AddTimer(keyOrHandle, duration, interval, tickFunc, finishFunc)
	if threadpool == nil or threadpool.timer == nil then
		return nil
	end

	local objectInfo = self:GetObjectInfo(keyOrHandle)
	if objectInfo == nil then
		return nil
	end

	local key = objectInfo.key
	local seq = nil
	local function removeTimerRecord()
		if seq ~= nil then
			self:RemoveTimerRecord(seq)
		end
	end
	local function isOwnerAlive()
		return key ~= nil and self.objects[key] ~= nil
	end
	local function callTimerCallback(callback, ...)
		if type(callback) ~= "function" then
			return
		end
		local ok, err = pcall(callback, ...)
		if not ok then
			print("[FGUI] timer callback error:", key, err)
		end
	end

	local wrappedTick = nil
	if type(tickFunc) == "function" then
		wrappedTick = function(timerInfo, tick)
			if not isOwnerAlive() then
				removeTimerRecord()
				return
			end
			callTimerCallback(tickFunc, timerInfo, tick)
		end
	end
	local wrappedFinish = function(timerInfo)
		if isOwnerAlive() then
			callTimerCallback(finishFunc, timerInfo)
		end
		removeTimerRecord()
	end

	seq = threadpool:timer(duration, interval, wrappedTick, wrappedFinish)
	if seq == nil then
		return nil
	end

	self.timers[seq] = {
		seq = seq,
		key = key,
		handle = objectInfo.handle,
	}
	local timerList = self.timersByKey[key]
	if timerList == nil then
		timerList = {}
		self.timersByKey[key] = timerList
	end
	timerList[seq] = true
	return seq
end

function FairyGuiManager:Delay(keyOrHandle, timeout, func)
	return self:AddTimer(keyOrHandle, timeout, timeout, nil, func)
end

function FairyGuiManager:RemoveTimerRecord(timerId)
	local timerInfo = self.timers[timerId]
	if timerInfo == nil then
		return false
	end

	self.timers[timerId] = nil
	local timerList = self.timersByKey[timerInfo.key]
	if timerList ~= nil then
		timerList[timerId] = nil
		if tableCount(timerList) <= 0 then
			self.timersByKey[timerInfo.key] = nil
		end
	end
	return true
end

function FairyGuiManager:CancelTimer(timerId)
	if timerId == nil then
		return false
	end

	local removed = self:RemoveTimerRecord(timerId)
	if threadpool ~= nil and threadpool.cancel_timer ~= nil then
		return threadpool:cancel_timer(timerId) or removed
	end
	return removed
end

function FairyGuiManager:ClearTimersForKey(key)
	if key == nil then
		return 0
	end

	local timerList = self.timersByKey[key]
	if timerList == nil then
		return 0
	end

	local timerIds = {}
	for timerId, _ in pairs(timerList) do
		table.insert(timerIds, timerId)
	end
	local removedCount = 0
	for _, timerId in ipairs(timerIds) do
		if self:CancelTimer(timerId) then
			removedCount = removedCount + 1
		end
	end
	return removedCount
end

function FairyGuiManager:GetTimerCountForKey(key)
	local timerList = self.timersByKey[key]
	return tableCount(timerList)
end

function FairyGuiManager:RegisterUI(name, config)
	if isBlank(name) or type(config) ~= "table" then
		return false
	end

	self.uiRegistry[name] = config
	config.name = config.name or name
	return true
end

function FairyGuiManager:RegisterUIRegistry(registry)
	if type(registry) ~= "table" then
		return
	end

	for name, config in pairs(registry) do
		self:RegisterUI(name, config)
	end
end

function FairyGuiManager:GetUIConfig(name)
	if isBlank(name) then
		return nil
	end
	return self.uiRegistry[name]
end

function FairyGuiManager:LoadUIRequires(config)
	if type(config) ~= "table" or type(config.requires) ~= "table" then
		return true
	end

	for _, moduleName in ipairs(config.requires) do
		local ok, err = pcall(require, moduleName)
		if not ok then
			print("[FGUI] require ui module failed:", moduleName, err)
			return false
		end
	end
	return true
end

function FairyGuiManager:BuildOpenParam(uiName, config, param)
	local openParam = copyTable(config.defaultParam)
	local copiedFields = {
		"key",
		"x",
		"y",
		"width",
		"height",
		"center",
		"restraint",
		"visible",
		"packageName",
		"objectName",
		"component",
		"layer",
		"cache",
		"modal",
		"modalAlpha",
		"unloadPackageOnClose",
			"clearPackage",
			"destroyOnClose",
			"stack",
			"stackMode",
			"closeOnMaskClick",
			"closeOnEscape",
			"pauseTimersOnHide",
			"popupMode",
			"popupGroup",
			"priority",
			"sortingPriority",
			"sortingOrder",
			"group",
			"uiGroup",
			"scene",
			"sceneName",
			"closeOnSceneChange",
			"destroyOnSceneChange",
			"fullScreen",
			"adaptScreen",
			"alignX",
			"alignY",
			"hAlign",
			"vAlign",
			"offsetX",
			"offsetY",
			"fitInScreen",
			"designWidth",
			"designHeight",
			"scaleMode",
			"useDesignScale",
			"marginLeft",
		"marginRight",
		"marginTop",
		"marginBottom",
	}

	for _, fieldName in ipairs(copiedFields) do
		if config[fieldName] ~= nil then
			openParam[fieldName] = config[fieldName]
		end
	end

	copyTable(param, openParam)
	openParam.key = openParam.key or uiName
	return openParam
end

function FairyGuiManager:GetRegisteredUIKey(name)
	if self.uiNameToKey[name] ~= nil then
		return self.uiNameToKey[name]
	end

	local config = self:GetUIConfig(name)
	if config == nil then
		return name
	end
	return config.key or name
end

function FairyGuiManager:GetObjectInfo(keyOrHandle)
	if type(keyOrHandle) == "number" then
		return self.objectsByHandle[keyOrHandle]
	end

	local key = self:GetRegisteredUIKey(keyOrHandle)
	return self.objects[key] or self.objects[keyOrHandle]
end

function FairyGuiManager:GetLayerBaseOrder(layerName)
	return self:GetLayers():GetLayerBaseOrder(layerName)
end

function FairyGuiManager:GetLayerRoot(layerName)
	return self:GetLayers():GetLayerRoot(layerName)
end

function FairyGuiManager:EnsureLayerRoot(layerName)
	return self:GetLayers():EnsureLayerRoot(layerName)
end

function FairyGuiManager:AttachToLayer(handle, layerName)
	return self:GetLayers():AttachToLayer(handle, layerName)
end

function FairyGuiManager:ResizeLayerRoots()
	return self:GetLayers():ResizeLayerRoots()
end

function FairyGuiManager:NextLayerSortingOrder(layerName, priority)
	return self:GetLayers():NextLayerSortingOrder(layerName, priority)
end

function FairyGuiManager:IsPopupLayer(layerName)
	return self:GetLayers():IsPopupLayer(layerName)
end

function FairyGuiManager:GetStackMode(objectInfo)
	return self:GetLayers():GetStackMode(objectInfo)
end

function FairyGuiManager:RemoveStackEntry(key)
	return self:GetLayers():RemoveStackEntry(key)
end

function FairyGuiManager:PushStack(objectInfo)
	return self:GetLayers():PushStack(objectInfo)
end

function FairyGuiManager:GetTopStackObject(stack, layerName)
	return self:GetLayers():GetTopStackObject(stack, layerName)
end

function FairyGuiManager:GetObjectResult(objectInfo)
	return self:GetLayers():GetObjectResult(objectInfo)
end

function FairyGuiManager:GetTopUI(layerName)
	return self:GetLayers():GetTopUI(layerName)
end

function FairyGuiManager:GetTopPopup()
	return self:GetLayers():GetTopPopup()
end

function FairyGuiManager:CloseTop(layerName, forceDestroy)
	return self:GetLayers():CloseTop(layerName, forceDestroy)
end

function FairyGuiManager:CloseTopPopup(forceDestroy)
	return self:GetLayers():CloseTopPopup(forceDestroy)
end

function FairyGuiManager:IsPopupOpenParam(param)
	return self:GetLayers():IsPopupOpenParam(param)
end

function FairyGuiManager:GetPopupGroup(param, objectInfo)
	return self:GetLayers():GetPopupGroup(param, objectInfo)
end

function FairyGuiManager:GetUIGroup(param, objectInfo)
	return self:GetLayers():GetUIGroup(param, objectInfo)
end

function FairyGuiManager:GetSceneName(param, objectInfo)
	return self:GetLayers():GetSceneName(param, objectInfo)
end

function FairyGuiManager:GetTopPopupObjectByGroup(popupGroup)
	return self:GetLayers():GetTopPopupObjectByGroup(popupGroup)
end

function FairyGuiManager:ReopenObjectInfo(objectInfo, param)
	return self:GetLayers():ReopenObjectInfo(objectInfo, param)
end

function FairyGuiManager:ClosePopupGroup(popupGroup, exceptKey, forceDestroy)
	return self:GetLayers():ClosePopupGroup(popupGroup, exceptKey, forceDestroy)
end

function FairyGuiManager:ApplyPopupOpenPolicy(param)
	return self:GetLayers():ApplyPopupOpenPolicy(param)
end

function FairyGuiManager:CanClosePopupByEscape(objectInfo)
	return self:GetLayers():CanClosePopupByEscape(objectInfo)
end

function FairyGuiManager:HandleKeyPressed(keyCode, keyText)
	return self:GetLayers():HandleKeyPressed(keyCode, keyText)
end

function FairyGuiManager:HandleKeyReleased(keyCode, keyText)
	return self:GetLayers():HandleKeyReleased(keyCode, keyText)
end

function FairyGuiManager:UpdateModalMaskSorting(objectInfo)
	return self:GetLayers():UpdateModalMaskSorting(objectInfo)
end

function FairyGuiManager:AssignLayer(objectInfo, layerName, forceNextOrder)
	return self:GetLayers():AssignLayer(objectInfo, layerName, forceNextOrder)
end

function FairyGuiManager:BringToFront(keyOrHandle, priority)
	return self:GetLayers():BringToFront(keyOrHandle, priority)
end

function FairyGuiManager:SetSortingPriority(keyOrHandle, priority, bringToFront)
	return self:GetLayers():SetSortingPriority(keyOrHandle, priority, bringToFront)
end

function FairyGuiManager:HandleModalMaskClick(key, reason)
	return self:GetLayers():HandleModalMaskClick(key, reason)
end

function FairyGuiManager:GetGuideMaskRect(param)
	return self:GetLayers():GetGuideMaskRect(param)
end

function FairyGuiManager:SetGuideMaskVisible(objectInfo, visible)
	return self:GetLayers():SetGuideMaskVisible(objectInfo, visible)
end

function FairyGuiManager:ClearGuideMaskHandles(objectInfo)
	return self:GetLayers():ClearGuideMaskHandles(objectInfo)
end

function FairyGuiManager:UpdateGuideMaskSorting(objectInfo)
	return self:GetLayers():UpdateGuideMaskSorting(objectInfo)
end

function FairyGuiManager:AddGuideMaskSegment(objectInfo, x, y, width, height, alpha, closeOnClick)
	return self:GetLayers():AddGuideMaskSegment(objectInfo, x, y, width, height, alpha, closeOnClick)
end

function FairyGuiManager:CreateGuideMaskSegments(objectInfo, param)
	return self:GetLayers():CreateGuideMaskSegments(objectInfo, param)
end

function FairyGuiManager:UpdateGuideMaskLayout(objectInfo)
	return self:GetLayers():UpdateGuideMaskLayout(objectInfo)
end

function FairyGuiManager:CreateModalMask(objectInfo, param)
	return self:GetLayers():CreateModalMask(objectInfo, param)
end

function FairyGuiManager:GetScreenWidth()
	return self:GetLayers():GetScreenWidth()
end

function FairyGuiManager:GetScreenHeight()
	return self:GetLayers():GetScreenHeight()
end

function FairyGuiManager:SetDesignResolution(width, height, scaleMode)
	return self:GetLayers():SetDesignResolution(width, height, scaleMode)
end

function FairyGuiManager:GetDesignTransform(param)
	return self:GetLayers():GetDesignTransform(param)
end

function FairyGuiManager:ApplyDesignRect(rect, param)
	return self:GetLayers():ApplyDesignRect(rect, param)
end

function FairyGuiManager:ClampLayoutRect(rect, screenWidth, screenHeight)
	return self:GetLayers():ClampLayoutRect(rect, screenWidth, screenHeight)
end

function FairyGuiManager:GetLayoutRect(param)
	return self:GetLayers():GetLayoutRect(param)
end

function FairyGuiManager:ApplyScreenAdapt(objectInfo)
	return self:GetLayers():ApplyScreenAdapt(objectInfo)
end

function FairyGuiManager:HandleWindowResized(width, height)
	return self:GetLayers():HandleWindowResized(width, height)
end

function FairyGuiManager:Open(name, param)
	local startMs = nowMs()
	local function finish(result, success)
		self:RecordPerf("open", nowMs() - startMs, tostring(name), success ~= false and result ~= nil)
		return result
	end

	local config = self:GetUIConfig(name)
	if config == nil then
		print("[FGUI] open failed, ui not registered:", tostring(name))
		return finish(nil, false)
	end

	if not self:LoadUIRequires(config) then
		return finish(nil, false)
	end

	local openParam = self:BuildOpenParam(name, config, param)
	openParam.uiName = name
	self.uiNameToKey[name] = openParam.key
	local policyResult = self:ApplyPopupOpenPolicy(openParam)
	if policyResult ~= nil then
		return finish(policyResult, true)
	end

	local packagePath = config.packagePath or config.package
	local component = openParam.objectName or openParam.component or config.objectName or config.component or name

	if config.viewClass ~= nil then
		openParam.packagePath = packagePath
		openParam.package = packagePath
		openParam.objectName = component
		openParam.component = component
		return finish(self:OpenView(config.viewClass, openParam))
	end

	local classlua = config.classlua or config.autoGen
	if classlua ~= nil then
		openParam.objectName = component
		openParam.component = component
		return finish(self:OpenUI(component, packagePath, classlua, openParam))
	end

	return finish(self:OpenObject(component, packagePath, component, openParam))
end

function FairyGuiManager:OpenObject(name, packagePath, objectName, param)
	param = param or {}
	objectName = objectName or name

	local resolvedPackagePath = self:ResolvePackagePath(packagePath)
	local packageName = self:LoadPackage(resolvedPackagePath, param.packageName)
	if isBlank(packageName) then
		return nil
	end

	local handle = self:CreateObject(packageName, objectName)
	if handle == nil or handle <= 0 then
		return nil
	end

	if not self:AttachToLayer(handle, param.layer or "Normal") then
		GameManager:removeFairyGuiObject(handle)
		return nil
	end

	if param.width ~= nil and param.height ~= nil then
		GameManager:setFairyGuiObjectSize(handle, param.width, param.height)
	end

	if param.x ~= nil and param.y ~= nil then
		GameManager:setFairyGuiObjectPosition(handle, param.x, param.y)
	elseif param.center then
		GameManager:centerFairyGuiObject(handle, param.restraint == true)
	end

	if param.visible ~= nil then
		GameManager:setFairyGuiObjectVisible(handle, param.visible == true)
	end

	local key = param.key or name or tostring(handle)
	self:RetainPackage(resolvedPackagePath, packageName)
	local objectInfo = {
		handle = handle,
		key = key,
		name = name,
		packagePath = resolvedPackagePath,
		packageName = packageName,
		objectName = objectName,
		param = param,
		uiName = param.uiName,
		cache = param.cache == true,
		unloadPackageOnClose = param.unloadPackageOnClose == true or param.clearPackage == true,
		layer = param.layer or "Normal",
		priority = tonumber(param.priority or param.sortingPriority) or 0,
		popupGroup = self:GetPopupGroup(param),
		popupMode = param.popupMode or "stack",
		uiGroup = self:GetUIGroup(param),
		sceneName = self:GetSceneName(param),
		closeOnSceneChange = param.closeOnSceneChange ~= false,
		destroyOnSceneChange = param.destroyOnSceneChange == true,
	}
	self.objects[key] = objectInfo
	self.objectsByHandle[handle] = objectInfo
	self.hiddenObjects[key] = nil
	self:AssignLayer(objectInfo, objectInfo.layer)
	self:CreateModalMask(objectInfo, param)
	self:ApplyScreenAdapt(objectInfo)
	self:PushStack(objectInfo)
	return handle
end

function FairyGuiManager:OpenMaskProbe(param)
	param = param or {}
	local key = param.key or "MaskProbe"
	self:CloseUI(key, true)

	local assets = param.assets or {}
	local backgroundPath = param.backgroundImage or assets.background
	local contentPath = param.contentImage or assets.content
	local stripAPath = param.stripAImage or assets.stripA
	local stripBPath = param.stripBImage or assets.stripB
	local stripCPath = param.stripCImage or assets.stripC
	local maskPath = param.maskImage or assets.mask
	if isBlank(backgroundPath) or isBlank(contentPath) or isBlank(stripAPath) or isBlank(stripBPath) or isBlank(stripCPath) or isBlank(maskPath) then
		print("[FGUI] open mask probe failed, missing image asset")
		return nil
	end

	local function addImage(parentHandle, name, path, x, y, width, height, alpha)
		local childHandle = self:CreateLoader(parentHandle, name, path)
		if childHandle == nil or childHandle <= 0 then
			return nil
		end
		self:SetPosition(childHandle, x, y)
		self:SetSize(childHandle, width, height)
		self:SetTouchable(childHandle, false)
		if alpha ~= nil then
			self:SetAlpha(childHandle, alpha)
		end
		if GameManager.addFairyGuiObjectToParent == nil or not GameManager:addFairyGuiObjectToParent(childHandle, parentHandle) then
			GameManager:removeFairyGuiObject(childHandle)
			return nil
		end
		return childHandle
	end

	local function addText(parentHandle, name, text, x, y, width, height, red, green, blue)
		local childHandle = self:CreateText(parentHandle, name, text, 18, red, green, blue)
		if childHandle == nil or childHandle <= 0 then
			return nil
		end
		self:SetPosition(childHandle, x, y)
		self:SetSize(childHandle, width, height)
		if GameManager.addFairyGuiObjectToParent == nil or not GameManager:addFairyGuiObjectToParent(childHandle, parentHandle) then
			GameManager:removeFairyGuiObject(childHandle)
			return nil
		end
		return childHandle
	end

	local function createPanel(parentHandle, name, x, y, inverted)
		local panelHandle = self:CreateContainer(name, parentHandle)
		if panelHandle == nil or panelHandle <= 0 then
			return nil
		end
		self:SetPosition(panelHandle, x, y)
		self:SetSize(panelHandle, 320, 210)
		self:SetTouchable(panelHandle, false)
		if GameManager.addFairyGuiObjectToParent == nil or not GameManager:addFairyGuiObjectToParent(panelHandle, parentHandle) then
			GameManager:removeFairyGuiObject(panelHandle)
			return nil
		end

		local background = addImage(panelHandle, "content_bg", contentPath, -70, 28, 460, 150)
		local stripA = addImage(panelHandle, "content_strip_a", stripAPath, -20, 50, 180, 72)
		local stripB = addImage(panelHandle, "content_strip_b", stripBPath, 130, 82, 160, 52)
		local stripC = addImage(panelHandle, "content_strip_c", stripCPath, 245, 20, 64, 64)
		local mask = addImage(panelHandle, "stencil_mask", maskPath, 72, 46, 176, 118)
		if background == nil or stripA == nil or stripB == nil or stripC == nil or mask == nil or not self:SetMask(panelHandle, mask, inverted) then
			GameManager:removeFairyGuiObject(panelHandle)
			return nil
		end
		return panelHandle
	end

	local handle = self:CreateContainer("FairyGuiMaskProbe")
	if handle == nil or handle <= 0 then
		return nil
	end
	self:SetSize(handle, 760, 330)
	self:SetTouchable(handle, true)

	local background = addImage(handle, "probe_bg", backgroundPath, 0, 24, 760, 286, 0.7)
	local title = addText(handle, "probe_title", "FairyGUI Mask Probe", 12, 0, 320, 24, 255, 244, 210)
	local normalLabel = addText(handle, "normal_label", "normal mask: only inside area is visible", 28, 42, 320, 24, 210, 255, 220)
	local invertedLabel = addText(handle, "inverted_label", "inverted mask: center area is cut out", 404, 42, 320, 24, 255, 220, 210)
	local normalPanel = createPanel(handle, "normal_panel", 28, 70, false)
	local invertedPanel = createPanel(handle, "inverted_panel", 404, 70, true)
	local normalOverlay = addImage(handle, "normal_mask_overlay", maskPath, 100, 116, 176, 118, 0.35)
	local invertedOverlay = addImage(handle, "inverted_mask_overlay", maskPath, 476, 116, 176, 118, 0.35)
	if background == nil or title == nil or normalLabel == nil or invertedLabel == nil or normalPanel == nil or invertedPanel == nil or normalOverlay == nil or invertedOverlay == nil then
		GameManager:removeFairyGuiObject(handle)
		return nil
	end

	local layerName = param.layer or "Top"
	if not self:AttachToLayer(handle, layerName) then
		GameManager:removeFairyGuiObject(handle)
		return nil
	end

	if param.x ~= nil and param.y ~= nil then
		self:SetPosition(handle, param.x, param.y)
	elseif param.center ~= false then
		GameManager:centerFairyGuiObject(handle, param.restraint == true)
	end

	local objectInfo = {
		handle = handle,
		key = key,
		name = "MaskProbe",
		objectName = "FairyGuiMaskProbe",
		param = param,
		uiName = "MaskProbe",
		cache = false,
		layer = layerName,
		popupGroup = self:GetPopupGroup(param),
		popupMode = param.popupMode or "stack",
		uiGroup = self:GetUIGroup(param),
		sceneName = self:GetSceneName(param),
		closeOnSceneChange = param.closeOnSceneChange ~= false,
		destroyOnSceneChange = param.destroyOnSceneChange == true,
	}
	self.objects[key] = objectInfo
	self.objectsByHandle[handle] = objectInfo
	self.uiNameToKey.MaskProbe = key
	self.hiddenObjects[key] = nil
	self:AssignLayer(objectInfo, objectInfo.layer)
	self:ApplyScreenAdapt(objectInfo)
	self:PushStack(objectInfo)
	return handle
end

function FairyGuiManager:OpenTextInputProbe(param)
	param = param or {}
	local key = param.key or "TextInputProbe"
	self:CloseUI(key, true)

	local handle = self:CreateContainer("FairyGuiTextInputProbe")
	if handle == nil or handle <= 0 then
		return nil
	end
	self:SetSize(handle, 480, 160)
	self:SetTouchable(handle, true)

	local titleHandle = self:CreateText(handle, "probe_title", param.title or "FairyGUI TextInput Probe", 18, 255, 244, 210)
	local inputHandle = self:CreateTextInput(handle, "probe_input", param.text or "", 20, 230, 255, 235)
	local hintHandle = self:CreateText(handle, "probe_hint", "type, backspace, enter", 16, 190, 210, 230)
	if titleHandle == nil or titleHandle <= 0 or inputHandle == nil or inputHandle <= 0 or hintHandle == nil or hintHandle <= 0 then
		GameManager:removeFairyGuiObject(handle)
		return nil
	end

	self:SetPosition(titleHandle, 0, 0)
	self:SetSize(titleHandle, 420, 28)
	self:SetPosition(inputHandle, 0, 44)
	self:SetSize(inputHandle, 420, 42)
	self:SetPosition(hintHandle, 0, 104)
	self:SetSize(hintHandle, 420, 28)
	if GameManager.addFairyGuiObjectToParent == nil
		or not GameManager:addFairyGuiObjectToParent(titleHandle, handle)
		or not GameManager:addFairyGuiObjectToParent(inputHandle, handle)
		or not GameManager:addFairyGuiObjectToParent(hintHandle, handle) then
		GameManager:removeFairyGuiObject(handle)
		return nil
	end

	local layerName = param.layer or "Top"
	if not self:AttachToLayer(handle, layerName) then
		GameManager:removeFairyGuiObject(handle)
		return nil
	end

	if param.x ~= nil and param.y ~= nil then
		self:SetPosition(handle, param.x, param.y)
	elseif param.center ~= false then
		GameManager:centerFairyGuiObject(handle, param.restraint == true)
	end

	local objectInfo = {
		handle = handle,
		key = key,
		name = "TextInputProbe",
		objectName = "FairyGuiTextInputProbe",
		param = param,
		uiName = "TextInputProbe",
		cache = false,
		layer = layerName,
		popupGroup = self:GetPopupGroup(param),
		popupMode = param.popupMode or "stack",
		uiGroup = self:GetUIGroup(param),
		sceneName = self:GetSceneName(param),
		closeOnSceneChange = param.closeOnSceneChange ~= false,
		destroyOnSceneChange = param.destroyOnSceneChange == true,
		inputHandle = inputHandle,
	}
	self.objects[key] = objectInfo
	self.objectsByHandle[handle] = objectInfo
	self.uiNameToKey.TextInputProbe = key
	self.hiddenObjects[key] = nil
	self:AssignLayer(objectInfo, objectInfo.layer)
	self:ApplyScreenAdapt(objectInfo)
	self:PushStack(objectInfo)
	self:AddChanged(handle, "probe_input", function(evt)
		print("[FGUI] text input changed:", self:GetText(handle, "probe_input"), evt.senderHandle)
	end)
	self:AddSubmit(handle, "probe_input", function(evt)
		print("[FGUI] text input submit:", self:GetText(handle, "probe_input"), evt.senderHandle)
	end)
	return handle, inputHandle
end

function FairyGuiManager:AddOwnedHandle(objectInfo, handle)
	if objectInfo == nil or handle == nil or handle <= 0 then
		return handle
	end
	if objectInfo.ownedHandles == nil then
		objectInfo.ownedHandles = {}
	end
	table.insert(objectInfo.ownedHandles, handle)
	return handle
end

function FairyGuiManager:AddObjectHandleToParent(childHandle, parentHandle)
	if childHandle == nil or childHandle <= 0 or parentHandle == nil or parentHandle <= 0 then
		return false
	end
	if GameManager == nil or GameManager.addFairyGuiObjectToParent == nil then
		return false
	end
	return GameManager:addFairyGuiObjectToParent(childHandle, parentHandle)
end

function FairyGuiManager:OpenServiceContainer(key, param)
	return self:GetServices():OpenServiceContainer(key, param)
end

function FairyGuiManager:AddServiceText(objectInfo, name, text, x, y, width, height, fontSize, red, green, blue)
	return self:GetServices():AddServiceText(objectInfo, name, text, x, y, width, height, fontSize, red, green, blue)
end

function FairyGuiManager:AddServiceImage(objectInfo, name, url, x, y, width, height, alpha)
	return self:GetServices():AddServiceImage(objectInfo, name, url, x, y, width, height, alpha)
end

function FairyGuiManager:AddServiceButton(objectInfo, name, text, x, y, width, height, callback)
	return self:GetServices():AddServiceButton(objectInfo, name, text, x, y, width, height, callback)
end

function FairyGuiManager:ApplyServiceLayout(objectInfo)
	return self:GetServices():ApplyServiceLayout(objectInfo)
end

function FairyGuiManager:GetServiceObject(serviceKey)
	return self:GetServices():GetServiceObject(serviceKey)
end

function FairyGuiManager:CloseService(serviceKey, reason)
	return self:GetServices():CloseService(serviceKey, reason)
end

function FairyGuiManager:CloseServices(serviceType, reason)
	return self:GetServices():CloseServices(serviceType, reason)
end

function FairyGuiManager:HandleServiceClosed(objectInfo, reason)
	return self:GetServices():HandleServiceClosed(objectInfo, reason)
end

function FairyGuiManager:ClearToastQueue()
	return self:GetServices():ClearToastQueue()
end

function FairyGuiManager:GetToastQueueCount()
	return self:GetServices():GetToastQueueCount()
end

function FairyGuiManager:CreateToastRequest(text, duration, param)
	return self:GetServices():CreateToastRequest(text, duration, param)
end

function FairyGuiManager:OpenToastRequest(request)
	return self:GetServices():OpenToastRequest(request)
end

function FairyGuiManager:ShowNextToast()
	return self:GetServices():ShowNextToast()
end

function FairyGuiManager:ShowToast(text, duration, param)
	return self:GetServices():ShowToast(text, duration, param)
end

function FairyGuiManager:CloseToast(reason)
	return self:GetServices():CloseToast(reason)
end

function FairyGuiManager:ShowTip(text, x, y, duration, param)
	return self:GetServices():ShowTip(text, x, y, duration, param)
end

function FairyGuiManager:ShowLoading(text, param)
	return self:GetServices():ShowLoading(text, param)
end

function FairyGuiManager:GetLoadingRefCount()
	return self:GetServices():GetLoadingRefCount()
end

function FairyGuiManager:HideLoading(paramOrReason)
	return self:GetServices():HideLoading(paramOrReason)
end

function FairyGuiManager:ShowGuideMask(param)
	return self:GetServices():ShowGuideMask(param)
end

function FairyGuiManager:HideGuideMask(reason)
	return self:GetServices():HideGuideMask(reason)
end

function FairyGuiManager:ShowMessageBox(title, message, buttons, callback, param)
	return self:GetServices():ShowMessageBox(title, message, buttons, callback, param)
end

function FairyGuiManager:ShowDialog(title, message, buttons, callback, param)
	return self:GetServices():ShowDialog(title, message, buttons, callback, param)
end

function FairyGuiManager:ShowPopupMenu(items, x, y, callback, param)
	return self:GetServices():ShowPopupMenu(items, x, y, callback, param)
end

function FairyGuiManager:GetServiceStats()
	return self:GetServices():GetServiceStats()
end

function FairyGuiManager:DumpServices()
	return self:GetServices():DumpServices()
end

function FairyGuiManager:BuildDebugPanelLines()
	return self:GetProfiler():BuildDebugPanelLines()
end

function FairyGuiManager:RefreshDebugPanel(key)
	return self:GetProfiler():RefreshDebugPanel(key)
end

function FairyGuiManager:ShowDebugPanel(param)
	return self:GetProfiler():ShowDebugPanel(param)
end

function FairyGuiManager:HideDebugPanel(key)
	return self:GetProfiler():HideDebugPanel(key)
end

function FairyGuiManager:OpenUI(name, packagePath, classluaOrObjectName, param)
	if isAutoGenClass(classluaOrObjectName) or (param and param.mvc == true) then
		return self:OpenMvcUI(name, packagePath, classluaOrObjectName, param)
	end
	return self:OpenObject(name, packagePath, classluaOrObjectName, param)
end

function FairyGuiManager:GetMVC(name)
	local className = getViewClassName(name)
	if isBlank(className) then
		return nil, nil, nil
	end

	local uiName = string.gsub(className, "AutoGen$", "")
	local ctrl = ClassList[uiName .. "Ctrl"]
	local model = ClassList[uiName .. "Model"]
	local view = ClassList[uiName .. "View"]
	return ctrl, model, view
end

function FairyGuiManager:CreateMvcInstance(classInfo, param)
	if classInfo == nil then
		return nil
	end
	if classInfo.Create ~= nil then
		return classInfo:Create(param)
	end
	if classInfo.new ~= nil then
		return classInfo.new(param)
	end
	return nil
end

function FairyGuiManager:InstMVC(name, param)
	param = param or {}

	local ctrlClass, modelClass, viewClass = self:GetMVC(name)
	if ctrlClass == nil or modelClass == nil or viewClass == nil then
		print("[FGUI] InstMVC failed, class missing:", tostring(name))
		return nil
	end

	local ctrlInst = self:CreateMvcInstance(ctrlClass, param.incomingParam)
	if ctrlInst == nil then
		return nil
	end

	local define = ctrlInst.define or {}
	ctrlInst.define = define

	local modelInst = self:CreateMvcInstance(modelClass, { define = define })
	if modelInst ~= nil and modelInst.SetIncomingParam ~= nil then
		modelInst:SetIncomingParam(param.incomingParam)
	end

	local viewInst = self:CreateMvcInstance(viewClass, {
		define = define,
		handle = param.handle,
		root = param.root,
		key = param.key,
		uiType = "FGUI",
	})
	if viewInst ~= nil and viewInst.GetHandleNodes ~= nil then
		viewInst:GetHandleNodes()
	end

	ctrlInst.model = modelInst
	ctrlInst.view = viewInst
	if ctrlInst.SetAutoGen ~= nil then
		ctrlInst:SetAutoGen(param.autoGen)
	else
		ctrlInst.autoGen = param.autoGen
	end
	return ctrlInst
end

function FairyGuiManager:UnInstMVC(ctrlInst)
	if ctrlInst == nil then
		return
	end
	ctrlInst.model = nil
	ctrlInst.view = nil
	ctrlInst.autoGen = nil
end

function FairyGuiManager:OpenMvcUI(name, packagePath, classlua, param)
	param = param or {}

	local autoGenClass = loadClass(classlua)
	if autoGenClass == nil then
		print("[FGUI] open mvc ui failed, class not found:", tostring(classlua))
		return nil
	end

	local className = autoGenClass.className or getViewClassName(classlua)
	local key = param.key or className or name
	local objectInfo = self.objects[key]
	if objectInfo ~= nil then
		objectInfo.param = param
		objectInfo.cache = param.cache == true or objectInfo.cache == true
		objectInfo.priority = tonumber(param.priority or param.sortingPriority) or objectInfo.priority or 0
		self.hiddenObjects[key] = nil
		self:SetVisible(objectInfo.handle, true)
		if objectInfo.modalMaskHandle ~= nil then
			self:SetVisible(objectInfo.modalMaskHandle, true)
		end
		self:BringToFront(objectInfo.handle)
		self:ApplyScreenAdapt(objectInfo)
		if objectInfo.view ~= nil then
			if objectInfo.view._Attach ~= nil then
				objectInfo.view:_Attach(objectInfo.handle, key, className, param)
			end
			callView(objectInfo.view, "OnReopen", param)
			callView(objectInfo.view, "OnOpen", param)
			callView(objectInfo.view, "OnShow", param)
		end
		return objectInfo.ctrl or objectInfo.view or objectInfo.handle
	end

	local openParam = {}
	for paramName, value in pairs(param) do
		openParam[paramName] = value
	end
	openParam.key = key

	local objectName = param.objectName or param.component or name
	local handle = self:OpenObject(name, packagePath, objectName, openParam)
	if handle == nil then
		return nil
	end

	local autoGen = autoGenClass.new(param)
	if autoGen == nil then
		self:CloseUI(handle)
		return nil
	end

	if autoGen._Attach ~= nil then
		autoGen:_Attach(handle, key, className, param)
	else
		autoGen.handle = handle
		autoGen.uiKey = key
		autoGen.viewName = className
		autoGen.param = param
	end

	local openedInfo = self.objectsByHandle[handle]
	if openedInfo ~= nil then
		openedInfo.view = autoGen
		openedInfo.autoGenClass = className
	end
	self.views[key] = autoGen
	self.viewsByHandle[handle] = autoGen

	callView(autoGen, "OnCreate", handle, param)
	if openedInfo ~= nil and autoGen.ctrl ~= nil then
		openedInfo.ctrl = autoGen.ctrl
	end
	if autoGen.ctrl ~= nil then
		self.controllers[key] = autoGen.ctrl
		self.controllersByHandle[handle] = autoGen.ctrl
	end
	callView(autoGen, "OnOpen", param)
	callView(autoGen, "OnShow", param)
	return autoGen.ctrl or autoGen
end

function FairyGuiManager:OpenView(viewClass, param)
	param = param or {}

	local viewClassInfo = loadViewClass(viewClass)
	if viewClassInfo == nil then
		print("[FGUI] open view failed, class not found:", tostring(viewClass))
		return nil
	end

	local view = viewClassInfo.new(param)
	if view == nil then
		return nil
	end

	local viewName = param.viewName or param.name or viewClassInfo.className
	local packagePath = param.packagePath or param.package
	local objectName = param.objectName or param.component or viewName
	local key = param.key or viewName
	local openParam = {}
	for name, value in pairs(param) do
		openParam[name] = value
	end
	openParam.key = key

	local objectInfo = self.objects[key]
	if objectInfo ~= nil then
		objectInfo.param = param
		objectInfo.cache = param.cache == true or objectInfo.cache == true
		objectInfo.priority = tonumber(param.priority or param.sortingPriority) or objectInfo.priority or 0
		self.hiddenObjects[key] = nil
		self:SetVisible(objectInfo.handle, true)
		if objectInfo.modalMaskHandle ~= nil then
			self:SetVisible(objectInfo.modalMaskHandle, true)
		end
		self:BringToFront(objectInfo.handle)
		self:ApplyScreenAdapt(objectInfo)
		if objectInfo.view ~= nil then
			if objectInfo.view._Attach ~= nil then
				objectInfo.view:_Attach(objectInfo.handle, key, viewClassInfo.className, param)
			end
			callView(objectInfo.view, "OnReopen", param)
			callView(objectInfo.view, "OnOpen", param)
			callView(objectInfo.view, "OnShow", param)
		end
		return objectInfo.view or objectInfo.handle
	end

	local handle = self:OpenObject(viewName, packagePath, objectName, openParam)
	if handle == nil then
		return nil
	end

	if view._Attach ~= nil then
		view:_Attach(handle, key, viewClassInfo.className, param)
	else
		view.handle = handle
		view.uiKey = key
		view.viewName = viewClassInfo.className
		view.param = param
	end

	local objectInfo = self.objectsByHandle[handle]
	if objectInfo ~= nil then
		objectInfo.view = view
	end
	self.views[key] = view
	self.viewsByHandle[handle] = view

	callView(view, "OnCreate", handle, param)
	callView(view, "OnOpen", param)
	callView(view, "OnShow", param)
	return view
end

function FairyGuiManager:GetUI(key)
	local objectInfo = self.objects[key]
	return objectInfo and objectInfo.handle or nil
end

function FairyGuiManager:GetView(keyOrHandle)
	if type(keyOrHandle) == "number" then
		return self.viewsByHandle[keyOrHandle]
	end
	local key = self:GetRegisteredUIKey(keyOrHandle)
	if self.views[key] ~= nil then
		return self.views[key]
	end
	return self.views[keyOrHandle]
end

function FairyGuiManager:GetController(keyOrHandle)
	if type(keyOrHandle) == "number" then
		return self.controllersByHandle[keyOrHandle]
	end

	local key = self:GetRegisteredUIKey(keyOrHandle)
	if self.controllers[key] ~= nil then
		return self.controllers[key]
	end
	return self.controllers[keyOrHandle]
end

function FairyGuiManager:GetChild(handle, childPath)
	return self:GetLists():GetChild(handle, childPath)
end

function FairyGuiManager:GetTargetHandle(handle, childPath)
	return self:GetLists():GetTargetHandle(handle, childPath)
end

function FairyGuiManager:ClearListCacheByListHandle(listHandle)
	return self:GetLists():ClearListCacheByListHandle(listHandle)
end

function FairyGuiManager:ClearListItemHandleCache(listHandle)
	return self:GetLists():ClearListItemHandleCache(listHandle)
end

function FairyGuiManager:ClearListCacheForHandle(handle)
	return self:GetLists():ClearListCacheForHandle(handle)
end

function FairyGuiManager:CreateListItemAdapter(listHandle, itemHandle, index, data)
	return self:GetLists():CreateListItemAdapter(listHandle, itemHandle, index, data)
end

function FairyGuiManager:GetListItemByHandle(listHandle, index, data)
	return self:GetLists():GetListItemByHandle(listHandle, index, data)
end

function FairyGuiManager:RenderListItemByHandle(listHandle, index)
	return self:GetLists():RenderListItemByHandle(listHandle, index)
end

function FairyGuiManager:GetListHandle(handle, childPath)
	return self:GetLists():GetListHandle(handle, childPath)
end

function FairyGuiManager:SetListItemCount(handle, childPath, itemCount)
	return self:GetLists():SetListItemCount(handle, childPath, itemCount)
end

function FairyGuiManager:GetListItemCount(handle, childPath)
	return self:GetLists():GetListItemCount(handle, childPath)
end

function FairyGuiManager:GetListItem(handle, childPath, index)
	return self:GetLists():GetListItem(handle, childPath, index)
end

function FairyGuiManager:SetListData(handle, childPath, dataList, renderer)
	return self:GetLists():SetListData(handle, childPath, dataList, renderer)
end

function FairyGuiManager:GetListData(handle, childPath, index)
	return self:GetLists():GetListData(handle, childPath, index)
end

function FairyGuiManager:RefreshListItem(handle, childPath, index)
	return self:GetLists():RefreshListItem(handle, childPath, index)
end

function FairyGuiManager:RefreshList(handle, childPath)
	return self:GetLists():RefreshList(handle, childPath)
end

function FairyGuiManager:UpdateListItem(handle, childPath, index, data)
	return self:GetLists():UpdateListItem(handle, childPath, index, data)
end

function FairyGuiManager:AppendListItem(handle, childPath, data)
	return self:GetLists():AppendListItem(handle, childPath, data)
end

function FairyGuiManager:RemoveListItem(handle, childPath, index)
	return self:GetLists():RemoveListItem(handle, childPath, index)
end

function FairyGuiManager:ClearList(handle, childPath)
	return self:GetLists():ClearList(handle, childPath)
end

function FairyGuiManager:SetListSelectedIndex(handle, childPath, selectedIndex)
	return self:GetLists():SetListSelectedIndex(handle, childPath, selectedIndex)
end

function FairyGuiManager:GetListSelectedIndex(handle, childPath)
	return self:GetLists():GetListSelectedIndex(handle, childPath)
end

function FairyGuiManager:ScrollListToView(handle, childPath, index)
	return self:GetLists():ScrollListToView(handle, childPath, index)
end

function FairyGuiManager:DebugInjectMouseMove(x, y)
	return self:GetEvents():DebugInjectMouseMove(x, y)
end

function FairyGuiManager:DebugInjectMouseDown(x, y, button)
	return self:GetEvents():DebugInjectMouseDown(x, y, button)
end

function FairyGuiManager:DebugInjectMouseUp(x, y, button)
	return self:GetEvents():DebugInjectMouseUp(x, y, button)
end

function FairyGuiManager:DebugInjectMouseWheel(x, y, wheelDelta)
	return self:GetEvents():DebugInjectMouseWheel(x, y, wheelDelta)
end

function FairyGuiManager:DebugInjectClick(x, y, button)
	return self:GetEvents():DebugInjectClick(x, y, button)
end

function FairyGuiManager:DebugInjectKeyPressed(keyCode, keyText)
	return self:GetEvents():DebugInjectKeyPressed(keyCode, keyText)
end

function FairyGuiManager:DebugInjectKeyReleased(keyCode, keyText)
	return self:GetEvents():DebugInjectKeyReleased(keyCode, keyText)
end

function FairyGuiManager:SetText(handle, childPath, text)
	if GameManager == nil then
		return false
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return GameManager:setFairyGuiObjectText(targetHandle, tostring(text or ""))
end

function FairyGuiManager:GetText(handle, childPath)
	if GameManager == nil or GameManager.getFairyGuiObjectText == nil then
		return ""
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return ""
	end
	return GameManager:getFairyGuiObjectText(targetHandle) or ""
end

function FairyGuiManager:Focus(handle, childPath)
	if GameManager == nil or GameManager.focusFairyGuiObject == nil then
		return false
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return GameManager:focusFairyGuiObject(targetHandle)
end

function FairyGuiManager:ClearFocus()
	if GameManager == nil or GameManager.clearFairyGuiFocus == nil then
		return false
	end
	return GameManager:clearFairyGuiFocus()
end

function FairyGuiManager:GetFocusedHandle()
	if GameManager == nil or GameManager.getFairyGuiFocusedObject == nil then
		return 0
	end
	return GameManager:getFairyGuiFocusedObject()
end

function FairyGuiManager:SetIcon(handle, childPath, icon)
	if GameManager == nil then
		return false
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return GameManager:setFairyGuiObjectIcon(targetHandle, tostring(icon or ""))
end

function FairyGuiManager:SetLoaderUrl(handle, childPath, url)
	if GameManager == nil then
		return false
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return GameManager:setFairyGuiObjectLoaderUrl(targetHandle, tostring(url or ""))
end

function FairyGuiManager:SetChildVisible(handle, childPath, visible)
	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return self:SetVisible(targetHandle, visible == true)
end

function FairyGuiManager:SetChildPosition(handle, childPath, x, y)
	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return self:SetPosition(targetHandle, x, y)
end

function FairyGuiManager:SetChildSize(handle, childPath, width, height)
	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return self:SetSize(targetHandle, width, height)
end

function FairyGuiManager:SetControllerIndex(handle, controllerName, selectedIndex)
	if GameManager == nil or handle == nil then
		return false
	end
	return GameManager:setFairyGuiObjectControllerIndex(handle, controllerName or "", selectedIndex or 0)
end

function FairyGuiManager:SetValue(handle, childPathOrValue, value)
	if GameManager == nil or GameManager.setFairyGuiObjectValue == nil or handle == nil then
		return false
	end
	local childPath = childPathOrValue
	if value == nil then
		value = childPathOrValue
		childPath = nil
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and GameManager:setFairyGuiObjectValue(targetHandle, value or 0) or false
end

function FairyGuiManager:GetValue(handle, childPath)
	if GameManager == nil or GameManager.getFairyGuiObjectValue == nil or handle == nil then
		return 0
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and GameManager:getFairyGuiObjectValue(targetHandle) or 0
end

function FairyGuiManager:SetMin(handle, childPathOrValue, minValue)
	if GameManager == nil or GameManager.setFairyGuiObjectMin == nil or handle == nil then
		return false
	end
	local childPath = childPathOrValue
	if minValue == nil then
		minValue = childPathOrValue
		childPath = nil
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and GameManager:setFairyGuiObjectMin(targetHandle, minValue or 0) or false
end

function FairyGuiManager:GetMin(handle, childPath)
	if GameManager == nil or GameManager.getFairyGuiObjectMin == nil or handle == nil then
		return 0
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and GameManager:getFairyGuiObjectMin(targetHandle) or 0
end

function FairyGuiManager:SetMax(handle, childPathOrValue, maxValue)
	if GameManager == nil or GameManager.setFairyGuiObjectMax == nil or handle == nil then
		return false
	end
	local childPath = childPathOrValue
	if maxValue == nil then
		maxValue = childPathOrValue
		childPath = nil
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and GameManager:setFairyGuiObjectMax(targetHandle, maxValue or 0) or false
end

function FairyGuiManager:GetMax(handle, childPath)
	if GameManager == nil or GameManager.getFairyGuiObjectMax == nil or handle == nil then
		return 0
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and GameManager:getFairyGuiObjectMax(targetHandle) or 0
end

function FairyGuiManager:SetProgress(handle, childPathOrValue, value, maxValue, minValue)
	local childPath = childPathOrValue
	if type(childPathOrValue) ~= "string" then
		minValue = maxValue
		maxValue = value
		value = childPathOrValue
		childPath = nil
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end

	if minValue ~= nil and not self:SetMin(targetHandle, minValue) then
		return false
	end
	if maxValue ~= nil and not self:SetMax(targetHandle, maxValue) then
		return false
	end
	return self:SetValue(targetHandle, value or 0)
end

function FairyGuiManager:SetSliderValue(handle, childPathOrValue, value)
	return self:SetValue(handle, childPathOrValue, value)
end

function FairyGuiManager:GetSliderValue(handle, childPath)
	return self:GetValue(handle, childPath)
end

function FairyGuiManager:SetProgressBarValue(handle, childPathOrValue, value)
	return self:SetValue(handle, childPathOrValue, value)
end

function FairyGuiManager:GetProgressBarValue(handle, childPath)
	return self:GetValue(handle, childPath)
end

function FairyGuiManager:SetComboBoxSelectedIndex(handle, childPathOrSelectedIndex, selectedIndex)
	if GameManager == nil or GameManager.setFairyGuiComboBoxSelectedIndex == nil or handle == nil then
		return false
	end
	local childPath = childPathOrSelectedIndex
	if selectedIndex == nil then
		selectedIndex = childPathOrSelectedIndex
		childPath = nil
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and GameManager:setFairyGuiComboBoxSelectedIndex(targetHandle, selectedIndex or 0) or false
end

function FairyGuiManager:GetComboBoxSelectedIndex(handle, childPath)
	if GameManager == nil or GameManager.getFairyGuiComboBoxSelectedIndex == nil or handle == nil then
		return -1
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and GameManager:getFairyGuiComboBoxSelectedIndex(targetHandle) or -1
end

function FairyGuiManager:SetComboBoxValue(handle, childPathOrValue, value)
	if GameManager == nil or GameManager.setFairyGuiComboBoxValue == nil or handle == nil then
		return false
	end
	local childPath = childPathOrValue
	if value == nil then
		value = childPathOrValue
		childPath = nil
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and GameManager:setFairyGuiComboBoxValue(targetHandle, value or "") or false
end

function FairyGuiManager:GetComboBoxValue(handle, childPath)
	if GameManager == nil or GameManager.getFairyGuiComboBoxValue == nil or handle == nil then
		return ""
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and GameManager:getFairyGuiComboBoxValue(targetHandle) or ""
end

function FairyGuiManager:RegisterTransitionCallback(handle, transitionName, callback)
	return self:GetEvents():RegisterTransitionCallback(handle, transitionName, callback)
end

function FairyGuiManager:RemoveTransitionCallback(callbackId)
	return self:GetEvents():RemoveTransitionCallback(callbackId)
end

function FairyGuiManager:ClearTransitionCallbacksForHandle(handle, transitionName)
	return self:GetEvents():ClearTransitionCallbacksForHandle(handle, transitionName)
end

function FairyGuiManager:PlayTransition(handle, transitionName, times, delay, callback)
	return self:GetEvents():PlayTransition(handle, transitionName, times, delay, callback)
end

function FairyGuiManager:StopTransition(handle, transitionName, setToComplete, processCallback)
	return self:GetEvents():StopTransition(handle, transitionName, setToComplete, processCallback)
end

function FairyGuiManager:_DispatchTransition(callbackId, objectHandle, transitionName)
	return self:GetEvents():_DispatchTransition(callbackId, objectHandle, transitionName)
end

function FairyGuiManager:AddEvent(handle, childPath, eventType, callback)
	return self:GetEvents():AddEvent(handle, childPath, eventType, callback)
end

function FairyGuiManager:AddClick(handle, childPath, callback)
	return self:GetEvents():AddClick(handle, childPath, callback)
end

function FairyGuiManager:AddChanged(handle, childPath, callback)
	return self:GetEvents():AddChanged(handle, childPath, callback)
end

function FairyGuiManager:AddClickItem(handle, childPath, callback)
	return self:GetEvents():AddClickItem(handle, childPath, callback)
end

function FairyGuiManager:AddRightClick(handle, childPath, callback)
	return self:GetEvents():AddRightClick(handle, childPath, callback)
end

function FairyGuiManager:AddMouseWheel(handle, childPath, callback)
	return self:GetEvents():AddMouseWheel(handle, childPath, callback)
end

function FairyGuiManager:AddKeyDown(handle, childPath, callback)
	return self:GetEvents():AddKeyDown(handle, childPath, callback)
end

function FairyGuiManager:AddKeyUp(handle, childPath, callback)
	return self:GetEvents():AddKeyUp(handle, childPath, callback)
end

function FairyGuiManager:AddSubmit(handle, childPath, callback)
	return self:GetEvents():AddSubmit(handle, childPath, callback)
end

function FairyGuiManager:AddTouchBegin(handle, childPath, callback)
	return self:GetEvents():AddTouchBegin(handle, childPath, callback)
end

function FairyGuiManager:AddTouchEnd(handle, childPath, callback)
	return self:GetEvents():AddTouchEnd(handle, childPath, callback)
end

function FairyGuiManager:AddDragStart(handle, childPath, callback)
	return self:GetEvents():AddDragStart(handle, childPath, callback)
end

function FairyGuiManager:AddDragMove(handle, childPath, callback)
	return self:GetEvents():AddDragMove(handle, childPath, callback)
end

function FairyGuiManager:AddDragEnd(handle, childPath, callback)
	return self:GetEvents():AddDragEnd(handle, childPath, callback)
end

function FairyGuiManager:RemoveBinding(bindingId)
	return self:GetEvents():RemoveBinding(bindingId)
end

function FairyGuiManager:ClearBindingsForHandle(handle)
	return self:GetEvents():ClearBindingsForHandle(handle)
end

function FairyGuiManager:_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId, wheelDelta, dragDeltaX, dragDeltaY)
	return self:GetEvents():_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId, wheelDelta, dragDeltaX, dragDeltaY)
end

function FairyGuiManager:SetPosition(handle, x, y)
	if handle == nil then
		return false
	end
	return GameManager:setFairyGuiObjectPosition(handle, x, y)
end

function FairyGuiManager:SetSize(handle, width, height)
	if handle == nil then
		return false
	end
	return GameManager:setFairyGuiObjectSize(handle, width, height)
end

function FairyGuiManager:SetVisible(handle, visible)
	if handle == nil then
		return false
	end
	return GameManager:setFairyGuiObjectVisible(handle, visible == true)
end

function FairyGuiManager:SetAlpha(handle, alpha)
	if handle == nil or GameManager == nil or GameManager.setFairyGuiObjectAlpha == nil then
		return false
	end
	return GameManager:setFairyGuiObjectAlpha(handle, alpha or 1)
end

function FairyGuiManager:SetTouchable(handle, touchable)
	if handle == nil or GameManager == nil or GameManager.setFairyGuiObjectTouchable == nil then
		return false
	end
	return GameManager:setFairyGuiObjectTouchable(handle, touchable == true)
end

function FairyGuiManager:SetMask(handle, maskHandle, inverted)
	if handle == nil or maskHandle == nil or GameManager == nil or GameManager.setFairyGuiObjectMask == nil then
		return false
	end
	return GameManager:setFairyGuiObjectMask(handle, maskHandle, inverted == true)
end

function FairyGuiManager:CloseUI(keyOrHandle, forceDestroy, reason)
	forceDestroy, reason = normalizeCloseArgs(forceDestroy, reason)

	local objectInfo = nil
	if type(keyOrHandle) == "number" then
		objectInfo = self.objectsByHandle[keyOrHandle]
	else
		objectInfo = self.objects[keyOrHandle]
	end

	if not objectInfo then
		return false
	end

	local startMs = nowMs()
	local handle = objectInfo.handle
	local closeName = objectInfo.key or tostring(keyOrHandle)
	local function finish(result, success)
		self:RecordPerf("close", nowMs() - startMs, closeName, success ~= false and result == true)
		return result
	end

	local param = objectInfo.param or {}
	local ownedHandles = self:CollectOwnedHandles(objectInfo)
	local closeSnapshot = self:CreateCloseSnapshot(objectInfo, ownedHandles)
	objectInfo.lastCloseReason = reason
	callView(objectInfo.view, "OnClose", reason, closeSnapshot)

	if objectInfo.cache == true and forceDestroy ~= true and param.destroyOnClose ~= true then
		self:ClearFocusForHandles(ownedHandles)
		if param.pauseTimersOnHide ~= false then
			callView(objectInfo.view, "ClearTimers")
			self:ClearTimersForKey(objectInfo.key)
		end
		callView(objectInfo.view, "OnHide", reason)
		self:SetVisible(handle, false)
		if objectInfo.modalMaskHandle ~= nil then
			self:SetVisible(objectInfo.modalMaskHandle, false)
		end
		self:SetGuideMaskVisible(objectInfo, false)
		self:RemoveStackEntry(objectInfo.key)
		self.hiddenObjects[objectInfo.key] = objectInfo
		return finish(true, true)
	end

	self:ClearFocusForHandles(ownedHandles)
	detachView(self, objectInfo, reason)
	self:RemoveStackEntry(objectInfo.key)
	self:ClearTimersForKey(objectInfo.key)
	for _, ownedHandle in ipairs(ownedHandles) do
		self:ClearBindingsForHandle(ownedHandle)
		self:ClearTransitionCallbacksForHandle(ownedHandle)
	end
	self:ClearGuideMaskHandles(objectInfo)
	if objectInfo.modalMaskHandle ~= nil then
		GameManager:removeFairyGuiObject(objectInfo.modalMaskHandle)
	end
	local removed = GameManager:removeFairyGuiObject(handle)
	if objectInfo.uiName ~= nil and self.uiNameToKey[objectInfo.uiName] == objectInfo.key then
		self.uiNameToKey[objectInfo.uiName] = nil
	end
	self:ClearListCacheForHandle(handle)
	for _, ownedHandle in ipairs(ownedHandles) do
		self:ClearListCacheByListHandle(ownedHandle)
		self.childrenByHandle[ownedHandle] = nil
	end
	if objectInfo.layer ~= nil and self.layerObjects[objectInfo.layer] ~= nil then
		self.layerObjects[objectInfo.layer][handle] = nil
	end
	self.objects[objectInfo.key] = nil
	self.objectsByHandle[handle] = nil
	self.hiddenObjects[objectInfo.key] = nil
	self:ReleasePackage(objectInfo.packagePath, objectInfo.packageName, objectInfo.unloadPackageOnClose == true)
	self:ValidateClosedObject(closeSnapshot, closeSnapshot and closeSnapshot.ownedHandles or ownedHandles, "CloseUI")
	self:HandleServiceClosed(objectInfo, reason)
	return finish(removed, removed == true)
end

function FairyGuiManager:CloseView(viewOrKeyOrHandle, forceDestroy, reason)
	if type(viewOrKeyOrHandle) == "table" then
		return self:CloseUI(viewOrKeyOrHandle.handle, forceDestroy, reason)
	end
	return self:CloseUI(viewOrKeyOrHandle, forceDestroy, reason)
end

function FairyGuiManager:Close(nameOrKeyOrHandle, forceDestroy, reason)
	if type(nameOrKeyOrHandle) == "number" then
		return self:CloseUI(nameOrKeyOrHandle, forceDestroy, reason)
	end
	return self:CloseUI(self:GetRegisteredUIKey(nameOrKeyOrHandle), forceDestroy, reason)
end

function FairyGuiManager:Destroy(nameOrKeyOrHandle, reason)
	return self:Close(nameOrKeyOrHandle, true, reason or "destroy")
end

function FairyGuiManager:Get(nameOrKeyOrHandle)
	local objectInfo = self:GetObjectInfo(nameOrKeyOrHandle)
	if objectInfo == nil then
		return nil
	end
	return objectInfo.ctrl or objectInfo.view or objectInfo.handle
end

function FairyGuiManager:MatchCloseFilter(objectInfo, filter)
	if objectInfo == nil then
		return false
	end
	filter = filter or {}
	if filter.exceptKey ~= nil and objectInfo.key == filter.exceptKey then
		return false
	end
	if filter.layerName ~= nil and objectInfo.layer ~= filter.layerName then
		return false
	end
	if filter.groupName ~= nil and objectInfo.uiGroup ~= filter.groupName and objectInfo.popupGroup ~= filter.groupName then
		return false
	end
	if filter.popupGroup ~= nil and objectInfo.popupGroup ~= filter.popupGroup then
		return false
	end
	if filter.sceneName ~= nil and objectInfo.sceneName ~= filter.sceneName then
		return false
	end
	if filter.sceneCleanup == true and objectInfo.closeOnSceneChange == false then
		return false
	end
	return true
end

function FairyGuiManager:CollectCloseHandles(filter)
	local handles = {}
	local handleSet = {}

	for index = #self.uiStack, 1, -1 do
		local entry = self.uiStack[index]
		local objectInfo = entry ~= nil and self.objects[entry.key] or nil
		if objectInfo ~= nil and self:MatchCloseFilter(objectInfo, filter) then
			table.insert(handles, objectInfo.handle)
			handleSet[objectInfo.handle] = true
		end
	end

	for handle, objectInfo in pairs(self.objectsByHandle) do
		if handleSet[handle] ~= true and self:MatchCloseFilter(objectInfo, filter) then
			table.insert(handles, handle)
			handleSet[handle] = true
		end
	end
	return handles
end

function FairyGuiManager:CloseByFilter(filter, forceDestroy)
	local handles = self:CollectCloseHandles(filter)
	local closeForceDestroy = forceDestroy ~= false
	local closedCount = 0

	for _, handle in ipairs(handles) do
		local objectInfo = self.objectsByHandle[handle]
		local objectForceDestroy = closeForceDestroy
		if filter ~= nil and filter.sceneCleanup == true and objectInfo ~= nil and objectInfo.destroyOnSceneChange == true then
			objectForceDestroy = true
		end
		if objectInfo ~= nil and self:CloseUI(handle, objectForceDestroy) then
			closedCount = closedCount + 1
		end
	end
	return closedCount
end

function FairyGuiManager:CloseAll(layerName, forceDestroy)
	return self:CloseByFilter({ layerName = layerName }, forceDestroy)
end

function FairyGuiManager:CloseLayer(layerName, forceDestroy)
	return self:CloseAll(layerName, forceDestroy)
end

function FairyGuiManager:CloseGroup(groupName, forceDestroy)
	return self:CloseByFilter({ groupName = groupName }, forceDestroy)
end

function FairyGuiManager:CloseScene(sceneName, forceDestroy)
	local targetSceneName = sceneName or self.currentSceneName or "Default"
	return self:CloseByFilter({ sceneName = targetSceneName, sceneCleanup = true }, forceDestroy)
end

function FairyGuiManager:CleanupScene(sceneName, forceDestroy)
	return self:CloseScene(sceneName, forceDestroy)
end

function FairyGuiManager:SetCurrentScene(sceneName, cleanupPrevious, forceDestroy)
	local nextSceneName = sceneName or "Default"
	local previousSceneName = self.currentSceneName or "Default"
	if cleanupPrevious == true and previousSceneName ~= nextSceneName then
		self:CloseScene(previousSceneName, forceDestroy)
	end
	self.currentSceneName = nextSceneName
	return true
end

function FairyGuiManager:ChangeScene(sceneName, forceDestroy)
	return self:SetCurrentScene(sceneName, true, forceDestroy)
end

function FairyGuiManager:DumpOpenUIs()
	print("[FGUI] DumpOpenUIs begin")
	for key, objectInfo in pairs(self.objects) do
		print("[FGUI] UI", key, "handle=", objectInfo.handle, "layer=", objectInfo.layer, "layerRoot=", self:GetLayerRoot(objectInfo.layer), "group=", objectInfo.uiGroup, "popupGroup=", objectInfo.popupGroup, "scene=", objectInfo.sceneName, "cache=", objectInfo.cache, "hidden=", self.hiddenObjects[key] ~= nil)
	end
	print("[FGUI] DumpOpenUIs end")
end

function FairyGuiManager:DumpLayerRoots()
	return self:GetLayers():DumpLayerRoots()
end

function FairyGuiManager:DumpScenes()
	local sceneStats = {}
	for key, objectInfo in pairs(self.objects) do
		local sceneName = objectInfo.sceneName or "Default"
		local stat = sceneStats[sceneName]
		if stat == nil then
			stat = { total = 0, hidden = 0 }
			sceneStats[sceneName] = stat
		end
		stat.total = stat.total + 1
		if self.hiddenObjects[key] ~= nil then
			stat.hidden = stat.hidden + 1
		end
	end
	print("[FGUI] DumpScenes current=", self.currentSceneName)
	for sceneName, stat in pairs(sceneStats) do
		print("[FGUI] Scene", sceneName, "total=", stat.total, "hidden=", stat.hidden)
	end
end

function FairyGuiManager:DumpPackages()
	return self:GetPackageManager():DumpPackages()
end

function FairyGuiManager:GetPackageRefs()
	return self:GetPackageManager():GetPackageRefs()
end

function FairyGuiManager:DumpResourceRefs()
	return self:GetPackageManager():DumpResourceRefs()
end

function FairyGuiManager:GetResourceWarnings()
	return self:GetPackageManager():GetResourceWarnings()
end

function FairyGuiManager:DumpResourceWarnings()
	return self:GetPackageManager():DumpResourceWarnings()
end

function FairyGuiManager:DumpBindings()
	return self:GetEvents():DumpBindings()
end

function FairyGuiManager:GetEventStats()
	return self:GetEvents():GetEventStats()
end

function FairyGuiManager:DumpEventStats()
	return self:GetEvents():DumpEventStats()
end

function FairyGuiManager:DumpStacks()
	return self:GetLayers():DumpStacks()
end

function FairyGuiManager:Dump()
	self:DumpHealth()
	self:DumpDebugStats()
	self:DumpOpenUIs()
	self:DumpPackages()
	self:DumpResourceRefs()
	self:DumpResourceWarnings()
	self:DumpBindings()
	self:DumpEventStats()
	self:DumpPerfStats()
	self:DumpStacks()
	self:DumpLayerRoots()
	self:DumpScenes()
	self:DumpServices()
	self:DumpRenderStats()
end

function FairyGuiManager_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId, wheelDelta, dragDeltaX, dragDeltaY)
	if _G.FairyGuiManager == nil then
		return false
	end
	return _G.FairyGuiManager:_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId, wheelDelta, dragDeltaX, dragDeltaY)
end

function FairyGuiManager_DispatchTransition(callbackId, objectHandle, transitionName)
	if _G.FairyGuiManager == nil then
		return false
	end
	return _G.FairyGuiManager:_DispatchTransition(callbackId, objectHandle, transitionName)
end

function FairyGuiManager_HandleKeyPressed(keyCode, keyText)
	if _G.FairyGuiManager == nil then
		return false
	end
	return _G.FairyGuiManager:HandleKeyPressed(keyCode, keyText)
end

function FairyGuiManager_HandleKeyReleased(keyCode, keyText)
	if _G.FairyGuiManager == nil then
		return false
	end
	return _G.FairyGuiManager:HandleKeyReleased(keyCode, keyText)
end

function FairyGuiManager_HandleWindowResized(width, height)
	if _G.FairyGuiManager == nil then
		return false
	end
	_G.FairyGuiManager:HandleWindowResized(width, height)
	return true
end

return FairyGuiManager
