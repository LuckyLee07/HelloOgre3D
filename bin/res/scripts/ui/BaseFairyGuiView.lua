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

function BaseFairyGuiView:SetVisible(visible)
	if FairyGuiManager == nil or self.handle == nil then
		return false
	end
	return FairyGuiManager:SetVisible(self.handle, visible == true)
end

function BaseFairyGuiView:Close()
	if FairyGuiManager == nil then
		return false
	end
	return FairyGuiManager:CloseView(self)
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

function BaseFairyGuiView:OnCreate(handle, param)
end

function BaseFairyGuiView:OnShow(param)
end

function BaseFairyGuiView:OnHide()
end

function BaseFairyGuiView:OnDestroy()
end

return BaseFairyGuiView
