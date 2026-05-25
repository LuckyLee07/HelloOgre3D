-- Generated scaffold by tools/fgui_autogen/fairygui_autogen.py.
-- Safe to edit. Re-run with --force preserves FairyGUIUserCode blocks.

local BusinessBenchmarkModel = Class("BusinessBenchmarkModel", ClassList.FairyGuiBaseModel)

-- <FairyGUIUserCode:BusinessBenchmarkModel:locals>
-- </FairyGUIUserCode:BusinessBenchmarkModel:locals>

function BusinessBenchmarkModel:Create(param)
	return ClassList.BusinessBenchmarkModel.new(param)
end

function BusinessBenchmarkModel:Init(param)
	ClassList.FairyGuiBaseModel.Init(self, param)
-- <FairyGUIUserCode:BusinessBenchmarkModel:Init>
	self.data.title = "Business Benchmark"
	self.data.selectedTab = 0
	self:ResetBenchmarkData(1)
-- </FairyGUIUserCode:BusinessBenchmarkModel:Init>
end

-- <FairyGUIUserCode:BusinessBenchmarkModel:custom>
function BusinessBenchmarkModel:SetIncomingParam(param)
	ClassList.FairyGuiBaseModel.SetIncomingParam(self, param)
	if type(param) == "table" then
		self.data.title = param.title or param.dateText or self.data.title
		self:ResetBenchmarkData(param.scale or 1)
	end
end

function BusinessBenchmarkModel:ResetBenchmarkData(scale)
	scale = math.max(1, tonumber(scale) or 1)
	self.data.menuTabData = {
		{ title = "Daily" },
		{ title = "Special" },
		{ title = "Shop" },
	}
	self.data.m2DayTaskListData = {}
	self.data.m2SpcTaskListData = {}
	self.data.m2ExcShopListData = {}
	for index = 1, 4 * scale do
		self.data.m2DayTaskListData[index] = self:CreateTask("Daily", index)
	end
	for index = 1, 2 * scale do
		self.data.m2SpcTaskListData[index] = self:CreateTask("Special", index)
	end
	for index = 1, 4 * scale do
		self.data.m2ExcShopListData[index] = self:CreateShopItem(index)
	end
end

function BusinessBenchmarkModel:CreateTask(prefix, index)
	local stateIndex = index % 3
	return {
		desc = prefix .. " task " .. tostring(index),
		num = tostring(index % 5) .. "/5",
		state = stateIndex == 0 and "done" or stateIndex == 1 and "go" or "get",
	}
end

function BusinessBenchmarkModel:CreateShopItem(index)
	local stateIndex = index % 3
	return {
		cost = tostring(index * 10),
		reward = tostring(index),
		state = stateIndex == 0 and "finish" or stateIndex == 1 and "exchange" or "owned",
	}
end

function BusinessBenchmarkModel:GetTitle()
	return self.data.title or "Business Benchmark"
end

function BusinessBenchmarkModel:GetSelectedTab()
	return tonumber(self.data.selectedTab) or 0
end

function BusinessBenchmarkModel:SetSelectedTab(selectedTab)
	self.data.selectedTab = tonumber(selectedTab) or 0
end

function BusinessBenchmarkModel:GetTaskList(taskType)
	return taskType == "special" and self.data.m2SpcTaskListData or self.data.m2DayTaskListData
end

function BusinessBenchmarkModel:AdvanceTaskState(taskType, index)
	local taskList = self:GetTaskList(taskType)
	local task = taskList ~= nil and taskList[index] or nil
	if task == nil then
		return nil
	end
	task.state = task.state == "go" and "get" or task.state == "get" and "done" or "go"
	return task
end

function BusinessBenchmarkModel:AdvanceShopState(index)
	local shopItem = self.data.m2ExcShopListData ~= nil and self.data.m2ExcShopListData[index] or nil
	if shopItem == nil then
		return nil
	end
	shopItem.state = shopItem.state == "exchange" and "owned" or shopItem.state == "owned" and "finish" or "exchange"
	return shopItem
end

function BusinessBenchmarkModel:AppendBenchmarkTask()
	local taskList = self.data.m2DayTaskListData
	local index = #taskList + 1
	local task = self:CreateTask("Added", index)
	table.insert(taskList, task)
	return task
end
-- </FairyGUIUserCode:BusinessBenchmarkModel:custom>

return BusinessBenchmarkModel
