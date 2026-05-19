local FairyGuiBaseMvcView = Class("FairyGuiBaseMvcView", ClassList.BaseFairyGuiView)

function FairyGuiBaseMvcView:Create(param)
	return ClassList.FairyGuiBaseMvcView.new(param)
end

function FairyGuiBaseMvcView:Init(param)
	param = param or {}
	self.handle = param.handle or param.root
	self.uiKey = param.key
	self.viewName = nil
	self.param = param
	self.define = param.define or {}
	self.uiType = param.uiType or "FGUI"
	self.widgets = {}
end

function FairyGuiBaseMvcView:GetRootHandle()
	return self.handle
end

function FairyGuiBaseMvcView:GetChild(childPath)
	if FairyGuiManager == nil or self.handle == nil then
		return nil
	end
	return FairyGuiManager:GetChild(self.handle, childPath)
end

function FairyGuiBaseMvcView:GetHandleNodes()
end

function FairyGuiBaseMvcView:InitView()
end

return FairyGuiBaseMvcView
