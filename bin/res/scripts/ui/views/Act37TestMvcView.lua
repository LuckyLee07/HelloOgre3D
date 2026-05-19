local Act37TestMvcView = Class("Act37TestMvcView", ClassList.FairyGuiBaseMvcView)

function Act37TestMvcView:Create(param)
	return ClassList.Act37TestMvcView.new(param)
end

function Act37TestMvcView:GetHandleNodes()
	self.widgets.root = self:GetRootHandle()
	self.widgets.btnClose = self:GetChild("btn_close")
	self.widgets.btnBuy = self:GetChild("btn_buy")
	self.widgets.btnRule = self:GetChild("btn_rule")
	self.widgets.btnDetail = self:GetChild("btn_detail")
	self.widgets.tabList = self:GetChild("tab_list")
	self.widgets.rewardList = self:GetChild("reward_list")
	self.widgets.txtTitle = self:GetChild("txt_title")
	self.widgets.txtTime = self:GetChild("txt_time")
	self.widgets.txtDesc = self:GetChild("txt_desc")
	self.widgets.redTag = self:GetChild("redTag")
	return self.widgets
end

function Act37TestMvcView:InitView()
	print("[FGUI] Act37TestMvcView InitView:", self:GetRootHandle())
end

return Act37TestMvcView
