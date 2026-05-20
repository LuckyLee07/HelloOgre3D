local Act37TestMvcCtrl = Class("Act37TestMvcCtrl", ClassList.FairyGuiBaseCtrl)

function Act37TestMvcCtrl:Create(param)
	return ClassList.Act37TestMvcCtrl.new(param)
end

function Act37TestMvcCtrl:RegisterUIEvents()
	self:AddClick("btn_close", function(evt)
		self:OnCloseClick(evt)
	end)
	self:AddClick("btn_buy", function(evt)
		self:OnActionClick("buy", evt)
	end)
	self:AddClick("btn_rule", function(evt)
		self:OnActionClick("rule", evt)
	end)
	self:AddClick("btn_detail", function(evt)
		self:OnActionClick("detail", evt)
	end)
	self:AddChanged("tab_list", function(evt)
		self:OnTabChanged(evt)
	end)
	self:AddClickItem("reward_list", function(evt)
		self:OnRewardItemClick(evt)
	end)
	self:AddRightClick("", function(evt)
		self:OnRootRightClick(evt)
	end)
	self:AddTouchBegin("", function(evt)
		self:OnTouchState("begin", evt)
	end)
	self:AddTouchEnd("", function(evt)
		self:OnTouchState("end", evt)
	end)
	self:AddDragStart("", function(evt)
		self:OnDragState("start", evt)
	end)
	self:AddDragMove("", function(evt)
		self:OnDragState("move", evt)
	end)
	self:AddDragEnd("", function(evt)
		self:OnDragState("end", evt)
	end)
end

function Act37TestMvcCtrl:Refresh()
	local param = self.model and self.model:GetIncomingParam() or {}
	self:SetControllerIndex("c1", 0)
	self:SetText("txt_title", "Act37 MVC Sample")
	self:SetText("txt_time", string.format("role=%s source=%s", tostring(param and param.roleId), tostring(param and param.source)))
	self:SetText("txt_desc", string.format("openCount=%s clickCount=%s", tostring(self.model:GetByKey("openCount")), tostring(self.model:GetByKey("clickCount"))))
	self:SetVisible("redTag", param and param.showRed == true)
	self:RefreshRewardList()
	print("[FGUI] Act37TestMvcCtrl Refresh:", param and param.roleId, param and param.source)
end

function Act37TestMvcCtrl:RefreshRewardList()
	local rewards = self.model:GetRewards()
	self:SetListData("reward_list", rewards, function(item, data, index)
		item:SetText("num", tostring(data.count))
		item:SetText("text", tostring(data.name))
		if data.icon ~= nil and data.icon ~= "" then
			item:SetIcon("icon", data.icon)
		end
		item:SetVisible("redTag", index == 1)
	end)
	self:SetListSelectedIndex("reward_list", 1)
end

function Act37TestMvcCtrl:OnReopen(param)
	print("[FGUI] Act37TestMvcCtrl OnReopen:", param and param.roleId, param and param.source)
end

function Act37TestMvcCtrl:Reset()
	print("[FGUI] Act37TestMvcCtrl Reset")
end

function Act37TestMvcCtrl:OnClose()
	print("[FGUI] Act37TestMvcCtrl OnClose")
end

function Act37TestMvcCtrl:OnRemove()
	print("[FGUI] Act37TestMvcCtrl OnRemove")
end

function Act37TestMvcCtrl:Dispose()
	print("[FGUI] Act37TestMvcCtrl Dispose")
end

function Act37TestMvcCtrl:OnResize(width, height)
	self:SetText("txt_desc", string.format("resize=%sx%s openCount=%s", tostring(width), tostring(height), tostring(self.model:GetByKey("openCount"))))
	print("[FGUI] Act37TestMvcCtrl OnResize:", width, height)
end

function Act37TestMvcCtrl:OnCloseClick(evt)
	print("[FGUI] Act37TestMvcCtrl close click:", evt.rootHandle, evt.bindingId)
	self:Close()
end

function Act37TestMvcCtrl:OnActionClick(actionName, evt)
	local clickCount = self.model:IncreaseClickCount()
	self:SetText("txt_desc", string.format("%s clickCount=%s", tostring(actionName), tostring(clickCount)))
	print("[FGUI] Act37TestMvcCtrl action click:", actionName, evt.eventType, evt.bindingId)
end

function Act37TestMvcCtrl:OnTabChanged(evt)
	local selectedIndex = self:GetListSelectedIndex("tab_list")
	print("[FGUI] Act37TestMvcCtrl tab changed:", evt.eventType, evt.bindingId, selectedIndex)
end

function Act37TestMvcCtrl:OnRewardItemClick(evt)
	local reward = evt.itemData or self.model:GetReward(evt.itemIndex or 0) or {}
	self:SetText("txt_desc", string.format("reward[%s]=%s x%s", tostring(evt.itemIndex), tostring(reward.name), tostring(reward.count)))
	print("[FGUI] Act37TestMvcCtrl reward item click:", evt.eventType, evt.bindingId, evt.itemIndex, reward.name, reward.count)
end

function Act37TestMvcCtrl:OnRootRightClick(evt)
	print("[FGUI] Act37TestMvcCtrl root right click:", evt.eventType, evt.bindingId)
end

function Act37TestMvcCtrl:OnTouchState(state, evt)
	print("[FGUI] Act37TestMvcCtrl touch:", state, evt.eventType, evt.bindingId)
end

function Act37TestMvcCtrl:OnDragState(state, evt)
	print("[FGUI] Act37TestMvcCtrl drag:", state, evt.eventType, evt.bindingId)
end

return Act37TestMvcCtrl
