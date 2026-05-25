-- Generated scaffold by tools/fgui_autogen/fairygui_autogen.py.
-- Safe to edit. Re-run with --force preserves FairyGUIUserCode blocks.

local BusinessBenchmarkCtrl = Class("BusinessBenchmarkCtrl", ClassList.FairyGuiBaseCtrl)

-- <FairyGUIUserCode:BusinessBenchmarkCtrl:locals>
-- </FairyGUIUserCode:BusinessBenchmarkCtrl:locals>

function BusinessBenchmarkCtrl:Create(param)
	return ClassList.BusinessBenchmarkCtrl.new(param)
end

function BusinessBenchmarkCtrl:RegisterUIEvents()
-- <FairyGUIUserCode:BusinessBenchmarkCtrl:RegisterUIEvents>
	self:AddClick(self:GetControlPath("btnClose"), function(evt)
		self:Close(true, "businessBenchmarkClose")
	end)
	self:AddClick(self:GetControlPath("btnHelp"), function(evt)
		self:OnHelpClick(evt)
	end)
	self:AddChanged(self:GetControlPath("menuTab"), function(evt)
		self:OnMenuTabChanged(evt)
	end)
	self:AddClickItem(self:GetControlPath("m2DayTaskList"), function(evt)
		self:OnTaskItemClick("day", evt)
	end)
	self:AddClickItem(self:GetControlPath("m2SpcTaskList"), function(evt)
		self:OnTaskItemClick("special", evt)
	end)
	self:AddClickItem(self:GetControlPath("m2ExcShopList"), function(evt)
		self:OnShopItemClick(evt)
	end)
-- </FairyGUIUserCode:BusinessBenchmarkCtrl:RegisterUIEvents>
end

function BusinessBenchmarkCtrl:Refresh()
	self:RefreshMenuTabList()
	self:RefreshM2DayTaskList()
	self:RefreshM2SpcTaskList()
	self:RefreshM2ExcShopList()

-- <FairyGUIUserCode:BusinessBenchmarkCtrl:Refresh>
	self:SetText(self:GetControlPath("tDate"), self.model:GetTitle())
	self:SetControllerIndex("m2_menuCtrl", self.model:GetSelectedTab())
-- </FairyGUIUserCode:BusinessBenchmarkCtrl:Refresh>
end

function BusinessBenchmarkCtrl:RefreshMenuTabList()
	local dataList = self.model ~= nil and self.model:GetByKey("menuTabData") or nil
	dataList = dataList or {}
	return self:SetListData("MenuTab", dataList, function(item, data, index)
		self:RenderMenuTabItem(item, data, index)
	end)
end

-- item component: tab_btn
-- item controls: title
function BusinessBenchmarkCtrl:RenderMenuTabItem(item, data, index)
-- <FairyGUIUserCode:BusinessBenchmarkCtrl:RenderMenuTabItem>
	item:SetText(self:GetListItemControlPath("MenuTab", "title") or "title", data.title or tostring(index))
-- </FairyGUIUserCode:BusinessBenchmarkCtrl:RenderMenuTabItem>
end

function BusinessBenchmarkCtrl:RefreshM2DayTaskList()
	local dataList = self.model ~= nil and self.model:GetByKey("m2DayTaskListData") or nil
	dataList = dataList or {}
	return self:SetListData("m2_dayTaskList", dataList, function(item, data, index)
		self:RenderM2DayTaskItem(item, data, index)
	end)
end

-- item component: task_item
-- item controls: icon, num, desc, btn_go, btn_get, btn_alrget
function BusinessBenchmarkCtrl:RenderM2DayTaskItem(item, data, index)
-- <FairyGUIUserCode:BusinessBenchmarkCtrl:RenderM2DayTaskItem>
	self:RenderTaskItem("m2DayTaskList", item, data, index)
-- </FairyGUIUserCode:BusinessBenchmarkCtrl:RenderM2DayTaskItem>
end

function BusinessBenchmarkCtrl:RefreshM2SpcTaskList()
	local dataList = self.model ~= nil and self.model:GetByKey("m2SpcTaskListData") or nil
	dataList = dataList or {}
	return self:SetListData("m2_spcTaskList", dataList, function(item, data, index)
		self:RenderM2SpcTaskItem(item, data, index)
	end)
end

-- item component: task_item
-- item controls: icon, num, desc, btn_go, btn_get, btn_alrget
function BusinessBenchmarkCtrl:RenderM2SpcTaskItem(item, data, index)
-- <FairyGUIUserCode:BusinessBenchmarkCtrl:RenderM2SpcTaskItem>
	self:RenderTaskItem("m2SpcTaskList", item, data, index)
-- </FairyGUIUserCode:BusinessBenchmarkCtrl:RenderM2SpcTaskItem>
end

function BusinessBenchmarkCtrl:RefreshM2ExcShopList()
	local dataList = self.model ~= nil and self.model:GetByKey("m2ExcShopListData") or nil
	dataList = dataList or {}
	return self:SetListData("m2_excShopList", dataList, function(item, data, index)
		self:RenderM2ExcShopItem(item, data, index)
	end)
end

-- item component: shop_item
-- item controls: bgimg, itembg1, itemicon1, itemmsk1, num1, item1, itembg2, itemicon2, itemmsk2, num2, itembtn, item2, desc, btn_exchange, btn_alrowned, btn_alrfinish
function BusinessBenchmarkCtrl:RenderM2ExcShopItem(item, data, index)
-- <FairyGUIUserCode:BusinessBenchmarkCtrl:RenderM2ExcShopItem>
	local stateIndex = data.state == "exchange" and 0 or data.state == "owned" and 1 or 2
	item:SetControllerIndex("c1", stateIndex)
	item:SetText(self:GetListItemControlPath("m2ExcShopList", "num1") or "num1", data.cost)
	item:SetText(self:GetListItemControlPath("m2ExcShopList", "num2") or "num2", data.reward)
	item:SetText(self:GetListItemControlPath("m2ExcShopList", "desc") or "desc", "Reward " .. tostring(index))
	item:SetText(self:GetListItemControlPath("m2ExcShopList", "btnExchange") or "btn_exchange", "Exchange")
	item:SetVisible(self:GetListItemControlPath("m2ExcShopList", "btnExchange") or "btn_exchange", data.state == "exchange")
	item:SetVisible(self:GetListItemControlPath("m2ExcShopList", "btnAlrowned") or "btn_alrowned", data.state == "owned")
	item:SetVisible(self:GetListItemControlPath("m2ExcShopList", "btnAlrfinish") or "btn_alrfinish", data.state == "finish")
-- </FairyGUIUserCode:BusinessBenchmarkCtrl:RenderM2ExcShopItem>
end

-- <FairyGUIUserCode:BusinessBenchmarkCtrl:custom>
function BusinessBenchmarkCtrl:RenderTaskItem(listName, item, data, index)
	local stateIndex = data.state == "go" and 0 or data.state == "get" and 1 or 2
	item:SetControllerIndex("c1", stateIndex)
	item:SetText(self:GetListItemControlPath(listName, "desc") or "desc", data.desc)
	item:SetText(self:GetListItemControlPath(listName, "num") or "num", data.num)
	item:SetText(self:GetListItemControlPath(listName, "btnGo") or "btn_go", "Go")
	item:SetText(self:GetListItemControlPath(listName, "btnGet") or "btn_get", "Get")
	item:SetText(self:GetListItemControlPath(listName, "btnAlrget") or "btn_alrget", "Done")
	item:SetVisible(self:GetListItemControlPath(listName, "btnGo") or "btn_go", data.state == "go")
	item:SetVisible(self:GetListItemControlPath(listName, "btnGet") or "btn_get", data.state == "get")
	item:SetVisible(self:GetListItemControlPath(listName, "btnAlrget") or "btn_alrget", data.state == "done")
end

function BusinessBenchmarkCtrl:OnHelpClick(evt)
	local task = self.model:AppendBenchmarkTask()
	return self:AppendListItem(self:GetControlPath("m2DayTaskList"), task)
end

function BusinessBenchmarkCtrl:OnMenuTabChanged(evt)
	local selectedIndex = math.max((self:GetListSelectedIndex(self:GetControlPath("menuTab")) or 1) - 1, 0)
	self.model:SetSelectedTab(selectedIndex)
	return self:SetControllerIndex("m2_menuCtrl", selectedIndex)
end

function BusinessBenchmarkCtrl:OnTaskItemClick(taskType, evt)
	local itemIndex = evt ~= nil and evt.itemIndex or nil
	if itemIndex == nil then
		return false
	end
	local listPath = taskType == "special" and self:GetControlPath("m2SpcTaskList") or self:GetControlPath("m2DayTaskList")
	local task = self.model:AdvanceTaskState(taskType, itemIndex)
	return task ~= nil and self:UpdateListItem(listPath, itemIndex, task) or false
end

function BusinessBenchmarkCtrl:OnShopItemClick(evt)
	local itemIndex = evt ~= nil and evt.itemIndex or nil
	if itemIndex == nil then
		return false
	end
	local item = self.model:AdvanceShopState(itemIndex)
	return item ~= nil and self:UpdateListItem(self:GetControlPath("m2ExcShopList"), itemIndex, item) or false
end

function BusinessBenchmarkCtrl:RunBenchmarkSelfTest()
	local listDefine = self:GetListItemDefine("m2_dayTaskList")
	local componentDefine = self:GetComponentDefine("task_item")
	local listPathOk = self:GetListItemControlPath("m2_dayTaskList", "btnGo") == "btn_go"
		and self:GetListItemControlType("m2_dayTaskList", "desc") == "GRichTextField"
	local componentPathOk = self:GetComponentControlPath("task_item", "desc") == "desc"
		and self:GetComponentControlType("task_item", "btnGet") == "GComponent"
	local firstItem = self:GetListItem(self:GetControlPath("m2DayTaskList"), 1)
	local controls = self:BindListItemControls("m2_dayTaskList", firstItem)
	local bindOk = controls.desc ~= nil and controls.btnGo ~= nil
	local listOk = listDefine ~= nil
		and componentDefine ~= nil
		and self:GetListItemCount(self:GetControlPath("m2DayTaskList")) >= 4
		and self:GetListItemCount(self:GetControlPath("m2ExcShopList")) >= 4
	local updateOk = self:OnTaskItemClick("day", { itemIndex = 1 }) == true
		and self:OnShopItemClick({ itemIndex = 1 }) == true
	local virtualData = {}
	for index = 1, 36 do
		virtualData[index] = self.model:CreateTask("Virtual", index)
	end
	local virtualOk = self:SetVirtualListData(self:GetControlPath("m2DayTaskList"), virtualData, function(item, data, index)
		self:RenderTaskItem("m2DayTaskList", item, data, index)
	end)
	local stats = self:GetListDebugStats(self:GetControlPath("m2DayTaskList"))
	local statsOk = stats ~= nil and stats.virtual == true and stats.dataCount >= 36 and stats.renderCount > 0
	print(
		"[FGUI] BusinessBenchmark self test:",
		"listPath=", listPathOk,
		"componentPath=", componentPathOk,
		"bind=", bindOk,
		"list=", listOk,
		"update=", updateOk,
		"virtual=", virtualOk,
		"stats=", stats and stats.renderCount, stats and stats.itemHandleCount)
	return listPathOk == true
		and componentPathOk == true
		and bindOk == true
		and listOk == true
		and updateOk == true
		and virtualOk == true
		and statsOk == true
end
-- </FairyGUIUserCode:BusinessBenchmarkCtrl:custom>

return BusinessBenchmarkCtrl
