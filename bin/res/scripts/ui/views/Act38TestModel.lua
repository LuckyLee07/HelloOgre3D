local Act38TestModel = Class("Act38TestModel", ClassList.FairyGuiBaseModel)

function Act38TestModel:Create(param)
	return ClassList.Act38TestModel.new(param)
end

function Act38TestModel:Init(param)
	ClassList.FairyGuiBaseModel.Init(self, param)
	self.data.openCount = 0
	self.data.selectedTab = 0
	self.data.dayTasks = {
		{ desc = "Enter dungeon", num = "1/3", state = "go" },
		{ desc = "Win battle", num = "3/3", state = "get" },
		{ desc = "Claimed reward", num = "3/3", state = "done" },
	}
	self.data.spcTasks = {
		{ desc = "Special task A", num = "0/1", state = "go" },
		{ desc = "Special task B", num = "1/1", state = "get" },
	}
	self.data.shopItems = {
		{ cost = "10", reward = "1", state = "exchange" },
		{ cost = "20", reward = "2", state = "owned" },
	}
end

function Act38TestModel:SetIncomingParam(param)
	local previousParam = self.data.incomingParam
	ClassList.FairyGuiBaseModel.SetIncomingParam(self, param)
	if previousParam == param then
		return
	end

	self.data.openCount = (self.data.openCount or 0) + 1
	param = param or {}
	if param.selectedTab ~= nil then
		self.data.selectedTab = param.selectedTab
	end
end

function Act38TestModel:GetDayTasks()
	return self.data.dayTasks
end

function Act38TestModel:GetSpcTasks()
	return self.data.spcTasks
end

function Act38TestModel:GetShopItems()
	return self.data.shopItems
end

function Act38TestModel:GetTaskList(taskType)
	if taskType == "special" then
		return self.data.spcTasks
	end
	return self.data.dayTasks
end

function Act38TestModel:GetTask(taskType, index)
	local taskList = self:GetTaskList(taskType)
	return taskList ~= nil and taskList[index] or nil
end

function Act38TestModel:AdvanceTaskState(taskType, index)
	local task = self:GetTask(taskType, index)
	if task == nil then
		return nil
	end

	if task.state == "go" then
		task.state = "get"
		task.num = "1/1"
	elseif task.state == "get" then
		task.state = "done"
	else
		task.state = "done"
	end
	return task
end

function Act38TestModel:CreateDayTask()
	local taskList = self.data.dayTasks
	local taskIndex = #taskList + 1
	return {
		desc = "Debug task " .. tostring(taskIndex),
		num = "0/1",
		state = "go",
	}
end

function Act38TestModel:AppendDayTask()
	local taskList = self.data.dayTasks
	local task = self:CreateDayTask()
	table.insert(taskList, task)
	return task
end

function Act38TestModel:AdvanceShopState(index)
	local item = self.data.shopItems[index]
	if item == nil then
		return nil
	end

	if item.state == "exchange" then
		item.state = "owned"
	elseif item.state == "owned" then
		item.state = "finish"
	else
		item.state = "finish"
	end
	return item
end

function Act38TestModel:SetSelectedTab(selectedTab)
	self.data.selectedTab = selectedTab or 0
end

function Act38TestModel:GetSelectedTab()
	return self.data.selectedTab or 0
end

return Act38TestModel
