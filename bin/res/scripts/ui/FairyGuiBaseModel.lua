local FairyGuiBaseModel = Class("FairyGuiBaseModel")

function FairyGuiBaseModel:Create(param)
	return ClassList.FairyGuiBaseModel.new(param)
end

function FairyGuiBaseModel:Init(param)
	param = param or {}
	self.define = param.define or {}
	self.data = {}
end

function FairyGuiBaseModel:SetIncomingParam(param)
	self.data.incomingParam = param
end

function FairyGuiBaseModel:GetIncomingParam()
	return self.data.incomingParam
end

function FairyGuiBaseModel:Get()
	return self.data
end

function FairyGuiBaseModel:GetByKey(key)
	return self.data[key]
end

function FairyGuiBaseModel:SetByKey(key, value)
	self.data[key] = value
end

return FairyGuiBaseModel
