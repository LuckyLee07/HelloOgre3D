local FairyGuiServiceMessageBox = {}

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

function FairyGuiServiceMessageBox.ShowMessageBox(service, title, message, buttons, callback, param)
	if service == nil or service.owner == nil then
		return nil
	end

	param = copyTable(param)
	local skin = service:ResolveServiceSkin("MessageBox", param)
	param.key = param.key or "__MessageBox"
	param.layer = param.layer or "Top"
	param.stackMode = param.stackMode or "Popup"
	param.popupGroup = param.popupGroup or "MessageBox"
	param.popupMode = param.popupMode or "replace"
	param.width = param.width or skin.width or 460
	param.height = param.height or skin.height or 240
	param.modal = param.modal ~= false
	param.resolvedServiceSkin = skin
	param.serviceSkinName = param.serviceSkinName or skin.name
	param.modalAlpha = param.modalAlpha or skin.modalAlpha or 0.45
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

	local objectInfo = service:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		return nil
	end
	local titleRed, titleGreen, titleBlue = getColor(objectInfo.serviceSkin or skin, "titleColor", 255, 236, 180)
	local bodyRed, bodyGreen, bodyBlue = getColor(objectInfo.serviceSkin or skin, "bodyColor", 230, 230, 230)
	service:AddServiceText(objectInfo, "message_title", title or "", 24, 24, param.width - 48, 34, param.titleFontSize or skin.titleFontSize or 24, titleRed, titleGreen, titleBlue)
	service:AddServiceText(objectInfo, "message_body", message or "", 24, 72, param.width - 48, 80, param.bodyFontSize or skin.bodyFontSize or 20, bodyRed, bodyGreen, bodyBlue)

	local buttonWidth = param.buttonWidth or skin.buttonWidth or 110
	local buttonHeight = param.buttonHeight or skin.buttonHeight or 44
	local buttonGap = param.buttonGap or skin.buttonGap or 16
	local totalWidth = #buttons * buttonWidth + math.max(#buttons - 1, 0) * buttonGap
	local startX = math.max((param.width - totalWidth) * 0.5, 0)
	local buttonRed, buttonGreen, buttonBlue = getColor(objectInfo.serviceSkin or skin, "buttonTextColor", 255, 255, 255)
	local buttonStyle = {
		fontSize = param.buttonFontSize or skin.buttonFontSize or 22,
		textColor = { buttonRed, buttonGreen, buttonBlue },
	}
	for index, buttonText in ipairs(buttons) do
		local label = type(buttonText) == "table" and (buttonText.text or buttonText.label or tostring(index)) or tostring(buttonText)
		service:AddServiceButton(objectInfo, "message_button_" .. tostring(index), label, startX + (index - 1) * (buttonWidth + buttonGap), param.height - buttonHeight - 24, buttonWidth, buttonHeight, function()
			if type(callback) == "function" then
				callback(index, label)
			end
			service:CloseUI(objectInfo.key, true, "messageBoxButton")
		end, buttonStyle)
	end
	return objectInfo.handle
end

function FairyGuiServiceMessageBox.ShowDialog(service, title, message, buttons, callback, param)
	if service == nil or service.owner == nil then
		return nil
	end
	return FairyGuiServiceMessageBox.ShowMessageBox(service, title, message, buttons, callback, param)
end

return FairyGuiServiceMessageBox
