local NativeApi = require("res.scripts.manager.fairygui.FairyGuiNativeApi")

local FairyGuiProbes = Class("FairyGuiProbes")

local function isBlank(value)
	return value == nil or value == ""
end

function FairyGuiProbes:Init(owner)
	self.owner = owner
end

function FairyGuiProbes:OpenMaskProbe(param)
	local self = self.owner
	if self == nil then
		return nil
	end

	param = param or {}
	local key = param.key or "MaskProbe"
	local uiName = param.uiName or key
	self:CloseUI(key, true)

	local assets = param.assets or {}
	local backgroundPath = param.backgroundImage or assets.background
	local contentPath = param.contentImage or assets.content
	local stripAPath = param.stripAImage or assets.stripA
	local stripBPath = param.stripBImage or assets.stripB
	local stripCPath = param.stripCImage or assets.stripC
	local maskPath = param.maskImage or assets.mask
	local useGraphMask = param.graphMask == true
	local useNestedMask = param.nestedMask == true
	if isBlank(backgroundPath) or isBlank(contentPath) or isBlank(stripAPath) or isBlank(stripBPath) or isBlank(stripCPath) or isBlank(maskPath) then
		print("[FGUI] open mask probe failed, missing image asset")
		return nil
	end

	local function addImage(parentHandle, name, path, x, y, width, height, alpha)
		local childHandle = self:CreateLoader(parentHandle, name, path)
		if childHandle == nil or childHandle <= 0 then
			return nil
		end
		self:SetPosition(childHandle, x, y)
		self:SetSize(childHandle, width, height)
		self:SetTouchable(childHandle, false)
		if alpha ~= nil then
			self:SetAlpha(childHandle, alpha)
		end
		if NativeApi.addFairyGuiObjectToParent == nil or not NativeApi:addFairyGuiObjectToParent(childHandle, parentHandle) then
			NativeApi:removeFairyGuiObject(childHandle)
			return nil
		end
		return childHandle
	end

	local function addGraphPolygon(parentHandle, name, x, y, width, height, sides, red, green, blue, alpha)
		local childHandle = self:CreateGraphRegularPolygon(parentHandle, name, width, height, sides or 6, red or 0.35, green or 0.8, blue or 1.0, alpha or 1.0)
		if childHandle == nil or childHandle <= 0 then
			return nil
		end
		self:SetPosition(childHandle, x, y)
		self:SetSize(childHandle, width, height)
		self:SetTouchable(childHandle, false)
		if NativeApi.addFairyGuiObjectToParent == nil or not NativeApi:addFairyGuiObjectToParent(childHandle, parentHandle) then
			NativeApi:removeFairyGuiObject(childHandle)
			return nil
		end
		return childHandle
	end

	local function addText(parentHandle, name, text, x, y, width, height, red, green, blue)
		local childHandle = self:CreateText(parentHandle, name, text, 18, red, green, blue)
		if childHandle == nil or childHandle <= 0 then
			return nil
		end
		self:SetPosition(childHandle, x, y)
		self:SetSize(childHandle, width, height)
		if NativeApi.addFairyGuiObjectToParent == nil or not NativeApi:addFairyGuiObjectToParent(childHandle, parentHandle) then
			NativeApi:removeFairyGuiObject(childHandle)
			return nil
		end
		return childHandle
	end

	local function createNestedPanel(parentHandle, inverted)
		local nestedHandle = self:CreateContainer("nested_mask_panel", parentHandle)
		if nestedHandle == nil or nestedHandle <= 0 then
			return nil
		end
		self:SetPosition(nestedHandle, 94, 82)
		self:SetSize(nestedHandle, 132, 74)
		self:SetTouchable(nestedHandle, false)
		if NativeApi.addFairyGuiObjectToParent == nil or not NativeApi:addFairyGuiObjectToParent(nestedHandle, parentHandle) then
			NativeApi:removeFairyGuiObject(nestedHandle)
			return nil
		end

		local nestedContent = addImage(nestedHandle, "nested_content", stripBPath, -28, 8, 184, 58, 0.92)
		local nestedAccent = addImage(nestedHandle, "nested_accent", stripCPath, 82, 0, 46, 46, 0.9)
		local nestedMask = nil
		if useGraphMask then
			nestedMask = addGraphPolygon(nestedHandle, "nested_graph_mask", 14, 8, 104, 58, 5, 1.0, 0.78, 0.25, 0.82)
		else
			nestedMask = addImage(nestedHandle, "nested_mask", maskPath, 14, 8, 104, 58)
		end
		if nestedContent == nil or nestedAccent == nil or nestedMask == nil or not self:SetMask(nestedHandle, nestedMask, inverted == true) then
			NativeApi:removeFairyGuiObject(nestedHandle)
			return nil
		end
		return nestedHandle
	end

	local function createPanel(parentHandle, name, x, y, inverted)
		local panelHandle = self:CreateContainer(name, parentHandle)
		if panelHandle == nil or panelHandle <= 0 then
			return nil
		end
		self:SetPosition(panelHandle, x, y)
		self:SetSize(panelHandle, 320, 210)
		self:SetTouchable(panelHandle, false)
		if NativeApi.addFairyGuiObjectToParent == nil or not NativeApi:addFairyGuiObjectToParent(panelHandle, parentHandle) then
			NativeApi:removeFairyGuiObject(panelHandle)
			return nil
		end

		local background = addImage(panelHandle, "content_bg", contentPath, -70, 28, 460, 150)
		local stripA = addImage(panelHandle, "content_strip_a", stripAPath, -20, 50, 180, 72)
		local stripB = addImage(panelHandle, "content_strip_b", stripBPath, 130, 82, 160, 52)
		local stripC = addImage(panelHandle, "content_strip_c", stripCPath, 245, 20, 64, 64)
		local mask = nil
		if useGraphMask then
			mask = addGraphPolygon(panelHandle, "stencil_graph_mask", 72, 46, 176, 118, 6, 0.2, 0.9, 1.0, 0.86)
		else
			mask = addImage(panelHandle, "stencil_mask", maskPath, 72, 46, 176, 118)
		end
		if background == nil or stripA == nil or stripB == nil or stripC == nil or mask == nil or not self:SetMask(panelHandle, mask, inverted) then
			NativeApi:removeFairyGuiObject(panelHandle)
			return nil
		end
		if useNestedMask and createNestedPanel(panelHandle, inverted) == nil then
			NativeApi:removeFairyGuiObject(panelHandle)
			return nil
		end
		return panelHandle
	end

	local handle = self:CreateContainer("FairyGuiMaskProbe")
	if handle == nil or handle <= 0 then
		return nil
	end
	self:SetSize(handle, 760, 330)
	self:SetTouchable(handle, true)

	local background = addImage(handle, "probe_bg", backgroundPath, 0, 24, 760, 286, 0.7)
	local title = addText(handle, "probe_title", "FairyGUI Mask Probe", 12, 0, 320, 24, 255, 244, 210)
	local normalLabel = addText(handle, "normal_label", "normal mask: only inside area is visible", 28, 42, 320, 24, 210, 255, 220)
	local invertedLabel = addText(handle, "inverted_label", "inverted mask: center area is cut out", 404, 42, 320, 24, 255, 220, 210)
	local normalPanel = createPanel(handle, "normal_panel", 28, 70, false)
	local invertedPanel = createPanel(handle, "inverted_panel", 404, 70, true)
	local normalOverlay = nil
	local invertedOverlay = nil
	if useGraphMask then
		normalOverlay = addGraphPolygon(handle, "normal_graph_mask_overlay", 100, 116, 176, 118, 6, 0.2, 0.9, 1.0, 0.35)
		invertedOverlay = addGraphPolygon(handle, "inverted_graph_mask_overlay", 476, 116, 176, 118, 6, 0.2, 0.9, 1.0, 0.35)
	else
		normalOverlay = addImage(handle, "normal_mask_overlay", maskPath, 100, 116, 176, 118, 0.35)
		invertedOverlay = addImage(handle, "inverted_mask_overlay", maskPath, 476, 116, 176, 118, 0.35)
	end
	if background == nil or title == nil or normalLabel == nil or invertedLabel == nil or normalPanel == nil or invertedPanel == nil or normalOverlay == nil or invertedOverlay == nil then
		NativeApi:removeFairyGuiObject(handle)
		return nil
	end

	local layerName = param.layer or "Top"
	if not self:AttachToLayer(handle, layerName, param) then
		NativeApi:removeFairyGuiObject(handle)
		return nil
	end

	if param.x ~= nil and param.y ~= nil then
		self:SetPosition(handle, param.x, param.y)
	elseif param.center ~= false then
		NativeApi:centerFairyGuiObject(handle, param.restraint == true)
	end

	local objectInfo = {
		handle = handle,
		key = key,
		name = uiName,
		objectName = "FairyGuiMaskProbe",
		param = param,
		uiName = uiName,
		cache = false,
		layer = layerName,
		popupGroup = self:GetPopupGroup(param),
		popupMode = param.popupMode or "stack",
		uiGroup = self:GetUIGroup(param),
		sceneName = self:GetSceneName(param),
		closeOnSceneChange = param.closeOnSceneChange ~= false,
		destroyOnSceneChange = param.destroyOnSceneChange == true,
		parentHandle = param.parentHandle or param.rootHandle,
		rootLayer = param.rootLayer,
		focusOrder = param.focusOrder,
		tabFocus = param.tabFocus ~= false,
	}
	self.objects[key] = objectInfo
	self.objectsByHandle[handle] = objectInfo
	self.uiNameToKey[uiName] = key
	self.hiddenObjects[key] = nil
	objectInfo.cacheHiddenAtMs = nil
	self:AssignLayer(objectInfo, objectInfo.layer)
	self:ApplyScreenAdapt(objectInfo)
	self:PushStack(objectInfo)
	return handle
end

function FairyGuiProbes:OpenTextInputProbe(param)
	local self = self.owner
	if self == nil then
		return nil
	end

	param = param or {}
	local key = param.key or "TextInputProbe"
	self:CloseUI(key, true)

	local handle = self:CreateContainer("FairyGuiTextInputProbe")
	if handle == nil or handle <= 0 then
		return nil
	end
	self:SetSize(handle, 480, 160)
	self:SetTouchable(handle, true)

	local titleHandle = self:CreateText(handle, "probe_title", param.title or "FairyGUI TextInput Probe", 18, 255, 244, 210)
	local inputHandle = self:CreateTextInput(handle, "probe_input", param.text or "", 20, 230, 255, 235)
	local hintHandle = self:CreateText(handle, "probe_hint", "type, backspace, enter", 16, 190, 210, 230)
	if titleHandle == nil or titleHandle <= 0 or inputHandle == nil or inputHandle <= 0 or hintHandle == nil or hintHandle <= 0 then
		NativeApi:removeFairyGuiObject(handle)
		return nil
	end

	self:SetPosition(titleHandle, 0, 0)
	self:SetSize(titleHandle, 420, 28)
	self:SetPosition(inputHandle, 0, 44)
	self:SetSize(inputHandle, 420, 42)
	self:SetPosition(hintHandle, 0, 104)
	self:SetSize(hintHandle, 420, 28)
	if NativeApi.addFairyGuiObjectToParent == nil
		or not NativeApi:addFairyGuiObjectToParent(titleHandle, handle)
		or not NativeApi:addFairyGuiObjectToParent(inputHandle, handle)
		or not NativeApi:addFairyGuiObjectToParent(hintHandle, handle) then
		NativeApi:removeFairyGuiObject(handle)
		return nil
	end

	local layerName = param.layer or "Top"
	if not self:AttachToLayer(handle, layerName, param) then
		NativeApi:removeFairyGuiObject(handle)
		return nil
	end

	if param.x ~= nil and param.y ~= nil then
		self:SetPosition(handle, param.x, param.y)
	elseif param.center ~= false then
		NativeApi:centerFairyGuiObject(handle, param.restraint == true)
	end

	local objectInfo = {
		handle = handle,
		key = key,
		name = "TextInputProbe",
		objectName = "FairyGuiTextInputProbe",
		param = param,
		uiName = "TextInputProbe",
		cache = false,
		layer = layerName,
		popupGroup = self:GetPopupGroup(param),
		popupMode = param.popupMode or "stack",
		uiGroup = self:GetUIGroup(param),
		sceneName = self:GetSceneName(param),
		closeOnSceneChange = param.closeOnSceneChange ~= false,
		destroyOnSceneChange = param.destroyOnSceneChange == true,
		inputHandle = inputHandle,
		parentHandle = param.parentHandle or param.rootHandle,
		rootLayer = param.rootLayer,
		focusOrder = param.focusOrder or { "probe_input" },
		tabFocus = param.tabFocus ~= false,
	}
	self.objects[key] = objectInfo
	self.objectsByHandle[handle] = objectInfo
	self.uiNameToKey.TextInputProbe = key
	self.hiddenObjects[key] = nil
	objectInfo.cacheHiddenAtMs = nil
	self:AssignLayer(objectInfo, objectInfo.layer)
	self:ApplyScreenAdapt(objectInfo)
	self:PushStack(objectInfo)
	if param.textInputPolicy ~= nil then
		self:SetTextInputPolicy(handle, "probe_input", param.textInputPolicy)
	elseif param.maxLength ~= nil or param.restrict ~= nil or param.inputType ~= nil then
		self:SetTextInputPolicy(handle, "probe_input", {
			maxLength = param.maxLength,
			restrict = param.restrict,
			inputType = param.inputType,
			allowDecimal = param.allowDecimal,
			allowNegative = param.allowNegative,
		})
	end
	self:AddChanged(handle, "probe_input", function(evt)
		print("[FGUI] text input changed:", self:GetText(handle, "probe_input"), evt.senderHandle)
	end)
	self:AddSubmit(handle, "probe_input", function(evt)
		print("[FGUI] text input submit:", self:GetText(handle, "probe_input"), evt.senderHandle)
	end)
	return handle, inputHandle
end

return FairyGuiProbes
