local Act38TestView = Class("Act38TestView", ClassList.FairyGuiBaseMvcView)

function Act38TestView:Create(param)
	return ClassList.Act38TestView.new(param)
end

function Act38TestView:GetHandleNodes()
	self.widgets.root = self:GetRootHandle()
	self.widgets.mask = self:GetChild("mask")
	self.widgets.loaderImgBg = self:GetChild("loader_img_bg")
	self.widgets.txtDate = self:GetChild("t_date")
	self.widgets.btnHelp = self:GetChild("btn_help")
	self.widgets.btnClose = self:GetChild("btn_close")
	self.widgets.menuBg = self:GetChild("menu_bgimg")
	self.widgets.menuTab = self:GetChild("MenuTab")
	self.widgets.dayTaskList = self:GetChild("m2_dayTaskList")
	self.widgets.spcTaskList = self:GetChild("m2_spcTaskList")
	self.widgets.excShopList = self:GetChild("m2_excShopList")
	return self.widgets
end

function Act38TestView:InitView()
	print("[FGUI] Act38TestView InitView:", self:GetRootHandle())
end

return Act38TestView
