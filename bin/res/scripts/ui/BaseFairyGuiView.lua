local BaseFairyGuiView = Class("BaseFairyGuiView")

-- Lifecycle order:
-- create: _Attach -> OnCreate -> OnOpen -> OnShow
-- reopen cache: _Attach -> OnReopen -> OnOpen -> OnShow
-- hide cache: OnClose(reason) -> OnHide(reason)
-- destroy: OnClose(reason) -> OnHide(reason) -> OnRemove(reason) -> OnDestroy(reason) -> _Detach

function BaseFairyGuiView:Init(param)
	self.handle = nil
	self.uiKey = nil
	self.viewName = nil
	self.param = param or {}
	self.bindings = {}
	self.timers = {}
	self.lifecycleState = "Init"
end

function BaseFairyGuiView:_Attach(handle, uiKey, viewName, param)
	self.handle = handle
	self.uiKey = uiKey
	self.viewName = viewName
	self.param = param or {}
	self.lifecycleState = "Attached"
end

function BaseFairyGuiView:_Detach()
	self:ClearTimers()
	self:ClearBindings()
	self.handle = nil
	self.uiKey = nil
	self.viewName = nil
	self.param = nil
	self.lifecycleState = "Detached"
end

function BaseFairyGuiView:GetHandle()
	return self.handle
end

function BaseFairyGuiView:GetKey()
	return self.uiKey
end

function BaseFairyGuiView:IsAlive()
	return self.handle ~= nil and self.lifecycleState ~= "Detached"
end

function BaseFairyGuiView:AssertAlive(apiName)
	if self:IsAlive() then
		return true
	end
	local message = string.format("[FGUI] view api called after detach: %s %s", tostring(self.viewName), tostring(apiName or ""))
	if FairyGuiManager ~= nil and FairyGuiManager.IsStrictLifecycleEnabled ~= nil and FairyGuiManager:IsStrictLifecycleEnabled() then
		error(message)
	end
	print(message)
	return false
end

function BaseFairyGuiView:GetChild(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return nil
	end
	return FairyGuiManager:GetChild(self.handle, childPath)
end

function BaseFairyGuiView:SetVisible(childPathOrVisible, visible)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end

	if visible == nil then
		return FairyGuiManager:SetVisible(self.handle, childPathOrVisible == true)
	end
	return FairyGuiManager:SetChildVisible(self.handle, childPathOrVisible, visible == true)
end

function BaseFairyGuiView:SetText(childPath, text)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetText(self.handle, childPath, text)
end

function BaseFairyGuiView:GetText(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return ""
	end
	return FairyGuiManager:GetText(self.handle, childPath)
end

function BaseFairyGuiView:SetTextInputPolicy(childPath, policy)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetTextInputPolicy(self.handle, childPath, policy)
end

function BaseFairyGuiView:GetTextInputPolicy(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return nil
	end
	return FairyGuiManager:GetTextInputPolicy(self.handle, childPath)
end

function BaseFairyGuiView:Focus(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:Focus(self.handle, childPath)
end

function BaseFairyGuiView:SetIcon(childPath, icon)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetIcon(self.handle, childPath, icon)
end

function BaseFairyGuiView:SetLoaderUrl(childPath, url)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetLoaderUrl(self.handle, childPath, url)
end

function BaseFairyGuiView:SetPosition(childPath, x, y)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetChildPosition(self.handle, childPath, x, y)
end

function BaseFairyGuiView:SetSize(childPath, width, height)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetChildSize(self.handle, childPath, width, height)
end

function BaseFairyGuiView:SetControllerIndex(controllerName, selectedIndex)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetControllerIndex(self.handle, controllerName, selectedIndex)
end

function BaseFairyGuiView:GetControllerIndex(controllerName)
	if FairyGuiManager == nil or self.handle == nil then
		return -1
	end
	return FairyGuiManager:GetControllerIndex(self.handle, controllerName)
end

function BaseFairyGuiView:SetControllerPage(controllerName, pageName)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetControllerPage(self.handle, controllerName, pageName)
end

function BaseFairyGuiView:GetControllerPage(controllerName)
	if FairyGuiManager == nil or self.handle == nil then
		return ""
	end
	return FairyGuiManager:GetControllerPage(self.handle, controllerName)
end

function BaseFairyGuiView:GetControllerPageId(controllerName)
	if FairyGuiManager == nil or self.handle == nil then
		return ""
	end
	return FairyGuiManager:GetControllerPageId(self.handle, controllerName)
end

function BaseFairyGuiView:GetControllerPageCount(controllerName)
	if FairyGuiManager == nil or self.handle == nil then
		return 0
	end
	return FairyGuiManager:GetControllerPageCount(self.handle, controllerName)
end

function BaseFairyGuiView:GetControllerPageNameAt(controllerName, pageIndex)
	if FairyGuiManager == nil or self.handle == nil then
		return ""
	end
	return FairyGuiManager:GetControllerPageNameAt(self.handle, controllerName, pageIndex)
end

function BaseFairyGuiView:GetControllerPageIdAt(controllerName, pageIndex)
	if FairyGuiManager == nil or self.handle == nil then
		return ""
	end
	return FairyGuiManager:GetControllerPageIdAt(self.handle, controllerName, pageIndex)
end

function BaseFairyGuiView:SetValue(childPathOrValue, value)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetValue(self.handle, childPathOrValue, value)
end

function BaseFairyGuiView:GetValue(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return 0
	end
	return FairyGuiManager:GetValue(self.handle, childPath)
end

function BaseFairyGuiView:SetMin(childPathOrValue, minValue)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetMin(self.handle, childPathOrValue, minValue)
end

function BaseFairyGuiView:GetMin(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return 0
	end
	return FairyGuiManager:GetMin(self.handle, childPath)
end

function BaseFairyGuiView:SetMax(childPathOrValue, maxValue)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetMax(self.handle, childPathOrValue, maxValue)
end

function BaseFairyGuiView:GetMax(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return 0
	end
	return FairyGuiManager:GetMax(self.handle, childPath)
end

function BaseFairyGuiView:SetProgress(childPathOrValue, value, maxValue, minValue)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetProgress(self.handle, childPathOrValue, value, maxValue, minValue)
end

function BaseFairyGuiView:SetComboBoxSelectedIndex(childPathOrSelectedIndex, selectedIndex)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetComboBoxSelectedIndex(self.handle, childPathOrSelectedIndex, selectedIndex)
end

function BaseFairyGuiView:GetComboBoxSelectedIndex(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return -1
	end
	return FairyGuiManager:GetComboBoxSelectedIndex(self.handle, childPath)
end

function BaseFairyGuiView:SetComboBoxValue(childPathOrValue, value)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetComboBoxValue(self.handle, childPathOrValue, value)
end

function BaseFairyGuiView:GetComboBoxValue(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return ""
	end
	return FairyGuiManager:GetComboBoxValue(self.handle, childPath)
end

function BaseFairyGuiView:PlayTransition(transitionName, times, delay, callback)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:PlayTransition(self.handle, transitionName, times, delay, callback)
end

function BaseFairyGuiView:StopTransition(transitionName, setToComplete, processCallback)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:StopTransition(self.handle, transitionName, setToComplete, processCallback)
end

function BaseFairyGuiView:SetListData(childPath, dataList, renderer)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetListData(self.handle, childPath, dataList, renderer)
end

function BaseFairyGuiView:SetVirtualListData(childPath, dataList, renderer, options)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetVirtualListData(self.handle, childPath, dataList, renderer, options)
end

function BaseFairyGuiView:SetTreeData(childPath, treeData, renderer, options)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetTreeData(self.handle, childPath, treeData, renderer, options)
end

function BaseFairyGuiView:GetTreeFlatData(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return {}
	end
	return FairyGuiManager:GetTreeFlatData(self.handle, childPath)
end

function BaseFairyGuiView:SetTreeNodeExpanded(childPath, nodeKey, expanded)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetTreeNodeExpanded(self.handle, childPath, nodeKey, expanded)
end

function BaseFairyGuiView:ToggleTreeNode(childPath, nodeKey)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:ToggleTreeNode(self.handle, childPath, nodeKey)
end

function BaseFairyGuiView:GetListData(childPath, index)
	if FairyGuiManager == nil or self.handle == nil then
		return nil
	end
	return FairyGuiManager:GetListData(self.handle, childPath, index)
end

function BaseFairyGuiView:GetListItem(childPath, index)
	if FairyGuiManager == nil or self.handle == nil then
		return nil
	end
	return FairyGuiManager:GetListItem(self.handle, childPath, index)
end

function BaseFairyGuiView:RefreshListItem(childPath, index)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:RefreshListItem(self.handle, childPath, index)
end

function BaseFairyGuiView:RefreshList(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:RefreshList(self.handle, childPath)
end

function BaseFairyGuiView:GetListDebugStats(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return {}
	end
	return FairyGuiManager:GetListDebugStats(self.handle, childPath)
end

function BaseFairyGuiView:DumpListDebugStats(childPath, label)
	if FairyGuiManager == nil or self.handle == nil then
		return {}
	end
	return FairyGuiManager:DumpListDebugStats(self.handle, childPath, label)
end

function BaseFairyGuiView:UpdateListItem(childPath, index, data)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:UpdateListItem(self.handle, childPath, index, data)
end

function BaseFairyGuiView:AppendListItem(childPath, data)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:AppendListItem(self.handle, childPath, data)
end

function BaseFairyGuiView:RemoveListItem(childPath, index)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:RemoveListItem(self.handle, childPath, index)
end

function BaseFairyGuiView:ClearList(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:ClearList(self.handle, childPath)
end

function BaseFairyGuiView:SetListItemCount(childPath, itemCount)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetListItemCount(self.handle, childPath, itemCount)
end

function BaseFairyGuiView:GetListItemCount(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return 0
	end
	return FairyGuiManager:GetListItemCount(self.handle, childPath)
end

function BaseFairyGuiView:SetListSelectedIndex(childPath, selectedIndex)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetListSelectedIndex(self.handle, childPath, selectedIndex)
end

function BaseFairyGuiView:GetListSelectedIndex(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return 0
	end
	return FairyGuiManager:GetListSelectedIndex(self.handle, childPath)
end

function BaseFairyGuiView:ScrollListToView(childPath, index)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:ScrollListToView(self.handle, childPath, index)
end

function BaseFairyGuiView:Close(forceDestroy, reason)
	if FairyGuiManager == nil then
		return false
	end
	return FairyGuiManager:CloseView(self, forceDestroy, reason)
end

function BaseFairyGuiView:AddBinding(bindingId)
	if bindingId ~= nil then
		table.insert(self.bindings, bindingId)
	end
	return bindingId
end

function BaseFairyGuiView:ClearBindings()
	if FairyGuiManager ~= nil then
		for _, bindingId in ipairs(self.bindings) do
			FairyGuiManager:RemoveBinding(bindingId)
		end
	end
	self.bindings = {}
end

function BaseFairyGuiView:TrackTimer(timerId)
	if timerId ~= nil then
		self.timers[timerId] = true
	end
	return timerId
end

function BaseFairyGuiView:RemoveTimer(timerId)
	if timerId == nil then
		return false
	end
	self.timers[timerId] = nil
	if FairyGuiManager ~= nil then
		return FairyGuiManager:CancelTimer(timerId)
	end
	return false
end

function BaseFairyGuiView:CancelTimer(timerId)
	return self:RemoveTimer(timerId)
end

function BaseFairyGuiView:ClearTimers()
	if FairyGuiManager ~= nil then
		for timerId, _ in pairs(self.timers) do
			FairyGuiManager:CancelTimer(timerId)
		end
	end
	self.timers = {}
end

function BaseFairyGuiView:GetTimerCount()
	local count = 0
	for _, _ in pairs(self.timers) do
		count = count + 1
	end
	return count
end

function BaseFairyGuiView:Timer(duration, interval, tickFunc, finishFunc)
	if FairyGuiManager == nil or self.handle == nil then
		return nil
	end
	if not self:AssertAlive("Timer") then
		return nil
	end

	local view = self
	local timerId = nil
	local wrappedTick = nil
	if type(tickFunc) == "function" then
		wrappedTick = function(...)
			if not view:IsAlive() then
				return
			end
			return tickFunc(...)
		end
	end
	local wrappedFinish = function(...)
		view.timers[timerId] = nil
		if not view:IsAlive() then
			return
		end
		if type(finishFunc) == "function" then
			return finishFunc(...)
		end
	end
	timerId = FairyGuiManager:AddTimer(self.handle, duration, interval, wrappedTick, wrappedFinish)
	return self:TrackTimer(timerId)
end

function BaseFairyGuiView:Delay(timeout, func)
	return self:Timer(timeout, timeout, nil, func)
end

function BaseFairyGuiView:AddClick(childPath, callback)
	if FairyGuiManager == nil or self.handle == nil then
		return nil
	end
	return self:AddBinding(FairyGuiManager:AddClick(self.handle, childPath, callback))
end

function BaseFairyGuiView:AddEvent(childPath, eventType, callback)
	if FairyGuiManager == nil or self.handle == nil then
		return nil
	end
	if not self:AssertAlive("AddEvent") then
		return nil
	end
	return self:AddBinding(FairyGuiManager:AddEvent(self.handle, childPath, eventType, callback))
end

function BaseFairyGuiView:AddChanged(childPath, callback)
	return self:AddEvent(childPath, "Changed", callback)
end

function BaseFairyGuiView:AddControllerChanged(controllerName, callback)
	if FairyGuiManager == nil or self.handle == nil then
		return nil
	end
	return self:AddBinding(FairyGuiManager:AddControllerChanged(self.handle, controllerName, callback))
end

function BaseFairyGuiView:AddClickItem(childPath, callback)
	return self:AddEvent(childPath, "ClickItem", callback)
end

function BaseFairyGuiView:AddRightClick(childPath, callback)
	return self:AddEvent(childPath, "RightClick", callback)
end

function BaseFairyGuiView:AddMouseWheel(childPath, callback)
	return self:AddEvent(childPath, "MouseWheel", callback)
end

function BaseFairyGuiView:AddKeyDown(childPath, callback)
	return self:AddEvent(childPath, "KeyDown", callback)
end

function BaseFairyGuiView:AddKeyUp(childPath, callback)
	return self:AddEvent(childPath, "KeyUp", callback)
end

function BaseFairyGuiView:AddSubmit(childPath, callback)
	return self:AddEvent(childPath, "Submit", callback)
end

function BaseFairyGuiView:AddTouchBegin(childPath, callback)
	return self:AddEvent(childPath, "TouchBegin", callback)
end

function BaseFairyGuiView:AddTouchEnd(childPath, callback)
	return self:AddEvent(childPath, "TouchEnd", callback)
end

function BaseFairyGuiView:AddDragStart(childPath, callback)
	return self:AddEvent(childPath, "DragStart", callback)
end

function BaseFairyGuiView:AddDragMove(childPath, callback)
	return self:AddEvent(childPath, "DragMove", callback)
end

function BaseFairyGuiView:AddDragEnd(childPath, callback)
	return self:AddEvent(childPath, "DragEnd", callback)
end

function BaseFairyGuiView:OnCreate(handle, param)
end

function BaseFairyGuiView:OnOpen(param)
end

function BaseFairyGuiView:OnReopen(param)
end

function BaseFairyGuiView:OnShow(param)
end

function BaseFairyGuiView:OnHide()
end

function BaseFairyGuiView:OnClose()
end

function BaseFairyGuiView:OnDestroy()
end

function BaseFairyGuiView:OnRemove()
end

function BaseFairyGuiView:OnResize(width, height)
end

return BaseFairyGuiView
