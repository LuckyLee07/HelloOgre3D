local FairyGuiServiceToast = {}

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
	service.serviceStats = state.serviceStats
	return state.serviceStats
end

local function syncServiceState(service)
	local state = service:GetStore():GetServiceState()
	state.toastQueue = service.toastQueue or {}
	state.toastActive = service.toastActive
	state.toastSerial = service.toastSerial or 0
	state.toastDedupe = service.toastDedupe or {}
	state.loadingRefs = service.loadingRefs or {}
	state.loadingRefTotal = service.loadingRefTotal or 0
	state.serviceStats = service.serviceStats or state.serviceStats
	state.serviceSkins = service.serviceSkins or state.serviceSkins
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

function FairyGuiServiceToast.ClearToastQueue(service)
	if service == nil or service.owner == nil then
		return
	end

	service.toastQueue = {}
	service.toastDedupe = {}
	if service.toastActive ~= nil and service.toastActive.dedupeKey ~= nil then
		service.toastDedupe[service.toastActive.dedupeKey] = true
	end
	syncServiceState(service)
end

function FairyGuiServiceToast.GetToastQueueCount(service)
	if service == nil or service.owner == nil then
		return 0
	end
	return #service.toastQueue
end

function FairyGuiServiceToast.CreateToastRequest(service, text, duration, param)
	if service == nil or service.owner == nil then
		return nil, "manager"
	end

	param = copyTable(param)
	local dedupeKey = nil
	if param.dedupe ~= false then
		dedupeKey = param.dedupeKey or tostring(text or "")
		if not isBlank(dedupeKey) and service.toastDedupe[dedupeKey] == true then
			local stats = ensureServiceStats(service)
			stats.toastDedupeIgnoredTotal = (stats.toastDedupeIgnoredTotal or 0) + 1
			stats.lastEvent = "toastDedupe"
			stats.lastType = "Toast"
			stats.lastKey = dedupeKey
			stats.lastReason = "dedupe"
			return nil, "dedupe"
		end
	end

	service.toastSerial = (service.toastSerial or 0) + 1
	local request = {
		id = service.toastSerial,
		text = text or "",
		duration = tonumber(duration or param.duration) or 2,
		param = param,
		dedupeKey = dedupeKey,
	}
	if dedupeKey ~= nil then
		service.toastDedupe[dedupeKey] = true
	end
	syncServiceState(service)
	return request, nil
end

function FairyGuiServiceToast.OpenToastRequest(service, request)
	if service == nil or service.owner == nil or request == nil then
		return nil
	end

	local param = copyTable(request.param)
	param.key = param.key or "__Toast"
	param.layer = param.layer or "Toast"
	param.stackMode = param.stackMode or "None"
	param.fullScreen = true
	param.touchable = false
	param.serviceType = "Toast"

	local objectInfo = service:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		if request.dedupeKey ~= nil then
			service.toastDedupe[request.dedupeKey] = nil
			syncServiceState(service)
		end
		return nil
	end

	objectInfo.toastRequestId = request.id
	objectInfo.toastText = request.text or ""
	local skin = objectInfo.serviceSkin or service:ResolveServiceSkin("Toast", param)
	local red, green, blue = getColor(skin, "textColor", 255, 244, 200)
	objectInfo.toastTextHandle = service:AddServiceText(objectInfo, "toast_text", objectInfo.toastText, 0, 0, param.width or skin.minWidth or 240, param.height or skin.height or 44, param.fontSize or skin.fontSize or 22, red, green, blue)
	service:ApplyServiceLayout(objectInfo)
	local stats = ensureServiceStats(service)
	stats.toastShownTotal = (stats.toastShownTotal or 0) + 1

	service.toastActive = {
		id = request.id,
		key = objectInfo.key,
		handle = objectInfo.handle,
		dedupeKey = request.dedupeKey,
	}
	syncServiceState(service)
	if request.duration > 0 then
		service:Delay(objectInfo.key, request.duration, function()
			service:CloseUI(objectInfo.key, true, "toastTimeout")
		end)
	end
	return objectInfo.handle
end

function FairyGuiServiceToast.ShowNextToast(service)
	if service == nil or service.owner == nil or service.toastActive ~= nil or #service.toastQueue <= 0 then
		return nil
	end
	local request = table.remove(service.toastQueue, 1)
	syncServiceState(service)
	return FairyGuiServiceToast.OpenToastRequest(service, request)
end

function FairyGuiServiceToast.ShowToast(service, text, duration, param)
	if service == nil or service.owner == nil then
		return nil
	end

	local request = nil
	local ignored = nil
	request, ignored = FairyGuiServiceToast.CreateToastRequest(service, text, duration, param)
	if request == nil then
		return service.toastActive and service.toastActive.handle or true
	end

	local requestParam = request.param or {}
	if requestParam.queue == false and service.toastActive ~= nil then
		service.toastQueue = {}
		syncServiceState(service)
		service:CloseUI(service.toastActive.key, true, "toastReplace")
	end
	if service.toastActive ~= nil then
		table.insert(service.toastQueue, request)
		local stats = ensureServiceStats(service)
		stats.toastQueuedTotal = (stats.toastQueuedTotal or 0) + 1
		syncServiceState(service)
		return request.id
	end
	return FairyGuiServiceToast.OpenToastRequest(service, request)
end

function FairyGuiServiceToast.CloseToast(service, reason)
	if service == nil or service.owner == nil then
		return false
	end

	local active = service.toastActive
	if active == nil then
		FairyGuiServiceToast.ClearToastQueue(service)
		return false
	end
	return service:CloseUI(active.key, true, reason or "closeToast")
end

return FairyGuiServiceToast
