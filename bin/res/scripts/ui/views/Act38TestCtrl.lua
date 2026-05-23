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
		return false
	end

	local listPath = taskType == "day" and "m2_dayTaskList" or "m2_spcTaskList"
	if evt.treeKey ~= nil then
		self:SetTreeNodeSelected(listPath, evt.treeKey)
		if evt.treeHasChildren == true then
			self:ToggleTreeNode(listPath, evt.treeKey)
		end
		print("[FGUI] Act38TestCtrl tree item click:", taskType, evt.treeKey, evt.treeDepth, evt.treeHasChildren, evt.treeExpanded)
		return true
	end

	local task = self.model:AdvanceTaskState(taskType, itemIndex)
	if task ~= nil then
		self:UpdateListItem(listPath, itemIndex, task)
	end
	print("[FGUI] Act38TestCtrl task item click:", taskType, itemIndex, task and task.desc, task and task.state)
	return task ~= nil
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

function Act38TestCtrl:RunVirtualListSelfTest()
	local virtualTasks = {}
	for index = 1, 80 do
		virtualTasks[index] = {
			desc = "Virtual Task " .. tostring(index),
			num = tostring(index) .. "/80",
			state = index % 3 == 0 and "done" or index % 2 == 0 and "get" or "go",
		}
	end

	local rendererCount = 0
	local virtualOk = self:SetVirtualListData("m2_dayTaskList", virtualTasks, function(item, data, index)
		rendererCount = rendererCount + 1
		self:RenderTaskItem(item, data, index)
	end)
	local countOk = self:GetListItemCount("m2_dayTaskList") == #virtualTasks
	local refreshOk = self:RefreshList("m2_dayTaskList")
	local scrollFirstOk = self:ScrollListToView("m2_dayTaskList", 1)
	local scrollMiddleOk = self:ScrollListToView("m2_dayTaskList", 40)
	local scrollEndOk = self:ScrollListToView("m2_dayTaskList", #virtualTasks)
	local renderStats = self:GetListDebugStats("m2_dayTaskList")

	local updatedTask = {
		desc = "Virtual Task Updated",
		num = "2/80",
		state = "get",
	}
	local updateOk = self:UpdateListItem("m2_dayTaskList", 2, updatedTask)
	local appendOk = self:AppendListItem("m2_dayTaskList", {
		desc = "Virtual Task Appended",
		num = tostring(#virtualTasks + 1) .. "/81",
		state = "go",
	})
	local removeOk = self:RemoveListItem("m2_dayTaskList", 3)
	local mutateCountOk = self:GetListItemCount("m2_dayTaskList") == #virtualTasks
	local dataOk = self:GetListData("m2_dayTaskList", 2) == updatedTask
	local stats = self:GetListDebugStats("m2_dayTaskList")
	local statsOk = renderStats.virtual == true
		and renderStats.dataCount >= #virtualTasks
		and renderStats.backendItemCount >= #virtualTasks
		and renderStats.renderCount > 0
		and renderStats.itemHandleCount > 0
		and renderStats.itemHandleCount < renderStats.dataCount

	print(
		"[FGUI] Act38TestCtrl virtual list self test:",
		"virtual=", virtualOk,
		"count=", countOk,
		"refresh=", refreshOk,
		"scroll=", scrollFirstOk, scrollMiddleOk, scrollEndOk,
		"mutate=", updateOk, appendOk, removeOk, mutateCountOk, dataOk,
		"renderer=", rendererCount,
		"renderStats=", renderStats.renderCount, renderStats.itemHandleCount, renderStats.realizedCount, renderStats.reuseCount,
		"finalStats=", stats.renderCount, stats.itemHandleCount, stats.realizedCount, stats.reuseCount,
		"last=", stats.lastIndex, stats.lastItemHandle)

	return virtualOk == true
		and countOk == true
		and refreshOk == true
		and scrollFirstOk == true
		and scrollMiddleOk == true
		and scrollEndOk == true
		and updateOk == true
		and appendOk == true
		and removeOk == true
		and mutateCountOk == true
		and dataOk == true
		and statsOk == true
end

function Act38TestCtrl:RunTreeSelfTest()
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
	local renderCount = 0
	local treeOk = self:SetTreeData("m2_spcTaskList", treeData, function(item, node, index)
		renderCount = renderCount + 1
		local indent = string.rep("  ", node.depth or 0)
		local marker = node.hasChildren and (node.expanded and "- " or "+ ") or "  "
		local selected = node.selected and "* " or ""
		item:SetControllerIndex("c1", node.hasChildren and 0 or 1)
		item:SetText("desc", selected .. indent .. marker .. tostring(node.text or node.key))
		item:SetText("num", tostring(index))
		item:SetVisible("btn_go", node.hasChildren == true)
		item:SetVisible("btn_get", node.hasChildren ~= true)
		item:SetVisible("btn_alrget", false)
	end, { resetState = true })
	local flatBefore = self:GetTreeFlatData("m2_spcTaskList")
	local addChildOk = self:AddTreeNode("m2_spcTaskList", "root", { key = "root.extra", text = "Extra Branch" })
	local flatAfterAdd = self:GetTreeFlatData("m2_spcTaskList")
	local updateOk = self:UpdateTreeNode("m2_spcTaskList", "root.daily", { text = "Daily Branch Updated" })
	local updatedNode = self:GetTreeNode("m2_spcTaskList", "root.daily")
	local selectOk = self:SetTreeNodeSelected("m2_spcTaskList", "root.daily")
	local selectedKey = self:GetTreeSelectedKey("m2_spcTaskList")
	local selectedNode = self:GetTreeSelectedNode("m2_spcTaskList")
	local collapseOk = self:SetTreeNodeExpanded("m2_spcTaskList", "root", false)
	local flatCollapsed = self:GetTreeFlatData("m2_spcTaskList")
	local expandOk = self:SetTreeNodeExpanded("m2_spcTaskList", "root", true)
	local removeOk = self:RemoveTreeNode("m2_spcTaskList", "root.special")
	local flatAfterRemove = self:GetTreeFlatData("m2_spcTaskList")
	local clickOk = self:OnTaskItemClick("special", {
		itemIndex = 1,
		treeKey = "root",
		treeDepth = 0,
		treeHasChildren = true,
		treeExpanded = true,
		itemData = flatAfterRemove[1],
	})
	local flatAfterClick = self:GetTreeFlatData("m2_spcTaskList")
	local clearOk = self:ClearTree("m2_spcTaskList")
	local flatAfterClear = self:GetTreeFlatData("m2_spcTaskList")

	local countsOk = #flatBefore == 4
		and #flatAfterAdd == 5
		and #flatCollapsed == 2
		and #flatAfterRemove == 4
		and #flatAfterClick == 2
		and #flatAfterClear == 0
	local dataOk = updatedNode ~= nil
		and updatedNode.text == "Daily Branch Updated"
		and selectedKey == "root.daily"
		and selectedNode == updatedNode

	print(
		"[FGUI] Act38TestCtrl tree self test:",
		"tree=", treeOk,
		"add/update/select=", addChildOk, updateOk, selectOk, selectedKey,
		"collapse/expand/remove/click/clear=", collapseOk, expandOk, removeOk, clickOk, clearOk,
		"counts=", #flatBefore, #flatAfterAdd, #flatCollapsed, #flatAfterRemove, #flatAfterClick, #flatAfterClear,
		"render=", renderCount)

	return treeOk == true
		and addChildOk == true
		and updateOk == true
		and selectOk == true
		and collapseOk == true
		and expandOk == true
		and removeOk == true
		and clickOk == true
		and clearOk == true
		and countsOk == true
		and dataOk == true
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
