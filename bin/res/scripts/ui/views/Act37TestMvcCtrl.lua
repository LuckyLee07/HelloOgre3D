local Act37TestMvcCtrl = Class("Act37TestMvcCtrl", ClassList.FairyGuiBaseCtrl)

function Act37TestMvcCtrl:Create(param)
	return ClassList.Act37TestMvcCtrl.new(param)
end

function Act37TestMvcCtrl:RegisterUIEvents()
	self:AddClick("", function(evt)
		self:OnRootClick(evt)
	end)
end

function Act37TestMvcCtrl:Refresh()
	local param = self.model and self.model:GetIncomingParam() or {}
	self:SetControllerIndex("c1", 0)
	print("[FGUI] Act37TestMvcCtrl Refresh:", param and param.roleId, param and param.source)
end

function Act37TestMvcCtrl:Reset()
	print("[FGUI] Act37TestMvcCtrl Reset")
end

function Act37TestMvcCtrl:OnRootClick(evt)
	print("[FGUI] Act37TestMvcCtrl root click:", evt.rootHandle, evt.bindingId)
end

return Act37TestMvcCtrl
