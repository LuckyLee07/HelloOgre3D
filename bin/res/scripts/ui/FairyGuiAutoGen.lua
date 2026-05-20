local FairyGuiAutoGen = Class("FairyGuiAutoGen", ClassList.BaseFairyGuiView)

function FairyGuiAutoGen:Init(param)
	self.handle = nil
	self.uiKey = nil
	self.viewName = nil
	self.param = param or {}
	self.bindings = {}
	self.ctrl = nil
end

function FairyGuiAutoGen:OnCreate(handle, param)
	self.ctrl = FairyGuiManager:InstMVC(self.viewName, {
		incomingParam = param,
		handle = handle,
		root = handle,
		key = self.uiKey,
		autoGen = self,
	})

	if self.ctrl == nil then
		return
	end
	if self.ctrl.Start ~= nil then
		self.ctrl:Start()
	end
	if self.ctrl.RegisterUIEvents ~= nil then
		self.ctrl:RegisterUIEvents()
	end
end

function FairyGuiAutoGen:OnOpen(param)
	if self.ctrl ~= nil and self.ctrl.model ~= nil and self.ctrl.model.SetIncomingParam ~= nil then
		self.ctrl.model:SetIncomingParam(param)
	end
	if self.ctrl ~= nil and self.ctrl.OnOpen ~= nil then
		self.ctrl:OnOpen(param)
	end
end

function FairyGuiAutoGen:OnShow(param)
	if self.ctrl ~= nil and self.ctrl.model ~= nil and self.ctrl.model.SetIncomingParam ~= nil then
		self.ctrl.model:SetIncomingParam(param)
	end
	if self.ctrl ~= nil and self.ctrl.Refresh ~= nil then
		self.ctrl:Refresh()
	end
end

function FairyGuiAutoGen:OnReopen(param)
	if self.ctrl ~= nil and self.ctrl.model ~= nil and self.ctrl.model.SetIncomingParam ~= nil then
		self.ctrl.model:SetIncomingParam(param)
	end
	if self.ctrl ~= nil and self.ctrl.OnReopen ~= nil then
		self.ctrl:OnReopen(param)
	end
end

function FairyGuiAutoGen:OnHide()
	if self.ctrl ~= nil and self.ctrl.Reset ~= nil then
		self.ctrl:Reset()
	end
end

function FairyGuiAutoGen:OnClose()
	if self.ctrl ~= nil and self.ctrl.OnClose ~= nil then
		self.ctrl:OnClose()
	end
end

function FairyGuiAutoGen:OnDestroy()
	if self.ctrl ~= nil then
		if self.ctrl.Dispose ~= nil then
			self.ctrl:Dispose()
		end
		FairyGuiManager:UnInstMVC(self.ctrl)
		self.ctrl = nil
	end
end

function FairyGuiAutoGen:OnRemove()
	if self.ctrl ~= nil and self.ctrl.OnRemove ~= nil then
		self.ctrl:OnRemove()
	end
end

function FairyGuiAutoGen:OnResize(width, height)
	if self.ctrl ~= nil and self.ctrl.OnResize ~= nil then
		self.ctrl:OnResize(width, height)
	end
end

return FairyGuiAutoGen
