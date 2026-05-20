local FairyGuiManager = Class("FairyGuiManager")

local instance = nil

local function isBlank(value)
	return value == nil or value == ""
end

local function normalizePath(path)
	if type(path) ~= "string" then
		return ""
	end
	return string.gsub(path, "\\", "/")
end

local function stripPackageExtension(path)
	return string.gsub(path, "%.fui$", "")
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

local DEFAULT_LAYER_ORDER = {
	Background = 1000,
	Normal = 2000,
	Popup = 3000,
	Guide = 4000,
	Top = 5000,
	Toast = 6000,
}

local KEY_ESCAPE = 1

local LIST_ITEM_METHODS = {}

local function getCurrentManager()
	if _G.FairyGuiManager ~= nil then
		return _G.FairyGuiManager
	end
	if ClassList ~= nil and ClassList.FairyGuiManager ~= nil then
		return ClassList.FairyGuiManager:GetInst()
	end
	return nil
end

function LIST_ITEM_METHODS:GetHandle()
	return self.handle
end

function LIST_ITEM_METHODS:SetText(childPath, text)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetText(self.handle, childPath, text) or false
end

function LIST_ITEM_METHODS:SetIcon(childPath, icon)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetIcon(self.handle, childPath, icon) or false
end

function LIST_ITEM_METHODS:SetLoaderUrl(childPath, url)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetLoaderUrl(self.handle, childPath, url) or false
end

function LIST_ITEM_METHODS:SetVisible(childPathOrVisible, visible)
	local manager = getCurrentManager()
	if manager == nil then
		return false
	end
	if visible == nil then
		return manager:SetVisible(self.handle, childPathOrVisible == true)
	end
	return manager:SetChildVisible(self.handle, childPathOrVisible, visible == true)
end

function LIST_ITEM_METHODS:SetPosition(childPath, x, y)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetChildPosition(self.handle, childPath, x, y) or false
end

function LIST_ITEM_METHODS:SetSize(childPath, width, height)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetChildSize(self.handle, childPath, width, height) or false
end

function LIST_ITEM_METHODS:SetControllerIndex(controllerName, selectedIndex)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetControllerIndex(self.handle, controllerName, selectedIndex) or false
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

local function callLifecycle(view, functionName, ...)
	local func = view and view[functionName]
	if func ~= nil then
		return func(view, ...)
	end
	return nil
end

local function detachView(manager, objectInfo)
	if objectInfo == nil or objectInfo.view == nil then
		return
	end

	local view = objectInfo.view
	callLifecycle(view, "OnHide")
	callLifecycle(view, "OnRemove")
	callLifecycle(view, "OnDestroy")
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
		self.timers = {}
		self.timersByKey = {}
	self.nextCallbackId = 1
end

function FairyGuiManager:IsAvailable()
	if GameManager == nil then
		return false
	end
	return GameManager:isFairyGuiAvailable()
end

function FairyGuiManager:GetPackageNameByPath(packagePath)
	packagePath = normalizePath(packagePath)
	packagePath = stripPackageExtension(packagePath)

	return string.match(packagePath, "([^/]+)$") or packagePath
end

function FairyGuiManager:SetPackageRoot(packageRoot)
	packageRoot = stripPackageExtension(normalizePath(packageRoot))
	packageRoot = string.gsub(packageRoot, "/$", "")
	if not isBlank(packageRoot) then
		self.packageRoot = packageRoot
	end
end

function FairyGuiManager:ResolvePackagePath(packagePath)
	packagePath = stripPackageExtension(normalizePath(packagePath))
	if isBlank(packagePath) then
		return ""
	end
	if string.find(packagePath, "/", 1, true) ~= nil or string.find(packagePath, ":", 1, true) ~= nil then
		return packagePath
	end
	return self.packageRoot .. "/" .. packagePath
end

function FairyGuiManager:AddPackage(packageList)
	if type(packageList) == "string" then
		return self:LoadPackage(packageList)
	end

	if type(packageList) ~= "table" then
		return nil
	end

	local lastPackageName = nil
	if #packageList > 0 then
		for index = 1, #packageList do
			lastPackageName = self:LoadPackage(packageList[index])
		end
	else
		for _, packagePath in pairs(packageList) do
			lastPackageName = self:LoadPackage(packagePath)
		end
	end
	return lastPackageName
end

function FairyGuiManager:LoadPackage(packagePath, packageName)
	if not self:IsAvailable() then
		return nil
	end

	packagePath = self:ResolvePackagePath(packagePath)
	if isBlank(packagePath) then
		return nil
	end

	local packageInfo = self.packages[packagePath]
	if packageInfo then
		return packageInfo.packageName
	end

	local loadedPackageName = GameManager:loadFairyGuiPackage(packagePath)
	if isBlank(loadedPackageName) then
		return nil
	end

	local resolvedPackageName = packageName
	if isBlank(resolvedPackageName) then
		resolvedPackageName = loadedPackageName
	end
	if isBlank(resolvedPackageName) then
		resolvedPackageName = self:GetPackageNameByPath(packagePath)
	end

	packageInfo = {
		packagePath = packagePath,
		packageName = resolvedPackageName,
		loadedPackageName = loadedPackageName,
		refCount = 0,
	}
	self.packages[packagePath] = packageInfo
	self.packages[resolvedPackageName] = packageInfo
	self.packagesByName[resolvedPackageName] = packageInfo
	return resolvedPackageName
end

function FairyGuiManager:RetainPackage(packagePath, packageName)
	packagePath = self:ResolvePackagePath(packagePath)
	local packageInfo = self.packages[packagePath] or self.packages[packageName]
	if packageInfo == nil then
		return nil
	end

	packageInfo.refCount = (packageInfo.refCount or 0) + 1
	return packageInfo
end

function FairyGuiManager:ReleasePackage(packagePath, packageName, unloadWhenZero)
	packagePath = self:ResolvePackagePath(packagePath)
	local packageInfo = self.packages[packagePath] or self.packages[packageName]
	if packageInfo == nil then
		return false
	end

	packageInfo.refCount = math.max((packageInfo.refCount or 0) - 1, 0)
	if packageInfo.refCount > 0 or unloadWhenZero ~= true then
		return true
	end

	if GameManager ~= nil and GameManager.removeFairyGuiPackage ~= nil then
		GameManager:removeFairyGuiPackage(packageInfo.packageName or packageName or "")
	end
	self.packages[packageInfo.packagePath] = nil
	self.packages[packageInfo.packageName] = nil
	self.packagesByName[packageInfo.packageName] = nil
	return true
end

function FairyGuiManager:PreloadPackage(packagePath, packageName)
	return self:LoadPackage(packagePath, packageName)
end

function FairyGuiManager:CreateObject(packageName, objectName)
	if not self:IsAvailable() or isBlank(packageName) or isBlank(objectName) then
		return 0
	end
	return GameManager:createFairyGuiObject(packageName, objectName)
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
	local commandCount = 0
	local triangleCount = 0
	local materialCount = 0
	local textureCount = 0
	local runtimeObjectHandle = 0
	local runtimeBinding = 0
	if GameManager ~= nil and GameManager.getFairyGuiLastRenderCommandCount ~= nil then
		commandCount = GameManager:getFairyGuiLastRenderCommandCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiLastTriangleCount ~= nil then
		triangleCount = GameManager:getFairyGuiLastTriangleCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiMaterialCount ~= nil then
		materialCount = GameManager:getFairyGuiMaterialCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiTextureCount ~= nil then
		textureCount = GameManager:getFairyGuiTextureCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiRuntimeObjectHandleCount ~= nil then
		runtimeObjectHandle = GameManager:getFairyGuiRuntimeObjectHandleCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiRuntimeListenerBindingCount ~= nil then
		runtimeBinding = GameManager:getFairyGuiRuntimeListenerBindingCount()
	end
	return {
		commandCount = commandCount or 0,
		triangleCount = triangleCount or 0,
		materialCount = materialCount or 0,
		textureCount = textureCount or 0,
		runtimeObjectHandle = runtimeObjectHandle or 0,
		runtimeBinding = runtimeBinding or 0,
	}
end

function FairyGuiManager:DumpRenderStats()
	local stats = self:GetRenderStats()
	print("[FGUI] RenderStats commandCount=", stats.commandCount, "triangleCount=", stats.triangleCount, "material=", stats.materialCount, "texture=", stats.textureCount, "runtimeObjectHandle=", stats.runtimeObjectHandle, "runtimeBinding=", stats.runtimeBinding)
end

function FairyGuiManager:GetDebugStats()
	local childCacheCount = 0
	for _, children in pairs(self.childrenByHandle) do
		childCacheCount = childCacheCount + tableCount(children)
	end

	local packageCount = 0
	local printed = {}
	for _, packageInfo in pairs(self.packagesByName) do
		if packageInfo ~= nil and printed[packageInfo.packageName] ~= true then
			packageCount = packageCount + 1
			printed[packageInfo.packageName] = true
		end
	end

	return {
		openUI = tableCount(self.objects),
		hiddenUI = tableCount(self.hiddenObjects),
		package = packageCount,
		layerRoot = tableCount(self.layerRoots),
		binding = tableCount(self.bindings),
		timer = tableCount(self.timers),
		objectHandle = tableCount(self.objectsByHandle),
		childCache = childCacheCount,
		view = tableCount(self.views),
		controller = tableCount(self.controllers),
	}
end

function FairyGuiManager:DumpDebugStats()
	local stats = self:GetDebugStats()
	print("[FGUI] DebugStats openUI=", stats.openUI, "hiddenUI=", stats.hiddenUI, "package=", stats.package, "layerRoot=", stats.layerRoot, "binding=", stats.binding, "timer=", stats.timer, "objectHandle=", stats.objectHandle, "childCache=", stats.childCache, "view=", stats.view, "controller=", stats.controller)
end

function FairyGuiManager:GetHealthStats()
	local debugStats = self:GetDebugStats()
	local renderStats = self:GetRenderStats()
	local threadTimerCount = 0
	if threadpool ~= nil and threadpool.get_timer_count ~= nil then
		threadTimerCount = threadpool:get_timer_count()
	end

	return {
		openUI = debugStats.openUI,
		hiddenUI = debugStats.hiddenUI,
		package = debugStats.package,
		layerRoot = debugStats.layerRoot,
		binding = debugStats.binding,
		timer = debugStats.timer,
		threadTimer = threadTimerCount,
		objectHandle = debugStats.objectHandle,
		childCache = debugStats.childCache,
		view = debugStats.view,
		controller = debugStats.controller,
		focusedHandle = self:GetFocusedHandle(),
		commandCount = renderStats.commandCount,
		triangleCount = renderStats.triangleCount,
		materialCount = renderStats.materialCount,
		textureCount = renderStats.textureCount,
		runtimeObjectHandle = renderStats.runtimeObjectHandle,
		runtimeBinding = renderStats.runtimeBinding,
	}
end

function FairyGuiManager:DumpHealth(verbose)
	local stats = self:GetHealthStats()
	print("[FGUI] Health openUI=", stats.openUI, "hiddenUI=", stats.hiddenUI, "package=", stats.package, "layerRoot=", stats.layerRoot, "binding=", stats.binding, "timer=", stats.timer, "threadTimer=", stats.threadTimer, "objectHandle=", stats.objectHandle, "childCache=", stats.childCache, "view=", stats.view, "controller=", stats.controller, "focusedHandle=", stats.focusedHandle, "runtimeObjectHandle=", stats.runtimeObjectHandle, "runtimeBinding=", stats.runtimeBinding, "material=", stats.materialCount, "texture=", stats.textureCount, "commandCount=", stats.commandCount, "triangleCount=", stats.triangleCount)
	if verbose == true then
		self:DumpResourceRefs()
		self:DumpResourceWarnings()
		self:DumpStacks()
		self:DumpLayerRoots()
	end
	return stats
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
			"popupMode",
			"popupGroup",
			"group",
			"uiGroup",
			"scene",
			"sceneName",
			"closeOnSceneChange",
			"destroyOnSceneChange",
			"fullScreen",
			"adaptScreen",
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
	layerName = layerName or "Normal"
	if self.layers[layerName] == nil then
		self.layers[layerName] = self.layers.Normal or DEFAULT_LAYER_ORDER.Normal
		self.layerNextOrder[layerName] = 0
		self.layerObjects[layerName] = {}
	end
	return self.layers[layerName]
end

function FairyGuiManager:GetLayerRoot(layerName)
	layerName = layerName or "Normal"
	return self.layerRoots ~= nil and self.layerRoots[layerName] or nil
end

function FairyGuiManager:EnsureLayerRoot(layerName)
	layerName = layerName or "Normal"
	if self.layerRoots == nil then
		self.layerRoots = {}
	end

	local rootHandle = self.layerRoots[layerName]
	if rootHandle ~= nil then
		return rootHandle
	end
	if GameManager == nil or GameManager.createFairyGuiContainer == nil then
		return nil
	end

	rootHandle = GameManager:createFairyGuiContainer("Layer_" .. layerName)
	if rootHandle == nil or rootHandle <= 0 then
		return nil
	end
	if GameManager.addFairyGuiObjectToRoot == nil or not GameManager:addFairyGuiObjectToRoot(rootHandle) then
		if GameManager.removeFairyGuiObject ~= nil then
			GameManager:removeFairyGuiObject(rootHandle)
		end
		return nil
	end

	self.layerRoots[layerName] = rootHandle
	self:SetPosition(rootHandle, 0, 0)
	self:SetSize(rootHandle, self:GetScreenWidth(), self:GetScreenHeight())
	if GameManager.setFairyGuiObjectSortingOrder ~= nil then
		GameManager:setFairyGuiObjectSortingOrder(rootHandle, self:GetLayerBaseOrder(layerName))
	end
	return rootHandle
end

function FairyGuiManager:AttachToLayer(handle, layerName)
	local rootHandle = self:EnsureLayerRoot(layerName)
	if rootHandle ~= nil and GameManager.addFairyGuiObjectToParent ~= nil then
		return GameManager:addFairyGuiObjectToParent(handle, rootHandle)
	end
	if GameManager ~= nil and GameManager.addFairyGuiObjectToRoot ~= nil then
		return GameManager:addFairyGuiObjectToRoot(handle)
	end
	return false
end

function FairyGuiManager:ResizeLayerRoots()
	if self.layerRoots == nil then
		return
	end

	local screenWidth = self:GetScreenWidth()
	local screenHeight = self:GetScreenHeight()
	for layerName, handle in pairs(self.layerRoots) do
		self:SetPosition(handle, 0, 0)
		self:SetSize(handle, screenWidth, screenHeight)
		if GameManager ~= nil and GameManager.setFairyGuiObjectSortingOrder ~= nil then
			GameManager:setFairyGuiObjectSortingOrder(handle, self:GetLayerBaseOrder(layerName))
		end
	end
end

function FairyGuiManager:NextLayerSortingOrder(layerName)
	layerName = layerName or "Normal"
	local nextOrder = (self.layerNextOrder[layerName] or 0) + 1
	self.layerNextOrder[layerName] = nextOrder
	return self:GetLayerBaseOrder(layerName) + nextOrder
end

function FairyGuiManager:IsPopupLayer(layerName)
	return layerName == "Popup" or layerName == "Guide" or layerName == "Top" or layerName == "Toast"
end

function FairyGuiManager:GetStackMode(objectInfo)
	if objectInfo == nil then
		return "None"
	end

	local param = objectInfo.param or {}
	if param.stack == false or param.stackMode == "None" then
		return "None"
	end
	if param.stackMode ~= nil then
		return param.stackMode
	end
	if self:IsPopupLayer(objectInfo.layer) then
		return "Popup"
	end
	return "Normal"
end

function FairyGuiManager:RemoveStackEntry(key)
	if key == nil or self.stackEntriesByKey[key] == nil then
		return false
	end

	local entry = self.stackEntriesByKey[key]
	self.stackEntriesByKey[key] = nil
	for index = #self.uiStack, 1, -1 do
		if self.uiStack[index].key == key then
			table.remove(self.uiStack, index)
		end
	end
	for index = #self.popupStack, 1, -1 do
		if self.popupStack[index].key == key then
			table.remove(self.popupStack, index)
		end
	end
	return entry ~= nil
end

function FairyGuiManager:PushStack(objectInfo)
	if objectInfo == nil or objectInfo.key == nil then
		return false
	end

	local stackMode = self:GetStackMode(objectInfo)
	self:RemoveStackEntry(objectInfo.key)
	if stackMode == "None" then
		objectInfo.stackMode = stackMode
		return false
	end

	self.nextStackSerial = (self.nextStackSerial or 0) + 1
	local entry = {
		key = objectInfo.key,
		handle = objectInfo.handle,
		layer = objectInfo.layer,
		mode = stackMode,
		popupGroup = objectInfo.popupGroup,
		popupMode = objectInfo.popupMode,
		serial = self.nextStackSerial,
	}
	self.stackEntriesByKey[objectInfo.key] = entry
	table.insert(self.uiStack, entry)
	if stackMode == "Popup" or self:IsPopupLayer(objectInfo.layer) then
		table.insert(self.popupStack, entry)
	end
	objectInfo.stackMode = stackMode
	objectInfo.stackSerial = entry.serial
	return true
end

function FairyGuiManager:GetTopStackObject(stack, layerName)
	for index = #stack, 1, -1 do
		local entry = stack[index]
		local objectInfo = entry ~= nil and self.objects[entry.key] or nil
		if objectInfo ~= nil and self.hiddenObjects[entry.key] == nil and (layerName == nil or objectInfo.layer == layerName) then
			return objectInfo, entry
		end
	end
	return nil, nil
end

function FairyGuiManager:GetObjectResult(objectInfo)
	if objectInfo == nil then
		return nil
	end
	return objectInfo.ctrl or objectInfo.view or objectInfo.handle
end

function FairyGuiManager:GetTopUI(layerName)
	local objectInfo = self:GetTopStackObject(self.uiStack, layerName)
	return self:GetObjectResult(objectInfo)
end

function FairyGuiManager:GetTopPopup()
	local objectInfo = self:GetTopStackObject(self.popupStack)
	return self:GetObjectResult(objectInfo)
end

function FairyGuiManager:CloseTop(layerName, forceDestroy)
	local objectInfo = self:GetTopStackObject(self.uiStack, layerName)
	if objectInfo == nil then
		return false
	end
	return self:CloseUI(objectInfo.key, forceDestroy)
end

function FairyGuiManager:CloseTopPopup(forceDestroy)
	local objectInfo = self:GetTopStackObject(self.popupStack)
	if objectInfo == nil then
		return false
	end
	return self:CloseUI(objectInfo.key, forceDestroy)
end

function FairyGuiManager:IsPopupOpenParam(param)
	if param == nil then
		return false
	end
	if param.stackMode == "Popup" then
		return true
	end
	return self:IsPopupLayer(param.layer)
end

function FairyGuiManager:GetPopupGroup(param, objectInfo)
	local group = param ~= nil and param.popupGroup or nil
	if isBlank(group) and objectInfo ~= nil then
		group = objectInfo.popupGroup
	end
	if isBlank(group) and param ~= nil then
		group = param.key or param.uiName
	end
	if isBlank(group) and objectInfo ~= nil then
		group = objectInfo.key or objectInfo.uiName
	end
	return group or "Popup"
end

function FairyGuiManager:GetUIGroup(param, objectInfo)
	local group = param ~= nil and (param.uiGroup or param.group) or nil
	if isBlank(group) and objectInfo ~= nil then
		group = objectInfo.uiGroup
	end
	if isBlank(group) and param ~= nil then
		group = param.popupGroup
	end
	if isBlank(group) and objectInfo ~= nil then
		group = objectInfo.popupGroup
	end
	return group
end

function FairyGuiManager:GetSceneName(param, objectInfo)
	local sceneName = param ~= nil and (param.sceneName or param.scene) or nil
	if isBlank(sceneName) and objectInfo ~= nil then
		sceneName = objectInfo.sceneName
	end
	if isBlank(sceneName) then
		sceneName = self.currentSceneName
	end
	return sceneName or "Default"
end

function FairyGuiManager:GetTopPopupObjectByGroup(popupGroup)
	if isBlank(popupGroup) then
		return nil
	end

	for index = #self.popupStack, 1, -1 do
		local entry = self.popupStack[index]
		local objectInfo = entry ~= nil and self.objects[entry.key] or nil
		if objectInfo ~= nil and self.hiddenObjects[entry.key] == nil and objectInfo.popupGroup == popupGroup then
			return objectInfo, entry
		end
	end
	return nil, nil
end

function FairyGuiManager:ReopenObjectInfo(objectInfo, param)
	if objectInfo == nil then
		return nil
	end

	objectInfo.param = param or objectInfo.param
	objectInfo.cache = objectInfo.param.cache == true or objectInfo.cache == true
	objectInfo.popupGroup = self:GetPopupGroup(objectInfo.param, objectInfo)
	objectInfo.popupMode = objectInfo.param.popupMode or objectInfo.popupMode or "stack"
	objectInfo.uiGroup = self:GetUIGroup(objectInfo.param, objectInfo)
	objectInfo.sceneName = self:GetSceneName(objectInfo.param, objectInfo)
	objectInfo.closeOnSceneChange = objectInfo.param.closeOnSceneChange ~= false
	self.hiddenObjects[objectInfo.key] = nil
	self:SetVisible(objectInfo.handle, true)
	if objectInfo.modalMaskHandle ~= nil then
		self:SetVisible(objectInfo.modalMaskHandle, true)
	end
	self:BringToFront(objectInfo.handle)
	self:ApplyScreenAdapt(objectInfo)
	if objectInfo.view ~= nil then
		if objectInfo.view._Attach ~= nil then
			local viewName = objectInfo.autoGenClass or objectInfo.name
			objectInfo.view:_Attach(objectInfo.handle, objectInfo.key, viewName, objectInfo.param)
		end
		callView(objectInfo.view, "OnReopen", objectInfo.param)
		callView(objectInfo.view, "OnOpen", objectInfo.param)
		callView(objectInfo.view, "OnShow", objectInfo.param)
	end
	return self:GetObjectResult(objectInfo)
end

function FairyGuiManager:ClosePopupGroup(popupGroup, exceptKey, forceDestroy)
	if isBlank(popupGroup) then
		return 0
	end

	local closeKeys = {}
	for index = #self.popupStack, 1, -1 do
		local entry = self.popupStack[index]
		local objectInfo = entry ~= nil and self.objects[entry.key] or nil
		if objectInfo ~= nil and objectInfo.key ~= exceptKey and objectInfo.popupGroup == popupGroup then
			table.insert(closeKeys, objectInfo.key)
		end
	end

	local closeCount = 0
	for _, key in ipairs(closeKeys) do
		if self:CloseUI(key, forceDestroy) then
			closeCount = closeCount + 1
		end
	end
	return closeCount
end

function FairyGuiManager:ApplyPopupOpenPolicy(param)
	if param == nil or param.__popupPolicyApplied == true then
		return nil
	end
	param.__popupPolicyApplied = true

	if not self:IsPopupOpenParam(param) then
		return nil
	end

	local popupMode = string.lower(tostring(param.popupMode or "stack"))
	param.popupGroup = self:GetPopupGroup(param)
	if popupMode == "single" then
		local objectInfo = self:GetTopPopupObjectByGroup(param.popupGroup)
		if objectInfo ~= nil then
			return self:ReopenObjectInfo(objectInfo, param)
		end
	elseif popupMode == "replace" then
		self:ClosePopupGroup(param.popupGroup, param.key, param.forceDestroyOnReplace == true)
	end
	return nil
end

function FairyGuiManager:CanClosePopupByEscape(objectInfo)
	if objectInfo == nil then
		return false
	end
	local param = objectInfo.param or {}
	return param.closeOnEscape ~= false
end

function FairyGuiManager:HandleKeyPressed(keyCode, keyText)
	if tonumber(keyCode) == KEY_ESCAPE then
		local objectInfo = self:GetTopStackObject(self.popupStack)
		if self:CanClosePopupByEscape(objectInfo) then
			return self:CloseUI(objectInfo.key)
		end
	end
	return false
end

function FairyGuiManager:HandleKeyReleased(keyCode, keyText)
	return false
end

function FairyGuiManager:UpdateModalMaskSorting(objectInfo)
	if objectInfo == nil or objectInfo.modalMaskHandle == nil then
		return false
	end
	if GameManager ~= nil and GameManager.setFairyGuiObjectSortingOrder ~= nil then
		GameManager:setFairyGuiObjectSortingOrder(objectInfo.modalMaskHandle, (objectInfo.sortingOrder or self:GetLayerBaseOrder(objectInfo.layer)) - 1)
	end
	if self.hiddenObjects[objectInfo.key] == nil then
		self:SetVisible(objectInfo.modalMaskHandle, true)
	end
	return true
end

function FairyGuiManager:AssignLayer(objectInfo, layerName)
	if objectInfo == nil or objectInfo.handle == nil then
		return false
	end

	layerName = layerName or objectInfo.layer or "Normal"
	if objectInfo.layer ~= nil and self.layerObjects[objectInfo.layer] ~= nil then
		self.layerObjects[objectInfo.layer][objectInfo.handle] = nil
	end

	objectInfo.layer = layerName
	objectInfo.sortingOrder = self:NextLayerSortingOrder(layerName)
	if self.layerObjects[layerName] == nil then
		self.layerObjects[layerName] = {}
	end
	self.layerObjects[layerName][objectInfo.handle] = true

	if GameManager ~= nil and GameManager.setFairyGuiObjectSortingOrder ~= nil then
		local result = GameManager:setFairyGuiObjectSortingOrder(objectInfo.handle, objectInfo.sortingOrder)
		self:UpdateModalMaskSorting(objectInfo)
		return result
	end
	self:UpdateModalMaskSorting(objectInfo)
	return true
end

function FairyGuiManager:BringToFront(keyOrHandle)
	local objectInfo = self:GetObjectInfo(keyOrHandle)
	if objectInfo == nil then
		return false
	end
	local result = self:AssignLayer(objectInfo, objectInfo.layer)
	self:PushStack(objectInfo)
	return result
end

function FairyGuiManager:CreateModalMask(objectInfo, param)
	if objectInfo == nil or objectInfo.handle == nil or param == nil or param.modal ~= true then
		return nil
	end
	if GameManager == nil or GameManager.createFairyGuiModalMask == nil then
		return nil
	end

	local alpha = param.modalAlpha or 0.45
	local maskHandle = GameManager:createFairyGuiModalMask(0, 0, 0, alpha)
	if maskHandle == nil or maskHandle <= 0 then
		return nil
	end

	if not self:AttachToLayer(maskHandle, objectInfo.layer) then
		GameManager:removeFairyGuiObject(maskHandle)
		return nil
	end
	if GameManager.setFairyGuiObjectSortingOrder ~= nil then
		GameManager:setFairyGuiObjectSortingOrder(maskHandle, (objectInfo.sortingOrder or self:GetLayerBaseOrder(objectInfo.layer)) - 1)
	end
	self:SetSize(maskHandle, self:GetScreenWidth(), self:GetScreenHeight())
	self:SetPosition(maskHandle, 0, 0)
	objectInfo.modalMaskHandle = maskHandle
	if param.closeOnMaskClick == true then
		local closeKey = objectInfo.key
		objectInfo.modalMaskBindingId = self:AddClick(maskHandle, "", function()
			self:CloseUI(closeKey)
		end)
	end
	return maskHandle
end

function FairyGuiManager:GetScreenWidth()
	if GameManager ~= nil and GameManager.getFairyGuiScreenWidth ~= nil then
		local width = GameManager:getFairyGuiScreenWidth()
		if width ~= nil and width > 0 then
			return width
		end
	end
	if GameManager ~= nil and GameManager.getScreenWidth ~= nil then
		return GameManager:getScreenWidth()
	end
	return 0
end

function FairyGuiManager:GetScreenHeight()
	if GameManager ~= nil and GameManager.getFairyGuiScreenHeight ~= nil then
		local height = GameManager:getFairyGuiScreenHeight()
		if height ~= nil and height > 0 then
			return height
		end
	end
	if GameManager ~= nil and GameManager.getScreenHeight ~= nil then
		return GameManager:getScreenHeight()
	end
	return 0
end

function FairyGuiManager:ApplyScreenAdapt(objectInfo)
	if objectInfo == nil or objectInfo.handle == nil then
		return false
	end

	local param = objectInfo.param or {}
	if param.fullScreen ~= true and param.adaptScreen ~= true then
		return false
	end

	local screenWidth = self:GetScreenWidth()
	local screenHeight = self:GetScreenHeight()
	if screenWidth <= 0 or screenHeight <= 0 then
		return false
	end

	local left = param.marginLeft or 0
	local right = param.marginRight or 0
	local top = param.marginTop or 0
	local bottom = param.marginBottom or 0
	self:SetPosition(objectInfo.handle, left, top)
	self:SetSize(objectInfo.handle, math.max(screenWidth - left - right, 0), math.max(screenHeight - top - bottom, 0))
	if objectInfo.modalMaskHandle ~= nil then
		self:SetPosition(objectInfo.modalMaskHandle, 0, 0)
		self:SetSize(objectInfo.modalMaskHandle, screenWidth, screenHeight)
	end
	return true
end

function FairyGuiManager:HandleWindowResized(width, height)
	self:ResizeLayerRoots()
	for _, objectInfo in pairs(self.objects) do
		self:ApplyScreenAdapt(objectInfo)
		if objectInfo.view ~= nil then
			callView(objectInfo.view, "OnResize", width, height)
		end
	end
end

function FairyGuiManager:Open(name, param)
	local config = self:GetUIConfig(name)
	if config == nil then
		print("[FGUI] open failed, ui not registered:", tostring(name))
		return nil
	end

	if not self:LoadUIRequires(config) then
		return nil
	end

	local openParam = self:BuildOpenParam(name, config, param)
	openParam.uiName = name
	self.uiNameToKey[name] = openParam.key
	local policyResult = self:ApplyPopupOpenPolicy(openParam)
	if policyResult ~= nil then
		return policyResult
	end

	local packagePath = config.packagePath or config.package
	local component = openParam.objectName or openParam.component or config.objectName or config.component or name

	if config.viewClass ~= nil then
		openParam.packagePath = packagePath
		openParam.package = packagePath
		openParam.objectName = component
		openParam.component = component
		return self:OpenView(config.viewClass, openParam)
	end

	local classlua = config.classlua or config.autoGen
	if classlua ~= nil then
		openParam.objectName = component
		openParam.component = component
		return self:OpenUI(component, packagePath, classlua, openParam)
	end

	return self:OpenObject(component, packagePath, component, openParam)
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
	if GameManager == nil or handle == nil or isBlank(childPath) then
		return handle
	end

	local childHandles = self.childrenByHandle[handle]
	if childHandles ~= nil and childHandles[childPath] ~= nil then
		return childHandles[childPath]
	end

	local childHandle = GameManager:getFairyGuiChild(handle, childPath)
	if childHandle == nil or childHandle <= 0 then
		return nil
	end

	if childHandles == nil then
		childHandles = {}
		self.childrenByHandle[handle] = childHandles
	end
	childHandles[childPath] = childHandle
	return childHandle
end

function FairyGuiManager:GetTargetHandle(handle, childPath)
	if isBlank(childPath) then
		return handle
	end
	return self:GetChild(handle, childPath)
end

function FairyGuiManager:ClearListCacheByListHandle(listHandle)
	if listHandle == nil then
		return
	end

	local itemHandles = self.listItemHandlesByHandle[listHandle]
	if itemHandles ~= nil then
		for _, itemHandle in pairs(itemHandles) do
			self.childrenByHandle[itemHandle] = nil
		end
	end
	self.listItemHandlesByHandle[listHandle] = nil
	self.listDataByHandle[listHandle] = nil
	self.listRenderersByHandle[listHandle] = nil
end

function FairyGuiManager:ClearListItemHandleCache(listHandle)
	if listHandle == nil then
		return
	end

	local itemHandles = self.listItemHandlesByHandle[listHandle]
	if itemHandles ~= nil then
		for _, itemHandle in pairs(itemHandles) do
			self.childrenByHandle[itemHandle] = nil
		end
	end
	self.listItemHandlesByHandle[listHandle] = nil
end

function FairyGuiManager:ClearListCacheForHandle(handle)
	if handle == nil then
		return
	end

	self:ClearListCacheByListHandle(handle)
	local childHandles = self.childrenByHandle[handle]
	if childHandles ~= nil then
		for _, childHandle in pairs(childHandles) do
			self:ClearListCacheByListHandle(childHandle)
		end
	end
end

function FairyGuiManager:CreateListItemAdapter(listHandle, itemHandle, index, data)
	if itemHandle == nil or itemHandle <= 0 then
		return nil
	end
	return setmetatable({
		listHandle = listHandle,
		handle = itemHandle,
		index = index,
		data = data,
	}, { __index = LIST_ITEM_METHODS })
end

function FairyGuiManager:GetListItemByHandle(listHandle, index, data)
	if GameManager == nil or listHandle == nil or index == nil then
		return nil
	end

	local itemHandles = self.listItemHandlesByHandle[listHandle]
	if itemHandles == nil then
		itemHandles = {}
		self.listItemHandlesByHandle[listHandle] = itemHandles
	end

	local itemHandle = itemHandles[index]
	if itemHandle == nil then
		itemHandle = GameManager:getFairyGuiListItem(listHandle, index - 1)
		if itemHandle == nil or itemHandle <= 0 then
			return nil
		end
		itemHandles[index] = itemHandle
	end
	return self:CreateListItemAdapter(listHandle, itemHandle, index, data)
end

function FairyGuiManager:RenderListItemByHandle(listHandle, index)
	local dataList = self.listDataByHandle[listHandle]
	local data = dataList ~= nil and dataList[index] or nil
	local item = self:GetListItemByHandle(listHandle, index, data)
	if item == nil then
		return false
	end

	local renderer = self.listRenderersByHandle[listHandle]
	if type(renderer) == "function" then
		renderer(item, data, index)
	else
		item:SetText("", tostring(data or ""))
	end
	return true
end

function FairyGuiManager:GetListHandle(handle, childPath)
	return self:GetTargetHandle(handle, childPath)
end

function FairyGuiManager:SetListItemCount(handle, childPath, itemCount)
	if GameManager == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	self:ClearListItemHandleCache(listHandle)
	return GameManager:setFairyGuiListItemCount(listHandle, itemCount or 0)
end

function FairyGuiManager:GetListItemCount(handle, childPath)
	if GameManager == nil then
		return 0
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return 0
	end
	return GameManager:getFairyGuiListItemCount(listHandle)
end

function FairyGuiManager:GetListItem(handle, childPath, index)
	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return nil
	end

	local dataList = self.listDataByHandle[listHandle]
	local data = dataList ~= nil and dataList[index] or nil
	return self:GetListItemByHandle(listHandle, index, data)
end

function FairyGuiManager:SetListData(handle, childPath, dataList, renderer)
	if type(dataList) ~= "table" then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or not self:SetListItemCount(handle, childPath, #dataList) then
		return false
	end

	self.listDataByHandle[listHandle] = dataList
	if type(renderer) == "function" then
		self.listRenderersByHandle[listHandle] = renderer
	else
		self.listRenderersByHandle[listHandle] = nil
	end
	for index, _ in ipairs(dataList) do
		self:RenderListItemByHandle(listHandle, index)
	end
	return true
end

function FairyGuiManager:GetListData(handle, childPath, index)
	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return nil
	end

	local dataList = self.listDataByHandle[listHandle]
	if index == nil then
		return dataList
	end
	return dataList ~= nil and dataList[index] or nil
end

function FairyGuiManager:RefreshListItem(handle, childPath, index)
	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or index == nil then
		return false
	end
	return self:RenderListItemByHandle(listHandle, index)
end

function FairyGuiManager:RefreshList(handle, childPath)
	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	local dataList = self.listDataByHandle[listHandle]
	local itemCount = dataList ~= nil and #dataList or self:GetListItemCount(handle, childPath)
	for index = 1, itemCount do
		self:RenderListItemByHandle(listHandle, index)
	end
	return true
end

function FairyGuiManager:UpdateListItem(handle, childPath, index, data)
	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or index == nil or index < 1 then
		return false
	end

	local dataList = self.listDataByHandle[listHandle]
	if dataList == nil then
		return false
	end
	dataList[index] = data
	return self:RenderListItemByHandle(listHandle, index)
end

function FairyGuiManager:AppendListItem(handle, childPath, data)
	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	local dataList = self.listDataByHandle[listHandle]
	if dataList == nil then
		dataList = {}
		self.listDataByHandle[listHandle] = dataList
	end
	table.insert(dataList, data)
	if not self:SetListItemCount(handle, childPath, #dataList) then
		return false
	end
	return self:RenderListItemByHandle(listHandle, #dataList)
end

function FairyGuiManager:RemoveListItem(handle, childPath, index)
	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or index == nil or index < 1 then
		return false
	end

	local dataList = self.listDataByHandle[listHandle]
	if dataList == nil or index > #dataList then
		return false
	end
	table.remove(dataList, index)
	if not self:SetListItemCount(handle, childPath, #dataList) then
		return false
	end
	return self:RefreshList(handle, childPath)
end

function FairyGuiManager:ClearList(handle, childPath)
	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	local renderer = self.listRenderersByHandle[listHandle]
	return self:SetListData(handle, childPath, {}, renderer)
end

function FairyGuiManager:SetListSelectedIndex(handle, childPath, selectedIndex)
	if GameManager == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end
	return GameManager:setFairyGuiListSelectedIndex(listHandle, (selectedIndex or 1) - 1)
end

function FairyGuiManager:GetListSelectedIndex(handle, childPath)
	if GameManager == nil then
		return 0
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return 0
	end

	local selectedIndex = GameManager:getFairyGuiListSelectedIndex(listHandle)
	return selectedIndex >= 0 and selectedIndex + 1 or 0
end

function FairyGuiManager:ScrollListToView(handle, childPath, index)
	if GameManager == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end
	return GameManager:scrollFairyGuiListToView(listHandle, (index or 1) - 1)
end

function FairyGuiManager:DebugInjectMouseMove(x, y)
	if GameManager == nil or GameManager.injectFairyGuiMouseMove == nil then
		return false
	end
	return GameManager:injectFairyGuiMouseMove(tonumber(x) or 0, tonumber(y) or 0)
end

function FairyGuiManager:DebugInjectMouseDown(x, y, button)
	if GameManager == nil or GameManager.injectFairyGuiMouseDown == nil then
		return false
	end
	return GameManager:injectFairyGuiMouseDown(tonumber(x) or 0, tonumber(y) or 0, tonumber(button) or 0)
end

function FairyGuiManager:DebugInjectMouseUp(x, y, button)
	if GameManager == nil or GameManager.injectFairyGuiMouseUp == nil then
		return false
	end
	return GameManager:injectFairyGuiMouseUp(tonumber(x) or 0, tonumber(y) or 0, tonumber(button) or 0)
end

function FairyGuiManager:DebugInjectMouseWheel(x, y, wheelDelta)
	if GameManager == nil or GameManager.injectFairyGuiMouseWheel == nil then
		return false
	end
	return GameManager:injectFairyGuiMouseWheel(tonumber(x) or 0, tonumber(y) or 0, tonumber(wheelDelta) or 0)
end

function FairyGuiManager:DebugInjectClick(x, y, button)
	if GameManager == nil or GameManager.injectFairyGuiClick == nil then
		return false
	end
	return GameManager:injectFairyGuiClick(tonumber(x) or 0, tonumber(y) or 0, tonumber(button) or 0)
end

function FairyGuiManager:DebugInjectKeyPressed(keyCode, keyText)
	if GameManager == nil or GameManager.injectFairyGuiKeyPressed == nil then
		return false
	end
	return GameManager:injectFairyGuiKeyPressed(tonumber(keyCode) or 0, tonumber(keyText) or 0)
end

function FairyGuiManager:DebugInjectKeyReleased(keyCode, keyText)
	if GameManager == nil or GameManager.injectFairyGuiKeyReleased == nil then
		return false
	end
	return GameManager:injectFairyGuiKeyReleased(tonumber(keyCode) or 0, tonumber(keyText) or 0)
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

function FairyGuiManager:AddEvent(handle, childPath, eventType, callback)
	if GameManager == nil or handle == nil or type(callback) ~= "function" then
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
	if GameManager.addFairyGuiEventListener ~= nil then
		bindingId = GameManager:addFairyGuiEventListener(handle, childPath or "", resolvedEventType, callbackId)
	elseif resolvedEventType == EVENT_TYPES.Click then
		bindingId = GameManager:addFairyGuiClickListener(handle, childPath or "", callbackId)
	end
	if bindingId == nil or bindingId <= 0 then
		self.callbacks[callbackId] = nil
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

function FairyGuiManager:AddClick(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "Click", callback)
end

function FairyGuiManager:AddChanged(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "Changed", callback)
end

function FairyGuiManager:AddClickItem(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "ClickItem", callback)
end

function FairyGuiManager:AddRightClick(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "RightClick", callback)
end

function FairyGuiManager:AddMouseWheel(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "MouseWheel", callback)
end

function FairyGuiManager:AddKeyDown(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "KeyDown", callback)
end

function FairyGuiManager:AddKeyUp(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "KeyUp", callback)
end

function FairyGuiManager:AddSubmit(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "Submit", callback)
end

function FairyGuiManager:AddTouchBegin(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "TouchBegin", callback)
end

function FairyGuiManager:AddTouchEnd(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "TouchEnd", callback)
end

function FairyGuiManager:AddDragStart(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "DragStart", callback)
end

function FairyGuiManager:AddDragMove(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "DragMove", callback)
end

function FairyGuiManager:AddDragEnd(handle, childPath, callback)
	return self:AddEvent(handle, childPath, "DragEnd", callback)
end

function FairyGuiManager:RemoveBinding(bindingId)
	local binding = self.bindings[bindingId]
	if binding == nil then
		return false
	end

	if GameManager ~= nil then
		GameManager:removeFairyGuiListener(bindingId)
	end
	self.callbacks[binding.callbackId] = nil
	self.bindings[bindingId] = nil

	local bindingList = self.bindingsByHandle[binding.handle]
	if bindingList ~= nil then
		bindingList[bindingId] = nil
	end
	return true
end

function FairyGuiManager:ClearBindingsForHandle(handle)
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

function FairyGuiManager:_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId, wheelDelta)
	local callback = self.callbacks[callbackId]
	if callback == nil then
		return false
	end

	local binding = self.bindings[bindingId]
	local itemIndex = rawItemIndex ~= nil and rawItemIndex >= 0 and rawItemIndex + 1 or nil
	local eventData = nil
	if binding ~= nil and itemIndex ~= nil then
		local listHandle = self:GetTargetHandle(binding.handle, binding.childPath)
		local listData = listHandle ~= nil and self.listDataByHandle[listHandle] or nil
		eventData = listData ~= nil and listData[itemIndex] or nil
	end

	local ok, err = pcall(callback, {
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
		eventType = EVENT_NAMES[eventType] or eventType,
		eventTypeId = eventType,
		bindingId = bindingId,
	})
	if not ok then
		print("[FGUI] event callback error:", err)
		return false
	end
	return true
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

function FairyGuiManager:CloseUI(keyOrHandle, forceDestroy)
	local objectInfo = nil
	if type(keyOrHandle) == "number" then
		objectInfo = self.objectsByHandle[keyOrHandle]
	else
		objectInfo = self.objects[keyOrHandle]
	end

	if not objectInfo then
		return false
	end

	local handle = objectInfo.handle
	local ownedHandles = self:CollectOwnedHandles(objectInfo)
	local closeSnapshot = self:CreateCloseSnapshot(objectInfo, ownedHandles)
	callView(objectInfo.view, "OnClose")

	if objectInfo.cache == true and forceDestroy ~= true and objectInfo.param.destroyOnClose ~= true then
		callView(objectInfo.view, "OnHide")
		self:SetVisible(handle, false)
		if objectInfo.modalMaskHandle ~= nil then
			self:SetVisible(objectInfo.modalMaskHandle, false)
		end
		self:RemoveStackEntry(objectInfo.key)
		self.hiddenObjects[objectInfo.key] = objectInfo
		return true
	end

	self:ClearFocusForHandles(ownedHandles)
	detachView(self, objectInfo)
	self:RemoveStackEntry(objectInfo.key)
	self:ClearTimersForKey(objectInfo.key)
	for _, ownedHandle in ipairs(ownedHandles) do
		self:ClearBindingsForHandle(ownedHandle)
	end
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
		return removed
	end

function FairyGuiManager:CloseView(viewOrKeyOrHandle, forceDestroy)
	if type(viewOrKeyOrHandle) == "table" then
		return self:CloseUI(viewOrKeyOrHandle.handle, forceDestroy)
	end
	return self:CloseUI(viewOrKeyOrHandle, forceDestroy)
end

function FairyGuiManager:Close(nameOrKeyOrHandle, forceDestroy)
	if type(nameOrKeyOrHandle) == "number" then
		return self:CloseUI(nameOrKeyOrHandle, forceDestroy)
	end
	return self:CloseUI(self:GetRegisteredUIKey(nameOrKeyOrHandle), forceDestroy)
end

function FairyGuiManager:Destroy(nameOrKeyOrHandle)
	return self:Close(nameOrKeyOrHandle, true)
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
	print("[FGUI] DumpLayerRoots begin")
	if self.layerRoots ~= nil then
		for layerName, handle in pairs(self.layerRoots) do
			print("[FGUI] LayerRoot", layerName, "handle=", handle, "baseOrder=", self:GetLayerBaseOrder(layerName))
		end
	end
	print("[FGUI] DumpLayerRoots end")
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
	print("[FGUI] DumpPackages begin")
	local printed = {}
	for _, packageInfo in pairs(self.packagesByName) do
		if packageInfo ~= nil and printed[packageInfo.packageName] ~= true then
			print("[FGUI] Package", packageInfo.packageName, "path=", packageInfo.packagePath, "refCount=", packageInfo.refCount or 0)
			printed[packageInfo.packageName] = true
		end
	end
	print("[FGUI] DumpPackages end")
end

function FairyGuiManager:GetPackageRefs()
	local refsByPackage = {}
	local printed = {}
	for _, packageInfo in pairs(self.packagesByName) do
		local packageKey = packageInfo ~= nil and (packageInfo.packageName or packageInfo.packagePath) or nil
		if packageInfo ~= nil and packageKey ~= nil and printed[packageKey] ~= true then
			refsByPackage[packageKey] = {
				packageName = packageInfo.packageName,
				packagePath = packageInfo.packagePath,
				refCount = packageInfo.refCount or 0,
				openCount = 0,
				hiddenCount = 0,
				cacheCount = 0,
				stackCount = 0,
				stackedKeys = {},
				objects = {},
			}
			printed[packageKey] = true
		end
	end

	local function getRef(objectInfo)
		local packageName = objectInfo.packageName
		local packagePath = objectInfo.packagePath
		local packageInfo = packagePath ~= nil and self.packages[packagePath] or nil
		if packageInfo == nil and packageName ~= nil then
			packageInfo = self.packagesByName[packageName] or self.packages[packageName]
		end
		local packageKey = packageInfo ~= nil and (packageInfo.packageName or packageInfo.packagePath) or (packageName or packagePath or "<dynamic>")
		local ref = refsByPackage[packageKey]
		if ref == nil then
			ref = {
				packageName = packageInfo ~= nil and packageInfo.packageName or packageName,
				packagePath = packageInfo ~= nil and packageInfo.packagePath or packagePath,
				refCount = packageInfo ~= nil and (packageInfo.refCount or 0) or 0,
				openCount = 0,
				hiddenCount = 0,
				cacheCount = 0,
				stackCount = 0,
				stackedKeys = {},
				objects = {},
			}
			refsByPackage[packageKey] = ref
		end
		return ref
	end

	for key, objectInfo in pairs(self.objects) do
		local ref = getRef(objectInfo)
		ref.openCount = ref.openCount + 1
		if self.hiddenObjects[key] ~= nil then
			ref.hiddenCount = ref.hiddenCount + 1
		end
		if objectInfo.cache == true then
			ref.cacheCount = ref.cacheCount + 1
		end
		table.insert(ref.objects, objectInfo)
	end

	for _, entry in ipairs(self.uiStack) do
		local objectInfo = entry ~= nil and self.objects[entry.key] or nil
		if objectInfo ~= nil then
			local ref = getRef(objectInfo)
			ref.stackCount = ref.stackCount + 1
			ref.stackedKeys[objectInfo.key] = true
		end
	end
	for _, entry in ipairs(self.popupStack) do
		local objectInfo = entry ~= nil and self.objects[entry.key] or nil
		if objectInfo ~= nil then
			local ref = getRef(objectInfo)
			ref.stackCount = ref.stackCount + 1
			ref.stackedKeys[objectInfo.key] = true
		end
	end
	return refsByPackage
end

function FairyGuiManager:DumpResourceRefs()
	print("[FGUI] DumpResourceRefs begin")
	local refsByPackage = self:GetPackageRefs()
	for packageKey, ref in pairs(refsByPackage) do
		print("[FGUI] ResourcePackage", packageKey, "path=", ref.packagePath, "refCount=", ref.refCount, "open=", ref.openCount, "hidden=", ref.hiddenCount, "cache=", ref.cacheCount, "stackRefs=", ref.stackCount)
		for _, objectInfo in ipairs(ref.objects) do
			print("[FGUI] ResourceUI", packageKey, "key=", objectInfo.key, "handle=", objectInfo.handle, "uiName=", objectInfo.uiName, "object=", objectInfo.objectName, "layer=", objectInfo.layer, "group=", objectInfo.uiGroup, "scene=", objectInfo.sceneName, "cache=", objectInfo.cache, "hidden=", self.hiddenObjects[objectInfo.key] ~= nil)
		end
	end
	print("[FGUI] DumpResourceRefs end")
end

function FairyGuiManager:GetResourceWarnings()
	local warnings = {}
	local refsByPackage = self:GetPackageRefs()
	for packageKey, ref in pairs(refsByPackage) do
		local isPackageRef = ref.packageName ~= nil or ref.packagePath ~= nil
		if isPackageRef and ref.openCount > 0 and ref.refCount < ref.openCount then
			table.insert(warnings, {
				kind = "refCountTooLow",
				packageKey = packageKey,
				detail = "refCount=" .. tostring(ref.refCount) .. " open=" .. tostring(ref.openCount),
			})
		end
		if isPackageRef and ref.refCount > ref.openCount and ref.openCount > 0 then
			table.insert(warnings, {
				kind = "refCountTooHigh",
				packageKey = packageKey,
				detail = "refCount=" .. tostring(ref.refCount) .. " open=" .. tostring(ref.openCount),
			})
		end
		if isPackageRef and ref.openCount <= 0 and ref.refCount > 0 then
			table.insert(warnings, {
				kind = "retainedWithoutOpen",
				packageKey = packageKey,
				detail = "refCount=" .. tostring(ref.refCount),
			})
		end
		if isPackageRef and ref.hiddenCount > 0 and ref.refCount <= 0 then
			table.insert(warnings, {
				kind = "hiddenWithoutRef",
				packageKey = packageKey,
				detail = "hidden=" .. tostring(ref.hiddenCount),
			})
		end
		for _, objectInfo in ipairs(ref.objects) do
			if self.hiddenObjects[objectInfo.key] == nil and ref.stackedKeys[objectInfo.key] ~= true and self:GetStackMode(objectInfo) ~= "None" then
				table.insert(warnings, {
					kind = "visibleWithoutStack",
					packageKey = packageKey,
					detail = "key=" .. tostring(objectInfo.key),
				})
			end
		end
	end
	return warnings
end

function FairyGuiManager:DumpResourceWarnings()
	local warnings = self:GetResourceWarnings()
	print("[FGUI] DumpResourceWarnings count=", #warnings)
	for _, warning in ipairs(warnings) do
		print("[FGUI] ResourceWarning", warning.kind, "package=", warning.packageKey, warning.detail or "")
	end
	return #warnings == 0, warnings
end

function FairyGuiManager:DumpBindings()
	local count = 0
	for _, _ in pairs(self.bindings) do
		count = count + 1
	end
	print("[FGUI] DumpBindings count=", count)
	for bindingId, binding in pairs(self.bindings) do
		print("[FGUI] Binding", bindingId, "handle=", binding.handle, "child=", binding.childPath, "eventType=", binding.eventType)
	end
end

function FairyGuiManager:DumpStacks()
	print("[FGUI] DumpStacks ui count=", #self.uiStack)
	for index, entry in ipairs(self.uiStack) do
		local objectInfo = self.objects[entry.key]
		print("[FGUI] UIStack", index, "key=", entry.key, "layer=", entry.layer, "mode=", entry.mode, "group=", entry.popupGroup, "popupMode=", entry.popupMode, "handle=", entry.handle, "hidden=", objectInfo ~= nil and self.hiddenObjects[entry.key] ~= nil)
	end
	print("[FGUI] DumpStacks popup count=", #self.popupStack)
	for index, entry in ipairs(self.popupStack) do
		local objectInfo = self.objects[entry.key]
		print("[FGUI] PopupStack", index, "key=", entry.key, "layer=", entry.layer, "mode=", entry.mode, "group=", entry.popupGroup, "popupMode=", entry.popupMode, "handle=", entry.handle, "hidden=", objectInfo ~= nil and self.hiddenObjects[entry.key] ~= nil)
	end
end

function FairyGuiManager:Dump()
	self:DumpHealth()
	self:DumpDebugStats()
	self:DumpOpenUIs()
	self:DumpPackages()
	self:DumpResourceRefs()
	self:DumpResourceWarnings()
	self:DumpBindings()
	self:DumpStacks()
	self:DumpLayerRoots()
	self:DumpScenes()
	self:DumpRenderStats()
end

function FairyGuiManager_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId, wheelDelta)
	if _G.FairyGuiManager == nil then
		return false
	end
	return _G.FairyGuiManager:_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId, wheelDelta)
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
