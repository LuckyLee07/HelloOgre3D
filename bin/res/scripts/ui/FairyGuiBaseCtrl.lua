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

function FairyGuiBaseCtrl:Close(forceDestroy)
	if self.autoGen ~= nil and self.autoGen.Close ~= nil then
		return self.autoGen:Close(forceDestroy)
	end
	return false
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

function FairyGuiBaseCtrl:AddClick(childPath, callback)
	return self.view ~= nil and self.view:AddClick(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddEvent(childPath, eventType, callback)
	return self.view ~= nil and self.view:AddEvent(childPath, eventType, callback) or nil
end

function FairyGuiBaseCtrl:AddChanged(childPath, callback)
	return self.view ~= nil and self.view:AddChanged(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddClickItem(childPath, callback)
	return self.view ~= nil and self.view:AddClickItem(childPath, callback) or nil
end

function FairyGuiBaseCtrl:AddRightClick(childPath, callback)
	return self.view ~= nil and self.view:AddRightClick(childPath, callback) or nil
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
