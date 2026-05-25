-- Generated scaffold by tools/fgui_autogen/fairygui_autogen.py.
-- Safe to edit. Re-run with --force preserves FairyGUIUserCode blocks.

local BusinessBenchmarkView = Class("BusinessBenchmarkView", ClassList.FairyGuiBaseMvcView)

-- <FairyGUIUserCode:BusinessBenchmarkView:locals>
-- </FairyGUIUserCode:BusinessBenchmarkView:locals>

function BusinessBenchmarkView:Create(param)
	return ClassList.BusinessBenchmarkView.new(param)
end

function BusinessBenchmarkView:GetHandleNodes()
	self.widgets.root = self:GetRootHandle()
	self.widgets.m2MenuCtrl = "m2_menuCtrl"
	self.widgets.mask = self:GetChild("mask")
	self.widgets.loaderImgBg = self:GetChild("loader_img_bg")
	self.widgets.tDate = self:GetChild("t_date")
	self.widgets.btnHelp = self:GetChild("btn_help")
	self.widgets.btnClose = self:GetChild("btn_close")
	self.widgets.menuBgimg = self:GetChild("menu_bgimg")
	self.widgets.menuTab = self:GetChild("MenuTab")
	self.widgets.m2DayTaskList = self:GetChild("m2_dayTaskList")
	self.widgets.m2SpcTaskList = self:GetChild("m2_spcTaskList")
	self.widgets.m2ExcShopList = self:GetChild("m2_excShopList")

-- <FairyGUIUserCode:BusinessBenchmarkView:GetHandleNodes>
	self.widgets.txtTitle = self.widgets.tDate
	self.widgets.dayList = self.widgets.m2DayTaskList
	self.widgets.specialList = self.widgets.m2SpcTaskList
	self.widgets.shopList = self.widgets.m2ExcShopList
-- </FairyGUIUserCode:BusinessBenchmarkView:GetHandleNodes>
	return self.widgets
end

function BusinessBenchmarkView:InitView()
-- <FairyGUIUserCode:BusinessBenchmarkView:InitView>
	print("[FGUI] BusinessBenchmarkView InitView:", self:GetRootHandle())
-- </FairyGUIUserCode:BusinessBenchmarkView:InitView>
end

-- <FairyGUIUserCode:BusinessBenchmarkView:custom>
-- </FairyGUIUserCode:BusinessBenchmarkView:custom>

return BusinessBenchmarkView
