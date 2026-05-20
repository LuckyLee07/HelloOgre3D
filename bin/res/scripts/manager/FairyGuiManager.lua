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

local EVENT_TYPES = {
	Changed = 2,
	TouchBegin = 10,
	TouchMove = 11,
	TouchEnd = 12,
	Click = 13,
	RightClick = 17,
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
	self.views = {}
	self.viewsByHandle = {}
	self.controllers = {}
	self.controllersByHandle = {}
	self.currentSceneName = "Default"
	self.layers = copyTable(DEFAULT_LAYER_ORDER)
		self.layerNextOrder = {}
		self.layerObjects = {}
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

	if GameManager.addFairyGuiObjectToRoot ~= nil then
		GameManager:addFairyGuiObjectToRoot(maskHandle)
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
	if GameManager ~= nil and GameManager.getScreenWidth ~= nil then
		return GameManager:getScreenWidth()
	end
	return 0
end

function FairyGuiManager:GetScreenHeight()
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

	if not GameManager:addFairyGuiObjectToRoot(handle) then
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

	self.listItemHandlesByHandle[listHandle] = nil
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
	for index, data in ipairs(dataList) do
		local item = self:GetListItemByHandle(listHandle, index, data)
		if item ~= nil and type(renderer) == "function" then
			renderer(item, data, index)
		elseif item ~= nil then
			item:SetText("", tostring(data))
		end
	end
	return true
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

function FairyGuiManager:DebugInjectClick(x, y, button)
	if GameManager == nil or GameManager.injectFairyGuiClick == nil then
		return false
	end
	return GameManager:injectFairyGuiClick(tonumber(x) or 0, tonumber(y) or 0, tonumber(button) or 0)
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

function FairyGuiManager:_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId)
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

	detachView(self, objectInfo)
	self:RemoveStackEntry(objectInfo.key)
	self:ClearBindingsForHandle(handle)
	if objectInfo.modalMaskHandle ~= nil then
		self:ClearBindingsForHandle(objectInfo.modalMaskHandle)
		GameManager:removeFairyGuiObject(objectInfo.modalMaskHandle)
	end
	local removed = GameManager:removeFairyGuiObject(handle)
		if objectInfo.uiName ~= nil and self.uiNameToKey[objectInfo.uiName] == objectInfo.key then
			self.uiNameToKey[objectInfo.uiName] = nil
		end
		self:ClearListCacheForHandle(handle)
		self.childrenByHandle[handle] = nil
		if objectInfo.layer ~= nil and self.layerObjects[objectInfo.layer] ~= nil then
			self.layerObjects[objectInfo.layer][handle] = nil
		end
		self.objects[objectInfo.key] = nil
		self.objectsByHandle[handle] = nil
		self.hiddenObjects[objectInfo.key] = nil
		self:ReleasePackage(objectInfo.packagePath, objectInfo.packageName, objectInfo.unloadPackageOnClose == true)
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
		print("[FGUI] UI", key, "handle=", objectInfo.handle, "layer=", objectInfo.layer, "group=", objectInfo.uiGroup, "popupGroup=", objectInfo.popupGroup, "scene=", objectInfo.sceneName, "cache=", objectInfo.cache, "hidden=", self.hiddenObjects[key] ~= nil)
	end
	print("[FGUI] DumpOpenUIs end")
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
	self:DumpOpenUIs()
	self:DumpPackages()
	self:DumpBindings()
	self:DumpStacks()
	self:DumpScenes()
end

function FairyGuiManager_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId)
	if _G.FairyGuiManager == nil then
		return false
	end
	return _G.FairyGuiManager:_DispatchEvent(callbackId, rootHandle, eventType, bindingId, senderHandle, itemHandle, rawItemIndex, x, y, button, touchId)
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
