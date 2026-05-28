local FairyGuiStore = Class("FairyGuiStore")

local function isBlank(value)
	return value == nil or value == ""
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

function FairyGuiStore:Init()
	self.objectState = {
		objects = {},
		objectsByHandle = {},
		uiRegistry = {},
		uiNameToKey = {},
		hiddenObjects = {},
		views = {},
		viewsByHandle = {},
		controllers = {},
		controllersByHandle = {},
		childKeysByParentKey = {},
		parentKeyByChildKey = {},
	}

	self.layerState = {
		currentSceneName = "Default",
		designWidth = nil,
		designHeight = nil,
		scaleMode = "stretch",
		layers = nil,
		layerPolicies = {},
		layerNextOrder = {},
		layerObjects = {},
		layerRoots = {},
		safeArea = nil,
		uiStack = {},
		popupStack = {},
		stackEntriesByKey = {},
		nextStackSerial = 0,
	}

	self.eventState = {
		callbacks = {},
		bindings = {},
		bindingsByHandle = {},
		transitionCallbacks = {},
		transitionCallbacksByHandle = {},
		timers = {},
		timersByKey = {},
		eventStats = {},
		eventDispatchTotal = 0,
		lastEvent = nil,
		nextCallbackId = 1,
	}

	self.listState = {
		childrenByHandle = {},
		listItemHandlesByHandle = {},
		listItemIndexByHandle = {},
		listDataByHandle = {},
		listRenderersByHandle = {},
		listVirtualByHandle = {},
		listVirtualOptionsByHandle = {},
		listVirtualStatsByHandle = {},
		treeDataByHandle = {},
		treeStateByHandle = {},
		treeRenderersByHandle = {},
		treeChildPathByHandle = {},
	}

	self.packageState = {
		packageRoot = "res/fuires",
		packages = {},
		packagesByName = {},
		resourceFallbacks = {},
		resourceFallbackKeys = {},
		resourceFallbackMaxCount = 128,
		resourceFallbackPolicy = {
			recordMissingChild = false,
		},
		cachePolicy = {
			maxHiddenPerPackage = 2,
			maxHiddenTotal = 6,
			warnHiddenMs = 30000,
		},
	}

	self.controlsState = {
		textInputPoliciesByHandle = {},
		textInputPolicyBindingsByHandle = {},
	}

	self.serviceState = {
		serviceSkins = nil,
		serviceStats = nil,
		toastQueue = {},
		toastActive = nil,
		toastSerial = 0,
		toastDedupe = {},
		loadingRefs = {},
		loadingRefTotal = 0,
	}

	self.profilerState = {
		perfStats = {},
	}
end

function FairyGuiStore:GetObjectState()
	return self.objectState
end

function FairyGuiStore:GetLayerState()
	return self.layerState
end

function FairyGuiStore:GetEventState()
	return self.eventState
end

function FairyGuiStore:GetListState()
	return self.listState
end

function FairyGuiStore:GetPackageState()
	return self.packageState
end

function FairyGuiStore:GetControlsState()
	return self.controlsState
end

function FairyGuiStore:GetServiceState()
	return self.serviceState
end

function FairyGuiStore:GetProfilerState()
	return self.profilerState
end

function FairyGuiStore:RegisterUI(name, config)
	if isBlank(name) or type(config) ~= "table" then
		return false
	end

	self.objectState.uiRegistry[name] = config
	config.name = config.name or name
	return true
end

function FairyGuiStore:GetUIConfig(name)
	if isBlank(name) then
		return nil
	end
	return self.objectState.uiRegistry[name]
end

function FairyGuiStore:BindUINameToKey(uiName, key)
	if isBlank(uiName) or key == nil then
		return false
	end

	self.objectState.uiNameToKey[uiName] = key
	return true
end

function FairyGuiStore:ClearUINameToKey(uiName, key)
	if isBlank(uiName) then
		return false
	end

	if key == nil or self.objectState.uiNameToKey[uiName] == key then
		self.objectState.uiNameToKey[uiName] = nil
		return true
	end
	return false
end

function FairyGuiStore:GetRegisteredUIKey(name)
	if self.objectState.uiNameToKey[name] ~= nil then
		return self.objectState.uiNameToKey[name]
	end

	local config = self:GetUIConfig(name)
	if config == nil then
		return name
	end
	return config.key or name
end

function FairyGuiStore:GetObjectInfo(keyOrHandle)
	if type(keyOrHandle) == "number" then
		return self.objectState.objectsByHandle[keyOrHandle]
	end

	local key = self:GetRegisteredUIKey(keyOrHandle)
	return self.objectState.objects[key] or self.objectState.objects[keyOrHandle]
end

function FairyGuiStore:GetObjectKey(keyOrHandle)
	local objectInfo = self:GetObjectInfo(keyOrHandle)
	return objectInfo ~= nil and objectInfo.key or nil
end

function FairyGuiStore:GetObjects()
	return self.objectState.objects
end

function FairyGuiStore:GetObjectsByHandle()
	return self.objectState.objectsByHandle
end

function FairyGuiStore:GetUIRegistry()
	return self.objectState.uiRegistry
end

function FairyGuiStore:GetHiddenObjects()
	return self.objectState.hiddenObjects
end

function FairyGuiStore:PutObject(objectInfo)
	if objectInfo == nil or objectInfo.key == nil then
		return false
	end

	self.objectState.objects[objectInfo.key] = objectInfo
	if objectInfo.handle ~= nil then
		self.objectState.objectsByHandle[objectInfo.handle] = objectInfo
	end
	if objectInfo.uiName ~= nil then
		self:BindUINameToKey(objectInfo.uiName, objectInfo.key)
	end
	self.objectState.hiddenObjects[objectInfo.key] = nil
	return true
end

function FairyGuiStore:RemoveObject(objectInfo)
	if objectInfo == nil or objectInfo.key == nil then
		return false
	end

	if objectInfo.uiName ~= nil then
		self:ClearUINameToKey(objectInfo.uiName, objectInfo.key)
	end
	self.objectState.objects[objectInfo.key] = nil
	if objectInfo.handle ~= nil then
		self.objectState.objectsByHandle[objectInfo.handle] = nil
	end
	self.objectState.hiddenObjects[objectInfo.key] = nil
	return true
end

function FairyGuiStore:HideObject(objectInfo)
	if objectInfo == nil or objectInfo.key == nil then
		return false
	end

	self.objectState.hiddenObjects[objectInfo.key] = objectInfo
	return true
end

function FairyGuiStore:ShowObject(objectInfo)
	if objectInfo == nil or objectInfo.key == nil then
		return false
	end

	self.objectState.hiddenObjects[objectInfo.key] = nil
	return true
end

function FairyGuiStore:IsHidden(keyOrObjectInfo)
	local key = type(keyOrObjectInfo) == "table" and keyOrObjectInfo.key or keyOrObjectInfo
	return key ~= nil and self.objectState.hiddenObjects[key] ~= nil
end

function FairyGuiStore:DetachChild(childKeyOrHandle)
	local childInfo = self:GetObjectInfo(childKeyOrHandle)
	local childKey = childInfo ~= nil and childInfo.key or childKeyOrHandle
	if childKey == nil then
		return false
	end

	local parentKey = self.objectState.parentKeyByChildKey[childKey]
	if parentKey == nil and childInfo ~= nil then
		parentKey = childInfo.parentKey
	end
	if parentKey ~= nil then
		local childMap = self.objectState.childKeysByParentKey[parentKey]
		if childMap ~= nil then
			childMap[childKey] = nil
			if tableCount(childMap) <= 0 then
				self.objectState.childKeysByParentKey[parentKey] = nil
			end
		end
	end
	self.objectState.parentKeyByChildKey[childKey] = nil
	if childInfo ~= nil then
		childInfo.parentKey = nil
	end
	return parentKey ~= nil
end

function FairyGuiStore:AttachChild(parentKeyOrHandle, childKeyOrHandle, options)
	local parentInfo = self:GetObjectInfo(parentKeyOrHandle)
	local childInfo = self:GetObjectInfo(childKeyOrHandle)
	if parentInfo == nil or childInfo == nil or parentInfo.key == childInfo.key then
		return false
	end

	self:DetachChild(childInfo.key)
	local childMap = self.objectState.childKeysByParentKey[parentInfo.key]
	if childMap == nil then
		childMap = {}
		self.objectState.childKeysByParentKey[parentInfo.key] = childMap
	end
	childMap[childInfo.key] = true
	self.objectState.parentKeyByChildKey[childInfo.key] = parentInfo.key
	childInfo.parentKey = parentInfo.key
	childInfo.parentHandle = options ~= nil and options.parentHandle or childInfo.parentHandle or parentInfo.handle
	childInfo.parentChildPath = options ~= nil and options.parentChildPath or childInfo.parentChildPath
	childInfo.closeWithParent = options == nil or options.closeWithParent ~= false
	return true
end

function FairyGuiStore:GetChildKeys(parentKeyOrHandle)
	local parentInfo = self:GetObjectInfo(parentKeyOrHandle)
	if parentInfo == nil then
		return {}
	end

	local result = {}
	local childMap = self.objectState.childKeysByParentKey[parentInfo.key]
	if childMap ~= nil then
		for childKey, _ in pairs(childMap) do
			table.insert(result, childKey)
		end
	end
	table.sort(result)
	return result
end

function FairyGuiStore:ClearChildren(parentKeyOrHandle)
	local parentInfo = self:GetObjectInfo(parentKeyOrHandle)
	local parentKey = parentInfo ~= nil and parentInfo.key or parentKeyOrHandle
	if parentKey == nil then
		return false
	end

	local childMap = self.objectState.childKeysByParentKey[parentKey]
	if childMap ~= nil then
		for childKey, _ in pairs(childMap) do
			self.objectState.parentKeyByChildKey[childKey] = nil
			local childInfo = self:GetObjectInfo(childKey)
			if childInfo ~= nil then
				childInfo.parentKey = nil
			end
		end
	end
	self.objectState.childKeysByParentKey[parentKey] = nil
	return true
end

function FairyGuiStore:PutView(key, handle, view)
	if key ~= nil then
		self.objectState.views[key] = view
	end
	if handle ~= nil then
		self.objectState.viewsByHandle[handle] = view
	end
	return true
end

function FairyGuiStore:RemoveView(key, handle)
	if key ~= nil then
		self.objectState.views[key] = nil
	end
	if handle ~= nil then
		self.objectState.viewsByHandle[handle] = nil
	end
	return true
end

function FairyGuiStore:GetView(keyOrHandle)
	if type(keyOrHandle) == "number" then
		return self.objectState.viewsByHandle[keyOrHandle]
	end

	local key = self:GetRegisteredUIKey(keyOrHandle)
	if self.objectState.views[key] ~= nil then
		return self.objectState.views[key]
	end
	return self.objectState.views[keyOrHandle]
end

function FairyGuiStore:PutController(key, handle, controller)
	if key ~= nil then
		self.objectState.controllers[key] = controller
	end
	if handle ~= nil then
		self.objectState.controllersByHandle[handle] = controller
	end
	return true
end

function FairyGuiStore:RemoveController(key, handle)
	if key ~= nil then
		self.objectState.controllers[key] = nil
	end
	if handle ~= nil then
		self.objectState.controllersByHandle[handle] = nil
	end
	return true
end

function FairyGuiStore:GetController(keyOrHandle)
	if type(keyOrHandle) == "number" then
		return self.objectState.controllersByHandle[keyOrHandle]
	end

	local key = self:GetRegisteredUIKey(keyOrHandle)
	if self.objectState.controllers[key] ~= nil then
		return self.objectState.controllers[key]
	end
	return self.objectState.controllers[keyOrHandle]
end

function FairyGuiStore:GetUIStack()
	return self.layerState.uiStack
end

function FairyGuiStore:GetPopupStack()
	return self.layerState.popupStack
end

function FairyGuiStore:GetLayerRoots()
	return self.layerState.layerRoots
end

function FairyGuiStore:GetLayers()
	return self.layerState.layers
end

function FairyGuiStore:GetCurrentScene()
	return self.layerState.currentSceneName or "Default"
end

function FairyGuiStore:SetCurrentScene(sceneName)
	self.layerState.currentSceneName = sceneName or "Default"
	return self.layerState.currentSceneName
end

function FairyGuiStore:RemoveLayerObject(layerName, handle)
	if layerName == nil or handle == nil then
		return false
	end

	local layerObjects = self.layerState.layerObjects[layerName]
	if layerObjects == nil then
		return false
	end
	layerObjects[handle] = nil
	return true
end

return FairyGuiStore
