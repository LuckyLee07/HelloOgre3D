local FairyGuiServiceTip = {}

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

function FairyGuiServiceTip.ShowTip(service, text, x, y, duration, param)
	if service == nil or service.owner == nil then
		return nil
	end

	param = copyTable(param)
	param.key = param.key or "__Tip"
	param.layer = param.layer or "Top"
	param.stackMode = param.stackMode or "None"
	param.fullScreen = true
	param.touchable = false
	param.serviceType = "Tip"
	param.x = x or param.x or 20
	param.y = y or param.y or 20

	local objectInfo = service:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		return nil
	end
	objectInfo.tipText = text or ""
	local skin = objectInfo.serviceSkin or service:ResolveServiceSkin("Tip", param)
	local red, green, blue = getColor(skin, "textColor", 180, 230, 255)
	objectInfo.tipTextHandle = service:AddServiceText(objectInfo, "tip_text", objectInfo.tipText, param.x, param.y, param.width or skin.width or 320, param.height or skin.height or 36, param.fontSize or skin.fontSize or 20, red, green, blue)
	service:ApplyServiceLayout(objectInfo)

	local delay = tonumber(param.delay or param.hoverDelay) or 0
	if delay > 0 then
		objectInfo.tipPending = true
		service:SetVisible(objectInfo.handle, false)
		service:Delay(objectInfo.key, delay, function()
			local tipInfo = service:GetObjectInfo(objectInfo.key)
			if tipInfo ~= nil then
				tipInfo.tipPending = false
				service:SetVisible(tipInfo.handle, true)
			end
		end)
	end

	local timeout = tonumber(duration or param.duration) or 2
	if timeout > 0 then
		service:Delay(objectInfo.key, timeout, function()
			service:CloseUI(objectInfo.key, true, "tipTimeout")
		end)
	end
	return objectInfo.handle
end

function FairyGuiServiceTip.ShowHoverTip(service, text, anchorRect, param)
	if service == nil or service.owner == nil then
		return nil
	end

	param = copyTable(param)
	param.anchorRect = param.anchorRect or anchorRect
	if param.delay == nil and param.hoverDelay == nil then
		param.hoverDelay = 0.25
	end
	param.placement = param.placement or "topLeft"
	return FairyGuiServiceTip.ShowTip(service, text, nil, nil, param.duration, param)
end

function FairyGuiServiceTip.HideTip(service, reason)
	if service == nil or service.owner == nil then
		return false
	end
	return service:Close("__Tip", true, reason or "hideTip")
end

return FairyGuiServiceTip
