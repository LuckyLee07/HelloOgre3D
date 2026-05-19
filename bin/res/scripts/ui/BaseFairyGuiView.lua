local BaseFairyGuiView = Class("BaseFairyGuiView")

function BaseFairyGuiView:Init(param)
	self.handle = nil
	self.uiKey = nil
	self.viewName = nil
	self.param = param or {}
	self.bindings = {}
end

function BaseFairyGuiView:_Attach(handle, uiKey, viewName, param)
	self.handle = handle
	self.uiKey = uiKey
	self.viewName = viewName
	self.param = param or {}
end

function BaseFairyGuiView:_Detach()
	self:ClearBindings()
	self.handle = nil
	self.uiKey = nil
	self.viewName = nil
	self.param = nil
end

function BaseFairyGuiView:GetHandle()
	return self.handle
end

function BaseFairyGuiView:GetKey()
	return self.uiKey
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

function BaseFairyGuiView:Close(forceDestroy)
	if FairyGuiManager == nil then
		return false
	end
	return FairyGuiManager:CloseView(self, forceDestroy)
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
	return self:AddBinding(FairyGuiManager:AddEvent(self.handle, childPath, eventType, callback))
end

function BaseFairyGuiView:AddChanged(childPath, callback)
	return self:AddEvent(childPath, "Changed", callback)
end

function BaseFairyGuiView:AddClickItem(childPath, callback)
	return self:AddEvent(childPath, "ClickItem", callback)
end

function BaseFairyGuiView:AddRightClick(childPath, callback)
	return self:AddEvent(childPath, "RightClick", callback)
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
