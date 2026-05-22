local FairyGuiLists = Class("FairyGuiLists")

local LIST_ITEM_METHODS = {}

local function isBlank(value)
	return value == nil or value == ""
end

local function getCurrentManager()
	if _G.FairyGuiManager ~= nil then
		return _G.FairyGuiManager
	end
	if ClassList ~= nil and ClassList.FairyGuiManager ~= nil then
		return ClassList.FairyGuiManager:GetInst()
	end
	return nil
end

function LIST_ITEM_METHODS:GetHandle()
	return self.handle
end

function LIST_ITEM_METHODS:SetText(childPath, text)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetText(self.handle, childPath, text) or false
end

function LIST_ITEM_METHODS:SetIcon(childPath, icon)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetIcon(self.handle, childPath, icon) or false
end

function LIST_ITEM_METHODS:SetLoaderUrl(childPath, url)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetLoaderUrl(self.handle, childPath, url) or false
end

function LIST_ITEM_METHODS:SetVisible(childPathOrVisible, visible)
	local manager = getCurrentManager()
	if manager == nil then
		return false
	end
	if visible == nil then
		return manager:SetVisible(self.handle, childPathOrVisible == true)
	end
	return manager:SetChildVisible(self.handle, childPathOrVisible, visible == true)
end

function LIST_ITEM_METHODS:SetPosition(childPath, x, y)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetChildPosition(self.handle, childPath, x, y) or false
end

function LIST_ITEM_METHODS:SetSize(childPath, width, height)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetChildSize(self.handle, childPath, width, height) or false
end

function LIST_ITEM_METHODS:SetControllerIndex(controllerName, selectedIndex)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetControllerIndex(self.handle, controllerName, selectedIndex) or false
end

function LIST_ITEM_METHODS:SetValue(childPathOrValue, value)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetValue(self.handle, childPathOrValue, value) or false
end

function LIST_ITEM_METHODS:SetProgress(childPathOrValue, value, maxValue, minValue)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetProgress(self.handle, childPathOrValue, value, maxValue, minValue) or false
end

function LIST_ITEM_METHODS:SetComboBoxSelectedIndex(childPathOrSelectedIndex, selectedIndex)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetComboBoxSelectedIndex(self.handle, childPathOrSelectedIndex, selectedIndex) or false
end

function LIST_ITEM_METHODS:SetComboBoxValue(childPathOrValue, value)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetComboBoxValue(self.handle, childPathOrValue, value) or false
end

function FairyGuiLists:Init(owner)
	self.owner = owner
end

function FairyGuiLists:GetChild(handle, childPath)
	local self = self.owner
	if self == nil or GameManager == nil or handle == nil or isBlank(childPath) then
		return handle
	end

	local childHandles = self.childrenByHandle[handle]
	if childHandles ~= nil and childHandles[childPath] ~= nil then
		return childHandles[childPath]
	end

	local childHandle = GameManager:getFairyGuiChild(handle, childPath)
	if childHandle == nil or childHandle <= 0 then
		return nil
	end

	if childHandles == nil then
		childHandles = {}
		self.childrenByHandle[handle] = childHandles
	end
	childHandles[childPath] = childHandle
	return childHandle
end

function FairyGuiLists:GetTargetHandle(handle, childPath)
	if isBlank(childPath) then
		return handle
	end
	return self:GetChild(handle, childPath)
end

function FairyGuiLists:ClearListCacheByListHandle(listHandle)
	local self = self.owner
	if self == nil or listHandle == nil then
		return
	end

	local itemHandles = self.listItemHandlesByHandle[listHandle]
	if itemHandles ~= nil then
		for _, itemHandle in pairs(itemHandles) do
			self.childrenByHandle[itemHandle] = nil
		end
	end
	self.listItemHandlesByHandle[listHandle] = nil
	self.listDataByHandle[listHandle] = nil
	self.listRenderersByHandle[listHandle] = nil
end

function FairyGuiLists:ClearListItemHandleCache(listHandle)
	local self = self.owner
	if self == nil or listHandle == nil then
		return
	end

	local itemHandles = self.listItemHandlesByHandle[listHandle]
	if itemHandles ~= nil then
		for _, itemHandle in pairs(itemHandles) do
			self.childrenByHandle[itemHandle] = nil
		end
	end
	self.listItemHandlesByHandle[listHandle] = nil
end

function FairyGuiLists:ClearListCacheForHandle(handle)
	local self = self.owner
	if self == nil or handle == nil then
		return
	end

	self:ClearListCacheByListHandle(handle)
	local childHandles = self.childrenByHandle[handle]
	if childHandles ~= nil then
		for _, childHandle in pairs(childHandles) do
			self:ClearListCacheByListHandle(childHandle)
		end
	end
end

function FairyGuiLists:CreateListItemAdapter(listHandle, itemHandle, index, data)
	if itemHandle == nil or itemHandle <= 0 then
		return nil
	end
	return setmetatable({
		listHandle = listHandle,
		handle = itemHandle,
		index = index,
		data = data,
	}, { __index = LIST_ITEM_METHODS })
end

function FairyGuiLists:GetListItemByHandle(listHandle, index, data)
	local self = self.owner
	if self == nil or GameManager == nil or listHandle == nil or index == nil then
		return nil
	end

	local itemHandles = self.listItemHandlesByHandle[listHandle]
	if itemHandles == nil then
		itemHandles = {}
		self.listItemHandlesByHandle[listHandle] = itemHandles
	end

	local itemHandle = itemHandles[index]
	if itemHandle == nil then
		itemHandle = GameManager:getFairyGuiListItem(listHandle, index - 1)
		if itemHandle == nil or itemHandle <= 0 then
			return nil
		end
		itemHandles[index] = itemHandle
	end
	return self:CreateListItemAdapter(listHandle, itemHandle, index, data)
end

function FairyGuiLists:RenderListItemByHandle(listHandle, index)
	local self = self.owner
	if self == nil then
		return false
	end

	local dataList = self.listDataByHandle[listHandle]
	local data = dataList ~= nil and dataList[index] or nil
	local item = self:GetListItemByHandle(listHandle, index, data)
	if item == nil then
		return false
	end

	local renderer = self.listRenderersByHandle[listHandle]
	if type(renderer) == "function" then
		renderer(item, data, index)
	else
		item:SetText("", tostring(data or ""))
	end
	return true
end

function FairyGuiLists:GetListHandle(handle, childPath)
	return self:GetTargetHandle(handle, childPath)
end

function FairyGuiLists:SetListItemCount(handle, childPath, itemCount)
	local self = self.owner
	if self == nil or GameManager == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	self:ClearListItemHandleCache(listHandle)
	return GameManager:setFairyGuiListItemCount(listHandle, itemCount or 0)
end

function FairyGuiLists:GetListItemCount(handle, childPath)
	local self = self.owner
	if self == nil or GameManager == nil then
		return 0
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return 0
	end
	return GameManager:getFairyGuiListItemCount(listHandle)
end

function FairyGuiLists:GetListItem(handle, childPath, index)
	local self = self.owner
	if self == nil then
		return nil
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return nil
	end

	local dataList = self.listDataByHandle[listHandle]
	local data = dataList ~= nil and dataList[index] or nil
	return self:GetListItemByHandle(listHandle, index, data)
end

function FairyGuiLists:SetListData(handle, childPath, dataList, renderer)
	local self = self.owner
	if self == nil or type(dataList) ~= "table" then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or not self:SetListItemCount(handle, childPath, #dataList) then
		return false
	end

	self.listDataByHandle[listHandle] = dataList
	if type(renderer) == "function" then
		self.listRenderersByHandle[listHandle] = renderer
	else
		self.listRenderersByHandle[listHandle] = nil
	end
	for index, _ in ipairs(dataList) do
		self:RenderListItemByHandle(listHandle, index)
	end
	return true
end

function FairyGuiLists:GetListData(handle, childPath, index)
	local self = self.owner
	if self == nil then
		return nil
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return nil
	end

	local dataList = self.listDataByHandle[listHandle]
	if index == nil then
		return dataList
	end
	return dataList ~= nil and dataList[index] or nil
end

function FairyGuiLists:RefreshListItem(handle, childPath, index)
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or index == nil then
		return false
	end
	return self:RenderListItemByHandle(listHandle, index)
end

function FairyGuiLists:RefreshList(handle, childPath)
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	local dataList = self.listDataByHandle[listHandle]
	local itemCount = dataList ~= nil and #dataList or self:GetListItemCount(handle, childPath)
	for index = 1, itemCount do
		self:RenderListItemByHandle(listHandle, index)
	end
	return true
end

function FairyGuiLists:UpdateListItem(handle, childPath, index, data)
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or index == nil or index < 1 then
		return false
	end

	local dataList = self.listDataByHandle[listHandle]
	if dataList == nil then
		return false
	end
	dataList[index] = data
	return self:RenderListItemByHandle(listHandle, index)
end

function FairyGuiLists:AppendListItem(handle, childPath, data)
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	local dataList = self.listDataByHandle[listHandle]
	if dataList == nil then
		dataList = {}
		self.listDataByHandle[listHandle] = dataList
	end
	table.insert(dataList, data)
	if not self:SetListItemCount(handle, childPath, #dataList) then
		return false
	end
	return self:RenderListItemByHandle(listHandle, #dataList)
end

function FairyGuiLists:RemoveListItem(handle, childPath, index)
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or index == nil or index < 1 then
		return false
	end

	local dataList = self.listDataByHandle[listHandle]
	if dataList == nil or index > #dataList then
		return false
	end
	table.remove(dataList, index)
	if not self:SetListItemCount(handle, childPath, #dataList) then
		return false
	end
	return self:RefreshList(handle, childPath)
end

function FairyGuiLists:ClearList(handle, childPath)
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	local renderer = self.listRenderersByHandle[listHandle]
	return self:SetListData(handle, childPath, {}, renderer)
end

function FairyGuiLists:SetListSelectedIndex(handle, childPath, selectedIndex)
	local self = self.owner
	if self == nil or GameManager == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end
	return GameManager:setFairyGuiListSelectedIndex(listHandle, (selectedIndex or 1) - 1)
end

function FairyGuiLists:GetListSelectedIndex(handle, childPath)
	local self = self.owner
	if self == nil or GameManager == nil then
		return 0
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return 0
	end

	local selectedIndex = GameManager:getFairyGuiListSelectedIndex(listHandle)
	return selectedIndex >= 0 and selectedIndex + 1 or 0
end

function FairyGuiLists:ScrollListToView(handle, childPath, index)
	local self = self.owner
	if self == nil or GameManager == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end
	return GameManager:scrollFairyGuiListToView(listHandle, (index or 1) - 1)
end

return FairyGuiLists
