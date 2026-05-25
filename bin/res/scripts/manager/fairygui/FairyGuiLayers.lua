local NativeApi = require("res.scripts.manager.fairygui.FairyGuiNativeApi")

local FairyGuiLayers = Class("FairyGuiLayers")

local DEFAULT_LAYER_ORDER = {
	Background = 1000,
	Normal = 2000,
	Popup = 3000,
	Guide = 4000,
	Top = 5000,
	Toast = 6000,
}

local DEFAULT_LAYER_POLICY = {
	Background = { baseOrder = 1000, popup = false, closeOnEscape = false, inputMode = "passThrough" },
	Normal = { baseOrder = 2000, popup = false, closeOnEscape = false, inputMode = "hitOnly" },
	Popup = { baseOrder = 3000, popup = true, closeOnEscape = true, inputMode = "modalOrHit" },
	Guide = { baseOrder = 4000, popup = true, closeOnEscape = true, inputMode = "guide" },
	Top = { baseOrder = 5000, popup = true, closeOnEscape = true, inputMode = "modalOrHit" },
	Toast = { baseOrder = 6000, popup = false, closeOnEscape = false, inputMode = "passThrough" },
}

local SORTING_PRIORITY_STEP = 100
local KEY_ESCAPE = 1
local KEY_TAB = 15

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

local function isBlank(value)
	return value == nil or value == ""
end

local function normalizeSafeArea(leftOrRect, top, right, bottom)
	local area = {}
	if type(leftOrRect) == "table" then
		area.left = tonumber(leftOrRect.left or leftOrRect.x or leftOrRect[1]) or 0
		area.top = tonumber(leftOrRect.top or leftOrRect.y or leftOrRect[2]) or 0
		area.right = tonumber(leftOrRect.right or leftOrRect[3]) or 0
		area.bottom = tonumber(leftOrRect.bottom or leftOrRect[4]) or 0
	else
		area.left = tonumber(leftOrRect) or 0
		area.top = tonumber(top) or 0
		area.right = tonumber(right) or 0
		area.bottom = tonumber(bottom) or 0
	end
	area.left = math.max(area.left, 0)
	area.top = math.max(area.top, 0)
	area.right = math.max(area.right, 0)
	area.bottom = math.max(area.bottom, 0)
	return area
end

local function normalizeRect(rect)
	if type(rect) ~= "table" then
		return nil
	end

	local x = tonumber(rect.x or rect.left or rect[1]) or 0
	local y = tonumber(rect.y or rect.top or rect[2]) or 0
	local width = tonumber(rect.width or rect.w or rect[3]) or 0
	local height = tonumber(rect.height or rect.h or rect[4]) or 0
	if width <= 0 or height <= 0 then
		return nil
	end
	return {
		x = x,
		y = y,
		width = width,
		height = height,
	}
end

local function callView(view, functionName, ...)
	local func = view and view[functionName]
	if func ~= nil then
		return func(view, ...)
	end
	return nil
end

function FairyGuiLayers:Init(owner)
	self.owner = owner
	self.currentSceneName = owner ~= nil and owner.currentSceneName or self.currentSceneName or "Default"
	self.designWidth = owner ~= nil and owner.designWidth or self.designWidth
	self.designHeight = owner ~= nil and owner.designHeight or self.designHeight
	self.scaleMode = owner ~= nil and owner.scaleMode or self.scaleMode or "stretch"
	self.layers = owner ~= nil and owner.layers or self.layers or copyTable(DEFAULT_LAYER_ORDER)
	self.layerPolicies = owner ~= nil and owner.layerPolicies or self.layerPolicies or {}
	self.layerNextOrder = owner ~= nil and owner.layerNextOrder or self.layerNextOrder or {}
	self.layerObjects = owner ~= nil and owner.layerObjects or self.layerObjects or {}
	self.layerRoots = owner ~= nil and owner.layerRoots or self.layerRoots or {}
	self.safeArea = owner ~= nil and owner.safeArea or self.safeArea or normalizeSafeArea(0, 0, 0, 0)
	self.uiStack = owner ~= nil and owner.uiStack or self.uiStack or {}
	self.popupStack = owner ~= nil and owner.popupStack or self.popupStack or {}
	self.stackEntriesByKey = owner ~= nil and owner.stackEntriesByKey or self.stackEntriesByKey or {}
	self.nextStackSerial = owner ~= nil and owner.nextStackSerial or self.nextStackSerial or 0
	for layerName, _ in pairs(self.layers) do
		self.layerNextOrder[layerName] = self.layerNextOrder[layerName] or 0
		self.layerObjects[layerName] = self.layerObjects[layerName] or {}
	end
	if owner ~= nil then
		owner.currentSceneName = self.currentSceneName
		owner.designWidth = self.designWidth
		owner.designHeight = self.designHeight
		owner.scaleMode = self.scaleMode
		owner.layers = self.layers
		owner.layerPolicies = self.layerPolicies
		owner.layerNextOrder = self.layerNextOrder
		owner.layerObjects = self.layerObjects
		owner.layerRoots = self.layerRoots
		owner.safeArea = self.safeArea
		owner.uiStack = self.uiStack
		owner.popupStack = self.popupStack
		owner.stackEntriesByKey = self.stackEntriesByKey
		owner.nextStackSerial = self.nextStackSerial
		for layerName, policy in pairs(DEFAULT_LAYER_POLICY) do
			if owner.layerPolicies[layerName] == nil then
				owner.layerPolicies[layerName] = copyTable(policy)
			end
		end
	end
end

function FairyGuiLayers:GetLayerPolicy(layerName)
	local self = self.owner
	layerName = layerName or "Normal"
	local defaultPolicy = DEFAULT_LAYER_POLICY[layerName] or {
		baseOrder = DEFAULT_LAYER_ORDER[layerName] or DEFAULT_LAYER_ORDER.Normal,
		popup = false,
		closeOnEscape = false,
		inputMode = "hitOnly",
	}
	if self == nil then
		return copyTable(defaultPolicy, { name = layerName })
	end

	self.layerPolicies = self.layerPolicies or {}
	if self.layerPolicies[layerName] == nil then
		self.layerPolicies[layerName] = copyTable(defaultPolicy)
	end
	local policy = self.layerPolicies[layerName]
	policy.name = layerName
	if policy.baseOrder == nil then
		policy.baseOrder = defaultPolicy.baseOrder
	end
	if policy.inputMode == nil then
		policy.inputMode = defaultPolicy.inputMode or "hitOnly"
	end
	return policy
end

function FairyGuiLayers:SetLayerPolicy(layerName, policy)
	local self = self.owner
	if self == nil or isBlank(layerName) or type(policy) ~= "table" then
		return nil
	end

	local layerPolicy = self:GetLayerPolicy(layerName)
	copyTable(policy, layerPolicy)
	if layerPolicy.baseOrder ~= nil then
		self.layers[layerName] = tonumber(layerPolicy.baseOrder) or self.layers[layerName]
	end
	return layerPolicy
end

function FairyGuiLayers:GetSafeArea()
	local self = self.owner
	if self == nil then
		return normalizeSafeArea(0, 0, 0, 0)
	end

	self.safeArea = self.safeArea or normalizeSafeArea(0, 0, 0, 0)
	return copyTable(self.safeArea)
end

function FairyGuiLayers:SetSafeArea(leftOrRect, top, right, bottom)
	local self = self.owner
	if self == nil then
		return normalizeSafeArea(0, 0, 0, 0)
	end

	self.safeArea = normalizeSafeArea(leftOrRect, top, right, bottom)
	for _, objectInfo in pairs(self.objects or {}) do
		local param = objectInfo.param or {}
		if param.useSafeArea == true or param.safeArea == true then
			self:ApplyScreenAdapt(objectInfo)
		end
	end
	return self:GetSafeArea()
end

function FairyGuiLayers:GetLayerBaseOrder(layerName)
	local self = self.owner
	if self == nil then
		return DEFAULT_LAYER_ORDER.Normal
	end

	layerName = layerName or "Normal"
	if self.layers[layerName] == nil then
		self.layers[layerName] = self:GetLayerPolicy(layerName).baseOrder or self.layers.Normal or DEFAULT_LAYER_ORDER.Normal
		self.layerNextOrder[layerName] = 0
		self.layerObjects[layerName] = {}
	end
	return self.layers[layerName]
end

function FairyGuiLayers:GetLayerRoot(layerName)
	local self = self.owner
	if self == nil then
		return nil
	end

	layerName = layerName or "Normal"
	return self.layerRoots ~= nil and self.layerRoots[layerName] or nil
end

function FairyGuiLayers:EnsureLayerRoot(layerName)
	local self = self.owner
	if self == nil then
		return nil
	end

	layerName = layerName or "Normal"
	if self.layerRoots == nil then
		self.layerRoots = {}
	end

	local rootHandle = self.layerRoots[layerName]
	if rootHandle ~= nil then
		return rootHandle
	end
	if NativeApi == nil or NativeApi.createFairyGuiContainer == nil then
		return nil
	end

	rootHandle = NativeApi:createFairyGuiContainer("Layer_" .. layerName)
	if rootHandle == nil or rootHandle <= 0 then
		return nil
	end
	if NativeApi.addFairyGuiObjectToRoot == nil or not NativeApi:addFairyGuiObjectToRoot(rootHandle) then
		if NativeApi.removeFairyGuiObject ~= nil then
			NativeApi:removeFairyGuiObject(rootHandle)
		end
		return nil
	end

	self.layerRoots[layerName] = rootHandle
	self:SetPosition(rootHandle, 0, 0)
	self:SetSize(rootHandle, self:GetScreenWidth(), self:GetScreenHeight())
	if NativeApi.setFairyGuiObjectSortingOrder ~= nil then
		NativeApi:setFairyGuiObjectSortingOrder(rootHandle, self:GetLayerBaseOrder(layerName))
	end
	return rootHandle
end

function FairyGuiLayers:ResolveAttachParent(layerName, param)
	local self = self.owner
	if self == nil then
		return nil
	end

	param = param or {}
	local explicitParent = param.parentHandle or param.rootHandle or param.parent or param.root
	if type(explicitParent) == "number" and explicitParent > 0 then
		return explicitParent
	end
	if type(param.parentKey) == "string" then
		local parentInfo = self:GetObjectInfo(param.parentKey)
		if parentInfo ~= nil then
			return parentInfo.handle
		end
	end
	if type(param.rootLayer) == "string" then
		return self:EnsureLayerRoot(param.rootLayer)
	end
	local rootHandle = self:EnsureLayerRoot(layerName)
	return rootHandle
end

function FairyGuiLayers:AttachToLayer(handle, layerName, param)
	local owner = self.owner
	if owner == nil then
		return false
	end

	local rootHandle = self:ResolveAttachParent(layerName, param)
	if rootHandle ~= nil and NativeApi.addFairyGuiObjectToParent ~= nil then
		return NativeApi:addFairyGuiObjectToParent(handle, rootHandle)
	end
	if NativeApi ~= nil and NativeApi.addFairyGuiObjectToRoot ~= nil then
		return NativeApi:addFairyGuiObjectToRoot(handle)
	end
	return false
end

function FairyGuiLayers:ResizeLayerRoots()
	local self = self.owner
	if self == nil or self.layerRoots == nil then
		return
	end

	local screenWidth = self:GetScreenWidth()
	local screenHeight = self:GetScreenHeight()
	for layerName, handle in pairs(self.layerRoots) do
		self:SetPosition(handle, 0, 0)
		self:SetSize(handle, screenWidth, screenHeight)
		if NativeApi ~= nil and NativeApi.setFairyGuiObjectSortingOrder ~= nil then
			NativeApi:setFairyGuiObjectSortingOrder(handle, self:GetLayerBaseOrder(layerName))
		end
	end
end

function FairyGuiLayers:NextLayerSortingOrder(layerName, priority)
	local self = self.owner
	if self == nil then
		return 0
	end

	layerName = layerName or "Normal"
	local nextOrder = (self.layerNextOrder[layerName] or 0) + 1
	self.layerNextOrder[layerName] = nextOrder
	return self:GetLayerBaseOrder(layerName) + (tonumber(priority) or 0) * SORTING_PRIORITY_STEP + nextOrder
end

function FairyGuiLayers:IsPopupLayer(layerName)
	local policy = self:GetLayerPolicy(layerName)
	return policy.popup == true
end

function FairyGuiLayers:GetStackMode(objectInfo)
	local self = self.owner
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
	if self ~= nil and self:IsPopupLayer(objectInfo.layer) then
		return "Popup"
	end
	return "Normal"
end

function FairyGuiLayers:RemoveStackEntry(key)
	local self = self.owner
	if self == nil or key == nil or self.stackEntriesByKey[key] == nil then
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

function FairyGuiLayers:PushStack(objectInfo)
	local self = self.owner
	if self == nil or objectInfo == nil or objectInfo.key == nil then
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
		priority = objectInfo.priority or 0,
		sortingOrder = objectInfo.sortingOrder or 0,
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

function FairyGuiLayers:GetTopStackObject(stack, layerName)
	local self = self.owner
	if self == nil then
		return nil, nil
	end

	local bestObject = nil
	local bestEntry = nil
	for index = #stack, 1, -1 do
		local entry = stack[index]
		local objectInfo = entry ~= nil and self.objects[entry.key] or nil
		if objectInfo ~= nil and self.hiddenObjects[entry.key] == nil and (layerName == nil or objectInfo.layer == layerName) then
			if bestObject == nil
				or (objectInfo.sortingOrder or 0) > (bestObject.sortingOrder or 0)
				or ((objectInfo.sortingOrder or 0) == (bestObject.sortingOrder or 0) and (entry.serial or 0) > (bestEntry.serial or 0)) then
				bestObject = objectInfo
				bestEntry = entry
			end
		end
	end
	return bestObject, bestEntry
end

function FairyGuiLayers:GetObjectResult(objectInfo)
	if objectInfo == nil then
		return nil
	end
	return objectInfo.ctrl or objectInfo.view or objectInfo.handle
end

function FairyGuiLayers:GetTopUI(layerName)
	local self = self.owner
	if self == nil then
		return nil
	end
	local objectInfo = self:GetTopStackObject(self.uiStack, layerName)
	return self:GetObjectResult(objectInfo)
end

function FairyGuiLayers:GetTopPopup()
	local self = self.owner
	if self == nil then
		return nil
	end
	local objectInfo = self:GetTopStackObject(self.popupStack)
	return self:GetObjectResult(objectInfo)
end

function FairyGuiLayers:CloseTop(layerName, forceDestroy)
	local self = self.owner
	if self == nil then
		return false
	end
	local objectInfo = self:GetTopStackObject(self.uiStack, layerName)
	if objectInfo == nil then
		return false
	end
	return self:CloseUI(objectInfo.key, forceDestroy)
end

function FairyGuiLayers:CloseTopPopup(forceDestroy)
	local self = self.owner
	if self == nil then
		return false
	end
	local objectInfo = self:GetTopStackObject(self.popupStack)
	if objectInfo == nil then
		return false
	end
	return self:CloseUI(objectInfo.key, forceDestroy)
end

function FairyGuiLayers:IsPopupOpenParam(param)
	local self = self.owner
	if param == nil then
		return false
	end
	if param.stack == false or param.stackMode == "None" then
		return false
	end
	if param.stackMode == "Popup" then
		return true
	end
	return self ~= nil and self:IsPopupLayer(param.layer)
end

function FairyGuiLayers:GetPopupGroup(param, objectInfo)
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

function FairyGuiLayers:GetUIGroup(param, objectInfo)
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

function FairyGuiLayers:GetSceneName(param, objectInfo)
	local self = self.owner
	local sceneName = param ~= nil and (param.sceneName or param.scene) or nil
	if isBlank(sceneName) and objectInfo ~= nil then
		sceneName = objectInfo.sceneName
	end
	if isBlank(sceneName) and self ~= nil then
		sceneName = self.currentSceneName
	end
	return sceneName or "Default"
end

function FairyGuiLayers:GetTopPopupObjectByGroup(popupGroup)
	local self = self.owner
	if self == nil or isBlank(popupGroup) then
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

function FairyGuiLayers:ReopenObjectInfo(objectInfo, param)
	local self = self.owner
	if self == nil or objectInfo == nil then
		return nil
	end

	objectInfo.param = param or objectInfo.param
	objectInfo.cache = objectInfo.param.cache == true or objectInfo.cache == true
	objectInfo.popupGroup = self:GetPopupGroup(objectInfo.param, objectInfo)
	objectInfo.popupMode = objectInfo.param.popupMode or objectInfo.popupMode or "stack"
	objectInfo.priority = tonumber(objectInfo.param.priority or objectInfo.param.sortingPriority) or objectInfo.priority or 0
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

function FairyGuiLayers:ClosePopupGroup(popupGroup, exceptKey, forceDestroy)
	local self = self.owner
	if self == nil or isBlank(popupGroup) then
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

function FairyGuiLayers:ApplyPopupOpenPolicy(param)
	local self = self.owner
	if self == nil or param == nil or param.__popupPolicyApplied == true then
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

function FairyGuiLayers:CanClosePopupByEscape(objectInfo)
	if objectInfo == nil then
		return false
	end
	local param = objectInfo.param or {}
	if param.closeOnEscape ~= nil then
		return param.closeOnEscape ~= false
	end
	return self:ShouldLayerCloseOnEscape(objectInfo.layer)
end

function FairyGuiLayers:ShouldLayerCloseOnEscape(layerName)
	local policy = self:GetLayerPolicy(layerName)
	return policy.closeOnEscape == true
end

function FairyGuiLayers:HandleKeyPressed(keyCode, keyText)
	local self = self.owner
	if self == nil then
		return false
	end
	if tonumber(keyCode) == KEY_TAB then
		return self:FocusNext(false)
	end
	if tonumber(keyCode) == KEY_ESCAPE then
		local objectInfo = self:GetTopStackObject(self.popupStack)
		if self:CanClosePopupByEscape(objectInfo) then
			return self:CloseUI(objectInfo.key)
		end
	end
	return false
end

function FairyGuiLayers:HandleKeyReleased(keyCode, keyText)
	return false
end

function FairyGuiLayers:UpdateModalMaskSorting(objectInfo)
	local self = self.owner
	if self == nil or objectInfo == nil then
		return false
	end
	local updated = false
	if objectInfo.modalMaskHandle ~= nil then
		if NativeApi ~= nil and NativeApi.setFairyGuiObjectSortingOrder ~= nil then
			NativeApi:setFairyGuiObjectSortingOrder(objectInfo.modalMaskHandle, (objectInfo.sortingOrder or self:GetLayerBaseOrder(objectInfo.layer)) - 1)
		end
		if self.hiddenObjects[objectInfo.key] == nil then
			self:SetVisible(objectInfo.modalMaskHandle, true)
		end
		updated = true
	end
	return self:UpdateGuideMaskSorting(objectInfo) or updated
end

function FairyGuiLayers:AssignLayer(objectInfo, layerName, forceNextOrder)
	local self = self.owner
	if self == nil or objectInfo == nil or objectInfo.handle == nil then
		return false
	end

	layerName = layerName or objectInfo.layer or "Normal"
	if objectInfo.layer ~= nil and self.layerObjects[objectInfo.layer] ~= nil then
		self.layerObjects[objectInfo.layer][objectInfo.handle] = nil
	end

	objectInfo.layer = layerName
	objectInfo.priority = tonumber(objectInfo.priority or (objectInfo.param and (objectInfo.param.priority or objectInfo.param.sortingPriority))) or 0
	if forceNextOrder ~= true and objectInfo.param ~= nil and objectInfo.param.sortingOrder ~= nil then
		objectInfo.sortingOrder = tonumber(objectInfo.param.sortingOrder) or self:NextLayerSortingOrder(layerName, objectInfo.priority)
	else
		objectInfo.sortingOrder = self:NextLayerSortingOrder(layerName, objectInfo.priority)
	end
	if self.layerObjects[layerName] == nil then
		self.layerObjects[layerName] = {}
	end
	self.layerObjects[layerName][objectInfo.handle] = true

	if NativeApi ~= nil and NativeApi.setFairyGuiObjectSortingOrder ~= nil then
		local result = NativeApi:setFairyGuiObjectSortingOrder(objectInfo.handle, objectInfo.sortingOrder)
		self:UpdateModalMaskSorting(objectInfo)
		return result
	end
	self:UpdateModalMaskSorting(objectInfo)
	return true
end

function FairyGuiLayers:BringToFront(keyOrHandle, priority)
	local self = self.owner
	if self == nil then
		return false
	end
	local objectInfo = self:GetObjectInfo(keyOrHandle)
	if objectInfo == nil then
		return false
	end
	if priority ~= nil then
		objectInfo.priority = tonumber(priority) or objectInfo.priority or 0
	end
	local result = self:AssignLayer(objectInfo, objectInfo.layer, true)
	self:PushStack(objectInfo)
	return result
end

function FairyGuiLayers:SetSortingPriority(keyOrHandle, priority, bringToFront)
	local self = self.owner
	if self == nil then
		return false
	end
	local objectInfo = self:GetObjectInfo(keyOrHandle)
	if objectInfo == nil then
		return false
	end
	objectInfo.priority = tonumber(priority) or 0
	if objectInfo.param ~= nil then
		objectInfo.param.priority = objectInfo.priority
		objectInfo.param.sortingOrder = nil
	end
	return bringToFront ~= false and self:BringToFront(objectInfo.handle) or self:AssignLayer(objectInfo, objectInfo.layer, true)
end

function FairyGuiLayers:HandleModalMaskClick(key, reason)
	local self = self.owner
	if self == nil then
		return false
	end
	local objectInfo = self.objects[key]
	if objectInfo == nil then
		return false
	end

	local param = objectInfo.param or {}
	if param.modalCloseOnlyWhenTop ~= false then
		local topPopup = self:GetTopStackObject(self.popupStack)
		if topPopup ~= nil and topPopup.key ~= objectInfo.key then
			return false
		end
	end
	return self:CloseUI(objectInfo.key, param.forceDestroyOnMaskClick == true, reason or "modalMaskClick")
end

function FairyGuiLayers:GetGuideMaskRect(param)
	local self = self.owner
	if self == nil then
		return nil
	end

	param = param or {}
	local rect = self:ApplyDesignRect(param.clickThroughRect or param.highlightRect or param.highlight or param.rect, param)
	if rect == nil then
		return nil
	end

	local screenWidth = self:GetScreenWidth()
	local screenHeight = self:GetScreenHeight()
	if screenWidth <= 0 or screenHeight <= 0 then
		return nil
	end

	local x = math.max(math.min(rect.x, screenWidth), 0)
	local y = math.max(math.min(rect.y, screenHeight), 0)
	local width = math.max(math.min(rect.width, screenWidth - x), 0)
	local height = math.max(math.min(rect.height, screenHeight - y), 0)
	if width <= 0 or height <= 0 then
		return nil
	end
	return {
		x = x,
		y = y,
		width = width,
		height = height,
	}
end

function FairyGuiLayers:SetGuideMaskVisible(objectInfo, visible)
	local self = self.owner
	if self == nil or objectInfo == nil or type(objectInfo.guideMaskHandles) ~= "table" then
		return false
	end
	for _, handle in ipairs(objectInfo.guideMaskHandles) do
		self:SetVisible(handle, visible == true)
	end
	return true
end

function FairyGuiLayers:ClearGuideMaskHandles(objectInfo)
	local self = self.owner
	if self == nil or objectInfo == nil or type(objectInfo.guideMaskHandles) ~= "table" then
		return false
	end

	for _, handle in ipairs(objectInfo.guideMaskHandles) do
		self:ClearBindingsForHandle(handle)
		if NativeApi ~= nil and NativeApi.removeFairyGuiObject ~= nil then
			NativeApi:removeFairyGuiObject(handle)
		end
	end
	objectInfo.guideMaskHandles = nil
	objectInfo.guideMaskRect = nil
	return true
end

function FairyGuiLayers:UpdateGuideMaskSorting(objectInfo)
	local self = self.owner
	if self == nil or objectInfo == nil or type(objectInfo.guideMaskHandles) ~= "table" then
		return false
	end
	if NativeApi == nil or NativeApi.setFairyGuiObjectSortingOrder == nil then
		return false
	end

	local sortingOrder = (objectInfo.sortingOrder or self:GetLayerBaseOrder(objectInfo.layer)) - 1
	for _, handle in ipairs(objectInfo.guideMaskHandles) do
		NativeApi:setFairyGuiObjectSortingOrder(handle, sortingOrder)
	end
	return true
end

function FairyGuiLayers:AddGuideMaskSegment(objectInfo, x, y, width, height, alpha, closeOnClick)
	local self = self.owner
	if self == nil or objectInfo == nil or width <= 0 or height <= 0 then
		return nil
	end
	if NativeApi == nil or NativeApi.createFairyGuiModalMask == nil then
		return nil
	end

	local maskHandle = NativeApi:createFairyGuiModalMask(0, 0, 0, alpha or 0.55)
	if maskHandle == nil or maskHandle <= 0 then
		return nil
	end
	if not self:AttachToLayer(maskHandle, objectInfo.layer) then
		NativeApi:removeFairyGuiObject(maskHandle)
		return nil
	end

	self:SetPosition(maskHandle, x, y)
	self:SetSize(maskHandle, width, height)
	self:SetTouchable(maskHandle, true)
	if NativeApi.setFairyGuiObjectSortingOrder ~= nil then
		NativeApi:setFairyGuiObjectSortingOrder(maskHandle, (objectInfo.sortingOrder or self:GetLayerBaseOrder(objectInfo.layer)) - 1)
	end
	if closeOnClick == true then
		local closeKey = objectInfo.key
		self:AddClick(maskHandle, "", function()
			self:HandleModalMaskClick(closeKey, "guideMaskClick")
		end)
	end

	objectInfo.guideMaskHandles = objectInfo.guideMaskHandles or {}
	table.insert(objectInfo.guideMaskHandles, maskHandle)
	return maskHandle
end

function FairyGuiLayers:CreateGuideMaskSegments(objectInfo, param)
	local self = self.owner
	if self == nil or objectInfo == nil then
		return false
	end
	self:ClearGuideMaskHandles(objectInfo)

	local rect = self:GetGuideMaskRect(param or objectInfo.param)
	if rect == nil then
		return false
	end

	local screenWidth = self:GetScreenWidth()
	local screenHeight = self:GetScreenHeight()
	local alpha = (param or objectInfo.param or {}).modalAlpha or 0.55
	local closeOnClick = (param or objectInfo.param or {}).closeOnMaskClick == true
	local right = rect.x + rect.width
	local bottom = rect.y + rect.height

	objectInfo.guideMaskRect = rect
	self:AddGuideMaskSegment(objectInfo, 0, 0, screenWidth, rect.y, alpha, closeOnClick)
	self:AddGuideMaskSegment(objectInfo, 0, bottom, screenWidth, math.max(screenHeight - bottom, 0), alpha, closeOnClick)
	self:AddGuideMaskSegment(objectInfo, 0, rect.y, rect.x, rect.height, alpha, closeOnClick)
	self:AddGuideMaskSegment(objectInfo, right, rect.y, math.max(screenWidth - right, 0), rect.height, alpha, closeOnClick)
	self:UpdateGuideMaskSorting(objectInfo)
	return type(objectInfo.guideMaskHandles) == "table" and #objectInfo.guideMaskHandles > 0
end

function FairyGuiLayers:UpdateGuideMaskLayout(objectInfo)
	local self = self.owner
	if self == nil or objectInfo == nil or objectInfo.guideMaskRect == nil then
		return false
	end
	return self:CreateGuideMaskSegments(objectInfo, objectInfo.param)
end

function FairyGuiLayers:CreateModalMask(objectInfo, param)
	local self = self.owner
	if self == nil or objectInfo == nil or objectInfo.handle == nil or param == nil or param.modal ~= true then
		return nil
	end
	if NativeApi == nil or NativeApi.createFairyGuiModalMask == nil then
		return nil
	end

	local alpha = param.modalAlpha or 0.45
	local maskHandle = NativeApi:createFairyGuiModalMask(0, 0, 0, alpha)
	if maskHandle == nil or maskHandle <= 0 then
		return nil
	end

	if not self:AttachToLayer(maskHandle, objectInfo.layer) then
		NativeApi:removeFairyGuiObject(maskHandle)
		return nil
	end
	if NativeApi.setFairyGuiObjectSortingOrder ~= nil then
		NativeApi:setFairyGuiObjectSortingOrder(maskHandle, (objectInfo.sortingOrder or self:GetLayerBaseOrder(objectInfo.layer)) - 1)
	end
	self:SetSize(maskHandle, self:GetScreenWidth(), self:GetScreenHeight())
	self:SetPosition(maskHandle, 0, 0)
	objectInfo.modalMaskHandle = maskHandle
	if param.closeOnMaskClick == true then
		local closeKey = objectInfo.key
		objectInfo.modalMaskBindingId = self:AddClick(maskHandle, "", function()
			self:HandleModalMaskClick(closeKey, "modalMaskClick")
		end)
	end
	return maskHandle
end

function FairyGuiLayers:GetScreenWidth()
	if NativeApi ~= nil and NativeApi.getFairyGuiScreenWidth ~= nil then
		local width = NativeApi:getFairyGuiScreenWidth()
		if width ~= nil and width > 0 then
			return width
		end
	end
	if NativeApi ~= nil and NativeApi.getScreenWidth ~= nil then
		return NativeApi:getScreenWidth()
	end
	return 0
end

function FairyGuiLayers:GetScreenHeight()
	if NativeApi ~= nil and NativeApi.getFairyGuiScreenHeight ~= nil then
		local height = NativeApi:getFairyGuiScreenHeight()
		if height ~= nil and height > 0 then
			return height
		end
	end
	if NativeApi ~= nil and NativeApi.getScreenHeight ~= nil then
		return NativeApi:getScreenHeight()
	end
	return 0
end

function FairyGuiLayers:SetDesignResolution(width, height, scaleMode)
	local self = self.owner
	if self == nil then
		return
	end
	self.designWidth = tonumber(width)
	self.designHeight = tonumber(height)
	if scaleMode ~= nil then
		self.scaleMode = scaleMode
	end
end

function FairyGuiLayers:GetDesignTransform(param)
	local self = self.owner
	if self == nil then
		return 1, 1, 0, 0
	end

	param = param or {}
	local screenWidth = self:GetScreenWidth()
	local screenHeight = self:GetScreenHeight()
	local designWidth = tonumber(param.designWidth or self.designWidth)
	local designHeight = tonumber(param.designHeight or self.designHeight)
	if screenWidth <= 0 or screenHeight <= 0 or designWidth == nil or designHeight == nil or designWidth <= 0 or designHeight <= 0 then
		return 1, 1, 0, 0
	end

	local scaleX = screenWidth / designWidth
	local scaleY = screenHeight / designHeight
	local scaleMode = param.scaleMode or self.scaleMode or "stretch"
	if scaleMode == "fit" then
		local scale = math.min(scaleX, scaleY)
		return scale, scale, (screenWidth - designWidth * scale) * 0.5, (screenHeight - designHeight * scale) * 0.5
	elseif scaleMode == "fill" then
		local scale = math.max(scaleX, scaleY)
		return scale, scale, (screenWidth - designWidth * scale) * 0.5, (screenHeight - designHeight * scale) * 0.5
	elseif scaleMode == "fitWidth" then
		return scaleX, scaleX, 0, (screenHeight - designHeight * scaleX) * 0.5
	elseif scaleMode == "fitHeight" then
		return scaleY, scaleY, (screenWidth - designWidth * scaleY) * 0.5, 0
	end
	return scaleX, scaleY, 0, 0
end

function FairyGuiLayers:ApplyDesignRect(rect, param)
	local self = self.owner
	if self == nil then
		return nil
	end

	rect = normalizeRect(rect)
	if rect == nil then
		return nil
	end
	param = param or {}
	if param.useDesignScale ~= true and param.designWidth == nil and param.designHeight == nil and param.scaleMode == nil and self.designWidth == nil and self.designHeight == nil then
		return rect
	end

	local scaleX, scaleY, offsetX, offsetY = self:GetDesignTransform(param)
	return {
		x = rect.x * scaleX + offsetX,
		y = rect.y * scaleY + offsetY,
		width = rect.width * scaleX,
		height = rect.height * scaleY,
	}
end

function FairyGuiLayers:ClampLayoutRect(rect, screenWidth, screenHeight, bounds)
	if rect == nil then
		return nil
	end
	bounds = bounds or { x = 0, y = 0, width = screenWidth, height = screenHeight }
	local minX = tonumber(bounds.x) or 0
	local minY = tonumber(bounds.y) or 0
	local maxX = minX + math.max((tonumber(bounds.width) or screenWidth) - rect.width, 0)
	local maxY = minY + math.max((tonumber(bounds.height) or screenHeight) - rect.height, 0)
	rect.x = math.max(math.min(rect.x, maxX), minX)
	rect.y = math.max(math.min(rect.y, maxY), minY)
	return rect
end

function FairyGuiLayers:GetLayoutRect(param)
	local self = self.owner
	if self == nil then
		return nil
	end

	param = param or {}
	local screenWidth = self:GetScreenWidth()
	local screenHeight = self:GetScreenHeight()
	if screenWidth <= 0 or screenHeight <= 0 then
		return nil
	end

	local left = tonumber(param.marginLeft or param.left) or 0
	local right = tonumber(param.marginRight or param.right) or 0
	local top = tonumber(param.marginTop or param.top) or 0
	local bottom = tonumber(param.marginBottom or param.bottomMargin) or 0
	local useSafeArea = param.useSafeArea == true or param.safeArea == true
	if useSafeArea then
		local safeArea = self:GetSafeArea()
		left = left + (safeArea.left or 0)
		right = right + (safeArea.right or 0)
		top = top + (safeArea.top or 0)
		bottom = bottom + (safeArea.bottom or 0)
	end
	local layoutWidth = math.max(screenWidth - left - right, 0)
	local layoutHeight = math.max(screenHeight - top - bottom, 0)
	if param.fullScreen == true or param.adaptScreen == true then
		return {
			x = left,
			y = top,
			width = layoutWidth,
			height = layoutHeight,
		}
	end

	local width = tonumber(param.width)
	local height = tonumber(param.height)
	if width == nil or height == nil then
		return nil
	end

	local alignX = param.alignX or param.hAlign
	local alignY = param.alignY or param.vAlign
	if param.center == true then
		alignX = alignX or "center"
		alignY = alignY or "middle"
	end

	local x = tonumber(param.x)
	local y = tonumber(param.y)
	local offsetX = tonumber(param.offsetX) or 0
	local offsetY = tonumber(param.offsetY) or 0
	if alignX == "center" then
		x = left + (layoutWidth - width) * 0.5 + offsetX
	elseif alignX == "right" then
		x = screenWidth - width - right + offsetX
	elseif x == nil then
		x = left + offsetX
	end
	if alignY == "middle" or alignY == "center" then
		y = top + (layoutHeight - height) * 0.5 + offsetY
	elseif alignY == "bottom" then
		y = screenHeight - height - bottom + offsetY
	elseif y == nil then
		y = top + offsetY
	end

	local rect = {
		x = x,
		y = y,
		width = width,
		height = height,
	}
	if param.fitInScreen == true then
		self:ClampLayoutRect(rect, screenWidth, screenHeight, useSafeArea and {
			x = left,
			y = top,
			width = layoutWidth,
			height = layoutHeight,
		} or nil)
	end
	return rect
end

function FairyGuiLayers:ApplyScreenAdapt(objectInfo)
	local self = self.owner
	if self == nil or objectInfo == nil or objectInfo.handle == nil then
		return false
	end

	local param = objectInfo.param or {}
	local screenWidth = self:GetScreenWidth()
	local screenHeight = self:GetScreenHeight()
	if screenWidth <= 0 or screenHeight <= 0 then
		return false
	end

	local rect = self:GetLayoutRect(param)
	if rect ~= nil then
		self:SetPosition(objectInfo.handle, rect.x, rect.y)
		self:SetSize(objectInfo.handle, rect.width, rect.height)
		objectInfo.lastLayoutRect = rect
	end
	if objectInfo.modalMaskHandle ~= nil then
		self:SetPosition(objectInfo.modalMaskHandle, 0, 0)
		self:SetSize(objectInfo.modalMaskHandle, screenWidth, screenHeight)
	end
	self:UpdateGuideMaskLayout(objectInfo)
	self:ApplyServiceLayout(objectInfo)
	return rect ~= nil
end

function FairyGuiLayers:HandleWindowResized(width, height)
	local self = self.owner
	if self == nil then
		return
	end
	self:ResizeLayerRoots()
	for _, objectInfo in pairs(self.objects) do
		self:ApplyScreenAdapt(objectInfo)
		if objectInfo.view ~= nil then
			callView(objectInfo.view, "OnResize", width, height)
		end
	end
end

function FairyGuiLayers:DumpLayerRoots()
	local self = self.owner
	if self == nil then
		return
	end
	print("[FGUI] DumpLayerRoots begin")
	if self.layerRoots ~= nil then
		for layerName, handle in pairs(self.layerRoots) do
			local policy = self:GetLayerPolicy(layerName)
			print("[FGUI] LayerRoot", layerName, "handle=", handle, "baseOrder=", self:GetLayerBaseOrder(layerName), "popup=", policy.popup == true, "closeOnEscape=", policy.closeOnEscape == true, "inputMode=", policy.inputMode or "")
		end
	end
	print("[FGUI] DumpLayerRoots end")
end

function FairyGuiLayers:DumpStacks()
	local self = self.owner
	if self == nil then
		return
	end
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
