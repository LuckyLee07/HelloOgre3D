local NativeApi = require("res.scripts.manager.fairygui.FairyGuiNativeApi")

local FairyGuiControls = Class("FairyGuiControls")

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

local function utf8CharLen(byteValue)
	if byteValue == nil then
		return 0
	end
	if byteValue < 0x80 then
		return 1
	elseif byteValue < 0xE0 then
		return 2
	elseif byteValue < 0xF0 then
		return 3
	elseif byteValue < 0xF8 then
		return 4
	end
	return 1
end

local function utf8CodepointCount(text)
	text = tostring(text or "")
	local index = 1
	local count = 0
	while index <= #text do
		index = index + math.max(utf8CharLen(string.byte(text, index)), 1)
		count = count + 1
	end
	return count
end

local function utf8SubCodepoints(text, maxCount)
	text = tostring(text or "")
	maxCount = tonumber(maxCount) or 0
	if maxCount <= 0 then
		return ""
	end

	local index = 1
	local count = 0
	while index <= #text and count < maxCount do
		index = index + math.max(utf8CharLen(string.byte(text, index)), 1)
		count = count + 1
	end
	return string.sub(text, 1, index - 1)
end

local function eachUtf8Char(text, callback)
	text = tostring(text or "")
	local index = 1
	while index <= #text do
		local charLen = math.max(utf8CharLen(string.byte(text, index)), 1)
		callback(string.sub(text, index, index + charLen - 1))
		index = index + charLen
	end
end

local function applyTextRestrict(text, policy)
	text = tostring(text or "")
	if type(policy) ~= "table" then
		return text
	end

	local inputType = tostring(policy.inputType or policy.type or "")
	local restrict = policy.restrict or policy.allowedChars
	if inputType == "" and isBlank(restrict) then
		return text
	end

	local output = {}
	eachUtf8Char(text, function(ch)
		local keep = true
		if inputType == "number" or inputType == "numeric" then
			keep = string.match(ch, "%d") ~= nil
				or (ch == "." and policy.allowDecimal == true)
				or (ch == "-" and policy.allowNegative == true and #output == 0)
		elseif inputType == "integer" then
			keep = string.match(ch, "%d") ~= nil
				or (ch == "-" and policy.allowNegative == true and #output == 0)
		end
		if keep and not isBlank(restrict) then
			keep = string.find(tostring(restrict), ch, 1, true) ~= nil
		end
		if keep then
			table.insert(output, ch)
		end
	end)
	return table.concat(output)
end

local function applyTextInputPolicy(text, policy)
	local normalized = applyTextRestrict(text, policy)
	local maxLength = tonumber(policy and (policy.maxLength or policy.maxLen) or nil)
	if maxLength ~= nil and maxLength > 0 and utf8CodepointCount(normalized) > maxLength then
		normalized = utf8SubCodepoints(normalized, maxLength)
	end
	return normalized
end

function FairyGuiControls:Init(owner)
	self.owner = owner
	self.textInputPoliciesByHandle = owner ~= nil and owner.textInputPoliciesByHandle or self.textInputPoliciesByHandle or {}
	self.textInputPolicyBindingsByHandle = owner ~= nil and owner.textInputPolicyBindingsByHandle or self.textInputPolicyBindingsByHandle or {}
	if owner ~= nil then
		owner.textInputPoliciesByHandle = self.textInputPoliciesByHandle
		owner.textInputPolicyBindingsByHandle = self.textInputPolicyBindingsByHandle
	end
end

function FairyGuiControls:ApplyTextInputPolicy(handle, childPath)
	local self = self.owner
	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	local policy = self.textInputPoliciesByHandle[targetHandle]
	if type(policy) ~= "table" then
		return true
	end
	if policy._applying == true then
		return true
	end

	local text = self:GetText(targetHandle, nil)
	local normalized = applyTextInputPolicy(text, policy)
	if normalized ~= text then
		policy._applying = true
		self:SetText(targetHandle, nil, normalized)
		policy._applying = nil
	end
	return true
end

function FairyGuiControls:SetTextInputPolicy(handle, childPath, policy)
	local self = self.owner
	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		self:RecordResourceFallback("missingTextInputPolicyTarget", {
			handle = handle,
			childPath = childPath,
		}, "SetTextInputPolicy target not found")
		return false
	end

	if type(policy) ~= "table" then
		local bindingId = self.textInputPolicyBindingsByHandle[targetHandle]
		if bindingId ~= nil then
			self:RemoveBinding(bindingId)
		end
		self.textInputPoliciesByHandle[targetHandle] = nil
		self.textInputPolicyBindingsByHandle[targetHandle] = nil
		return true
	end

	local policyCopy = copyTable(policy)
	self.textInputPoliciesByHandle[targetHandle] = policyCopy
	if self.textInputPolicyBindingsByHandle[targetHandle] == nil then
		local bindingId = self:AddChanged(handle, childPath, function()
			self:ApplyTextInputPolicy(targetHandle, nil)
		end)
		if bindingId ~= nil then
			self.textInputPolicyBindingsByHandle[targetHandle] = bindingId
		end
	end
	self:ApplyTextInputPolicy(targetHandle, nil)
	return true
end

function FairyGuiControls:GetTextInputPolicy(handle, childPath)
	local self = self.owner
	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return nil
	end
	local policy = self.textInputPoliciesByHandle[targetHandle]
	if type(policy) ~= "table" then
		return nil
	end
	return copyTable(policy)
end

function FairyGuiControls:ClearTextInputPoliciesForHandles(handles)
	local self = self.owner
	if type(handles) ~= "table" then
		return
	end
	for _, handle in ipairs(handles) do
		local bindingId = self.textInputPolicyBindingsByHandle[handle]
		if bindingId ~= nil then
			self.textInputPolicyBindingsByHandle[handle] = nil
		end
		self.textInputPoliciesByHandle[handle] = nil
	end
end

function FairyGuiControls:SetText(handle, childPath, text)
	local self = self.owner
	if NativeApi == nil then
		return false
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return NativeApi:setFairyGuiObjectText(targetHandle, tostring(text or ""))
end

function FairyGuiControls:GetText(handle, childPath)
	local self = self.owner
	if NativeApi == nil or NativeApi.getFairyGuiObjectText == nil then
		return ""
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return ""
	end
	return NativeApi:getFairyGuiObjectText(targetHandle) or ""
end

function FairyGuiControls:Focus(handle, childPath)
	local self = self.owner
	if NativeApi == nil or NativeApi.focusFairyGuiObject == nil then
		return false
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return NativeApi:focusFairyGuiObject(targetHandle)
end

function FairyGuiControls:ResolveFocusHandle(objectInfo, target)
	local self = self.owner
	if objectInfo == nil then
		return nil
	end
	if type(target) == "number" then
		return target > 0 and target or nil
	end
	if type(target) == "string" then
		return self:GetTargetHandle(objectInfo.handle, target)
	end
	if type(target) == "table" then
		if type(target.handle) == "number" then
			return target.handle > 0 and target.handle or nil
		end
		if type(target.childPath) == "string" then
			return self:GetTargetHandle(objectInfo.handle, target.childPath)
		end
	end
	return nil
end

function FairyGuiControls:GetFocusHandles(objectInfo)
	local self = self.owner
	if objectInfo == nil or objectInfo.tabFocus == false then
		return {}
	end

	local handles = {}
	local handleSet = {}
	local function push(handle)
		if type(handle) == "number" and handle > 0 and handleSet[handle] ~= true then
			handleSet[handle] = true
			table.insert(handles, handle)
		end
	end

	if type(objectInfo.focusOrder) == "table" then
		for _, target in ipairs(objectInfo.focusOrder) do
			push(self:ResolveFocusHandle(objectInfo, target))
		end
	end
	push(objectInfo.inputHandle)
	return handles
end

function FairyGuiControls:RegisterFocusOrder(keyOrHandle, focusOrder)
	local self = self.owner
	local objectInfo = self:GetObjectInfo(keyOrHandle)
	if objectInfo == nil or type(focusOrder) ~= "table" then
		return 0
	end
	objectInfo.focusOrder = focusOrder
	objectInfo.focusHandles = nil
	return #self:GetFocusHandles(objectInfo)
end

function FairyGuiControls:CollectFocusHandles()
	local self = self.owner
	local entries = {}
	for _, entry in ipairs(self.uiStack or {}) do
		local objectInfo = entry ~= nil and self.objects[entry.key] or nil
		if objectInfo ~= nil and self.hiddenObjects[entry.key] == nil then
			table.insert(entries, {
				objectInfo = objectInfo,
				serial = entry.serial or 0,
				sortingOrder = objectInfo.sortingOrder or entry.sortingOrder or 0,
			})
		end
	end
	table.sort(entries, function(a, b)
		if a.sortingOrder == b.sortingOrder then
			return a.serial < b.serial
		end
		return a.sortingOrder < b.sortingOrder
	end)

	local handles = {}
	for _, entry in ipairs(entries) do
		for _, handle in ipairs(self:GetFocusHandles(entry.objectInfo)) do
			table.insert(handles, handle)
		end
	end
	return handles
end

function FairyGuiControls:FocusNext(reverse)
	local self = self.owner
	local handles = self:CollectFocusHandles()
	if #handles == 0 then
		return false
	end

	local focusedHandle = self:GetFocusedHandle()
	local currentIndex = nil
	for index, handle in ipairs(handles) do
		if handle == focusedHandle then
			currentIndex = index
			break
		end
	end

	local nextIndex = nil
	if currentIndex == nil then
		nextIndex = reverse == true and #handles or 1
	elseif reverse == true then
		nextIndex = currentIndex - 1
		if nextIndex < 1 then
			nextIndex = #handles
		end
	else
		nextIndex = currentIndex + 1
		if nextIndex > #handles then
			nextIndex = 1
		end
	end
	return self:Focus(handles[nextIndex])
end

function FairyGuiControls:ClearFocus()
	if NativeApi == nil or NativeApi.clearFairyGuiFocus == nil then
		return false
	end
	return NativeApi:clearFairyGuiFocus()
end

function FairyGuiControls:GetFocusedHandle()
	if NativeApi == nil or NativeApi.getFairyGuiFocusedObject == nil then
		return 0
	end
	return NativeApi:getFairyGuiFocusedObject()
end

function FairyGuiControls:SetIcon(handle, childPath, icon)
	local self = self.owner
	if NativeApi == nil then
		return false
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return NativeApi:setFairyGuiObjectIcon(targetHandle, tostring(icon or ""))
end

function FairyGuiControls:SetLoaderUrl(handle, childPath, url)
	local self = self.owner
	if NativeApi == nil then
		return false
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return NativeApi:setFairyGuiObjectLoaderUrl(targetHandle, tostring(url or ""))
end

function FairyGuiControls:SetChildVisible(handle, childPath, visible)
	local self = self.owner
	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return self:SetVisible(targetHandle, visible == true)
end

function FairyGuiControls:SetChildPosition(handle, childPath, x, y)
	local self = self.owner
	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return self:SetPosition(targetHandle, x, y)
end

function FairyGuiControls:SetChildSize(handle, childPath, width, height)
	local self = self.owner
	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end
	return self:SetSize(targetHandle, width, height)
end

function FairyGuiControls:SetControllerIndex(handle, controllerName, selectedIndex)
	if NativeApi == nil or handle == nil then
		return false
	end
	return NativeApi:setFairyGuiObjectControllerIndex(handle, controllerName or "", selectedIndex or 0)
end

function FairyGuiControls:GetControllerIndex(handle, controllerName)
	if NativeApi == nil or NativeApi.getFairyGuiObjectControllerIndex == nil or handle == nil then
		return -1
	end
	return NativeApi:getFairyGuiObjectControllerIndex(handle, controllerName or "")
end

function FairyGuiControls:SetControllerPage(handle, controllerName, pageName)
	if NativeApi == nil or NativeApi.setFairyGuiObjectControllerPage == nil or handle == nil then
		return false
	end
	return NativeApi:setFairyGuiObjectControllerPage(handle, controllerName or "", pageName or "")
end

function FairyGuiControls:GetControllerPage(handle, controllerName)
	if NativeApi == nil or NativeApi.getFairyGuiObjectControllerPage == nil or handle == nil then
		return ""
	end
	return NativeApi:getFairyGuiObjectControllerPage(handle, controllerName or "")
end

function FairyGuiControls:GetControllerPageId(handle, controllerName)
	if NativeApi == nil or NativeApi.getFairyGuiObjectControllerPageId == nil or handle == nil then
		return ""
	end
	return NativeApi:getFairyGuiObjectControllerPageId(handle, controllerName or "")
end

function FairyGuiControls:GetControllerPageCount(handle, controllerName)
	if NativeApi == nil or NativeApi.getFairyGuiObjectControllerPageCount == nil or handle == nil then
		return 0
	end
	return NativeApi:getFairyGuiObjectControllerPageCount(handle, controllerName or "")
end

function FairyGuiControls:GetControllerPageNameAt(handle, controllerName, pageIndex)
	if NativeApi == nil or NativeApi.getFairyGuiObjectControllerPageNameAt == nil or handle == nil then
		return ""
	end
	return NativeApi:getFairyGuiObjectControllerPageNameAt(handle, controllerName or "", pageIndex or 0)
end

function FairyGuiControls:GetControllerPageIdAt(handle, controllerName, pageIndex)
	if NativeApi == nil or NativeApi.getFairyGuiObjectControllerPageIdAt == nil or handle == nil then
		return ""
	end
	return NativeApi:getFairyGuiObjectControllerPageIdAt(handle, controllerName or "", pageIndex or 0)
end

function FairyGuiControls:SetValue(handle, childPathOrValue, value)
	local self = self.owner
	if NativeApi == nil or NativeApi.setFairyGuiObjectValue == nil or handle == nil then
		return false
	end
	local childPath = childPathOrValue
	if value == nil then
		value = childPathOrValue
		childPath = nil
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and NativeApi:setFairyGuiObjectValue(targetHandle, value or 0) or false
end

function FairyGuiControls:GetValue(handle, childPath)
	local self = self.owner
	if NativeApi == nil or NativeApi.getFairyGuiObjectValue == nil or handle == nil then
		return 0
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and NativeApi:getFairyGuiObjectValue(targetHandle) or 0
end

function FairyGuiControls:SetMin(handle, childPathOrValue, minValue)
	local self = self.owner
	if NativeApi == nil or NativeApi.setFairyGuiObjectMin == nil or handle == nil then
		return false
	end
	local childPath = childPathOrValue
	if minValue == nil then
		minValue = childPathOrValue
		childPath = nil
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and NativeApi:setFairyGuiObjectMin(targetHandle, minValue or 0) or false
end

function FairyGuiControls:GetMin(handle, childPath)
	local self = self.owner
	if NativeApi == nil or NativeApi.getFairyGuiObjectMin == nil or handle == nil then
		return 0
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and NativeApi:getFairyGuiObjectMin(targetHandle) or 0
end

function FairyGuiControls:SetMax(handle, childPathOrValue, maxValue)
	local self = self.owner
	if NativeApi == nil or NativeApi.setFairyGuiObjectMax == nil or handle == nil then
		return false
	end
	local childPath = childPathOrValue
	if maxValue == nil then
		maxValue = childPathOrValue
		childPath = nil
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and NativeApi:setFairyGuiObjectMax(targetHandle, maxValue or 0) or false
end

function FairyGuiControls:GetMax(handle, childPath)
	local self = self.owner
	if NativeApi == nil or NativeApi.getFairyGuiObjectMax == nil or handle == nil then
		return 0
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and NativeApi:getFairyGuiObjectMax(targetHandle) or 0
end

function FairyGuiControls:SetProgress(handle, childPathOrValue, value, maxValue, minValue)
	local self = self.owner
	local childPath = childPathOrValue
	if type(childPathOrValue) ~= "string" then
		minValue = maxValue
		maxValue = value
		value = childPathOrValue
		childPath = nil
	end

	local targetHandle = self:GetTargetHandle(handle, childPath)
	if targetHandle == nil then
		return false
	end

	if minValue ~= nil and not self:SetMin(targetHandle, minValue) then
		return false
	end
	if maxValue ~= nil and not self:SetMax(targetHandle, maxValue) then
		return false
	end
	return self:SetValue(targetHandle, value or 0)
end

function FairyGuiControls:SetSliderValue(handle, childPathOrValue, value)
	local self = self.owner
	return self:SetValue(handle, childPathOrValue, value)
end

function FairyGuiControls:GetSliderValue(handle, childPath)
	local self = self.owner
	return self:GetValue(handle, childPath)
end

function FairyGuiControls:SetProgressBarValue(handle, childPathOrValue, value)
	local self = self.owner
	return self:SetValue(handle, childPathOrValue, value)
end

function FairyGuiControls:GetProgressBarValue(handle, childPath)
	local self = self.owner
	return self:GetValue(handle, childPath)
end

function FairyGuiControls:SetComboBoxSelectedIndex(handle, childPathOrSelectedIndex, selectedIndex)
	local self = self.owner
	if NativeApi == nil or NativeApi.setFairyGuiComboBoxSelectedIndex == nil or handle == nil then
		return false
	end
	local childPath = childPathOrSelectedIndex
	if selectedIndex == nil then
		selectedIndex = childPathOrSelectedIndex
		childPath = nil
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and NativeApi:setFairyGuiComboBoxSelectedIndex(targetHandle, selectedIndex or 0) or false
end

function FairyGuiControls:GetComboBoxSelectedIndex(handle, childPath)
	local self = self.owner
	if NativeApi == nil or NativeApi.getFairyGuiComboBoxSelectedIndex == nil or handle == nil then
		return -1
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and NativeApi:getFairyGuiComboBoxSelectedIndex(targetHandle) or -1
end

function FairyGuiControls:SetComboBoxValue(handle, childPathOrValue, value)
	local self = self.owner
	if NativeApi == nil or NativeApi.setFairyGuiComboBoxValue == nil or handle == nil then
		return false
	end
	local childPath = childPathOrValue
	if value == nil then
		value = childPathOrValue
		childPath = nil
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and NativeApi:setFairyGuiComboBoxValue(targetHandle, value or "") or false
end

function FairyGuiControls:GetComboBoxValue(handle, childPath)
	local self = self.owner
	if NativeApi == nil or NativeApi.getFairyGuiComboBoxValue == nil or handle == nil then
		return ""
	end
	local targetHandle = self:GetTargetHandle(handle, childPath)
	return targetHandle ~= nil and NativeApi:getFairyGuiComboBoxValue(targetHandle) or ""
end

return FairyGuiControls
