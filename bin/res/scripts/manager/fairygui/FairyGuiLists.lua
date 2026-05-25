local NativeApi = require("res.scripts.manager.fairygui.FairyGuiNativeApi")

local FairyGuiLists = Class("FairyGuiLists")

local LIST_ITEM_METHODS = {}

local function isBlank(value)
	return value == nil or value == ""
end

local function tableCount(source)
	local count = 0
	if type(source) ~= "table" then
		return count
	end
	for _, _ in pairs(source) do
		count = count + 1
	end
	return count
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

function LIST_ITEM_METHODS:GetChild(childPath)
	local manager = getCurrentManager()
	return manager ~= nil and manager:GetChild(self.handle, childPath) or nil
end

function LIST_ITEM_METHODS:GetChildByDefine(controlDefines, fieldName)
	if type(controlDefines) ~= "table" then
		return nil
	end

	local define = controlDefines[fieldName]
	if define == nil then
		for _, item in pairs(controlDefines) do
			if type(item) == "table" and item.path == fieldName then
				define = item
				break
			end
		end
	end

	local childPath = type(define) == "table" and define.path or define
	return childPath ~= nil and self:GetChild(childPath) or nil
end

function LIST_ITEM_METHODS:BindControls(controlDefines)
	local controls = {}
	if type(controlDefines) ~= "table" then
		return controls
	end

	for fieldName, define in pairs(controlDefines) do
		local childPath = type(define) == "table" and define.path or define
		if childPath ~= nil then
			controls[fieldName] = self:GetChild(childPath)
		end
	end
	return controls
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

function LIST_ITEM_METHODS:SetControllerPage(controllerName, pageName)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetControllerPage(self.handle, controllerName, pageName) or false
end

function LIST_ITEM_METHODS:SetValue(childPathOrValue, value)
	local manager = getCurrentManager()
	return manager ~= nil and manager:SetValue(self.handle, childPathOrValue, value) or false
end

local function buildTreeKey(parentKey, index, node)
	if type(node) == "table" and node.key ~= nil then
		return tostring(node.key)
	end
	return tostring(parentKey or "root") .. "." .. tostring(index)
end

local function copyTreeRow(node, nodeKey, parentKey, depth, expanded, hasChildren)
	local row = {}
	if type(node) == "table" then
		for key, value in pairs(node) do
			if key ~= "children" then
				row[key] = value
			end
		end
	end
	row.key = nodeKey
	row.parentKey = parentKey
	row.depth = depth
	row.expanded = expanded
	row.hasChildren = hasChildren
	row.source = node
	return row
end

local function flattenTreeNodes(nodes, state, output, depth, parentKey)
	if type(nodes) ~= "table" then
		return
	end

	state.expanded = state.expanded or {}
	for index, node in ipairs(nodes) do
		local nodeKey = buildTreeKey(parentKey, index, node)
		local children = type(node) == "table" and node.children or nil
		local hasChildren = type(children) == "table" and #children > 0
		local expanded = state.expanded[nodeKey]
		if expanded == nil then
			expanded = type(node) ~= "table" or node.expanded ~= false
			state.expanded[nodeKey] = expanded
		end

		local row = copyTreeRow(node, nodeKey, parentKey, depth, expanded, hasChildren)
		row.selected = state.selectedKey ~= nil and tostring(state.selectedKey) == tostring(nodeKey)
		table.insert(output, row)
		if hasChildren and expanded == true then
			flattenTreeNodes(children, state, output, depth + 1, nodeKey)
		end
	end
end

local function findTreeNode(nodes, nodeKey, parentNodes, parentNode)
	if type(nodes) ~= "table" or nodeKey == nil then
		return nil
	end

	nodeKey = tostring(nodeKey)
	for index, node in ipairs(nodes) do
		local currentKey = buildTreeKey(parentNode ~= nil and parentNode.key or nil, index, node)
		if tostring(currentKey) == nodeKey then
			return node, nodes, index, parentNode, currentKey
		end
		local childNode, childParentNodes, childIndex, childParentNode, childKey = findTreeNode(type(node) == "table" and node.children or nil, nodeKey, nodes, node)
		if childNode ~= nil then
			return childNode, childParentNodes, childIndex, childParentNode, childKey
		end
	end
	return nil
end

local function collectTreeKeys(node, output)
	if type(node) ~= "table" then
		return output
	end

	output = output or {}
	if node.key ~= nil then
		output[tostring(node.key)] = true
	end
	if type(node.children) == "table" then
		for _, child in ipairs(node.children) do
			collectTreeKeys(child, output)
		end
	end
	return output
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
	self.childrenByHandle = owner ~= nil and owner.childrenByHandle or self.childrenByHandle or {}
	self.listItemHandlesByHandle = owner ~= nil and owner.listItemHandlesByHandle or self.listItemHandlesByHandle or {}
	self.listItemIndexByHandle = owner ~= nil and owner.listItemIndexByHandle or self.listItemIndexByHandle or {}
	self.listDataByHandle = owner ~= nil and owner.listDataByHandle or self.listDataByHandle or {}
	self.listRenderersByHandle = owner ~= nil and owner.listRenderersByHandle or self.listRenderersByHandle or {}
	self.listVirtualByHandle = owner ~= nil and owner.listVirtualByHandle or self.listVirtualByHandle or {}
	self.listVirtualOptionsByHandle = owner ~= nil and owner.listVirtualOptionsByHandle or self.listVirtualOptionsByHandle or {}
	self.listVirtualStatsByHandle = owner ~= nil and owner.listVirtualStatsByHandle or self.listVirtualStatsByHandle or {}
	self.treeDataByHandle = owner ~= nil and owner.treeDataByHandle or self.treeDataByHandle or {}
	self.treeStateByHandle = owner ~= nil and owner.treeStateByHandle or self.treeStateByHandle or {}
	self.treeRenderersByHandle = owner ~= nil and owner.treeRenderersByHandle or self.treeRenderersByHandle or {}
	self.treeChildPathByHandle = owner ~= nil and owner.treeChildPathByHandle or self.treeChildPathByHandle or {}
	if owner ~= nil then
		owner.childrenByHandle = self.childrenByHandle
		owner.listItemHandlesByHandle = self.listItemHandlesByHandle
		owner.listItemIndexByHandle = self.listItemIndexByHandle
		owner.listDataByHandle = self.listDataByHandle
		owner.listRenderersByHandle = self.listRenderersByHandle
		owner.listVirtualByHandle = self.listVirtualByHandle
		owner.listVirtualOptionsByHandle = self.listVirtualOptionsByHandle
		owner.listVirtualStatsByHandle = self.listVirtualStatsByHandle
		owner.treeDataByHandle = self.treeDataByHandle
		owner.treeStateByHandle = self.treeStateByHandle
		owner.treeRenderersByHandle = self.treeRenderersByHandle
		owner.treeChildPathByHandle = self.treeChildPathByHandle
	end
end

function FairyGuiLists:GetOrCreateVirtualStats(listHandle)
	local owner = self.owner
	if owner == nil or listHandle == nil then
		return nil
	end

	local stats = owner.listVirtualStatsByHandle[listHandle]
	if stats == nil then
		stats = {
			virtual = owner.listVirtualByHandle[listHandle] == true,
			dataCount = 0,
			renderCount = 0,
			itemHandleCount = 0,
			realizedCount = 0,
			reuseCount = 0,
			lastIndex = 0,
			lastItemHandle = 0,
			lastRenderOk = false,
			loop = false,
			fallback = false,
		}
		owner.listVirtualStatsByHandle[listHandle] = stats
	end
	return stats
end

function FairyGuiLists:ResetVirtualStats(listHandle, dataCount, options, virtualEnabled)
	local owner = self.owner
	if owner == nil or listHandle == nil then
		return nil
	end

	local stats = {
		virtual = virtualEnabled == true,
		dataCount = dataCount or 0,
		renderCount = 0,
		itemHandleCount = 0,
		realizedCount = 0,
		reuseCount = 0,
		lastIndex = 0,
		lastItemHandle = 0,
		lastRenderOk = false,
		loop = options ~= nil and options.loop == true,
		fallback = virtualEnabled ~= true,
	}
	owner.listVirtualStatsByHandle[listHandle] = stats
	return stats
end

function FairyGuiLists:UpdateVirtualStatsCounts(listHandle)
	local owner = self.owner
	if owner == nil or listHandle == nil then
		return nil
	end

	local stats = owner.listVirtualStatsByHandle[listHandle]
	if stats == nil then
		return nil
	end
	stats.itemHandleCount = tableCount(owner.listItemHandlesByHandle[listHandle])
	stats.realizedCount = tableCount(owner.listItemIndexByHandle[listHandle])
	local dataList = owner.listDataByHandle[listHandle]
	stats.dataCount = type(dataList) == "table" and #dataList or 0
	return stats
end

function FairyGuiLists:GetChild(handle, childPath)
	local self = self.owner
	if self == nil or NativeApi == nil or handle == nil or isBlank(childPath) then
		return handle
	end

	local childHandles = self.childrenByHandle[handle]
	if childHandles ~= nil and childHandles[childPath] ~= nil then
		return childHandles[childPath]
	end

	local childHandle = NativeApi:getFairyGuiChild(handle, childPath)
	if childHandle == nil or childHandle <= 0 then
		local policy = self.resourceFallbackPolicy or {}
		if policy.recordMissingChild == true then
			local objectInfo = self:GetObjectInfo(handle)
			self:RecordResourceFallback("missingChild", {
				handle = handle,
				key = objectInfo ~= nil and objectInfo.key or nil,
				uiName = objectInfo ~= nil and objectInfo.uiName or nil,
				packageName = objectInfo ~= nil and objectInfo.packageName or nil,
				packagePath = objectInfo ~= nil and objectInfo.packagePath or nil,
				objectName = objectInfo ~= nil and objectInfo.objectName or nil,
				childPath = childPath,
			}, "getFairyGuiChild returned empty handle")
		end
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
	local lists = self
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
	self.listItemIndexByHandle[listHandle] = nil
	self.listDataByHandle[listHandle] = nil
	self.listRenderersByHandle[listHandle] = nil
	self.listVirtualByHandle[listHandle] = nil
	self.listVirtualOptionsByHandle[listHandle] = nil
	self.listVirtualStatsByHandle[listHandle] = nil
	self.treeDataByHandle[listHandle] = nil
	self.treeStateByHandle[listHandle] = nil
	self.treeRenderersByHandle[listHandle] = nil
	self.treeChildPathByHandle[listHandle] = nil
	lists:UpdateVirtualStatsCounts(listHandle)
end

function FairyGuiLists:ClearListItemHandleCache(listHandle)
	local lists = self
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
	self.listItemIndexByHandle[listHandle] = nil
	lists:UpdateVirtualStatsCounts(listHandle)
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
	if self == nil or NativeApi == nil or listHandle == nil or index == nil then
		return nil
	end

	local itemHandles = self.listItemHandlesByHandle[listHandle]
	if itemHandles == nil then
		itemHandles = {}
		self.listItemHandlesByHandle[listHandle] = itemHandles
	end

	local itemHandle = itemHandles[index]
	if itemHandle == nil then
		itemHandle = NativeApi:getFairyGuiListItem(listHandle, index - 1)
		if itemHandle == nil or itemHandle <= 0 then
			return nil
		end
		itemHandles[index] = itemHandle
	end
	return self:CreateListItemAdapter(listHandle, itemHandle, index, data)
end

function FairyGuiLists:RenderListItemByHandle(listHandle, index, itemHandle)
	local self = self.owner
	if self == nil then
		return false
	end

	local dataList = self.listDataByHandle[listHandle]
	local data = dataList ~= nil and dataList[index] or nil
	local item = itemHandle ~= nil and self:CreateListItemAdapter(listHandle, itemHandle, index, data) or self:GetListItemByHandle(listHandle, index, data)
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

function FairyGuiLists:RenderVirtualListItemByHandle(listHandle, index, itemHandle)
	local lists = self
	local self = self.owner
	if self == nil or listHandle == nil or index == nil or itemHandle == nil or itemHandle <= 0 then
		return false
	end

	local itemHandles = self.listItemHandlesByHandle[listHandle]
	if itemHandles == nil then
		itemHandles = {}
		self.listItemHandlesByHandle[listHandle] = itemHandles
	end

	local itemIndexes = self.listItemIndexByHandle[listHandle]
	if itemIndexes == nil then
		itemIndexes = {}
		self.listItemIndexByHandle[listHandle] = itemIndexes
	end

	local stats = lists:GetOrCreateVirtualStats(listHandle)
	local oldIndex = itemIndexes[itemHandle]
	if oldIndex ~= nil and oldIndex ~= index then
		itemHandles[oldIndex] = nil
		self.childrenByHandle[itemHandle] = nil
		if stats ~= nil then
			stats.reuseCount = (stats.reuseCount or 0) + 1
		end
	end

	local oldHandle = itemHandles[index]
	if oldHandle ~= nil and oldHandle ~= itemHandle then
		itemIndexes[oldHandle] = nil
		self.childrenByHandle[oldHandle] = nil
	end

	itemHandles[index] = itemHandle
	itemIndexes[itemHandle] = index

	if stats ~= nil then
		stats.renderCount = (stats.renderCount or 0) + 1
		stats.lastIndex = index
		stats.lastItemHandle = itemHandle
	end
	local rendered = self:RenderListItemByHandle(listHandle, index, itemHandle)
	if stats ~= nil then
		stats.lastRenderOk = rendered == true
	end
	lists:UpdateVirtualStatsCounts(listHandle)
	return rendered
end

function FairyGuiLists:GetListHandle(handle, childPath)
	return self:GetTargetHandle(handle, childPath)
end

function FairyGuiLists:SetListItemCount(handle, childPath, itemCount)
	local self = self.owner
	if self == nil or NativeApi == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	self:ClearListItemHandleCache(listHandle)
	return NativeApi:setFairyGuiListItemCount(listHandle, itemCount or 0)
end

function FairyGuiLists:GetListItemCount(handle, childPath)
	local self = self.owner
	if self == nil or NativeApi == nil then
		return 0
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return 0
	end
	return NativeApi:getFairyGuiListItemCount(listHandle)
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

function FairyGuiLists:SetVirtualListData(handle, childPath, dataList, renderer, options)
	local lists = self
	local self = self.owner
	if self == nil or type(dataList) ~= "table" then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	self.listDataByHandle[listHandle] = dataList
	if type(renderer) == "function" then
		self.listRenderersByHandle[listHandle] = renderer
	else
		self.listRenderersByHandle[listHandle] = nil
	end
	self:ClearListItemHandleCache(listHandle)
	self.listVirtualOptionsByHandle[listHandle] = options or {}

	local virtualEnabled = false
	if NativeApi ~= nil and NativeApi.setFairyGuiListVirtual ~= nil then
		virtualEnabled = NativeApi:setFairyGuiListVirtual(listHandle, options ~= nil and options.loop == true) == true
	end
	self.listVirtualByHandle[listHandle] = virtualEnabled
	lists:ResetVirtualStats(listHandle, #dataList, options, virtualEnabled)
	if virtualEnabled then
		local countOk = NativeApi:setFairyGuiListItemCount(listHandle, #dataList)
		if countOk and NativeApi.refreshFairyGuiList ~= nil then
			NativeApi:refreshFairyGuiList(listHandle)
		end
		lists:UpdateVirtualStatsCounts(listHandle)
		return countOk == true
	end
	return self:SetListData(handle, childPath, dataList, renderer)
end

function FairyGuiLists:BuildTreeFlatData(listHandle)
	local self = self.owner
	if self == nil or listHandle == nil then
		return {}
	end

	local flat = {}
	local state = self.treeStateByHandle[listHandle] or { expanded = {} }
	self.treeStateByHandle[listHandle] = state
	flattenTreeNodes(self.treeDataByHandle[listHandle], state, flat, 0, nil)
	return flat
end

function FairyGuiLists:RefreshTree(handle, childPath)
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	local flat = self:GetLists():BuildTreeFlatData(listHandle)
	local renderer = self.treeRenderersByHandle[listHandle]
	return self:GetLists():SetListData(handle, childPath, flat, function(item, node, index)
		item.treeNode = node
		item.treeDepth = node ~= nil and node.depth or 0
		item.treeKey = node ~= nil and node.key or nil
		if type(renderer) == "function" then
			renderer(item, node, index)
		else
			local indent = string.rep("  ", item.treeDepth or 0)
			local marker = node ~= nil and node.hasChildren and (node.expanded and "- " or "+ ") or "  "
			item:SetText("", indent .. marker .. tostring(node and (node.text or node.label or node.key) or ""))
		end
	end)
end

function FairyGuiLists:SetTreeData(handle, childPath, treeData, renderer, options)
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	local nodes = treeData
	if type(treeData) == "table" and treeData.nodes ~= nil then
		nodes = treeData.nodes
	end
	if type(nodes) ~= "table" then
		return false
	end

	self.treeDataByHandle[listHandle] = nodes
	self.treeChildPathByHandle[listHandle] = childPath
	if options ~= nil and options.resetState == true or self.treeStateByHandle[listHandle] == nil then
		self.treeStateByHandle[listHandle] = { expanded = {} }
	end
	if type(renderer) == "function" then
		self.treeRenderersByHandle[listHandle] = renderer
	else
		self.treeRenderersByHandle[listHandle] = nil
	end
	return self:GetLists():RefreshTree(handle, childPath)
end

function FairyGuiLists:GetTreeFlatData(handle, childPath)
	local self = self.owner
	if self == nil then
		return {}
	end

	local listHandle = self:GetListHandle(handle, childPath)
	return self:GetLists():BuildTreeFlatData(listHandle)
end

function FairyGuiLists:GetTreeNode(handle, childPath, nodeKey)
	local self = self.owner
	if self == nil or nodeKey == nil then
		return nil
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return nil
	end
	local node = findTreeNode(self.treeDataByHandle[listHandle], nodeKey)
	return node
end

function FairyGuiLists:AddTreeNode(handle, childPath, parentKey, node, index)
	local self = self.owner
	if self == nil or type(node) ~= "table" then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or self.treeDataByHandle[listHandle] == nil then
		return false
	end

	local nodes = self.treeDataByHandle[listHandle]
	if parentKey ~= nil and parentKey ~= "" then
		local parentNode = findTreeNode(nodes, parentKey)
		if parentNode == nil then
			return false
		end
		if type(parentNode.children) ~= "table" then
			parentNode.children = {}
		end
		nodes = parentNode.children
		local state = self.treeStateByHandle[listHandle] or { expanded = {} }
		self.treeStateByHandle[listHandle] = state
		state.expanded[tostring(parentKey)] = true
	end

	index = tonumber(index)
	if index ~= nil and index >= 1 and index <= #nodes + 1 then
		table.insert(nodes, index, node)
	else
		table.insert(nodes, node)
	end
	return self:GetLists():RefreshTree(handle, childPath)
end

function FairyGuiLists:RemoveTreeNode(handle, childPath, nodeKey)
	local self = self.owner
	if self == nil or nodeKey == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or self.treeDataByHandle[listHandle] == nil then
		return false
	end

	local node, parentNodes, index = findTreeNode(self.treeDataByHandle[listHandle], nodeKey)
	if node == nil or parentNodes == nil or index == nil then
		return false
	end

	local removedKeys = collectTreeKeys(node)
	table.remove(parentNodes, index)
	local state = self.treeStateByHandle[listHandle]
	if state ~= nil then
		if type(state.expanded) == "table" then
			for key, _ in pairs(removedKeys) do
				state.expanded[key] = nil
			end
		end
		if state.selectedKey ~= nil and removedKeys[tostring(state.selectedKey)] == true then
			state.selectedKey = nil
		end
	end
	return self:GetLists():RefreshTree(handle, childPath)
end

function FairyGuiLists:ClearTree(handle, childPath)
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or self.treeDataByHandle[listHandle] == nil then
		return false
	end

	self.treeDataByHandle[listHandle] = {}
	self.treeStateByHandle[listHandle] = { expanded = {} }
	return self:GetLists():RefreshTree(handle, childPath)
end

function FairyGuiLists:UpdateTreeNode(handle, childPath, nodeKey, data)
	local self = self.owner
	if self == nil or nodeKey == nil or type(data) ~= "table" then
		return false
	end

	local node = self:GetLists():GetTreeNode(handle, childPath, nodeKey)
	if node == nil then
		return false
	end
	for key, value in pairs(data) do
		node[key] = value
	end
	return self:GetLists():RefreshTree(handle, childPath)
end

function FairyGuiLists:SetTreeNodeExpanded(handle, childPath, nodeKey, expanded)
	local self = self.owner
	if self == nil or nodeKey == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or self.treeDataByHandle[listHandle] == nil then
		return false
	end

	local state = self.treeStateByHandle[listHandle] or { expanded = {} }
	self.treeStateByHandle[listHandle] = state
	state.expanded[tostring(nodeKey)] = expanded == true
	return self:GetLists():RefreshTree(handle, childPath)
end

function FairyGuiLists:SetTreeNodeSelected(handle, childPath, nodeKey)
	local self = self.owner
	if self == nil or nodeKey == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or self.treeDataByHandle[listHandle] == nil then
		return false
	end
	local node = findTreeNode(self.treeDataByHandle[listHandle], nodeKey)
	if node == nil then
		return false
	end

	local state = self.treeStateByHandle[listHandle] or { expanded = {} }
	self.treeStateByHandle[listHandle] = state
	state.selectedKey = tostring(nodeKey)
	local refreshOk = self:GetLists():RefreshTree(handle, childPath)
	local flat = self:GetLists():BuildTreeFlatData(listHandle)
	for index, row in ipairs(flat) do
		if tostring(row.key) == tostring(nodeKey) then
			if NativeApi ~= nil and NativeApi.setFairyGuiListSelectedIndex ~= nil then
				NativeApi:setFairyGuiListSelectedIndex(listHandle, index - 1)
			end
			break
		end
	end
	return refreshOk
end

function FairyGuiLists:GetTreeSelectedKey(handle, childPath)
	local self = self.owner
	if self == nil then
		return nil
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return nil
	end
	local state = self.treeStateByHandle[listHandle]
	return state ~= nil and state.selectedKey or nil
end

function FairyGuiLists:GetTreeSelectedNode(handle, childPath)
	local selectedKey = self:GetTreeSelectedKey(handle, childPath)
	if selectedKey == nil then
		return nil
	end
	return self:GetTreeNode(handle, childPath, selectedKey)
end

function FairyGuiLists:ToggleTreeNode(handle, childPath, nodeKey)
	local self = self.owner
	if self == nil or nodeKey == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil or self.treeDataByHandle[listHandle] == nil then
		return false
	end

	local state = self.treeStateByHandle[listHandle] or { expanded = {} }
	self.treeStateByHandle[listHandle] = state
	nodeKey = tostring(nodeKey)
	state.expanded[nodeKey] = state.expanded[nodeKey] ~= true
	return self:GetLists():RefreshTree(handle, childPath)
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
	if self.listVirtualByHandle[listHandle] == true then
		local itemHandles = self.listItemHandlesByHandle[listHandle]
		local itemHandle = itemHandles ~= nil and itemHandles[index] or nil
		if itemHandle ~= nil then
			return self:RenderListItemByHandle(listHandle, index, itemHandle)
		end
		return NativeApi ~= nil and NativeApi.refreshFairyGuiList ~= nil and NativeApi:refreshFairyGuiList(listHandle) == true
	end
	return self:RenderListItemByHandle(listHandle, index)
end

function FairyGuiLists:RefreshList(handle, childPath)
	local lists = self
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	if self.listVirtualByHandle[listHandle] == true and NativeApi ~= nil and NativeApi.refreshFairyGuiList ~= nil then
		local refreshOk = NativeApi:refreshFairyGuiList(listHandle)
		lists:UpdateVirtualStatsCounts(listHandle)
		return refreshOk
	end

	local dataList = self.listDataByHandle[listHandle]
	local itemCount = dataList ~= nil and #dataList or self:GetListItemCount(handle, childPath)
	for index = 1, itemCount do
		self:RenderListItemByHandle(listHandle, index)
	end
	return true
end

function FairyGuiLists:UpdateListItem(handle, childPath, index, data)
	local lists = self
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
	if self.listVirtualByHandle[listHandle] == true then
		local itemHandles = self.listItemHandlesByHandle[listHandle]
		local itemHandle = itemHandles ~= nil and itemHandles[index] or nil
		local renderOk = itemHandle ~= nil and self:RenderListItemByHandle(listHandle, index, itemHandle) or false
		local refreshOk = NativeApi ~= nil and NativeApi.refreshFairyGuiList ~= nil and NativeApi:refreshFairyGuiList(listHandle) == true
		lists:UpdateVirtualStatsCounts(listHandle)
		return renderOk == true or refreshOk == true
	end
	return self:RenderListItemByHandle(listHandle, index)
end

function FairyGuiLists:AppendListItem(handle, childPath, data)
	local lists = self
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
	if self.listVirtualByHandle[listHandle] == true then
		if NativeApi == nil or NativeApi.setFairyGuiListItemCount == nil then
			return false
		end
		local countOk = NativeApi:setFairyGuiListItemCount(listHandle, #dataList)
		if countOk and NativeApi.refreshFairyGuiList ~= nil then
			NativeApi:refreshFairyGuiList(listHandle)
		end
		lists:UpdateVirtualStatsCounts(listHandle)
		return countOk == true
	end
	if not self:SetListItemCount(handle, childPath, #dataList) then
		return false
	end
	return self:RenderListItemByHandle(listHandle, #dataList)
end

function FairyGuiLists:RemoveListItem(handle, childPath, index)
	local lists = self
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
	if self.listVirtualByHandle[listHandle] == true then
		self:ClearListItemHandleCache(listHandle)
		if NativeApi == nil or NativeApi.setFairyGuiListItemCount == nil then
			return false
		end
		local countOk = NativeApi:setFairyGuiListItemCount(listHandle, #dataList)
		if countOk and NativeApi.refreshFairyGuiList ~= nil then
			NativeApi:refreshFairyGuiList(listHandle)
		end
		lists:UpdateVirtualStatsCounts(listHandle)
		return countOk == true
	end
	if not self:SetListItemCount(handle, childPath, #dataList) then
		return false
	end
	return self:RefreshList(handle, childPath)
end

function FairyGuiLists:ClearList(handle, childPath)
	local lists = self
	local self = self.owner
	if self == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end

	local renderer = self.listRenderersByHandle[listHandle]
	if self.listVirtualByHandle[listHandle] == true then
		self.listDataByHandle[listHandle] = {}
		self:ClearListItemHandleCache(listHandle)
		if NativeApi == nil or NativeApi.setFairyGuiListItemCount == nil then
			return false
		end
		local countOk = NativeApi:setFairyGuiListItemCount(listHandle, 0)
		if countOk and NativeApi.refreshFairyGuiList ~= nil then
			NativeApi:refreshFairyGuiList(listHandle)
		end
		lists:UpdateVirtualStatsCounts(listHandle)
		return countOk == true
	end
	return self:SetListData(handle, childPath, {}, renderer)
end

function FairyGuiLists:GetListDebugStats(handle, childPath)
	local self = self.owner
	if self == nil then
		return {}
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return {}
	end

	local dataList = self.listDataByHandle[listHandle]
	local itemHandles = self.listItemHandlesByHandle[listHandle]
	local itemIndexes = self.listItemIndexByHandle[listHandle]
	local stats = self.listVirtualStatsByHandle[listHandle]
	local result = {
		listHandle = listHandle,
		childPath = childPath,
		virtual = self.listVirtualByHandle[listHandle] == true,
		dataCount = type(dataList) == "table" and #dataList or 0,
		itemHandleCount = tableCount(itemHandles),
		realizedCount = tableCount(itemIndexes),
		renderer = type(self.listRenderersByHandle[listHandle]) == "function",
		backendItemCount = NativeApi ~= nil and NativeApi.getFairyGuiListItemCount ~= nil and NativeApi:getFairyGuiListItemCount(listHandle) or 0,
		renderCount = 0,
		reuseCount = 0,
		lastIndex = 0,
		lastItemHandle = 0,
		lastRenderOk = false,
		loop = false,
		fallback = false,
	}
	if stats ~= nil then
		result.virtual = stats.virtual == true
		result.dataCount = stats.dataCount or result.dataCount
		result.itemHandleCount = stats.itemHandleCount or result.itemHandleCount
		result.realizedCount = stats.realizedCount or result.realizedCount
		result.renderCount = stats.renderCount or 0
		result.reuseCount = stats.reuseCount or 0
		result.lastIndex = stats.lastIndex or 0
		result.lastItemHandle = stats.lastItemHandle or 0
		result.lastRenderOk = stats.lastRenderOk == true
		result.loop = stats.loop == true
		result.fallback = stats.fallback == true
	end
	return result
end

function FairyGuiLists:DumpListDebugStats(handle, childPath, label)
	local stats = self:GetListDebugStats(handle, childPath)
	print(
		"[FGUI] list debug stats:",
		label or childPath or "",
		"listHandle=", stats.listHandle,
		"virtual=", stats.virtual,
		"data=", stats.dataCount,
		"backend=", stats.backendItemCount,
		"items=", stats.itemHandleCount,
		"realized=", stats.realizedCount,
		"render=", stats.renderCount,
		"reuse=", stats.reuseCount,
		"last=", stats.lastIndex,
		stats.lastItemHandle,
		"fallback=", stats.fallback)
	return stats
end

function FairyGuiLists:SetListSelectedIndex(handle, childPath, selectedIndex)
	local self = self.owner
	if self == nil or NativeApi == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end
	return NativeApi:setFairyGuiListSelectedIndex(listHandle, (selectedIndex or 1) - 1)
end

function FairyGuiLists:GetListSelectedIndex(handle, childPath)
	local self = self.owner
	if self == nil or NativeApi == nil then
		return 0
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return 0
	end

	local selectedIndex = NativeApi:getFairyGuiListSelectedIndex(listHandle)
	return selectedIndex >= 0 and selectedIndex + 1 or 0
end

function FairyGuiLists:ScrollListToView(handle, childPath, index)
	local self = self.owner
	if self == nil or NativeApi == nil then
		return false
	end

	local listHandle = self:GetListHandle(handle, childPath)
	if listHandle == nil then
		return false
	end
	return NativeApi:scrollFairyGuiListToView(listHandle, (index or 1) - 1)
end

return FairyGuiLists
