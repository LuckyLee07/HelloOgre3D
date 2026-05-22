local FairyGuiServices = Class("FairyGuiServices")

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

local function ensureServiceStats(owner)
	if owner.serviceStats == nil then
		owner.serviceStats = {
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
	if owner.serviceStats.byType == nil then
		owner.serviceStats.byType = {}
	end
	return owner.serviceStats
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
	for _, objectInfo in pairs(owner.objects or {}) do
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

function FairyGuiServices:Init(owner)
	self.owner = owner
	if owner ~= nil then
		ensureServiceStats(owner)
	end
end

function FairyGuiServices:OpenServiceContainer(key, param)
	local self = self.owner
	if self == nil then
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
	param.layer = param.layer or "Top"
	param.scene = param.scene or param.sceneName or self.currentSceneName
	param.closeOnSceneChange = param.closeOnSceneChange ~= false

	local handle = self:CreateContainer(param.name or key)
	if handle == nil or handle <= 0 then
		recordServiceStat(self, "failed", param.serviceType, key, "createContainer")
		recordServicePerf(self, startMs, param.serviceType, false)
		return nil
	end
	if not self:AttachToLayer(handle, param.layer) then
		GameManager:removeFairyGuiObject(handle)
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
	}
	self.objects[key] = objectInfo
	self.objectsByHandle[handle] = objectInfo
	self.uiNameToKey[key] = key
	self.hiddenObjects[key] = nil
	self:AssignLayer(objectInfo, objectInfo.layer)
	self:CreateModalMask(objectInfo, param)
	self:ApplyScreenAdapt(objectInfo)
	self:PushStack(objectInfo)
	recordServiceStat(self, "created", objectInfo.serviceType, objectInfo.key, "open")
	recordServicePerf(self, startMs, objectInfo.serviceType, true)
	return objectInfo
end

function FairyGuiServices:AddServiceText(objectInfo, name, text, x, y, width, height, fontSize, red, green, blue)
	local self = self.owner
	if self == nil or objectInfo == nil then
		return nil
	end

	local textHandle = self:CreateText(objectInfo.handle, name or "", tostring(text or ""), fontSize or 22, red or 255, green or 255, blue or 255)
	if textHandle == nil or textHandle <= 0 then
		return nil
	end
	self:SetPosition(textHandle, x or 0, y or 0)
	self:SetSize(textHandle, width or 120, height or 32)
	if not self:AddObjectHandleToParent(textHandle, objectInfo.handle) then
		GameManager:removeFairyGuiObject(textHandle)
		return nil
	end
	return self:AddOwnedHandle(objectInfo, textHandle)
end

function FairyGuiServices:AddServiceImage(objectInfo, name, url, x, y, width, height, alpha)
	local self = self.owner
	if self == nil or objectInfo == nil or isBlank(url) then
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
		GameManager:removeFairyGuiObject(imageHandle)
		return nil
	end
	return self:AddOwnedHandle(objectInfo, imageHandle)
end

function FairyGuiServices:AddServiceButton(objectInfo, name, text, x, y, width, height, callback)
	local self = self.owner
	if self == nil or objectInfo == nil then
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
		GameManager:removeFairyGuiObject(buttonHandle)
		return nil
	end
	self:AddOwnedHandle(objectInfo, buttonHandle)
	self:AddServiceText(objectInfo, (name or "") .. "_text", text or "", x or 0, y or 0, width or 120, height or 44, 22, 255, 255, 255)
	if type(callback) == "function" then
		self:AddClick(buttonHandle, "", callback)
	end
	return buttonHandle
end

function FairyGuiServices:ApplyServiceLayout(objectInfo)
	local self = self.owner
	if self == nil or objectInfo == nil then
		return false
	end

	local param = objectInfo.param or {}
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
		local width = param.width or math.min(math.max(string.len(text) * 14 + 80, 240), math.max(screenWidth - 80, 240))
		local height = param.height or 44
		local rect = {
			x = param.x or math.max((screenWidth - width) * 0.5, 0),
			y = param.y or math.max(screenHeight - (param.bottom or 120), 0),
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
		local width = param.width or 280
		local height = param.height or 36
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
	end
	return false
end

function FairyGuiServices:GetServiceObject(serviceKey)
	local self = self.owner
	if self == nil then
		return nil
	end
	return self:GetObjectInfo(serviceKey)
end

function FairyGuiServices:CloseService(serviceKey, reason)
	local self = self.owner
	if self == nil then
		return false
	end
	return self:Close(serviceKey, true, reason or "closeService")
end

function FairyGuiServices:CloseServices(serviceType, reason)
	local self = self.owner
	if self == nil then
		return 0
	end

	local keys = {}
	for key, objectInfo in pairs(self.objects) do
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

function FairyGuiServices:HandleServiceClosed(objectInfo, reason)
	local self = self.owner
	if self == nil or objectInfo == nil or objectInfo.serviceType == nil then
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
	end
end

function FairyGuiServices:ClearToastQueue()
	local self = self.owner
	if self == nil then
		return
	end

	self.toastQueue = {}
	self.toastDedupe = {}
	if self.toastActive ~= nil and self.toastActive.dedupeKey ~= nil then
		self.toastDedupe[self.toastActive.dedupeKey] = true
	end
end

function FairyGuiServices:GetToastQueueCount()
	local self = self.owner
	if self == nil then
		return 0
	end
	return #self.toastQueue
end

function FairyGuiServices:CreateToastRequest(text, duration, param)
	local self = self.owner
	if self == nil then
		return nil, "manager"
	end

	param = copyTable(param)
	local dedupeKey = nil
	if param.dedupe ~= false then
		dedupeKey = param.dedupeKey or tostring(text or "")
		if not isBlank(dedupeKey) and self.toastDedupe[dedupeKey] == true then
			local stats = ensureServiceStats(self)
			stats.toastDedupeIgnoredTotal = (stats.toastDedupeIgnoredTotal or 0) + 1
			stats.lastEvent = "toastDedupe"
			stats.lastType = "Toast"
			stats.lastKey = dedupeKey
			stats.lastReason = "dedupe"
			return nil, "dedupe"
		end
	end

	self.toastSerial = (self.toastSerial or 0) + 1
	local request = {
		id = self.toastSerial,
		text = text or "",
		duration = tonumber(duration or param.duration) or 2,
		param = param,
		dedupeKey = dedupeKey,
	}
	if dedupeKey ~= nil then
		self.toastDedupe[dedupeKey] = true
	end
	return request, nil
end

function FairyGuiServices:OpenToastRequest(request)
	local self = self.owner
	if self == nil or request == nil then
		return nil
	end

	local param = copyTable(request.param)
	param.key = param.key or "__Toast"
	param.layer = param.layer or "Toast"
	param.stackMode = param.stackMode or "None"
	param.fullScreen = true
	param.touchable = false
	param.serviceType = "Toast"

	local objectInfo = self:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		if request.dedupeKey ~= nil then
			self.toastDedupe[request.dedupeKey] = nil
		end
		return nil
	end

	objectInfo.toastRequestId = request.id
	objectInfo.toastText = request.text or ""
	objectInfo.toastTextHandle = self:AddServiceText(objectInfo, "toast_text", objectInfo.toastText, 0, 0, param.width or 240, param.height or 44, param.fontSize or 22, 255, 244, 200)
	self:ApplyServiceLayout(objectInfo)
	local stats = ensureServiceStats(self)
	stats.toastShownTotal = (stats.toastShownTotal or 0) + 1

	self.toastActive = {
		id = request.id,
		key = objectInfo.key,
		handle = objectInfo.handle,
		dedupeKey = request.dedupeKey,
	}
	if request.duration > 0 then
		self:Delay(objectInfo.key, request.duration, function()
			self:CloseUI(objectInfo.key, true, "toastTimeout")
		end)
	end
	return objectInfo.handle
end

function FairyGuiServices:ShowNextToast()
	local self = self.owner
	if self == nil or self.toastActive ~= nil or #self.toastQueue <= 0 then
		return nil
	end
	local request = table.remove(self.toastQueue, 1)
	return self:OpenToastRequest(request)
end

function FairyGuiServices:ShowToast(text, duration, param)
	local self = self.owner
	if self == nil then
		return nil
	end

	local request = nil
	local ignored = nil
	request, ignored = self:CreateToastRequest(text, duration, param)
	if request == nil then
		return self.toastActive and self.toastActive.handle or true
	end

	local requestParam = request.param or {}
	if requestParam.queue == false and self.toastActive ~= nil then
		self.toastQueue = {}
		self:CloseUI(self.toastActive.key, true, "toastReplace")
	end
	if self.toastActive ~= nil then
		table.insert(self.toastQueue, request)
		local stats = ensureServiceStats(self)
		stats.toastQueuedTotal = (stats.toastQueuedTotal or 0) + 1
		return request.id
	end
	return self:OpenToastRequest(request)
end

function FairyGuiServices:CloseToast(reason)
	local self = self.owner
	if self == nil then
		return false
	end

	local active = self.toastActive
	if active == nil then
		self:ClearToastQueue()
		return false
	end
	return self:CloseUI(active.key, true, reason or "closeToast")
end

function FairyGuiServices:ShowTip(text, x, y, duration, param)
	local self = self.owner
	if self == nil then
		return nil
	end

	param = copyTable(param)
	param.key = param.key or "__Tip"
	param.layer = param.layer or "Top"
	param.stackMode = param.stackMode or "None"
	param.fullScreen = true
	param.touchable = false
	param.serviceType = "Tip"

	local objectInfo = self:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		return nil
	end
	self:AddServiceText(objectInfo, "tip_text", text or "", x or 20, y or 20, param.width or 320, param.height or 36, param.fontSize or 20, 180, 230, 255)

	local timeout = tonumber(duration or param.duration) or 2
	if timeout > 0 then
		self:Delay(objectInfo.key, timeout, function()
			self:CloseUI(objectInfo.key, true, "tipTimeout")
		end)
	end
	return objectInfo.handle
end

function FairyGuiServices:ShowLoading(text, param)
	local self = self.owner
	if self == nil then
		return nil
	end

	param = copyTable(param)
	local refKey = param.refKey or "Default"
	self.loadingRefs[refKey] = (self.loadingRefs[refKey] or 0) + 1
	self.loadingRefTotal = (self.loadingRefTotal or 0) + 1
	local stats = ensureServiceStats(self)
	stats.loadingShowTotal = (stats.loadingShowTotal or 0) + 1
	stats.loadingPeakRefTotal = math.max(stats.loadingPeakRefTotal or 0, self.loadingRefTotal or 0)

	local existing = self:GetObjectInfo("__Loading")
	if existing ~= nil then
		existing.loadingText = text or existing.loadingText
		if not isBlank(text) then
			self:SetText(existing.handle, "loading_text", text)
		end
		self:ApplyServiceLayout(existing)
		return existing.handle
	end

	param.key = param.key or "__Loading"
	param.layer = param.layer or "Top"
	param.stackMode = param.stackMode or "Popup"
	param.popupGroup = param.popupGroup or "Loading"
	param.popupMode = param.popupMode or "replace"
	param.fullScreen = true
	param.modal = param.modal ~= false
	param.closeOnMaskClick = false
	param.touchable = false
	param.serviceType = "Loading"

	local objectInfo = self:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		self.loadingRefs[refKey] = math.max((self.loadingRefs[refKey] or 1) - 1, 0)
		self.loadingRefTotal = math.max((self.loadingRefTotal or 1) - 1, 0)
		return nil
	end
	objectInfo.loadingText = text or "Loading..."
	objectInfo.loadingTextHandle = self:AddServiceText(objectInfo, "loading_text", text or "Loading...", 0, 0, param.width or 280, param.height or 36, param.fontSize or 24, 255, 255, 255)
	self:ApplyServiceLayout(objectInfo)
	local timeout = tonumber(param.timeout)
	if timeout ~= nil and timeout > 0 then
		self:Delay(objectInfo.key, timeout, function()
			self:HideLoading({ force = true, reason = "loadingTimeout" })
		end)
	end
	return objectInfo.handle
end

function FairyGuiServices:GetLoadingRefCount()
	local self = self.owner
	if self == nil then
		return 0
	end
	return self.loadingRefTotal or 0
end

function FairyGuiServices:HideLoading(paramOrReason)
	local self = self.owner
	if self == nil then
		return false
	end

	local param = type(paramOrReason) == "table" and paramOrReason or { reason = paramOrReason }
	local refKey = param.refKey or "Default"
	local force = param.force == true
	local reason = param.reason or "hideLoading"
	local stats = ensureServiceStats(self)
	stats.loadingHideTotal = (stats.loadingHideTotal or 0) + 1
	if force then
		stats.loadingForceHideTotal = (stats.loadingForceHideTotal or 0) + 1
	end

	if not force then
		local refCount = self.loadingRefs[refKey] or 0
		if refCount > 0 then
			self.loadingRefs[refKey] = refCount - 1
			self.loadingRefTotal = math.max((self.loadingRefTotal or 1) - 1, 0)
			if self.loadingRefs[refKey] <= 0 then
				self.loadingRefs[refKey] = nil
			end
		end
		if (self.loadingRefTotal or 0) > 0 then
			return true
		end
	end

	self.loadingRefs = {}
	self.loadingRefTotal = 0
	return self:Close("__Loading", true, reason)
end

function FairyGuiServices:ShowGuideMask(param)
	local self = self.owner
	if self == nil then
		return nil
	end

	param = copyTable(param)
	local highlightRect = self:GetGuideMaskRect(param)
	param.key = param.key or "__GuideMask"
	param.layer = param.layer or "Guide"
	param.stackMode = param.stackMode or "Popup"
	param.popupGroup = param.popupGroup or "GuideMask"
	param.popupMode = param.popupMode or "replace"
	param.fullScreen = true
	param.modal = highlightRect == nil
	param.modalAlpha = param.modalAlpha or 0.55
	param.closeOnMaskClick = param.closeOnMaskClick == true
	param.touchable = false
	param.serviceType = "GuideMask"

	local objectInfo = self:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		return nil
	end
	if highlightRect ~= nil then
		self:CreateGuideMaskSegments(objectInfo, param)
	end
	if not isBlank(param.text) then
		self:AddServiceText(objectInfo, "guide_text", param.text, param.textX or 80, param.textY or 80, param.textWidth or 520, param.textHeight or 48, param.fontSize or 24, 255, 255, 255)
	end
	return objectInfo.handle
end

function FairyGuiServices:HideGuideMask(reason)
	local self = self.owner
	if self == nil then
		return false
	end
	return self:Close("__GuideMask", true, reason or "hideGuideMask")
end

function FairyGuiServices:ShowMessageBox(title, message, buttons, callback, param)
	local self = self.owner
	if self == nil then
		return nil
	end

	param = copyTable(param)
	param.key = param.key or "__MessageBox"
	param.layer = param.layer or "Top"
	param.stackMode = param.stackMode or "Popup"
	param.popupGroup = param.popupGroup or "MessageBox"
	param.popupMode = param.popupMode or "replace"
	param.width = param.width or 460
	param.height = param.height or 240
	param.modal = param.modal ~= false
	param.modalAlpha = param.modalAlpha or 0.45
	param.closeOnMaskClick = param.closeOnMaskClick == true
	param.closeOnEscape = param.closeOnEscape ~= false
	param.touchable = true
	param.serviceType = "MessageBox"
	if param.center ~= false then
		if param.x == nil and param.alignX == nil and param.hAlign == nil then
			param.alignX = "center"
		end
		if param.y == nil and param.alignY == nil and param.vAlign == nil then
			param.alignY = "middle"
		end
	end
	param.fitInScreen = param.fitInScreen ~= false

	buttons = type(buttons) == "table" and buttons or { "OK" }

	local objectInfo = self:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		return nil
	end
	self:AddServiceText(objectInfo, "message_title", title or "", 24, 24, param.width - 48, 34, 24, 255, 236, 180)
	self:AddServiceText(objectInfo, "message_body", message or "", 24, 72, param.width - 48, 80, 20, 230, 230, 230)

	local buttonWidth = param.buttonWidth or 110
	local buttonGap = param.buttonGap or 16
	local totalWidth = #buttons * buttonWidth + math.max(#buttons - 1, 0) * buttonGap
	local startX = math.max((param.width - totalWidth) * 0.5, 0)
	for index, buttonText in ipairs(buttons) do
		local label = type(buttonText) == "table" and (buttonText.text or buttonText.label or tostring(index)) or tostring(buttonText)
		self:AddServiceButton(objectInfo, "message_button_" .. tostring(index), label, startX + (index - 1) * (buttonWidth + buttonGap), param.height - 68, buttonWidth, 44, function()
			if type(callback) == "function" then
				callback(index, label)
			end
			self:CloseUI(objectInfo.key, true, "messageBoxButton")
		end)
	end
	return objectInfo.handle
end

function FairyGuiServices:ShowDialog(title, message, buttons, callback, param)
	local self = self.owner
	if self == nil then
		return nil
	end
	return self:ShowMessageBox(title, message, buttons, callback, param)
end

function FairyGuiServices:ShowPopupMenu(items, x, y, callback, param)
	local self = self.owner
	if self == nil then
		return nil
	end

	param = copyTable(param)
	param.key = param.key or "__PopupMenu"
	param.layer = param.layer or "Top"
	param.stackMode = param.stackMode or "Popup"
	param.popupGroup = param.popupGroup or "PopupMenu"
	param.popupMode = param.popupMode or "replace"
	param.modal = param.modal ~= false
	param.modalAlpha = param.modalAlpha or 0.05
	param.closeOnMaskClick = param.closeOnMaskClick ~= false
	param.touchable = true
	param.serviceType = "PopupMenu"

	items = type(items) == "table" and items or {}
	local itemHeight = param.itemHeight or 34
	param.width = param.width or 220
	param.height = param.height or math.max(#items * itemHeight, itemHeight)
	param.x = x or param.x or 0
	param.y = y or param.y or 0
	param.fitInScreen = param.fitInScreen ~= false

	local objectInfo = self:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		return nil
	end
	for index, item in ipairs(items) do
		local label = type(item) == "table" and (item.text or item.label or tostring(index)) or tostring(item)
		self:AddServiceButton(objectInfo, "popup_item_" .. tostring(index), label, 0, (index - 1) * itemHeight, param.width, itemHeight, function()
			if type(callback) == "function" then
				callback(index, item)
			end
			self:CloseUI(objectInfo.key, true, "popupMenuSelect")
		end)
	end
	return objectInfo.handle
end

function FairyGuiServices:GetServiceStats()
	local self = self.owner
	if self == nil then
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
	for key, objectInfo in pairs(self.objects) do
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
			if self.hiddenObjects[key] ~= nil then
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

function FairyGuiServices:DumpServices()
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
