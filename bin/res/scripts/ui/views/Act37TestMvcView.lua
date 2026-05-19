local Act37TestMvcView = Class("Act37TestMvcView", ClassList.FairyGuiBaseMvcView)

function Act37TestMvcView:Create(param)
	return ClassList.Act37TestMvcView.new(param)
end

function Act37TestMvcView:GetHandleNodes()
	self.widgets.root = self:GetRootHandle()
end

function Act37TestMvcView:InitView()
	print("[FGUI] Act37TestMvcView InitView:", self:GetRootHandle())
end

return Act37TestMvcView
