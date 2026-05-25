require("res.scripts.manager.fairygui.FairyGuiProfiler")
require("res.scripts.manager.fairygui.FairyGuiLifecycle")
require("res.scripts.manager.fairygui.FairyGuiPackage")
require("res.scripts.manager.fairygui.FairyGuiServices")
require("res.scripts.manager.fairygui.FairyGuiLayers")
require("res.scripts.manager.fairygui.FairyGuiEvents")
require("res.scripts.manager.fairygui.FairyGuiLists")
require("res.scripts.manager.fairygui.FairyGuiControls")
require("res.scripts.manager.fairygui.FairyGuiProbes")

local NativeApi = require("res.scripts.manager.fairygui.FairyGuiNativeApi")

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

local function isEnvEnabled(name)
	local value = os.getenv and os.getenv(name) or nil
	return value == "1" or value == "true" or value == "TRUE" or value == "True"
end

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
	self.lifecycle = ClassList.FairyGuiLifecycle.new(self)
	self.profiler = ClassList.FairyGuiProfiler.new(self)
	self.packageManager = ClassList.FairyGuiPackage.new(self)
	self.services = ClassList.FairyGuiServices.new(self)
	self.layersManager = ClassList.FairyGuiLayers.new(self)
	self.events = ClassList.FairyGuiEvents.new(self)
	self.lists = ClassList.FairyGuiLists.new(self)
	self.controls = ClassList.FairyGuiControls.new(self)
	self.probes = ClassList.FairyGuiProbes.new(self)
end

function FairyGuiManager:GetLifecycle()
	if self.lifecycle == nil then
		self.lifecycle = ClassList.FairyGuiLifecycle.new(self)
	end
	return self.lifecycle
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

function FairyGuiManager:GetControls()
	if self.controls == nil then
		self.controls = ClassList.FairyGuiControls.new(self)
	end
	return self.controls
end

function FairyGuiManager:GetProbes()
	if self.probes == nil then
		self.probes = ClassList.FairyGuiProbes.new(self)
	end
	return self.probes
end

function FairyGuiManager:IsAvailable()
	return NativeApi:Call("isFairyGuiAvailable", false) == true
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

function FairyGuiManager:LoadPackage(packagePath, packageName, meta)
	return self:GetPackageManager():LoadPackage(packagePath, packageName, meta)
end

function FairyGuiManager:RetainPackage(packagePath, packageName)
	return self:GetPackageManager():RetainPackage(packagePath, packageName)
end

function FairyGuiManager:ReleasePackage(packagePath, packageName, unloadWhenZero)
	return self:GetPackageManager():ReleasePackage(packagePath, packageName, unloadWhenZero)
end

function FairyGuiManager:PreloadPackage(packagePath, packageName, meta)
	return self:GetPackageManager():PreloadPackage(packagePath, packageName, meta)
end

function FairyGuiManager:PreloadScene(sceneName, options)
	return self:GetPackageManager():PreloadScene(sceneName, options)
end

function FairyGuiManager:PreloadScenePackages(sceneName, options)
	return self:PreloadScene(sceneName, options)
end

function FairyGuiManager:UnloadPackagesByFilter(filter)
	return self:GetPackageManager():UnloadPackagesByFilter(filter)
end

function FairyGuiManager:UnloadPackageGroup(groupName, force)
	return self:GetPackageManager():UnloadPackageGroup(groupName, force)
end

function FairyGuiManager:UnloadPackageTag(tagName, force)
	return self:GetPackageManager():UnloadPackageTag(tagName, force)
end

function FairyGuiManager:CreateObject(packageName, objectName)
	return self:GetPackageManager():CreateObject(packageName, objectName)
end

function FairyGuiManager:CreateContainer(name, ownerHandle)
	if not self:IsAvailable() then
		return 0
	end
	if ownerHandle ~= nil and NativeApi.createFairyGuiChildContainer ~= nil then
		return NativeApi:createFairyGuiChildContainer(ownerHandle, name or "")
	end
	if NativeApi.createFairyGuiContainer == nil then
		return 0
	end
	return NativeApi:createFairyGuiContainer(name or "")
end

function FairyGuiManager:CreateLoader(ownerHandle, name, url)
	if not self:IsAvailable() or NativeApi.createFairyGuiLoader == nil then
		return 0
	end
	return NativeApi:createFairyGuiLoader(ownerHandle or 0, name or "", url or "")
end

function FairyGuiManager:CreateText(ownerHandle, name, text, fontSize, red, green, blue)
	if not self:IsAvailable() or NativeApi.createFairyGuiText == nil then
		return 0
	end
	return NativeApi:createFairyGuiText(ownerHandle or 0, name or "", text or "", fontSize or 18, red or 255, green or 255, blue or 255)
end

function FairyGuiManager:CreateTextInput(ownerHandle, name, text, fontSize, red, green, blue)
	if not self:IsAvailable() or NativeApi.createFairyGuiTextInput == nil then
		return 0
	end
	return NativeApi:createFairyGuiTextInput(ownerHandle or 0, name or "", text or "", fontSize or 18, red or 255, green or 255, blue or 255)
end

function FairyGuiManager:CreateGraphRect(ownerHandle, name, width, height, red, green, blue, alpha)
	if not self:IsAvailable() or NativeApi.createFairyGuiGraphRect == nil then
		return 0
	end
	return NativeApi:createFairyGuiGraphRect(ownerHandle or 0, name or "", width or 1, height or 1, red or 1, green or 1, blue or 1, alpha or 1)
end

function FairyGuiManager:CreateGraphRegularPolygon(ownerHandle, name, width, height, sides, red, green, blue, alpha)
	if not self:IsAvailable() or NativeApi.createFairyGuiGraphRegularPolygon == nil then
		return 0
	end
	return NativeApi:createFairyGuiGraphRegularPolygon(ownerHandle or 0, name or "", width or 1, height or 1, sides or 6, red or 1, green or 1, blue or 1, alpha or 1)
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
	return self:GetLifecycle():CollectOwnedHandles(objectInfo)
end

function FairyGuiManager:CreateCloseSnapshot(objectInfo, ownedHandles)
	return self:GetLifecycle():CreateCloseSnapshot(objectInfo, ownedHandles)
end

function FairyGuiManager:CaptureCloseSnapshot(keyOrHandle)
	return self:GetLifecycle():CaptureCloseSnapshot(keyOrHandle)
end

function FairyGuiManager:GetCloseResidue(objectInfo, ownedHandles)
	return self:GetLifecycle():GetCloseResidue(objectInfo, ownedHandles)
end

function FairyGuiManager:ValidateClosedObject(objectInfo, ownedHandles, label, printWhenClean)
	return self:GetLifecycle():ValidateClosedObject(objectInfo, ownedHandles, label, printWhenClean)
end

function FairyGuiManager:ClearFocusForHandles(ownedHandles)
	return self:GetLifecycle():ClearFocusForHandles(ownedHandles)
end

function FairyGuiManager:AddTimer(keyOrHandle, duration, interval, tickFunc, finishFunc)
	return self:GetLifecycle():AddTimer(keyOrHandle, duration, interval, tickFunc, finishFunc)
end

function FairyGuiManager:Delay(keyOrHandle, timeout, func)
	return self:GetLifecycle():Delay(keyOrHandle, timeout, func)
end

function FairyGuiManager:RemoveTimerRecord(timerId)
	return self:GetLifecycle():RemoveTimerRecord(timerId)
end

function FairyGuiManager:CancelTimer(timerId)
	return self:GetLifecycle():CancelTimer(timerId)
end

function FairyGuiManager:ClearTimersForKey(key)
	return self:GetLifecycle():ClearTimersForKey(key)
end

function FairyGuiManager:GetTimerCountForKey(key)
	return self:GetLifecycle():GetTimerCountForKey(key)
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
			"parentHandle",
			"rootHandle",
			"parentKey",
			"rootLayer",
			"focusOrder",
			"tabFocus",
			"group",
			"uiGroup",
			"packageGroup",
			"tag",
			"tags",
			"packageTag",
			"packageTags",
			"preload",
			"preloadScene",
			"preloadScenes",
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
			"useSafeArea",
			"safeArea",
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

function FairyGuiManager:GetObjectKey(keyOrHandle)
	local objectInfo = self:GetObjectInfo(keyOrHandle)
	return objectInfo ~= nil and objectInfo.key or nil
end

function FairyGuiManager:DetachChildUI(childKeyOrHandle)
	local childInfo = self:GetObjectInfo(childKeyOrHandle)
	local childKey = childInfo ~= nil and childInfo.key or childKeyOrHandle
	if childKey == nil then
		return false
	end

	local parentKey = self.parentKeyByChildKey[childKey]
	if parentKey == nil and childInfo ~= nil then
		parentKey = childInfo.parentKey
	end
	if parentKey ~= nil then
		local childMap = self.childKeysByParentKey[parentKey]
		if childMap ~= nil then
			childMap[childKey] = nil
			if tableCount(childMap) <= 0 then
				self.childKeysByParentKey[parentKey] = nil
			end
		end
	end
	self.parentKeyByChildKey[childKey] = nil
	if childInfo ~= nil then
		childInfo.parentKey = nil
	end
	return parentKey ~= nil
end

function FairyGuiManager:AttachChildUI(parentKeyOrHandle, childKeyOrHandle, options)
	local parentInfo = self:GetObjectInfo(parentKeyOrHandle)
	local childInfo = self:GetObjectInfo(childKeyOrHandle)
	if parentInfo == nil or childInfo == nil or parentInfo.key == childInfo.key then
		return false
	end

	self:DetachChildUI(childInfo.key)
	local childMap = self.childKeysByParentKey[parentInfo.key]
	if childMap == nil then
		childMap = {}
		self.childKeysByParentKey[parentInfo.key] = childMap
	end
	childMap[childInfo.key] = true
	self.parentKeyByChildKey[childInfo.key] = parentInfo.key
	childInfo.parentKey = parentInfo.key
	childInfo.parentHandle = options ~= nil and options.parentHandle or childInfo.parentHandle or parentInfo.handle
	childInfo.parentChildPath = options ~= nil and options.parentChildPath or childInfo.parentChildPath
	childInfo.closeWithParent = options == nil or options.closeWithParent ~= false
	return true
end

function FairyGuiManager:GetChildUIKeys(parentKeyOrHandle)
	local parentInfo = self:GetObjectInfo(parentKeyOrHandle)
	if parentInfo == nil then
		return {}
	end

	local result = {}
	local childMap = self.childKeysByParentKey[parentInfo.key]
	if childMap ~= nil then
		for childKey, _ in pairs(childMap) do
			table.insert(result, childKey)
		end
	end
	table.sort(result)
	return result
end

function FairyGuiManager:CloseChildUIs(parentKeyOrHandle, forceDestroy, reason)
	local parentInfo = self:GetObjectInfo(parentKeyOrHandle)
	if parentInfo == nil then
		return 0
	end

	local childKeys = self:GetChildUIKeys(parentInfo.key)
	local closedCount = 0
	for _, childKey in ipairs(childKeys) do
		local childInfo = self.objects[childKey]
		if childInfo ~= nil and childInfo.closeWithParent ~= false then
			if self:CloseUI(childKey, forceDestroy, reason or "parentClose") then
				closedCount = closedCount + 1
			end
		else
			self:DetachChildUI(childKey)
		end
	end
	return closedCount
end

function FairyGuiManager:OpenChild(parentKeyOrHandle, name, param)
	local parentInfo = self:GetObjectInfo(parentKeyOrHandle)
	if parentInfo == nil then
		print("[FGUI] open child failed, parent missing:", tostring(parentKeyOrHandle), tostring(name))
		return nil
	end

	param = copyTable(param)
	local attachHandle = parentInfo.handle
	local parentChildPath = param.parentChildPath or param.childPath
	if not isBlank(parentChildPath) then
		local childHandle = self:GetTargetHandle(parentInfo.handle, parentChildPath)
		if childHandle ~= nil then
			attachHandle = childHandle
		end
	end
	param.parentKey = parentInfo.key
	param.parentHandle = attachHandle
	param.layer = param.layer or parentInfo.layer
	param.scene = param.scene or parentInfo.sceneName
	param.group = param.group or parentInfo.uiGroup
	param.stackMode = param.stackMode or "None"
	param.key = param.key or (parentInfo.key .. "." .. tostring(name))

	local result = self:Open(name, param)
	local childInfo = self:GetObjectInfo(param.key)
	if childInfo ~= nil then
		self:AttachChildUI(parentInfo.key, childInfo.key, {
			parentHandle = attachHandle,
			parentChildPath = parentChildPath,
			closeWithParent = param.closeWithParent,
		})
	end
	return result
end

function FairyGuiManager:GetLayerBaseOrder(layerName)
	return self:GetLayers():GetLayerBaseOrder(layerName)
end

function FairyGuiManager:GetLayerPolicy(layerName)
	return self:GetLayers():GetLayerPolicy(layerName)
end

function FairyGuiManager:SetLayerPolicy(layerName, policy)
	return self:GetLayers():SetLayerPolicy(layerName, policy)
end

function FairyGuiManager:GetSafeArea()
	return self:GetLayers():GetSafeArea()
end

function FairyGuiManager:SetSafeArea(leftOrRect, top, right, bottom)
	return self:GetLayers():SetSafeArea(leftOrRect, top, right, bottom)
end

function FairyGuiManager:GetLayerRoot(layerName)
	return self:GetLayers():GetLayerRoot(layerName)
end

function FairyGuiManager:EnsureLayerRoot(layerName)
	return self:GetLayers():EnsureLayerRoot(layerName)
end

function FairyGuiManager:AttachToLayer(handle, layerName, param)
	return self:GetLayers():AttachToLayer(handle, layerName, param)
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
	local packageName = self:LoadPackage(resolvedPackagePath, param.packageName, {
		uiName = param.uiName,
		group = param.group or param.uiGroup,
		packageGroup = param.packageGroup,
		tag = param.tag,
		tags = param.tags,
		packageTag = param.packageTag,
		packageTags = param.packageTags,
		scene = param.scene or param.sceneName,
	})
	if isBlank(packageName) then
		return nil
	end

	local handle = self:CreateObject(packageName, objectName)
	if handle == nil or handle <= 0 then
		return nil
	end

	if not self:AttachToLayer(handle, param.layer or "Normal", param) then
		NativeApi:removeFairyGuiObject(handle)
		return nil
	end

	if param.width ~= nil and param.height ~= nil then
		NativeApi:setFairyGuiObjectSize(handle, param.width, param.height)
	end

	if param.x ~= nil and param.y ~= nil then
		NativeApi:setFairyGuiObjectPosition(handle, param.x, param.y)
	elseif param.center then
		NativeApi:centerFairyGuiObject(handle, param.restraint == true)
	end

	if param.visible ~= nil then
		NativeApi:setFairyGuiObjectVisible(handle, param.visible == true)
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
		packageGroup = param.packageGroup or param.group or param.uiGroup,
		packageTags = param.packageTags or param.tags or param.packageTag or param.tag,
		popupGroup = self:GetPopupGroup(param),
		popupMode = param.popupMode or "stack",
		uiGroup = self:GetUIGroup(param),
		sceneName = self:GetSceneName(param),
		closeOnSceneChange = param.closeOnSceneChange ~= false,
		destroyOnSceneChange = param.destroyOnSceneChange == true,
		parentKey = param.parentKey,
		parentHandle = param.parentHandle or param.rootHandle,
		closeWithParent = param.closeWithParent ~= false,
		rootLayer = param.rootLayer,
		focusOrder = param.focusOrder,
		tabFocus = param.tabFocus ~= false,
	}
	self.objects[key] = objectInfo
	self.objectsByHandle[handle] = objectInfo
	self.hiddenObjects[key] = nil
	objectInfo.cacheHiddenAtMs = nil
	self:AssignLayer(objectInfo, objectInfo.layer)
	self:CreateModalMask(objectInfo, param)
	self:ApplyScreenAdapt(objectInfo)
	self:PushStack(objectInfo)
	return handle
end

function FairyGuiManager:OpenMaskProbe(param)
	return self:GetProbes():OpenMaskProbe(param)
end

function FairyGuiManager:OpenTextInputProbe(param)
	return self:GetProbes():OpenTextInputProbe(param)
end
function FairyGuiManager:AddOwnedHandle(objectInfo, handle)
	return self:GetLifecycle():AddOwnedHandle(objectInfo, handle)
end

function FairyGuiManager:AddObjectHandleToParent(childHandle, parentHandle)
	return self:GetLifecycle():AddObjectHandleToParent(childHandle, parentHandle)
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

function FairyGuiManager:GetDebugPanelSnapshot(options)
	return self:GetProfiler():GetDebugPanelSnapshot(options)
end

function FairyGuiManager:GetAiDebugSnapshot(options)
	return self:GetProfiler():GetAiDebugSnapshot(options)
end

function FairyGuiManager:BuildDebugPanelLines(options)
	return self:GetProfiler():BuildDebugPanelLines(options)
end

function FairyGuiManager:BuildAiDebugPanelLines(options)
	return self:GetProfiler():BuildAiDebugPanelLines(options)
end

function FairyGuiManager:RefreshDebugPanel(key)
	return self:GetProfiler():RefreshDebugPanel(key)
end

function FairyGuiManager:RefreshAiDebugPanel(key)
	return self:GetProfiler():RefreshAiDebugPanel(key)
end

function FairyGuiManager:ShowDebugPanel(param)
	return self:GetProfiler():ShowDebugPanel(param)
end

function FairyGuiManager:ShowAiDebugPanel(param)
	return self:GetProfiler():ShowAiDebugPanel(param)
end

function FairyGuiManager:HideDebugPanel(key)
	return self:GetProfiler():HideDebugPanel(key)
end

function FairyGuiManager:HideAiDebugPanel(key)
	return self:GetProfiler():HideAiDebugPanel(key)
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
		objectInfo.cacheHiddenAtMs = nil
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
		objectInfo.cacheHiddenAtMs = nil
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

function FairyGuiManager:RenderListItemByHandle(listHandle, index, itemHandle)
	return self:GetLists():RenderListItemByHandle(listHandle, index, itemHandle)
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

function FairyGuiManager:SetVirtualListData(handle, childPath, dataList, renderer, options)
	return self:GetLists():SetVirtualListData(handle, childPath, dataList, renderer, options)
end

function FairyGuiManager:SetTreeData(handle, childPath, treeData, renderer, options)
	return self:GetLists():SetTreeData(handle, childPath, treeData, renderer, options)
end

function FairyGuiManager:GetTreeFlatData(handle, childPath)
	return self:GetLists():GetTreeFlatData(handle, childPath)
end

function FairyGuiManager:GetTreeNode(handle, childPath, nodeKey)
	return self:GetLists():GetTreeNode(handle, childPath, nodeKey)
end

function FairyGuiManager:AddTreeNode(handle, childPath, parentKey, node, index)
	return self:GetLists():AddTreeNode(handle, childPath, parentKey, node, index)
end

function FairyGuiManager:RemoveTreeNode(handle, childPath, nodeKey)
	return self:GetLists():RemoveTreeNode(handle, childPath, nodeKey)
end

function FairyGuiManager:ClearTree(handle, childPath)
	return self:GetLists():ClearTree(handle, childPath)
end

function FairyGuiManager:UpdateTreeNode(handle, childPath, nodeKey, data)
	return self:GetLists():UpdateTreeNode(handle, childPath, nodeKey, data)
end

function FairyGuiManager:SetTreeNodeExpanded(handle, childPath, nodeKey, expanded)
	return self:GetLists():SetTreeNodeExpanded(handle, childPath, nodeKey, expanded)
end

function FairyGuiManager:SetTreeNodeSelected(handle, childPath, nodeKey)
	return self:GetLists():SetTreeNodeSelected(handle, childPath, nodeKey)
end

function FairyGuiManager:GetTreeSelectedKey(handle, childPath)
	return self:GetLists():GetTreeSelectedKey(handle, childPath)
end

function FairyGuiManager:GetTreeSelectedNode(handle, childPath)
	return self:GetLists():GetTreeSelectedNode(handle, childPath)
end

function FairyGuiManager:ToggleTreeNode(handle, childPath, nodeKey)
	return self:GetLists():ToggleTreeNode(handle, childPath, nodeKey)
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

function FairyGuiManager:GetListDebugStats(handle, childPath)
	return self:GetLists():GetListDebugStats(handle, childPath)
end

function FairyGuiManager:DumpListDebugStats(handle, childPath, label)
	return self:GetLists():DumpListDebugStats(handle, childPath, label)
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

function FairyGuiManager:DebugInjectImeCommitText(text)
	if NativeApi == nil or NativeApi.injectFairyGuiImeCommitText == nil then
		return false
	end
	return NativeApi:injectFairyGuiImeCommitText(text or "")
end

function FairyGuiManager:DebugInjectImeCompositionText(text)
	if NativeApi == nil or NativeApi.injectFairyGuiImeCompositionText == nil then
		return false
	end
	return NativeApi:injectFairyGuiImeCompositionText(text or "")
end

function FairyGuiManager:DebugClearImeComposition()
	if NativeApi == nil or NativeApi.clearFairyGuiImeComposition == nil then
		return false
	end
	return NativeApi:clearFairyGuiImeComposition()
end

function FairyGuiManager:GetImeDebugString()
	if NativeApi == nil or NativeApi.getFairyGuiImeDebugString == nil then
		return ""
	end
	return NativeApi:getFairyGuiImeDebugString() or ""
end

function FairyGuiManager:RecordResourceFallback(kind, context, detail)
	return self:GetPackageManager():RecordResourceFallback(kind, context, detail)
end

function FairyGuiManager:GetResourceFallbacks()
	return self:GetPackageManager():GetResourceFallbacks()
end

function FairyGuiManager:ClearResourceFallbacks()
	return self:GetPackageManager():ClearResourceFallbacks()
end

function FairyGuiManager:SetResourceFallbackPolicy(policy)
	return self:GetPackageManager():SetResourceFallbackPolicy(policy)
end

function FairyGuiManager:GetResourceFallbackPolicy()
	return self:GetPackageManager():GetResourceFallbackPolicy()
end

function FairyGuiManager:DumpResourceFallbacks(filter)
	return self:GetPackageManager():DumpResourceFallbacks(filter)
end

function FairyGuiManager:ApplyTextInputPolicy(handle, childPath)
	return self:GetControls():ApplyTextInputPolicy(handle, childPath)
end

function FairyGuiManager:SetTextInputPolicy(handle, childPath, policy)
	return self:GetControls():SetTextInputPolicy(handle, childPath, policy)
end

function FairyGuiManager:GetTextInputPolicy(handle, childPath)
	return self:GetControls():GetTextInputPolicy(handle, childPath)
end

function FairyGuiManager:ClearTextInputPoliciesForHandles(handles)
	return self:GetControls():ClearTextInputPoliciesForHandles(handles)
end

function FairyGuiManager:SetText(handle, childPath, text)
	return self:GetControls():SetText(handle, childPath, text)
end

function FairyGuiManager:GetText(handle, childPath)
	return self:GetControls():GetText(handle, childPath)
end

function FairyGuiManager:Focus(handle, childPath)
	return self:GetControls():Focus(handle, childPath)
end

function FairyGuiManager:ResolveFocusHandle(objectInfo, target)
	return self:GetControls():ResolveFocusHandle(objectInfo, target)
end

function FairyGuiManager:GetFocusHandles(objectInfo)
	return self:GetControls():GetFocusHandles(objectInfo)
end

function FairyGuiManager:RegisterFocusOrder(keyOrHandle, focusOrder)
	return self:GetControls():RegisterFocusOrder(keyOrHandle, focusOrder)
end

function FairyGuiManager:CollectFocusHandles()
	return self:GetControls():CollectFocusHandles()
end

function FairyGuiManager:FocusNext(reverse)
	return self:GetControls():FocusNext(reverse)
end

function FairyGuiManager:ClearFocus()
	return self:GetControls():ClearFocus()
end

function FairyGuiManager:GetFocusedHandle()
	return self:GetControls():GetFocusedHandle()
end

function FairyGuiManager:SetIcon(handle, childPath, icon)
	return self:GetControls():SetIcon(handle, childPath, icon)
end

function FairyGuiManager:SetLoaderUrl(handle, childPath, url)
	return self:GetControls():SetLoaderUrl(handle, childPath, url)
end

function FairyGuiManager:SetChildVisible(handle, childPath, visible)
	return self:GetControls():SetChildVisible(handle, childPath, visible)
end

function FairyGuiManager:SetChildPosition(handle, childPath, x, y)
	return self:GetControls():SetChildPosition(handle, childPath, x, y)
end

function FairyGuiManager:SetChildSize(handle, childPath, width, height)
	return self:GetControls():SetChildSize(handle, childPath, width, height)
end

function FairyGuiManager:SetControllerIndex(handle, controllerName, selectedIndex)
	return self:GetControls():SetControllerIndex(handle, controllerName, selectedIndex)
end

function FairyGuiManager:GetControllerIndex(handle, controllerName)
	return self:GetControls():GetControllerIndex(handle, controllerName)
end

function FairyGuiManager:SetControllerPage(handle, controllerName, pageName)
	return self:GetControls():SetControllerPage(handle, controllerName, pageName)
end

function FairyGuiManager:GetControllerPage(handle, controllerName)
	return self:GetControls():GetControllerPage(handle, controllerName)
end

function FairyGuiManager:GetControllerPageId(handle, controllerName)
	return self:GetControls():GetControllerPageId(handle, controllerName)
end

function FairyGuiManager:GetControllerPageCount(handle, controllerName)
	return self:GetControls():GetControllerPageCount(handle, controllerName)
end

function FairyGuiManager:GetControllerPageNameAt(handle, controllerName, pageIndex)
	return self:GetControls():GetControllerPageNameAt(handle, controllerName, pageIndex)
end

function FairyGuiManager:GetControllerPageIdAt(handle, controllerName, pageIndex)
	return self:GetControls():GetControllerPageIdAt(handle, controllerName, pageIndex)
end

function FairyGuiManager:SetValue(handle, childPathOrValue, value)
	return self:GetControls():SetValue(handle, childPathOrValue, value)
end

function FairyGuiManager:GetValue(handle, childPath)
	return self:GetControls():GetValue(handle, childPath)
end

function FairyGuiManager:SetMin(handle, childPathOrValue, minValue)
	return self:GetControls():SetMin(handle, childPathOrValue, minValue)
end

function FairyGuiManager:GetMin(handle, childPath)
	return self:GetControls():GetMin(handle, childPath)
end

function FairyGuiManager:SetMax(handle, childPathOrValue, maxValue)
	return self:GetControls():SetMax(handle, childPathOrValue, maxValue)
end

function FairyGuiManager:GetMax(handle, childPath)
	return self:GetControls():GetMax(handle, childPath)
end

function FairyGuiManager:SetProgress(handle, childPathOrValue, value, maxValue, minValue)
	return self:GetControls():SetProgress(handle, childPathOrValue, value, maxValue, minValue)
end

function FairyGuiManager:SetSliderValue(handle, childPathOrValue, value)
	return self:GetControls():SetSliderValue(handle, childPathOrValue, value)
end

function FairyGuiManager:GetSliderValue(handle, childPath)
	return self:GetControls():GetSliderValue(handle, childPath)
end

function FairyGuiManager:SetProgressBarValue(handle, childPathOrValue, value)
	return self:GetControls():SetProgressBarValue(handle, childPathOrValue, value)
end

function FairyGuiManager:GetProgressBarValue(handle, childPath)
	return self:GetControls():GetProgressBarValue(handle, childPath)
end

function FairyGuiManager:SetComboBoxSelectedIndex(handle, childPathOrSelectedIndex, selectedIndex)
	return self:GetControls():SetComboBoxSelectedIndex(handle, childPathOrSelectedIndex, selectedIndex)
end

function FairyGuiManager:GetComboBoxSelectedIndex(handle, childPath)
	return self:GetControls():GetComboBoxSelectedIndex(handle, childPath)
end

function FairyGuiManager:SetComboBoxValue(handle, childPathOrValue, value)
	return self:GetControls():SetComboBoxValue(handle, childPathOrValue, value)
end

function FairyGuiManager:GetComboBoxValue(handle, childPath)
	return self:GetControls():GetComboBoxValue(handle, childPath)
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

function FairyGuiManager:AddControllerChanged(handle, controllerName, callback)
	return self:GetEvents():AddControllerChanged(handle, controllerName, callback)
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
	return NativeApi:setFairyGuiObjectPosition(handle, x, y)
end

function FairyGuiManager:SetSize(handle, width, height)
	if handle == nil then
		return false
	end
	return NativeApi:setFairyGuiObjectSize(handle, width, height)
end

function FairyGuiManager:SetVisible(handle, visible)
	if handle == nil then
		return false
	end
	return NativeApi:setFairyGuiObjectVisible(handle, visible == true)
end

function FairyGuiManager:SetAlpha(handle, alpha)
	if handle == nil or NativeApi == nil or NativeApi.setFairyGuiObjectAlpha == nil then
		return false
	end
	return NativeApi:setFairyGuiObjectAlpha(handle, alpha or 1)
end

function FairyGuiManager:SetTouchable(handle, touchable)
	if handle == nil or NativeApi == nil or NativeApi.setFairyGuiObjectTouchable == nil then
		return false
	end
	return NativeApi:setFairyGuiObjectTouchable(handle, touchable == true)
end

function FairyGuiManager:SetMask(handle, maskHandle, inverted)
	if handle == nil or maskHandle == nil or NativeApi == nil or NativeApi.setFairyGuiObjectMask == nil then
		return false
	end
	return NativeApi:setFairyGuiObjectMask(handle, maskHandle, inverted == true)
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
	self:CloseChildUIs(objectInfo.key, forceDestroy, reason)

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
		objectInfo.cacheHiddenAtMs = nowMs()
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
		NativeApi:removeFairyGuiObject(objectInfo.modalMaskHandle)
	end
	local removed = NativeApi:removeFairyGuiObject(handle)
	if objectInfo.uiName ~= nil and self.uiNameToKey[objectInfo.uiName] == objectInfo.key then
		self.uiNameToKey[objectInfo.uiName] = nil
	end
	self:ClearListCacheForHandle(handle)
	for _, ownedHandle in ipairs(ownedHandles) do
		self:ClearListCacheByListHandle(ownedHandle)
		self.childrenByHandle[ownedHandle] = nil
	end
	self:ClearTextInputPoliciesForHandles(ownedHandles)
	if objectInfo.layer ~= nil and self.layerObjects[objectInfo.layer] ~= nil then
		self.layerObjects[objectInfo.layer][handle] = nil
	end
	self:DetachChildUI(objectInfo.key)
	self.childKeysByParentKey[objectInfo.key] = nil
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

function FairyGuiManager:DumpPackageRef(packageKey)
	return self:GetPackageManager():DumpPackageRef(packageKey)
end

function FairyGuiManager:GetPackageRefs()
	return self:GetPackageManager():GetPackageRefs()
end

function FairyGuiManager:DumpUI(keyOrHandle)
	local objectInfo = self:GetObjectInfo(keyOrHandle)
	if objectInfo == nil then
		print("[FGUI] DumpUI missing:", tostring(keyOrHandle))
		return false
	end
	local hidden = self.hiddenObjects[objectInfo.key] ~= nil
	local ownedHandles = self:CollectOwnedHandles(objectInfo)
	print("[FGUI] DumpUI",
		"key=", tostring(objectInfo.key),
		"handle=", tostring(objectInfo.handle),
		"uiName=", tostring(objectInfo.uiName or objectInfo.name or ""),
		"package=", tostring(objectInfo.packageName or objectInfo.packagePath or ""),
		"object=", tostring(objectInfo.objectName or ""),
		"layer=", tostring(objectInfo.layer or ""),
		"group=", tostring(objectInfo.uiGroup or objectInfo.popupGroup or ""),
		"scene=", tostring(objectInfo.sceneName or ""),
		"cache=", objectInfo.cache == true,
		"hidden=", hidden,
		"stackMode=", tostring(self:GetStackMode(objectInfo)),
		"owned=", #ownedHandles)
	local childHandles = self.childrenByHandle[objectInfo.handle] or {}
	for childPath, childHandle in pairs(childHandles) do
		print("[FGUI] DumpUIChild", tostring(objectInfo.key), "path=", tostring(childPath), "handle=", tostring(childHandle))
	end
	return true
end

function FairyGuiManager:DumpDebugTarget(target)
	if target == nil or target == "" or target == "top" then
		local top = self:GetTopUI()
		return self:DumpUI(top ~= nil and top.key or nil)
	end
	if target == "topPopup" then
		local topPopup = self:GetTopPopup()
		return self:DumpUI(topPopup ~= nil and topPopup.key or nil)
	end
	local objectInfo = self:GetObjectInfo(target)
	if objectInfo ~= nil then
		return self:DumpUI(objectInfo.key)
	end
	if self:DumpPackageRef(target) then
		return true
	end
	print("[FGUI] DumpDebugTarget missing:", tostring(target))
	return false
end

function FairyGuiManager:CloseDebugTarget(target, forceDestroy)
	if target == nil or target == "" or target == "top" then
		return self:CloseTop(nil, forceDestroy == true)
	end
	if target == "topPopup" then
		return self:CloseTopPopup(forceDestroy == true)
	end
	local objectInfo = self:GetObjectInfo(target)
	if objectInfo == nil then
		print("[FGUI] CloseDebugTarget missing:", tostring(target))
		return false
	end
	return self:Close(objectInfo.key, forceDestroy == true, "debugPanelCloseTarget")
end

function FairyGuiManager:DumpResourceRefs()
	return self:GetPackageManager():DumpResourceRefs()
end

function FairyGuiManager:GetResourceWarnings()
	return self:GetPackageManager():GetResourceWarnings()
end

function FairyGuiManager:SetCachePolicy(policy)
	if type(policy) ~= "table" then
		return self.cachePolicy
	end
	self.cachePolicy = self.cachePolicy or {}
	for name, value in pairs(policy) do
		self.cachePolicy[name] = value
	end
	return self.cachePolicy
end

function FairyGuiManager:GetCachePolicy()
	return copyTable(self.cachePolicy or {})
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
	self:DumpResourceFallbacks()
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

function FairyGuiManager_RenderVirtualListItem(listHandle, index, itemHandle)
	if _G.FairyGuiManager == nil then
		return false
	end
	return _G.FairyGuiManager:GetLists():RenderVirtualListItemByHandle(listHandle, index, itemHandle)
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
