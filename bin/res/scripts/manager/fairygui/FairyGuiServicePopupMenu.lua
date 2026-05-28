local FairyGuiServicePopupMenu = {}

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

local function normalizeRect(service, rect, param)
	if service == nil or type(rect) ~= "table" then
		return nil
	end

	local applied = service.ApplyDesignRect ~= nil and service:ApplyDesignRect(rect, param or {}) or rect
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

local function resolveAnchorRect(service, param, fallbackX, fallbackY)
	param = param or {}
	local rect = normalizeRect(service, param.anchorRect or param.followRect or param.targetRect or param.rect, param)
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

local function buildAnchoredRect(service, anchorRect, width, height, param)
	param = param or {}
	width = math.max(tonumber(width) or 0, 0)
	height = math.max(tonumber(height) or 0, 0)
	local screenWidth = service ~= nil and service:GetScreenWidth() or 0
	local screenHeight = service ~= nil and service:GetScreenHeight() or 0
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
	if service ~= nil and param.fitInScreen ~= false and service.ClampLayoutRect ~= nil and screenWidth > 0 and screenHeight > 0 then
		service:ClampLayoutRect(rect, screenWidth, screenHeight)
	end
	return rect
end

function FairyGuiServicePopupMenu.ShowPopupMenu(service, items, x, y, callback, param)
	if service == nil or service.owner == nil then
		return nil
	end

	param = copyTable(param)
	local skin = service:ResolveServiceSkin("PopupMenu", param)
	param.key = param.key or "__PopupMenu"
	param.layer = param.layer or "Top"
	param.stackMode = param.stackMode or "Popup"
	param.popupGroup = param.popupGroup or "PopupMenu"
	param.popupMode = param.popupMode or "replace"
	param.modal = param.modal ~= false
	param.resolvedServiceSkin = skin
	param.serviceSkinName = param.serviceSkinName or skin.name
	param.modalAlpha = param.modalAlpha or skin.modalAlpha or 0.05
	param.closeOnMaskClick = param.closeOnMaskClick ~= false
	param.touchable = true
	param.serviceType = "PopupMenu"

	items = type(items) == "table" and items or {}
	local itemHeight = param.itemHeight or skin.itemHeight or 34
	param.width = param.width or skin.width or 220
	param.height = param.height or math.max(#items * itemHeight, itemHeight)
	param.fitInScreen = param.fitInScreen ~= false
	param.anchorGap = param.anchorGap or skin.anchorGap
	local anchorRect = resolveAnchorRect(service, param, x, y)
	local layoutRect = buildAnchoredRect(service, anchorRect, param.width, param.height, param)
	param.x = layoutRect.x
	param.y = layoutRect.y

	local objectInfo = service:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		return nil
	end
	objectInfo.popupMenuAnchorRect = anchorRect
	objectInfo.popupMenuLayoutRect = layoutRect
	local red, green, blue = getColor(objectInfo.serviceSkin or skin, "itemTextColor", 255, 255, 255)
	local itemStyle = {
		fontSize = param.fontSize or skin.fontSize or 22,
		textColor = { red, green, blue },
	}
	for index, item in ipairs(items) do
		local label = type(item) == "table" and (item.text or item.label or tostring(index)) or tostring(item)
		service:AddServiceButton(objectInfo, "popup_item_" .. tostring(index), label, 0, (index - 1) * itemHeight, param.width, itemHeight, function()
			if type(callback) == "function" then
				callback(index, item)
			end
			service:CloseUI(objectInfo.key, true, "popupMenuSelect")
		end, itemStyle)
	end
	return objectInfo.handle
end

return FairyGuiServicePopupMenu
