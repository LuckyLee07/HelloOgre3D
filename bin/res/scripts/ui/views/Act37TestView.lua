local Act37TestView = Class("Act37TestView", ClassList.BaseFairyGuiView)

function Act37TestView:OnCreate(handle, param)
	print("[FGUI] Act37TestView OnCreate:", handle)
	self:AddClick("", function(evt)
		print("[FGUI] Act37TestView click:", evt.rootHandle, evt.bindingId)
	end)
end

function Act37TestView:OnDestroy()
	print("[FGUI] Act37TestView OnDestroy:", self.handle)
end

return Act37TestView
