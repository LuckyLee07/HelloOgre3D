local Act38TestCtrl = Class("Act38TestCtrl", ClassList.FairyGuiBaseCtrl)

function Act38TestCtrl:Create(param)
	return ClassList.Act38TestCtrl.new(param)
end

function Act38TestCtrl:RegisterUIEvents()
	self:AddClick("btn_close", function(evt)
		self:OnCloseClick(evt)
	end)
	self:AddClick("btn_help", function(evt)
		self:OnHelpClick(evt)
	end)
	self:AddChanged("MenuTab", function(evt)
		self:OnMenuTabChanged(evt)
	end)
	self:AddClickItem("m2_dayTaskList", function(evt)
		self:OnTaskItemClick("day", evt)
	end)
	self:AddMouseWheel("m2_dayTaskList", function(evt)
		self:OnTaskListMouseWheel("day", evt)
	end)
	self:AddClickItem("m2_spcTaskList", function(evt)
		self:OnTaskItemClick("special", evt)
	end)
	self:AddClickItem("m2_excShopList", function(evt)
		self:OnShopItemClick(evt)
	end)
end

function Act38TestCtrl:Refresh()
	local param = self.model and self.model:GetIncomingParam() or {}
	local selectedTab = self.model:GetSelectedTab()
	self:SetControllerIndex("m2_menuCtrl", selectedTab)
	self:SetText("t_date", param.dateText or "Act38 MVC Sample")
	self:RefreshTaskLists()
	self:RefreshShopList()
	print("[FGUI] Act38TestCtrl Refresh:", self:GetHandle(), "tab=", selectedTab, "openCount=", self.model:GetByKey("openCount"))
end

function Act38TestCtrl:RefreshTaskLists()
	self:SetListData("m2_dayTaskList", self.model:GetDayTasks(), function(item, data, index)
		self:RenderTaskItem(item, data, index)
	end)
	self:SetListData("m2_spcTaskList", self.model:GetSpcTasks(), function(item, data, index)
		self:RenderTaskItem(item, data, index)
	end)
	self:ScrollListToView("m2_dayTaskList", 1)
end

function Act38TestCtrl:RefreshShopList()
	self:SetListData("m2_excShopList", self.model:GetShopItems(), function(item, data, index)
		self:RenderShopItem(item, data, index)
	end)
	self:ScrollListToView("m2_excShopList", 1)
end

function Act38TestCtrl:RenderTaskItem(item, data, index)
	local stateIndex = data.state == "go" and 0 or data.state == "get" and 1 or 2
	item:SetControllerIndex("c1", stateIndex)
	item:SetText("desc", data.desc)
	item:SetText("num", data.num)
	item:SetText("btn_go", "Go")
	item:SetText("btn_get", "Get")
	item:SetText("btn_alrget", "Done")
	item:SetVisible("btn_go", data.state == "go")
	item:SetVisible("btn_get", data.state == "get")
	item:SetVisible("btn_alrget", data.state == "done")
end

function Act38TestCtrl:RenderShopItem(item, data, index)
	local stateIndex = data.state == "exchange" and 0 or data.state == "owned" and 1 or 2
	item:SetControllerIndex("c1", stateIndex)
	item:SetText("num1", data.cost)
	item:SetText("num2", data.reward)
	item:SetText("btn_exchange", "Exchange")
	item:SetVisible("btn_exchange", data.state == "exchange")
	item:SetVisible("btn_alrowned", data.state == "owned")
	item:SetVisible("btn_alrfinish", data.state == "finish")
end

function Act38TestCtrl:OnReopen(param)
	print("[FGUI] Act38TestCtrl OnReopen:", param and param.selectedTab)
end

function Act38TestCtrl:OnClose()
	print("[FGUI] Act38TestCtrl OnClose")
end

function Act38TestCtrl:OnRemove()
	print("[FGUI] Act38TestCtrl OnRemove")
end

function Act38TestCtrl:Dispose()
	print("[FGUI] Act38TestCtrl Dispose")
end

function Act38TestCtrl:OnResize(width, height)
	print("[FGUI] Act38TestCtrl OnResize:", width, height)
end

function Act38TestCtrl:OnCloseClick(evt)
	print("[FGUI] Act38TestCtrl close click:", evt.rootHandle, evt.bindingId)
	self:Close(true)
end

function Act38TestCtrl:OnHelpClick(evt)
	local task = self.model:CreateDayTask()
	local ok = self:AppendListItem("m2_dayTaskList", task)
	print("[FGUI] Act38TestCtrl help click append task:", ok, #self.model:GetDayTasks(), task.desc)
end

function Act38TestCtrl:OnMenuTabChanged(evt)
	local selectedIndex = math.max((self:GetListSelectedIndex("MenuTab") or 1) - 1, 0)
	self.model:SetSelectedTab(selectedIndex)
	self:SetControllerIndex("m2_menuCtrl", selectedIndex)
	print("[FGUI] Act38TestCtrl tab changed:", selectedIndex, evt.eventType, evt.bindingId)
end

function Act38TestCtrl:OnTaskItemClick(taskType, evt)
	local itemIndex = evt.itemIndex
	if itemIndex == nil then
		return
	end

	local listPath = taskType == "day" and "m2_dayTaskList" or "m2_spcTaskList"
	local task = self.model:AdvanceTaskState(taskType, itemIndex)
	if task ~= nil then
		self:UpdateListItem(listPath, itemIndex, task)
	end
	print("[FGUI] Act38TestCtrl task item click:", taskType, itemIndex, task and task.desc, task and task.state)
end

function Act38TestCtrl:OnShopItemClick(evt)
	local itemIndex = evt.itemIndex
	if itemIndex == nil then
		return
	end

	local item = self.model:AdvanceShopState(itemIndex)
	if item ~= nil then
		self:UpdateListItem("m2_excShopList", itemIndex, item)
	end
	print("[FGUI] Act38TestCtrl shop item click:", itemIndex, item and item.cost, item and item.reward, item and item.state)
end

function Act38TestCtrl:OnTaskListMouseWheel(taskType, evt)
	print("[FGUI] Act38TestCtrl list mouse wheel:", taskType, evt.wheelDelta, evt.x, evt.y)
end

function Act38TestCtrl:RunListApiSelfTest()
	local task = self.model:CreateDayTask()
	local appendOk = self:AppendListItem("m2_dayTaskList", task)
	local dayTask = self.model:AdvanceTaskState("day", 1)
	local updateTaskOk = dayTask ~= nil and self:UpdateListItem("m2_dayTaskList", 1, dayTask) or false
	local shopItem = self.model:AdvanceShopState(1)
	local updateShopOk = shopItem ~= nil and self:UpdateListItem("m2_excShopList", 1, shopItem) or false

	print(
		"[FGUI] Act38TestCtrl list api self test:",
		appendOk,
		updateTaskOk,
		updateShopOk,
		self:GetListItemCount("m2_dayTaskList"),
		dayTask and dayTask.state,
		shopItem and shopItem.state)
	return appendOk and updateTaskOk and updateShopOk
end

function Act38TestCtrl:RunComplexControlSelfTest()
	local controllerName = "m2_menuCtrl"
	local pageCount = self:GetControllerPageCount(controllerName)
	local beforeIndex = self:GetControllerIndex(controllerName)
	local changedCount = 0
	local lastControllerEvent = nil
	local bindingId = self:AddControllerChanged(controllerName, function(evt)
		changedCount = changedCount + 1
		lastControllerEvent = evt
		print("[FGUI] Act38TestCtrl controller changed:", evt.controllerName, evt.selectedIndex, evt.selectedPage, evt.selectedPageId)
	end)

	local targetIndex = pageCount > 1 and ((beforeIndex + 1) % pageCount) or beforeIndex
	local setIndexOk = pageCount > 0 and self:SetControllerIndex(controllerName, targetIndex) or false
	local indexOk = self:GetControllerIndex(controllerName) == targetIndex
	local targetPage = pageCount > 0 and self:GetControllerPageNameAt(controllerName, beforeIndex >= 0 and beforeIndex or 0) or ""
	local targetPageId = pageCount > 0 and self:GetControllerPageIdAt(controllerName, beforeIndex >= 0 and beforeIndex or 0) or ""
	local setPageOk = targetPage ~= "" and self:SetControllerPage(controllerName, targetPage) or setIndexOk
	local pageOk = targetPage == "" or self:GetControllerPage(controllerName) == targetPage

	local virtualTasks = {}
	for index = 1, 24 do
		virtualTasks[index] = {
			desc = "Virtual Task " .. tostring(index),
			num = tostring(index) .. "/24",
			state = index % 3 == 0 and "done" or index % 2 == 0 and "get" or "go",
		}
	end
	local virtualOk = self:SetVirtualListData("m2_dayTaskList", virtualTasks, function(item, data, index)
		self:RenderTaskItem(item, data, index)
	end)
	local virtualCountOk = self:GetListItemCount("m2_dayTaskList") == #virtualTasks

	local treeData = {
		{
			key = "root",
			text = "Root Task",
			expanded = true,
			children = {
				{ key = "root.daily", text = "Daily Branch" },
				{ key = "root.special", text = "Special Branch" },
			},
		},
		{ key = "solo", text = "Solo Task" },
	}
	local treeOk = self:SetTreeData("m2_spcTaskList", treeData, function(item, node, index)
		local indent = string.rep("  ", node.depth or 0)
		local marker = node.hasChildren and (node.expanded and "- " or "+ ") or "  "
		item:SetControllerIndex("c1", node.hasChildren and 0 or 1)
		item:SetText("desc", indent .. marker .. tostring(node.text or node.key))
		item:SetText("num", tostring(index))
		item:SetVisible("btn_go", node.hasChildren == true)
		item:SetVisible("btn_get", node.hasChildren ~= true)
		item:SetVisible("btn_alrget", false)
	end)
	local flatBefore = self:GetTreeFlatData("m2_spcTaskList")
	local collapseOk = self:SetTreeNodeExpanded("m2_spcTaskList", "root", false)
	local flatCollapsed = self:GetTreeFlatData("m2_spcTaskList")
	local expandOk = self:SetTreeNodeExpanded("m2_spcTaskList", "root", true)
	local flatExpanded = self:GetTreeFlatData("m2_spcTaskList")
	local treeCountOk = #flatBefore == 4 and #flatCollapsed == 2 and #flatExpanded == 4

	print(
		"[FGUI] Act38TestCtrl complex control self test:",
		"binding=", bindingId,
		"page=", pageCount, beforeIndex, targetIndex, targetPage, targetPageId,
		"controller=", setIndexOk, indexOk, setPageOk, pageOk, changedCount,
		"virtual=", virtualOk, virtualCountOk,
		"tree=", treeOk, collapseOk, expandOk, treeCountOk,
		"lastEvent=", lastControllerEvent and lastControllerEvent.selectedIndex)

	return bindingId ~= nil
		and pageCount > 0
		and setIndexOk == true
		and indexOk == true
		and setPageOk == true
		and pageOk == true
		and changedCount > 0
		and virtualOk == true
		and virtualCountOk == true
		and treeOk == true
		and collapseOk == true
		and expandOk == true
		and treeCountOk == true
end

return Act38TestCtrl
