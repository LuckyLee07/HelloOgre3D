local Act37TestMvcModel = Class("Act37TestMvcModel", ClassList.FairyGuiBaseModel)

function Act37TestMvcModel:Create(param)
	return ClassList.Act37TestMvcModel.new(param)
end

function Act37TestMvcModel:Init(param)
	ClassList.FairyGuiBaseModel.Init(self, param)
	self.data.openCount = 0
	self.data.clickCount = 0
	self.data.lastRoleId = nil
	self.data.lastSource = nil
end

function Act37TestMvcModel:SetIncomingParam(param)
	local previousParam = self.data.incomingParam
	ClassList.FairyGuiBaseModel.SetIncomingParam(self, param)
	if previousParam == param then
		return
	end

	param = param or {}
	self.data.openCount = (self.data.openCount or 0) + 1
	self.data.lastRoleId = param.roleId
	self.data.lastSource = param.source
	self.data.showRed = param.showRed == true
end

function Act37TestMvcModel:IncreaseClickCount()
	self.data.clickCount = (self.data.clickCount or 0) + 1
	return self.data.clickCount
end

return Act37TestMvcModel
