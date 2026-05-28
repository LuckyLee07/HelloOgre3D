local NativeApi = require("res.scripts.manager.fairygui.FairyGuiNativeApi")

local FairyGuiServiceCore = {}

local function isBlank(value)
	return value == nil or value == ""
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

local function nowMs()
	return os.clock and os.clock() * 1000 or 0
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

FairyGuiServiceCore.DEFAULT_SERVICE_SKINS = {
	Toast = {
		Default = {
			minWidth = 240,
			height = 44,
			paddingX = 80,
			charWidth = 14,
			fontSize = 22,
			bottom = 120,
			textColor = { 255, 244, 200 },
			resource = { mode = "dynamic" },
		},
	},
	Tip = {
		Default = {
			width = 320,
			height = 36,
			fontSize = 20,
			anchorGap = 6,
			textColor = { 180, 230, 255 },
			resource = { mode = "dynamic" },
		},
	},
	Loading = {
		Default = {
			width = 280,
			height = 36,
			fontSize = 24,
			textColor = { 255, 255, 255 },
			modalAlpha = 0.45,
			resource = { mode = "dynamic" },
		},
	},
	GuideMask = {
		Default = {
			fontSize = 24,
			textX = 80,
			textY = 80,
			textWidth = 520,
			textHeight = 48,
			modalAlpha = 0.55,
			textColor = { 255, 255, 255 },
			resource = { mode = "dynamic" },
		},
	},
	MessageBox = {
		Default = {
			width = 460,
			height = 240,
			titleFontSize = 24,
			bodyFontSize = 20,
			buttonFontSize = 22,
			buttonWidth = 110,
			buttonHeight = 44,
			buttonGap = 16,
			modalAlpha = 0.45,
			titleColor = { 255, 236, 180 },
			bodyColor = { 230, 230, 230 },
			buttonTextColor = { 255, 255, 255 },
			resource = { mode = "dynamic" },
		},
	},
	PopupMenu = {
		Default = {
			width = 220,
			itemHeight = 34,
			fontSize = 22,
			anchorGap = 6,
			modalAlpha = 0.05,
			itemTextColor = { 255, 255, 255 },
			resource = { mode = "dynamic" },
		},
	},
}

local DEFAULT_SERVICE_SKINS = FairyGuiServiceCore.DEFAULT_SERVICE_SKINS

local function cloneValue(source)
	if type(source) ~= "table" then
		return source
	end

	local result = {}
	for key, value in pairs(source) do
		result[key] = cloneValue(value)
	end
	return result
end

local function mergeTable(base, override)
	local result = cloneValue(base) or {}
	if type(override) ~= "table" then
		return result
	end

	for key, value in pairs(override) do
		if type(value) == "table" and type(result[key]) == "table" then
			result[key] = mergeTable(result[key], value)
		else
			result[key] = cloneValue(value)
		end
	end
	return result
end

local function ensureServiceSkins(service)
	if service == nil then
		return cloneValue(DEFAULT_SERVICE_SKINS)
	end

	local state = service:GetStore():GetServiceState()
	state.serviceSkins = state.serviceSkins or {}
	for serviceType, skins in pairs(DEFAULT_SERVICE_SKINS) do
		state.serviceSkins[serviceType] = state.serviceSkins[serviceType] or {}
		for skinName, skin in pairs(skins) do
			if state.serviceSkins[serviceType][skinName] == nil then
				state.serviceSkins[serviceType][skinName] = cloneValue(skin)
			end
		end
	end
	service.serviceSkins = state.serviceSkins
	return state.serviceSkins
end

local function getColor(style, key, red, green, blue)
	local color = style ~= nil and style[key] or nil
	if type(color) == "table" then
		return tonumber(color.r or color.red or color[1]) or red,
			tonumber(color.g or color.green or color[2]) or green,
			tonumber(color.b or color.blue or color[3]) or blue
	end
	return red, green, blue
end

local function ensureServiceStats(service)
	local state = service:GetStore():GetServiceState()
	if state.serviceStats == nil then
		state.serviceStats = {
			createdTotal = 0,
			closedTotal = 0,
			failedTotal = 0,
			peakOpen = 0,
			toastQueuedTotal = 0,
			toastDedupeIgnoredTotal = 0,
			toastShownTotal = 0,
			toastTimeoutTotal = 0,
			loadingShowTotal = 0,
			loadingHideTotal = 0,
			loadingForceHideTotal = 0,
			loadingPeakRefTotal = 0,
			byType = {},
		}
	end
	if state.serviceStats.byType == nil then
		state.serviceStats.byType = {}
	end
	service.serviceStats = state.serviceStats
	return state.serviceStats
end

local function ensureServiceTypeStats(owner, serviceType)
	local stats = ensureServiceStats(owner)
	serviceType = isBlank(serviceType) and "<unknown>" or serviceType
	if stats.byType[serviceType] == nil then
		stats.byType[serviceType] = {
			created = 0,
			closed = 0,
			failed = 0,
			lastKey = "",
			lastReason = "",
		}
	end
	return stats.byType[serviceType], stats
end

local function getServiceOpenTotal(owner)
	local count = 0
	local store = owner ~= nil and owner:GetStore() or nil
	for _, objectInfo in pairs(store ~= nil and store:GetObjects() or {}) do
		if objectInfo.serviceType ~= nil then
			count = count + 1
		end
	end
	return count
end

local function recordServiceStat(owner, eventName, serviceType, key, reason)
	if owner == nil then
		return nil
	end

	local typeStats, stats = ensureServiceTypeStats(owner, serviceType)
	if eventName == "created" then
		stats.createdTotal = stats.createdTotal + 1
		typeStats.created = typeStats.created + 1
	elseif eventName == "closed" then
		stats.closedTotal = stats.closedTotal + 1
		typeStats.closed = typeStats.closed + 1
	elseif eventName == "failed" then
		stats.failedTotal = stats.failedTotal + 1
		typeStats.failed = typeStats.failed + 1
	end

	stats.lastEvent = eventName or ""
	stats.lastType = serviceType or ""
	stats.lastKey = key or ""
	stats.lastReason = reason or ""
	typeStats.lastKey = key or ""
	typeStats.lastReason = reason or ""
	stats.peakOpen = math.max(stats.peakOpen or 0, getServiceOpenTotal(owner))
	return stats
end

local function recordServicePerf(owner, startMs, serviceType, success)
	if owner ~= nil and owner.RecordPerf ~= nil then
		owner:RecordPerf("service", nowMs() - startMs, tostring(serviceType or ""), success ~= false)
	end
end

local function normalizeRect(owner, rect, param)
	if owner == nil or type(rect) ~= "table" then
		return nil
	end

	local applied = owner.ApplyDesignRect ~= nil and owner:ApplyDesignRect(rect, param or {}) or rect
	if type(applied) ~= "table" then
		return nil
	end

	local x = tonumber(applied.x) or 0
	local y = tonumber(applied.y) or 0
	local width = math.max(tonumber(applied.width) or 0, 0)
	local height = math.max(tonumber(applied.height) or 0, 0)
	return {
		x = x,
		y = y,
		width = width,
		height = height,
	}
end

local function resolveAnchorRect(owner, param, fallbackX, fallbackY)
	param = param or {}
	local rect = normalizeRect(owner, param.anchorRect or param.followRect or param.targetRect or param.rect, param)
	if rect ~= nil then
		return rect
	end
	if fallbackX ~= nil or fallbackY ~= nil or param.x ~= nil or param.y ~= nil then
		return {
			x = tonumber(fallbackX or param.x) or 0,
			y = tonumber(fallbackY or param.y) or 0,
			width = tonumber(param.anchorWidth or param.targetWidth) or 0,
			height = tonumber(param.anchorHeight or param.targetHeight) or 0,
		}
	end
	return nil
end

local function buildAnchoredRect(owner, anchorRect, width, height, param)
	param = param or {}
	width = math.max(tonumber(width) or 0, 0)
	height = math.max(tonumber(height) or 0, 0)
	local screenWidth = owner ~= nil and owner:GetScreenWidth() or 0
	local screenHeight = owner ~= nil and owner:GetScreenHeight() or 0
	local gap = tonumber(param.anchorGap or param.gap) or 6
	local offsetX = tonumber(param.offsetX) or 0
	local offsetY = tonumber(param.offsetY) or 0
	local placement = param.placement or param.followPlacement or "bottomLeft"
	local x = tonumber(param.x) or 0
	local y = tonumber(param.y) or 0

	if anchorRect ~= nil then
		if placement == "bottomRight" then
			x = anchorRect.x + anchorRect.width - width
			y = anchorRect.y + anchorRect.height + gap
		elseif placement == "topLeft" then
			x = anchorRect.x
			y = anchorRect.y - height - gap
		elseif placement == "topRight" then
			x = anchorRect.x + anchorRect.width - width
			y = anchorRect.y - height - gap
		elseif placement == "right" then
			x = anchorRect.x + anchorRect.width + gap
			y = anchorRect.y
		elseif placement == "left" then
			x = anchorRect.x - width - gap
			y = anchorRect.y
		elseif placement == "center" then
			x = anchorRect.x + (anchorRect.width - width) * 0.5
			y = anchorRect.y + (anchorRect.height - height) * 0.5
		else
			x = anchorRect.x
			y = anchorRect.y + anchorRect.height + gap
		end
	end

	local rect = {
		x = x + offsetX,
		y = y + offsetY,
		width = width,
		height = height,
	}
	if owner ~= nil and param.fitInScreen ~= false and owner.ClampLayoutRect ~= nil and screenWidth > 0 and screenHeight > 0 then
		owner:ClampLayoutRect(rect, screenWidth, screenHeight)
	end
	return rect
end

local function bindServiceState(service, state)
	if service == nil or state == nil then
		return
	end
	service.toastQueue = state.toastQueue
	service.toastActive = state.toastActive
	service.toastSerial = state.toastSerial
	service.toastDedupe = state.toastDedupe
	service.loadingRefs = state.loadingRefs
	service.loadingRefTotal = state.loadingRefTotal
	service.serviceStats = state.serviceStats
	service.serviceSkins = state.serviceSkins
	service.serviceState = state
end

local function syncServiceState(service)
	local state = service ~= nil and service:GetStore():GetServiceState() or nil
	if state == nil then
		return
	end
	state.toastQueue = service.toastQueue or {}
	state.toastActive = service.toastActive
	state.toastSerial = service.toastSerial or 0
	state.toastDedupe = service.toastDedupe or {}
	state.loadingRefs = service.loadingRefs or {}
	state.loadingRefTotal = service.loadingRefTotal or 0
	state.serviceStats = service.serviceStats or state.serviceStats
	state.serviceSkins = service.serviceSkins or state.serviceSkins
	bindServiceState(service, state)
end

function FairyGuiServiceCore.Init(self, owner)
	self.owner = owner
	local serviceState = owner ~= nil and owner:GetStore():GetServiceState() or {}
	self.toastQueue = serviceState.toastQueue or {}
	self.toastActive = serviceState.toastActive
	self.toastSerial = serviceState.toastSerial or 0
	self.toastDedupe = serviceState.toastDedupe or {}
	self.loadingRefs = serviceState.loadingRefs or {}
	self.loadingRefTotal = serviceState.loadingRefTotal or 0
	serviceState.toastQueue = self.toastQueue
	serviceState.toastActive = self.toastActive
	serviceState.toastSerial = self.toastSerial
	serviceState.toastDedupe = self.toastDedupe
	serviceState.loadingRefs = self.loadingRefs
	serviceState.loadingRefTotal = self.loadingRefTotal
	if owner ~= nil then
		ensureServiceSkins(self)
		self.serviceStats = serviceState.serviceStats or self.serviceStats or ensureServiceStats(self)
		serviceState.serviceStats = self.serviceStats
		syncServiceState(self)
		ensureServiceStats(self)
	end
end

function FairyGuiServiceCore.RegisterServiceSkin(self, serviceType, skinName, skin)
	local owner = self.owner
	if owner == nil or isBlank(serviceType) or isBlank(skinName) or type(skin) ~= "table" then
		return false
	end

	local skins = ensureServiceSkins(self)
	skins[serviceType] = skins[serviceType] or {}
	local base = skins[serviceType][skinName] or skins[serviceType].Default or {}
	local merged = mergeTable(base, skin)
	merged.name = skinName
	merged.serviceType = serviceType
	skins[serviceType][skinName] = merged
	return true
end

function FairyGuiServiceCore.GetServiceSkin(self, serviceType, skinName)
	local owner = self.owner
	if owner == nil or isBlank(serviceType) then
		return nil
	end

	local skins = ensureServiceSkins(self)
	local skinSet = skins[serviceType]
	if skinSet == nil then
		return nil
	end
	local name = isBlank(skinName) and "Default" or skinName
	local skin = skinSet[name] or skinSet.Default
	return cloneValue(skin)
end

function FairyGuiServiceCore.ResolveServiceSkin(self, serviceType, param)
	local owner = self.owner
	param = param or {}
	local skinName = param.serviceSkinName or param.skinName
	local inlineSkin = nil
	if type(param.serviceSkin) == "table" then
		inlineSkin = param.serviceSkin
	elseif type(param.skin) == "table" then
		inlineSkin = param.skin
	else
		skinName = skinName or param.serviceSkin or param.skin
	end
	skinName = isBlank(skinName) and "Default" or tostring(skinName)

	local skins = ensureServiceSkins(self)
	local skinSet = skins[serviceType] or {}
	local actualSkinName = skinSet[skinName] ~= nil and skinName or "Default"
	local base = skinSet[actualSkinName] or skinSet.Default or {}
	local resolved = mergeTable(base, inlineSkin)
	resolved.name = actualSkinName
	resolved.requestedName = skinName
	resolved.serviceType = serviceType
	resolved.resource = resolved.resource or { mode = "dynamic" }
	return resolved
end

function FairyGuiServiceCore.OpenServiceContainer(self, key, param)
	local owner = self.owner
	if owner == nil then
		return nil
	end

	local startMs = nowMs()
	param = param or {}
	key = param.key or key
	if isBlank(key) then
		recordServiceStat(self, "failed", param.serviceType, key, "emptyKey")
		recordServicePerf(self, startMs, param.serviceType, false)
		return nil
	end

	self:CloseUI(key, true, "serviceReplace")
	param.key = key
	param.serviceType = param.serviceType or key
	param.resolvedServiceSkin = param.resolvedServiceSkin or self:ResolveServiceSkin(param.serviceType, param)
	param.serviceSkinName = param.serviceSkinName or (param.resolvedServiceSkin and param.resolvedServiceSkin.name) or "Default"
	param.layer = param.layer or "Top"
	param.scene = param.scene or param.sceneName or self:GetCurrentScene()
	param.closeOnSceneChange = param.closeOnSceneChange ~= false

	local handle = self:CreateContainer(param.name or key)
	if handle == nil or handle <= 0 then
		recordServiceStat(self, "failed", param.serviceType, key, "createContainer")
		recordServicePerf(self, startMs, param.serviceType, false)
		return nil
	end
	if not self:AttachToLayer(handle, param.layer, param) then
		NativeApi:removeFairyGuiObject(handle)
		recordServiceStat(self, "failed", param.serviceType, key, "attachLayer")
		recordServicePerf(self, startMs, param.serviceType, false)
		return nil
	end

	if param.width ~= nil and param.height ~= nil then
		self:SetSize(handle, param.width, param.height)
	elseif param.fullScreen == true or param.adaptScreen == true then
		self:SetSize(handle, self:GetScreenWidth(), self:GetScreenHeight())
	end
	if param.x ~= nil and param.y ~= nil then
		self:SetPosition(handle, param.x, param.y)
	end
	self:SetTouchable(handle, param.touchable == true)

	local objectInfo = {
		handle = handle,
		key = key,
		name = key,
		objectName = param.serviceType,
		param = param,
		uiName = key,
		cache = false,
		layer = param.layer,
		priority = tonumber(param.priority or param.sortingPriority) or 0,
		popupGroup = self:GetPopupGroup(param),
		popupMode = param.popupMode or "stack",
		uiGroup = self:GetUIGroup(param),
		sceneName = self:GetSceneName(param),
		closeOnSceneChange = param.closeOnSceneChange ~= false,
		destroyOnSceneChange = param.destroyOnSceneChange == true,
		ownedHandles = {},
		serviceType = param.serviceType,
		serviceSkinName = param.serviceSkinName,
		serviceSkin = param.resolvedServiceSkin,
		serviceResource = param.resolvedServiceSkin and param.resolvedServiceSkin.resource or nil,
		parentHandle = param.parentHandle or param.rootHandle,
		rootLayer = param.rootLayer,
		focusOrder = param.focusOrder,
		tabFocus = param.tabFocus ~= false,
	}
	self:RegisterObject(objectInfo)
	self:AssignLayer(objectInfo, objectInfo.layer)
	self:CreateModalMask(objectInfo, param)
	self:ApplyScreenAdapt(objectInfo)
	self:PushStack(objectInfo)
	recordServiceStat(self, "created", objectInfo.serviceType, objectInfo.key, "open")
	recordServicePerf(self, startMs, objectInfo.serviceType, true)
	return objectInfo
end

function FairyGuiServiceCore.AddServiceText(self, objectInfo, name, text, x, y, width, height, fontSize, red, green, blue)
	local owner = self.owner
	if owner == nil or objectInfo == nil then
		return nil
	end

	local textHandle = self:CreateText(objectInfo.handle, name or "", tostring(text or ""), fontSize or 22, red or 255, green or 255, blue or 255)
	if textHandle == nil or textHandle <= 0 then
		return nil
	end
	self:SetPosition(textHandle, x or 0, y or 0)
	self:SetSize(textHandle, width or 120, height or 32)
	if not self:AddObjectHandleToParent(textHandle, objectInfo.handle) then
		NativeApi:removeFairyGuiObject(textHandle)
		return nil
	end
	return self:AddOwnedHandle(objectInfo, textHandle)
end

function FairyGuiServiceCore.AddServiceImage(self, objectInfo, name, url, x, y, width, height, alpha)
	local owner = self.owner
	if owner == nil or objectInfo == nil or isBlank(url) then
		return nil
	end

	local imageHandle = self:CreateLoader(objectInfo.handle, name or "", url or "")
	if imageHandle == nil or imageHandle <= 0 then
		return nil
	end
	self:SetPosition(imageHandle, x or 0, y or 0)
	self:SetSize(imageHandle, width or 120, height or 60)
	self:SetTouchable(imageHandle, false)
	if alpha ~= nil then
		self:SetAlpha(imageHandle, alpha)
	end
	if not self:AddObjectHandleToParent(imageHandle, objectInfo.handle) then
		NativeApi:removeFairyGuiObject(imageHandle)
		return nil
	end
	return self:AddOwnedHandle(objectInfo, imageHandle)
end

function FairyGuiServiceCore.AddServiceButton(self, objectInfo, name, text, x, y, width, height, callback, style)
	local owner = self.owner
	if owner == nil or objectInfo == nil then
		return nil
	end

	local buttonHandle = self:CreateContainer(name or "", objectInfo.handle)
	if buttonHandle == nil or buttonHandle <= 0 then
		return nil
	end
	self:SetPosition(buttonHandle, x or 0, y or 0)
	self:SetSize(buttonHandle, width or 120, height or 44)
	self:SetTouchable(buttonHandle, true)
	if not self:AddObjectHandleToParent(buttonHandle, objectInfo.handle) then
		NativeApi:removeFairyGuiObject(buttonHandle)
		return nil
	end
	self:AddOwnedHandle(objectInfo, buttonHandle)
	local red, green, blue = getColor(style, "textColor", 255, 255, 255)
	self:AddServiceText(objectInfo, (name or "") .. "_text", text or "", x or 0, y or 0, width or 120, height or 44, style and style.fontSize or 22, red, green, blue)
	if type(callback) == "function" then
		self:AddClick(buttonHandle, "", callback)
	end
	return buttonHandle
end

function FairyGuiServiceCore.ApplyServiceLayout(self, objectInfo)
	local owner = self.owner
	if owner == nil or objectInfo == nil then
		return false
	end

	local param = objectInfo.param or {}
	local skin = objectInfo.serviceSkin or self:ResolveServiceSkin(objectInfo.serviceType, param)
	local screenWidth = self:GetScreenWidth()
	local screenHeight = self:GetScreenHeight()
	if screenWidth <= 0 or screenHeight <= 0 then
		return false
	end

	if objectInfo.serviceType == "DebugPanel" then
		local width = param.width or 460
		local height = param.height or 236
		local rect = {
			x = param.x or math.max(screenWidth - width - (param.right or 24), 0),
			y = param.y or (param.top or 24),
			width = width,
			height = height,
		}
		self:ClampLayoutRect(rect, screenWidth, screenHeight)
		self:SetPosition(objectInfo.handle, rect.x, rect.y)
		self:SetSize(objectInfo.handle, rect.width, rect.height)
		if objectInfo.debugPanelBackgroundHandle ~= nil then
			self:SetPosition(objectInfo.debugPanelBackgroundHandle, 0, 0)
			self:SetSize(objectInfo.debugPanelBackgroundHandle, rect.width, rect.height)
		end
		return true
	end

	if objectInfo.serviceType == "Toast" and objectInfo.toastTextHandle ~= nil then
		local text = tostring(objectInfo.toastText or "")
		local minWidth = tonumber(skin.minWidth) or 240
		local paddingX = tonumber(skin.paddingX) or 80
		local charWidth = tonumber(skin.charWidth) or 14
		local width = param.width or math.min(math.max(string.len(text) * charWidth + paddingX, minWidth), math.max(screenWidth - 80, minWidth))
		local height = param.height or skin.height or 44
		local rect = {
			x = param.x or math.max((screenWidth - width) * 0.5, 0),
			y = param.y or math.max(screenHeight - (param.bottom or skin.bottom or 120), 0),
			width = width,
			height = height,
		}
		if param.fitInScreen ~= false then
			self:ClampLayoutRect(rect, screenWidth, screenHeight)
		end
		self:SetPosition(objectInfo.toastTextHandle, rect.x, rect.y)
		self:SetSize(objectInfo.toastTextHandle, rect.width, rect.height)
		objectInfo.toastLayoutRect = rect
		return true
	elseif objectInfo.serviceType == "Loading" and objectInfo.loadingTextHandle ~= nil then
		local width = param.width or skin.width or 280
		local height = param.height or skin.height or 36
		local rect = {
			x = param.x or math.max(screenWidth * 0.5 - width * 0.5, 0),
			y = param.y or math.max(screenHeight * 0.5 - height * 0.5, 0),
			width = width,
			height = height,
		}
		self:SetPosition(objectInfo.loadingTextHandle, rect.x, rect.y)
		self:SetSize(objectInfo.loadingTextHandle, rect.width, rect.height)
		objectInfo.loadingLayoutRect = rect
		return true
	elseif objectInfo.serviceType == "Tip" and objectInfo.tipTextHandle ~= nil then
		local width = param.width or skin.width or 320
		local height = param.height or skin.height or 36
		param.anchorGap = param.anchorGap or skin.anchorGap
		local anchorRect = resolveAnchorRect(self, param, param.x, param.y)
		local rect = buildAnchoredRect(self, anchorRect, width, height, param)
		self:SetPosition(objectInfo.tipTextHandle, rect.x, rect.y)
		self:SetSize(objectInfo.tipTextHandle, rect.width, rect.height)
		objectInfo.tipAnchorRect = anchorRect
		objectInfo.tipLayoutRect = rect
		return true
	elseif objectInfo.serviceType == "PopupMenu" then
		local width = param.width or skin.width or 220
		local height = param.height or skin.itemHeight or 34
		param.anchorGap = param.anchorGap or skin.anchorGap
		local anchorRect = resolveAnchorRect(self, param, param.x, param.y)
		local rect = buildAnchoredRect(self, anchorRect, width, height, param)
		self:SetPosition(objectInfo.handle, rect.x, rect.y)
		self:SetSize(objectInfo.handle, rect.width, rect.height)
		objectInfo.popupMenuAnchorRect = anchorRect
		objectInfo.popupMenuLayoutRect = rect
		return true
	end
	return false
end

function FairyGuiServiceCore.GetServiceObject(self, serviceKey)
	local owner = self.owner
	if owner == nil then
		return nil
	end
	return self:GetObjectInfo(serviceKey)
end

function FairyGuiServiceCore.CloseService(self, serviceKey, reason)
	local owner = self.owner
	if owner == nil then
		return false
	end
	return self:Close(serviceKey, true, reason or "closeService")
end

function FairyGuiServiceCore.CloseServices(self, serviceType, reason)
	local owner = self.owner
	if owner == nil then
		return 0
	end

	local keys = {}
	for key, objectInfo in pairs(self:GetStore():GetObjects()) do
		if objectInfo.serviceType ~= nil and (serviceType == nil or objectInfo.serviceType == serviceType) then
			table.insert(keys, key)
		end
	end

	local closeCount = 0
	for _, key in ipairs(keys) do
		if self:CloseService(key, reason or "closeServices") then
			closeCount = closeCount + 1
		end
	end
	return closeCount
end

function FairyGuiServiceCore.HandleServiceClosed(self, objectInfo, reason)
	local owner = self.owner
	if owner == nil or objectInfo == nil or objectInfo.serviceType == nil then
		return
	end

	local stats = recordServiceStat(self, "closed", objectInfo.serviceType, objectInfo.key, reason or "close")
	if objectInfo.serviceType == "Toast" then
		local active = self.toastActive
		if active ~= nil and active.key == objectInfo.key then
			if active.dedupeKey ~= nil then
				self.toastDedupe[active.dedupeKey] = nil
			end
			self.toastActive = nil
			syncServiceState(self)
		end
		if reason == "toastTimeout" then
			if stats ~= nil then
				stats.toastTimeoutTotal = (stats.toastTimeoutTotal or 0) + 1
			end
			self:ShowNextToast()
		elseif reason ~= "toastReplace" then
			self:ClearToastQueue()
		end
	elseif objectInfo.serviceType == "Loading" then
		self.loadingRefs = {}
		self.loadingRefTotal = 0
		syncServiceState(self)
	end
end

function FairyGuiServiceCore.GetServiceStats(self)
	local owner = self.owner
	if owner == nil then
		return {
			__meta = {
				toastQueue = 0,
				toastActive = nil,
				loadingRefTotal = 0,
				serviceOpenTotal = 0,
				serviceHiddenTotal = 0,
				serviceKindCount = 0,
				createdTotal = 0,
				closedTotal = 0,
				failedTotal = 0,
				peakOpen = 0,
			},
		}
	end

	local counters = ensureServiceStats(self)
	local stats = {
		__meta = {
			toastQueue = self:GetToastQueueCount(),
			toastActive = self.toastActive ~= nil and self.toastActive.key or nil,
			loadingRefTotal = self:GetLoadingRefCount(),
			createdTotal = counters.createdTotal or 0,
			closedTotal = counters.closedTotal or 0,
			failedTotal = counters.failedTotal or 0,
			peakOpen = counters.peakOpen or 0,
			toastQueuedTotal = counters.toastQueuedTotal or 0,
			toastDedupeIgnoredTotal = counters.toastDedupeIgnoredTotal or 0,
			toastShownTotal = counters.toastShownTotal or 0,
			toastTimeoutTotal = counters.toastTimeoutTotal or 0,
			loadingShowTotal = counters.loadingShowTotal or 0,
			loadingHideTotal = counters.loadingHideTotal or 0,
			loadingForceHideTotal = counters.loadingForceHideTotal or 0,
			loadingPeakRefTotal = counters.loadingPeakRefTotal or 0,
			lastEvent = counters.lastEvent or "",
			lastType = counters.lastType or "",
			lastKey = counters.lastKey or "",
			lastReason = counters.lastReason or "",
			byType = counters.byType or {},
		},
	}
	local serviceKinds = {}
	local serviceOpenTotal = 0
	local serviceHiddenTotal = 0
	local store = self:GetStore()
	for key, objectInfo in pairs(store:GetObjects()) do
		if objectInfo.serviceType ~= nil then
			local serviceType = objectInfo.serviceType
			local history = counters.byType and counters.byType[serviceType] or nil
			local stat = stats[serviceType] or {
				open = 0,
				hidden = 0,
				keys = {},
				created = history and history.created or 0,
				closed = history and history.closed or 0,
				failed = history and history.failed or 0,
			}
			stat.open = stat.open + 1
			if store:IsHidden(key) then
				stat.hidden = stat.hidden + 1
				serviceHiddenTotal = serviceHiddenTotal + 1
			end
			table.insert(stat.keys, key)
			stats[serviceType] = stat
			serviceKinds[serviceType] = true
			serviceOpenTotal = serviceOpenTotal + 1
		end
	end
	stats.__meta.serviceOpenTotal = serviceOpenTotal
	stats.__meta.serviceHiddenTotal = serviceHiddenTotal
	stats.__meta.serviceKindCount = tableCount(serviceKinds)
	counters.peakOpen = math.max(counters.peakOpen or 0, serviceOpenTotal)
	stats.__meta.peakOpen = counters.peakOpen
	return stats
end

function FairyGuiServiceCore.DumpServices(self)
	print("[FGUI] DumpServices begin")
	local stats = self:GetServiceStats()
	local meta = stats.__meta or {}
	for serviceType, stat in pairs(stats) do
		if serviceType ~= "__meta" then
			print("[FGUI] Service", serviceType, "open=", stat.open, "hidden=", stat.hidden, "created=", stat.created or 0, "closed=", stat.closed or 0, "failed=", stat.failed or 0, "keys=", table.concat(stat.keys, ","))
		end
	end
	print("[FGUI] ServiceMeta open=", meta.serviceOpenTotal or 0, "kind=", meta.serviceKindCount or 0, "peak=", meta.peakOpen or 0, "created=", meta.createdTotal or 0, "closed=", meta.closedTotal or 0, "failed=", meta.failedTotal or 0, "toastActive=", meta.toastActive, "toastQueue=", meta.toastQueue, "toastQueuedTotal=", meta.toastQueuedTotal or 0, "toastDedupeIgnored=", meta.toastDedupeIgnoredTotal or 0, "loadingRefs=", meta.loadingRefTotal, "loadingPeakRefs=", meta.loadingPeakRefTotal or 0, "last=", tostring(meta.lastEvent or "") .. "/" .. tostring(meta.lastType or "") .. "/" .. tostring(meta.lastReason or ""))
	print("[FGUI] DumpServices end")
end

return FairyGuiServiceCore
