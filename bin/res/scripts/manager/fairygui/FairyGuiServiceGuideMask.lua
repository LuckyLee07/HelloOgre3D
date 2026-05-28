local FairyGuiServiceGuideMask = {}

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

function FairyGuiServiceGuideMask.ShowGuideMask(service, param)
	if service == nil or service.owner == nil then
		return nil
	end

	param = copyTable(param)
	local highlightRects = service:GetGuideMaskRects(param)
	local skin = service:ResolveServiceSkin("GuideMask", param)
	param.key = param.key or "__GuideMask"
	param.layer = param.layer or "Guide"
	param.stackMode = param.stackMode or "Popup"
	param.popupGroup = param.popupGroup or "GuideMask"
	param.popupMode = param.popupMode or "replace"
	param.fullScreen = true
	param.modal = highlightRects == nil
	param.resolvedServiceSkin = skin
	param.serviceSkinName = param.serviceSkinName or skin.name
	param.modalAlpha = param.modalAlpha or skin.modalAlpha or 0.55
	param.closeOnMaskClick = param.closeOnMaskClick == true
	param.touchable = false
	param.serviceType = "GuideMask"

	local objectInfo = service:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		return nil
	end
	if highlightRects ~= nil then
		service:CreateGuideMaskSegments(objectInfo, param)
	end
	if not isBlank(param.text) then
		local red, green, blue = getColor(objectInfo.serviceSkin or skin, "textColor", 255, 255, 255)
		service:AddServiceText(objectInfo, "guide_text", param.text, param.textX or skin.textX or 80, param.textY or skin.textY or 80, param.textWidth or skin.textWidth or 520, param.textHeight or skin.textHeight or 48, param.fontSize or skin.fontSize or 24, red, green, blue)
	end
	return objectInfo.handle
end

function FairyGuiServiceGuideMask.HideGuideMask(service, reason)
	if service == nil or service.owner == nil then
		return false
	end
	return service:Close("__GuideMask", true, reason or "hideGuideMask")
end

return FairyGuiServiceGuideMask
