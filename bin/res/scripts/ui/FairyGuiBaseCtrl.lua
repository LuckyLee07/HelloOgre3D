local FairyGuiBaseCtrl = Class("FairyGuiBaseCtrl")

function FairyGuiBaseCtrl:Create(param)
	return ClassList.FairyGuiBaseCtrl.new(param)
end

function FairyGuiBaseCtrl:Init(param)
	self.param = param or {}
	self.define = {}
	self.define.uiName = string.gsub(self.className or "", "Ctrl$", "")
	self.model = nil
	self.view = nil
	self.autoGen = nil
end

function FairyGuiBaseCtrl:SetAutoGen(autoGen)
	self.autoGen = autoGen
end

function FairyGuiBaseCtrl:GetAutoGen()
	return self.autoGen
end

function FairyGuiBaseCtrl:GetControlPath(name)
	if self.autoGen ~= nil and self.autoGen.GetControlPath ~= nil then
		return self.autoGen:GetControlPath(name) or name
	end
	return name
end

function FairyGuiBaseCtrl:GetControlType(name)
	if self.autoGen ~= nil and self.autoGen.GetControlType ~= nil then
		return self.autoGen:GetControlType(name)
	end
	return nil
end

function FairyGuiBaseCtrl:GetControlDefine(name)
	if self.autoGen ~= nil and self.autoGen.GetControlDefine ~= nil then
		return self.autoGen:GetControlDefine(name)
	end
	return nil
end

function FairyGuiBaseCtrl:RequireControlPath(name)
	if self.autoGen ~= nil and self.autoGen.RequireControlPath ~= nil then
		return self.autoGen:RequireControlPath(name)
	end
	local path = self:GetControlPath(name)
	if path == nil then
		error("[FGUI] Missing control path: " .. tostring(name), 2)
	end
	return path
end

function FairyGuiBaseCtrl:RequireControlType(name)
	if self.autoGen ~= nil and self.autoGen.RequireControlType ~= nil then
		return self.autoGen:RequireControlType(name)
	end
	local controlType = self:GetControlType(name)
	if controlType == nil then
		error("[FGUI] Missing control type: " .. tostring(name), 2)
	end
	return controlType
end

function FairyGuiBaseCtrl:GetListItemDefine(listName)
	if self.autoGen ~= nil and self.autoGen.GetListItemDefine ~= nil then
		return self.autoGen:GetListItemDefine(listName)
	end
	return nil
end

function FairyGuiBaseCtrl:RequireListItemDefine(listName)
	if self.autoGen ~= nil and self.autoGen.RequireListItemDefine ~= nil then
		return self.autoGen:RequireListItemDefine(listName)
	end
	local define = self:GetListItemDefine(listName)
	if define == nil then
		error("[FGUI] Missing list item define: " .. tostring(listName), 2)
	end
	return define
end

function FairyGuiBaseCtrl:GetListItemControlPath(listName, controlName)
	if self.autoGen ~= nil and self.autoGen.GetListItemControlPath ~= nil then
		return self.autoGen:GetListItemControlPath(listName, controlName)
	end
	return nil
end

function FairyGuiBaseCtrl:RequireListItemControlPath(listName, controlName)
	if self.autoGen ~= nil and self.autoGen.RequireListItemControlPath ~= nil then
		return self.autoGen:RequireListItemControlPath(listName, controlName)
	end
	local path = self:GetListItemControlPath(listName, controlName)
	if path == nil then
		error("[FGUI] Missing list item control path: " .. tostring(listName) .. "." .. tostring(controlName), 2)
	end
	return path
end

function FairyGuiBaseCtrl:GetListItemControlType(listName, controlName)
	if self.autoGen ~= nil and self.autoGen.GetListItemControlType ~= nil then
		return self.autoGen:GetListItemControlType(listName, controlName)
	end
	return nil
end

function FairyGuiBaseCtrl:RequireListItemControlType(listName, controlName)
	if self.autoGen ~= nil and self.autoGen.RequireListItemControlType ~= nil then
		return self.autoGen:RequireListItemControlType(listName, controlName)
	end
	local controlType = self:GetListItemControlType(listName, controlName)
	if controlType == nil then
		error("[FGUI] Missing list item control type: " .. tostring(listName) .. "." .. tostring(controlName), 2)
	end
	return controlType
end

function FairyGuiBaseCtrl:BindListItemControls(listName, item)
	local define = self:GetListItemDefine(listName)
	if define == nil or item == nil then
		return {}
	end
	if item.BindControls ~= nil then
		return item:BindControls(define.controls)
	end
	return {}
end

function FairyGuiBaseCtrl:GetComponentDefine(componentName)
	if self.autoGen ~= nil and self.autoGen.GetComponentDefine ~= nil then
		return self.autoGen:GetComponentDefine(componentName)
	end
	return nil
end

function FairyGuiBaseCtrl:RequireComponentDefine(componentName)
	if self.autoGen ~= nil and self.autoGen.RequireComponentDefine ~= nil then
		return self.autoGen:RequireComponentDefine(componentName)
	end
	local define = self:GetComponentDefine(componentName)
	if define == nil then
		error("[FGUI] Missing component define: " .. tostring(componentName), 2)
	end
	return define
end

function FairyGuiBaseCtrl:GetComponentControlPath(componentName, controlName)
	if self.autoGen ~= nil and self.autoGen.GetComponentControlPath ~= nil then
		return self.autoGen:GetComponentControlPath(componentName, controlName)
	end
	return nil
end

function FairyGuiBaseCtrl:RequireComponentControlPath(componentName, controlName)
	if self.autoGen ~= nil and self.autoGen.RequireComponentControlPath ~= nil then
		return self.autoGen:RequireComponentControlPath(componentName, controlName)
	end
	local path = self:GetComponentControlPath(componentName, controlName)
	if path == nil then
		error("[FGUI] Missing component control path: " .. tostring(componentName) .. "." .. tostring(controlName), 2)
	end
	return path
end

function FairyGuiBaseCtrl:GetComponentControlType(componentName, controlName)
	if self.autoGen ~= nil and self.autoGen.GetComponentControlType ~= nil then
		return self.autoGen:GetComponentControlType(componentName, controlName)
	end
	return nil
end

function FairyGuiBaseCtrl:RequireComponentControlType(componentName, controlName)
	if self.autoGen ~= nil and self.autoGen.RequireComponentControlType ~= nil then
		return self.autoGen:RequireComponentControlType(componentName, controlName)
	end
	local controlType = self:GetComponentControlType(componentName, controlName)
	if controlType == nil then
		error("[FGUI] Missing component control type: " .. tostring(componentName) .. "." .. tostring(controlName), 2)
	end
	return controlType
end

function FairyGuiBaseCtrl:ValidateAutoGenContract(required)
	if self.autoGen ~= nil and self.autoGen.ValidateContract ~= nil then
		return self.autoGen:ValidateContract(required)
	end
	return false, "missingAutoGenValidator"
end

function FairyGuiBaseCtrl:Start()
	if self.model ~= nil and self.model.SetIncomingParam ~= nil then
		self.model:SetIncomingParam(self.param)
	end
end

function FairyGuiBaseCtrl:RegisterUIEvents()
end

function FairyGuiBaseCtrl:Refresh()
end

function FairyGuiBaseCtrl:Reset()
end

function FairyGuiBaseCtrl:OnOpen(param)
end

function FairyGuiBaseCtrl:OnReopen(param)
end

function FairyGuiBaseCtrl:OnClose()
end

function FairyGuiBaseCtrl:OnRemove()
end

function FairyGuiBaseCtrl:OnResize(width, height)
end

function FairyGuiBaseCtrl:Dispose()
end

function FairyGuiBaseCtrl:Close(forceDestroy, reason)
	if self.autoGen ~= nil and self.autoGen.Close ~= nil then
		return self.autoGen:Close(forceDestroy, reason)
	end
	return false
end

function FairyGuiBaseCtrl:OpenChild(uiName, param)
	return self.view ~= nil and self.view:OpenChild(uiName, param) or nil
end

function FairyGuiBaseCtrl:CloseChildren(forceDestroy, reason)
	return self.view ~= nil and self.view:CloseChildren(forceDestroy, reason) or 0
end

function FairyGuiBaseCtrl:GetChildUIKeys()
	return self.view ~= nil and self.view:GetChildUIKeys() or {}
end

function FairyGuiBaseCtrl:IsAlive()
	return self.view ~= nil and self.view.IsAlive ~= nil and self.view:IsAlive()
end

function FairyGuiBaseCtrl:Timer(duration, interval, tickFunc, finishFunc)
	return self.view ~= nil and self.view.Timer ~= nil and self.view:Timer(duration, interval, tickFunc, finishFunc) or nil
end

function FairyGuiBaseCtrl:Delay(timeout, func)
	return self.view ~= nil and self.view.Delay ~= nil and self.view:Delay(timeout, func) or nil
end

function FairyGuiBaseCtrl:CancelTimer(timerId)
	return self.view ~= nil and self.view.CancelTimer ~= nil and self.view:CancelTimer(timerId) or false
end

function FairyGuiBaseCtrl:ClearTimers()
	if self.view ~= nil and self.view.ClearTimers ~= nil then
		self.view:ClearTimers()
	end
end

function FairyGuiBaseCtrl:GetTimerCount()
	return self.view ~= nil and self.view.GetTimerCount ~= nil and self.view:GetTimerCount() or 0
end

function FairyGuiBaseCtrl:GetHandle()
	if self.view ~= nil and self.view.GetRootHandle ~= nil then
		return self.view:GetRootHandle()
	end
	if self.autoGen ~= nil and self.autoGen.GetHandle ~= nil then
		return self.autoGen:GetHandle()
	end
	return nil
end

function FairyGuiBaseCtrl:SetText(childPath, text)
	return self.view ~= nil and self.view:SetText(childPath, text) or false
end

function FairyGuiBaseCtrl:GetText(childPath)
	return self.view ~= nil and self.view:GetText(childPath) or ""
end

function FairyGuiBaseCtrl:SetTextInputPolicy(childPath, policy)
	return self.view ~= nil and self.view:SetTextInputPolicy(childPath, policy) or false
end

function FairyGuiBaseCtrl:GetTextInputPolicy(childPath)
	return self.view ~= nil and self.view:GetTextInputPolicy(childPath) or nil
end

function FairyGuiBaseCtrl:ConfigureTextInput(childPath, policy)
	return self.view ~= nil and self.view:ConfigureTextInput(childPath, policy) or false
end

function FairyGuiBaseCtrl:GetTextInputDisplayText(childPath)
	return self.view ~= nil and self.view:GetTextInputDisplayText(childPath) or ""
end

function FairyGuiBaseCtrl:GetTextInputDebugInfo(childPath)
	return self.view ~= nil and self.view:GetTextInputDebugInfo(childPath) or nil
end

function FairyGuiBaseCtrl:Focus(childPath)
	return self.view ~= nil and self.view:Focus(childPath) or false
end

function FairyGuiBaseCtrl:SetVisible(childPathOrVisible, visible)
	return self.view ~= nil and self.view:SetVisible(childPathOrVisible, visible) or false
end

function FairyGuiBaseCtrl:SetPosition(childPath, x, y)
	return self.view ~= nil and self.view:SetPosition(childPath, x, y) or false
end

function FairyGuiBaseCtrl:SetSize(childPath, width, height)
	return self.view ~= nil and self.view:SetSize(childPath, width, height) or false
end

function FairyGuiBaseCtrl:SetIcon(childPath, icon)
	return self.view ~= nil and self.view:SetIcon(childPath, icon) or false
end

function FairyGuiBaseCtrl:SetLoaderUrl(childPath, url)
	return self.view ~= nil and self.view:SetLoaderUrl(childPath, url) or false
end

function FairyGuiBaseCtrl:SetControllerIndex(controllerName, selectedIndex)
	return self.view ~= nil and self.view:SetControllerIndex(controllerName, selectedIndex) or false
end

function FairyGuiBaseCtrl:GetControllerIndex(controllerName)
	return self.view ~= nil and self.view:GetControllerIndex(controllerName) or -1
end

function FairyGuiBaseCtrl:SetControllerPage(controllerName, pageName)
	return self.view ~= nil and self.view:SetControllerPage(controllerName, pageName) or false
end

function FairyGuiBaseCtrl:GetControllerPage(controllerName)
	return self.view ~= nil and self.view:GetControllerPage(controllerName) or ""
end

function FairyGuiBaseCtrl:GetControllerPageId(controllerName)
	return self.view ~= nil and self.view:GetControllerPageId(controllerName) or ""
end

function FairyGuiBaseCtrl:GetControllerPageCount(controllerName)
	return self.view ~= nil and self.view:GetControllerPageCount(controllerName) or 0
end

function FairyGuiBaseCtrl:GetControllerPageNameAt(controllerName, pageIndex)
	return self.view ~= nil and self.view:GetControllerPageNameAt(controllerName, pageIndex) or ""
end

function FairyGuiBaseCtrl:GetControllerPageIdAt(controllerName, pageIndex)
	return self.view ~= nil and self.view:GetControllerPageIdAt(controllerName, pageIndex) or ""
end

function FairyGuiBaseCtrl:SetValue(childPathOrValue, value)
	return self.view ~= nil and self.view:SetValue(childPathOrValue, value) or false
end

function FairyGuiBaseCtrl:GetValue(childPath)
	return self.view ~= nil and self.view:GetValue(childPath) or 0
end

function FairyGuiBaseCtrl:SetMin(childPathOrValue, minValue)
	return self.view ~= nil and self.view:SetMin(childPathOrValue, minValue) or false
end

function FairyGuiBaseCtrl:GetMin(childPath)
	return self.view ~= nil and self.view:GetMin(childPath) or 0
end

function FairyGuiBaseCtrl:SetMax(childPathOrValue, maxValue)
	return self.view ~= nil and self.view:SetMax(childPathOrValue, maxValue) or false
end

function FairyGuiBaseCtrl:GetMax(childPath)
	return self.view ~= nil and self.view:GetMax(childPath) or 0
end

function FairyGuiBaseCtrl:SetProgress(childPathOrValue, value, maxValue, minValue)
	return self.view ~= nil and self.view:SetProgress(childPathOrValue, value, maxValue, minValue) or false
end

function FairyGuiBaseCtrl:SetComboBoxSelectedIndex(childPathOrSelectedIndex, selectedIndex)
	return self.view ~= nil and self.view:SetComboBoxSelectedIndex(childPathOrSelectedIndex, selectedIndex) or false
end

function FairyGuiBaseCtrl:GetComboBoxSelectedIndex(childPath)
	return self.view ~= nil and self.view:GetComboBoxSelectedIndex(childPath) or -1
end

function FairyGuiBaseCtrl:SetComboBoxValue(childPathOrValue, value)
	return self.view ~= nil and self.view:SetComboBoxValue(childPathOrValue, value) or false
end

function FairyGuiBaseCtrl:GetComboBoxValue(childPath)
	return self.view ~= nil and self.view:GetComboBoxValue(childPath) or ""
end

function FairyGuiBaseCtrl:PlayTransition(transitionName, times, delay, callback)
	return self.view ~= nil and self.view:PlayTransition(transitionName, times, delay, callback) or false
end

function FairyGuiBaseCtrl:StopTransition(transitionName, setToComplete, processCallback)
	return self.view ~= nil and self.view:StopTransition(transitionName, setToComplete, processCallback) or false
end

function FairyGuiBaseCtrl:SetListData(childPath, dataList, renderer)
	return self.view ~= nil and self.view:SetListData(childPath, dataList, renderer) or false
end

function FairyGuiBaseCtrl:SetVirtualListData(childPath, dataList, renderer, options)
	return self.view ~= nil and self.view:SetVirtualListData(childPath, dataList, renderer, options) or false
end

function FairyGuiBaseCtrl:SetTreeData(childPath, treeData, renderer, options)
	return self.view ~= nil and self.view:SetTreeData(childPath, treeData, renderer, options) or false
end

function FairyGuiBaseCtrl:GetTreeFlatData(childPath)
	return self.view ~= nil and self.view:GetTreeFlatData(childPath) or {}
end

function FairyGuiBaseCtrl:GetTreeNode(childPath, nodeKey)
	return self.view ~= nil and self.view:GetTreeNode(childPath, nodeKey) or nil
end

function FairyGuiBaseCtrl:AddTreeNode(childPath, parentKey, node, index)
	return self.view ~= nil and self.view:AddTreeNode(childPath, parentKey, node, index) or false
end

function FairyGuiBaseCtrl:RemoveTreeNode(childPath, nodeKey)
	return self.view ~= nil and self.view:RemoveTreeNode(childPath, nodeKey) or false
end

function FairyGuiBaseCtrl:ClearTree(childPath)
	return self.view ~= nil and self.view:ClearTree(childPath) or false
end

function FairyGuiBaseCtrl:UpdateTreeNode(childPath, nodeKey, data)
	return self.view ~= nil and self.view:UpdateTreeNode(childPath, nodeKey, data) or false
end

function FairyGuiBaseCtrl:SetTreeNodeExpanded(childPath, nodeKey, expanded)
	return self.view ~= nil and self.view:SetTreeNodeExpanded(childPath, nodeKey, expanded) or false
end

function FairyGuiBaseCtrl:SetTreeNodeSelected(childPath, nodeKey)
	return self.view ~= nil and self.view:SetTreeNodeSelected(childPath, nodeKey) or false
end

function FairyGuiBaseCtrl:GetTreeSelectedKey(childPath)
	return self.view ~= nil and self.view:GetTreeSelectedKey(childPath) or nil
end

function FairyGuiBaseCtrl:GetTreeSelectedNode(childPath)
	return self.view ~= nil and self.view:GetTreeSelectedNode(childPath) or nil
end

function FairyGuiBaseCtrl:ToggleTreeNode(childPath, nodeKey)
	return self.view ~= nil and self.view:ToggleTreeNode(childPath, nodeKey) or false
end

function FairyGuiBaseCtrl:GetListData(childPath, index)
	return self.view ~= nil and self.view:GetListData(childPath, index) or nil
end

function FairyGuiBaseCtrl:GetListItem(childPath, index)
	return self.view ~= nil and self.view:GetListItem(childPath, index) or nil
end

function FairyGuiBaseCtrl:RefreshListItem(childPath, index)
	return self.view ~= nil and self.view:RefreshListItem(childPath, index) or false
end

function FairyGuiBaseCtrl:RefreshList(childPath)
	return self.view ~= nil and self.view:RefreshList(childPath) or false
end

function FairyGuiBaseCtrl:GetListDebugStats(childPath)
	return self.view ~= nil and self.view:GetListDebugStats(childPath) or {}
end

function FairyGuiBaseCtrl:DumpListDebugStats(childPath, label)
	return self.view ~= nil and self.view:DumpListDebugStats(childPath, label) or {}
end

function FairyGuiBaseCtrl:UpdateListItem(childPath, index, data)
	return self.view ~= nil and self.view:UpdateListItem(childPath, index, data) or false
end

function FairyGuiBaseCtrl:AppendListItem(childPath, data)
	return self.view ~= nil and self.view:AppendListItem(childPath, data) or false
end

function FairyGuiBaseCtrl:RemoveListItem(childPath, index)
	return self.view ~= nil and self.view:RemoveListItem(childPath, index) or false
end

function FairyGuiBaseCtrl:ClearList(childPath)
	return self.view ~= nil and self.view:ClearList(childPath) or false
end

function FairyGuiBaseCtrl:SetListItemCount(childPath, itemCount)
	return self.view ~= nil and self.view:SetListItemCount(childPath, itemCount) or false
end

function FairyGuiBaseCtrl:GetListItemCount(childPath)
	return self.view ~= nil and self.view:GetListItemCount(childPath) or 0
end

function FairyGuiBaseCtrl:SetListSelectedIndex(childPath, selectedIndex)
	return self.view ~= nil and self.view:SetListSelectedIndex(childPath, selectedIndex) or false
end

function FairyGuiBaseCtrl:GetListSelectedIndex(childPath)
	return self.view ~= nil and self.view:GetListSelectedIndex(childPath) or 0
end

function FairyGuiBaseCtrl:ScrollListToView(childPath, index)
	return self.view ~= nil and self.view:ScrollListToView(childPath, index) or false
end

function FairyGuiBaseCtrl:AddClick(childPath, callback)
	return self.view ~= nil and self.view:AddClick(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddEvent(childPath, eventType, callback)
	return self.view ~= nil and self.view:AddEvent(childPath, eventType, callback) or nil
end

function FairyGuiBaseCtrl:AddChanged(childPath, callback)
	return self.view ~= nil and self.view:AddChanged(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddFocusIn(childPath, callback)
	return self.view ~= nil and self.view:AddFocusIn(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddFocusOut(childPath, callback)
	return self.view ~= nil and self.view:AddFocusOut(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddControllerChanged(controllerName, callback)
	return self.view ~= nil and self.view:AddControllerChanged(controllerName, callback) or nil
end

function FairyGuiBaseCtrl:AddClickItem(childPath, callback)
	return self.view ~= nil and self.view:AddClickItem(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddRightClick(childPath, callback)
	return self.view ~= nil and self.view:AddRightClick(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddMouseWheel(childPath, callback)
	return self.view ~= nil and self.view:AddMouseWheel(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddKeyDown(childPath, callback)
	return self.view ~= nil and self.view:AddKeyDown(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddKeyUp(childPath, callback)
	return self.view ~= nil and self.view:AddKeyUp(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddSubmit(childPath, callback)
	return self.view ~= nil and self.view:AddSubmit(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddTouchBegin(childPath, callback)
	return self.view ~= nil and self.view:AddTouchBegin(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddTouchEnd(childPath, callback)
	return self.view ~= nil and self.view:AddTouchEnd(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddDragStart(childPath, callback)
	return self.view ~= nil and self.view:AddDragStart(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddDragMove(childPath, callback)
	return self.view ~= nil and self.view:AddDragMove(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddDragEnd(childPath, callback)
	return self.view ~= nil and self.view:AddDragEnd(childPath, callback) or nil
end

return FairyGuiBaseCtrl
