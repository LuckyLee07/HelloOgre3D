local FairyGuiLayers = Class("FairyGuiLayers")

local DEFAULT_LAYER_ORDER = {
	Background = 1000,
	Normal = 2000,
	Popup = 3000,
	Guide = 4000,
	Top = 5000,
	Toast = 6000,
}

local SORTING_PRIORITY_STEP = 100
local KEY_ESCAPE = 1

local function isBlank(value)
	return value == nil or value == ""
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
end

function FairyGuiLayers:GetLayerBaseOrder(layerName)
	local self = self.owner
	if self == nil then
		return DEFAULT_LAYER_ORDER.Normal
	end

	layerName = layerName or "Normal"
	if self.layers[layerName] == nil then
		self.layers[layerName] = self.layers.Normal or DEFAULT_LAYER_ORDER.Normal
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

function FairyGuiLayers:AttachToLayer(handle, layerName)
	local self = self.owner
	if self == nil then
		return false
	end

	local rootHandle = self:EnsureLayerRoot(layerName)
	if rootHandle ~= nil and GameManager.addFairyGuiObjectToParent ~= nil then
		return GameManager:addFairyGuiObjectToParent(handle, rootHandle)
	end
	if GameManager ~= nil and GameManager.addFairyGuiObjectToRoot ~= nil then
		return GameManager:addFairyGuiObjectToRoot(handle)
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
		if GameManager ~= nil and GameManager.setFairyGuiObjectSortingOrder ~= nil then
			GameManager:setFairyGuiObjectSortingOrder(handle, self:GetLayerBaseOrder(layerName))
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
	return layerName == "Popup" or layerName == "Guide" or layerName == "Top" or layerName == "Toast"
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
	return param.closeOnEscape ~= false
end

function FairyGuiLayers:HandleKeyPressed(keyCode, keyText)
	local self = self.owner
	if self == nil then
		return false
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
		if GameManager ~= nil and GameManager.setFairyGuiObjectSortingOrder ~= nil then
			GameManager:setFairyGuiObjectSortingOrder(objectInfo.modalMaskHandle, (objectInfo.sortingOrder or self:GetLayerBaseOrder(objectInfo.layer)) - 1)
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

	if GameManager ~= nil and GameManager.setFairyGuiObjectSortingOrder ~= nil then
		local result = GameManager:setFairyGuiObjectSortingOrder(objectInfo.handle, objectInfo.sortingOrder)
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
		if GameManager ~= nil and GameManager.removeFairyGuiObject ~= nil then
			GameManager:removeFairyGuiObject(handle)
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
	if GameManager == nil or GameManager.setFairyGuiObjectSortingOrder == nil then
		return false
	end

	local sortingOrder = (objectInfo.sortingOrder or self:GetLayerBaseOrder(objectInfo.layer)) - 1
	for _, handle in ipairs(objectInfo.guideMaskHandles) do
		GameManager:setFairyGuiObjectSortingOrder(handle, sortingOrder)
	end
	return true
end

function FairyGuiLayers:AddGuideMaskSegment(objectInfo, x, y, width, height, alpha, closeOnClick)
	local self = self.owner
	if self == nil or objectInfo == nil or width <= 0 or height <= 0 then
		return nil
	end
	if GameManager == nil or GameManager.createFairyGuiModalMask == nil then
		return nil
	end

	local maskHandle = GameManager:createFairyGuiModalMask(0, 0, 0, alpha or 0.55)
	if maskHandle == nil or maskHandle <= 0 then
		return nil
	end
	if not self:AttachToLayer(maskHandle, objectInfo.layer) then
		GameManager:removeFairyGuiObject(maskHandle)
		return nil
	end

	self:SetPosition(maskHandle, x, y)
	self:SetSize(maskHandle, width, height)
	self:SetTouchable(maskHandle, true)
	if GameManager.setFairyGuiObjectSortingOrder ~= nil then
		GameManager:setFairyGuiObjectSortingOrder(maskHandle, (objectInfo.sortingOrder or self:GetLayerBaseOrder(objectInfo.layer)) - 1)
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
			self:HandleModalMaskClick(closeKey, "modalMaskClick")
		end)
	end
	return maskHandle
end

function FairyGuiLayers:GetScreenWidth()
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

function FairyGuiLayers:GetScreenHeight()
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

function FairyGuiLayers:ClampLayoutRect(rect, screenWidth, screenHeight)
	if rect == nil then
		return nil
	end
	rect.x = math.max(math.min(rect.x, math.max(screenWidth - rect.width, 0)), 0)
	rect.y = math.max(math.min(rect.y, math.max(screenHeight - rect.height, 0)), 0)
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
	if param.fullScreen == true or param.adaptScreen == true then
		return {
			x = left,
			y = top,
			width = math.max(screenWidth - left - right, 0),
			height = math.max(screenHeight - top - bottom, 0),
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
		x = (screenWidth - width) * 0.5 + offsetX
	elseif alignX == "right" then
		x = screenWidth - width - right + offsetX
	elseif x == nil then
		x = left + offsetX
	end
	if alignY == "middle" or alignY == "center" then
		y = (screenHeight - height) * 0.5 + offsetY
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
		self:ClampLayoutRect(rect, screenWidth, screenHeight)
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
			print("[FGUI] LayerRoot", layerName, "handle=", handle, "baseOrder=", self:GetLayerBaseOrder(layerName))
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
