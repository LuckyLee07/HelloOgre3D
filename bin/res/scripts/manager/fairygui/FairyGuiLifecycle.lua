local NativeApi = require("res.scripts.manager.fairygui.FairyGuiNativeApi")

local FairyGuiLifecycle = Class("FairyGuiLifecycle")

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

local nonOwnedObjectHandleFields = {
	parentHandle = true,
	rootHandle = true,
	attachHandle = true,
}

local function pushUniqueHandle(handles, handleSet, handle)
	if type(handle) ~= "number" or handle <= 0 or handleSet[handle] == true then
		return
	end
	handleSet[handle] = true
	table.insert(handles, handle)
end

local function containsHandle(handles, targetHandle)
	if type(handles) ~= "table" or targetHandle == nil then
		return false
	end

	for _, handle in ipairs(handles) do
		if handle == targetHandle then
			return true
		end
	end
	return false
end

local function getObjectState(owner)
	if owner ~= nil and owner.GetStore ~= nil then
		local store = owner:GetStore()
		if store ~= nil and store.GetObjectState ~= nil then
			return store:GetObjectState()
		end
	end
	return nil
end

function FairyGuiLifecycle:Init(owner)
	self.owner = owner

	local objectState = getObjectState(owner)
	self.objectState = objectState
end

function FairyGuiLifecycle:CollectOwnedHandles(objectInfo)
	local self = self.owner
	local handles = {}
	local handleSet = {}
	if objectInfo == nil then
		return handles, handleSet
	end

	pushUniqueHandle(handles, handleSet, objectInfo.handle)
	if type(objectInfo.ownedHandles) == "table" then
		for _, ownedHandle in pairs(objectInfo.ownedHandles) do
			pushUniqueHandle(handles, handleSet, ownedHandle)
		end
	end
	for fieldName, value in pairs(objectInfo) do
		if type(fieldName) == "string" and type(value) == "number" and string.match(fieldName, "Handle$") ~= nil and nonOwnedObjectHandleFields[fieldName] ~= true then
			pushUniqueHandle(handles, handleSet, value)
		elseif type(fieldName) == "string" and type(value) == "table" and string.match(fieldName, "Handles$") ~= nil then
			for _, ownedHandle in pairs(value) do
				pushUniqueHandle(handles, handleSet, ownedHandle)
			end
		end
	end

	local listState = self:GetStore():GetListState()
	local index = 1
	while index <= #handles do
		local handle = handles[index]
		local childHandles = listState.childrenByHandle[handle]
		if childHandles ~= nil then
			for _, childHandle in pairs(childHandles) do
				pushUniqueHandle(handles, handleSet, childHandle)
			end
		end

		local itemHandles = listState.listItemHandlesByHandle[handle]
		if itemHandles ~= nil then
			for _, itemHandle in pairs(itemHandles) do
				pushUniqueHandle(handles, handleSet, itemHandle)
			end
		end
		index = index + 1
	end
	return handles, handleSet
end

function FairyGuiLifecycle:CreateCloseSnapshot(objectInfo, ownedHandles)
	local self = self.owner
	if objectInfo == nil then
		return nil
	end

	local snapshot = {
		key = objectInfo.key,
		handle = objectInfo.handle,
		uiName = objectInfo.uiName,
		layer = objectInfo.layer,
		modalMaskHandle = objectInfo.modalMaskHandle,
		viewRef = objectInfo.view,
		ctrlRef = objectInfo.ctrl,
		ownedHandles = {},
	}
	if ownedHandles == nil then
		ownedHandles = self:CollectOwnedHandles(objectInfo)
	end
	for index, handle in ipairs(ownedHandles) do
		snapshot.ownedHandles[index] = handle
	end
	return snapshot
end

function FairyGuiLifecycle:CaptureCloseSnapshot(keyOrHandle)
	local self = self.owner
	local objectInfo = self:GetObjectInfo(keyOrHandle)
	if objectInfo == nil then
		return nil
	end
	local ownedHandles = self:CollectOwnedHandles(objectInfo)
	return self:CreateCloseSnapshot(objectInfo, ownedHandles)
end

function FairyGuiLifecycle:GetCloseResidue(objectInfo, ownedHandles)
	local self = self.owner
	local issues = {}
	if objectInfo == nil then
		table.insert(issues, "objectInfo=nil")
		return issues
	end
	local store = self:GetStore()
	local objectState = store:GetObjectState()
	local layerState = store:GetLayerState()
	local eventState = store:GetEventState()
	local listState = store:GetListState()

	local key = objectInfo.key
	local handle = objectInfo.handle
	local handleSet = {}
	local handles = ownedHandles or objectInfo.ownedHandles or {}
	for _, ownedHandle in ipairs(handles) do
		if type(ownedHandle) == "number" and ownedHandle > 0 then
			handleSet[ownedHandle] = true
		end
	end
	if type(handle) == "number" and handle > 0 then
		handleSet[handle] = true
	end
	if type(objectInfo.modalMaskHandle) == "number" and objectInfo.modalMaskHandle > 0 then
		handleSet[objectInfo.modalMaskHandle] = true
	end

	if key ~= nil and objectState.objects[key] ~= nil then
		table.insert(issues, "objects[" .. tostring(key) .. "]")
	end
	if key ~= nil and objectState.hiddenObjects[key] ~= nil then
		table.insert(issues, "hiddenObjects[" .. tostring(key) .. "]")
	end
	if key ~= nil and objectState.views[key] ~= nil then
		table.insert(issues, "views[" .. tostring(key) .. "]")
	end
	if key ~= nil and objectState.controllers[key] ~= nil then
		table.insert(issues, "controllers[" .. tostring(key) .. "]")
	end
	if key ~= nil and objectState.childKeysByParentKey[key] ~= nil then
		table.insert(issues, "childKeysByParentKey[" .. tostring(key) .. "]")
	end
	if key ~= nil and objectState.parentKeyByChildKey[key] ~= nil then
		table.insert(issues, "parentKeyByChildKey[" .. tostring(key) .. "]")
	end
	if key ~= nil and layerState.stackEntriesByKey[key] ~= nil then
		table.insert(issues, "stackEntriesByKey[" .. tostring(key) .. "]")
	end
	if key ~= nil and eventState.timersByKey[key] ~= nil and tableCount(eventState.timersByKey[key]) > 0 then
		table.insert(issues, "timersByKey[" .. tostring(key) .. "]")
	end
	if objectInfo.uiName ~= nil and objectState.uiNameToKey[objectInfo.uiName] == key then
		table.insert(issues, "uiNameToKey[" .. tostring(objectInfo.uiName) .. "]")
	end
	if objectInfo.viewRef ~= nil and objectInfo.viewRef.GetTimerCount ~= nil and objectInfo.viewRef:GetTimerCount() > 0 then
		table.insert(issues, "viewTimer[" .. tostring(objectInfo.viewRef:GetTimerCount()) .. "]")
	end
	if objectInfo.ctrlRef ~= nil and objectInfo.ctrlRef.GetTimerCount ~= nil and objectInfo.ctrlRef:GetTimerCount() > 0 then
		table.insert(issues, "ctrlTimer[" .. tostring(objectInfo.ctrlRef:GetTimerCount()) .. "]")
	end

	for ownedHandle, _ in pairs(handleSet) do
		if objectState.objectsByHandle[ownedHandle] ~= nil then
			table.insert(issues, "objectsByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if objectState.viewsByHandle[ownedHandle] ~= nil then
			table.insert(issues, "viewsByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if objectState.controllersByHandle[ownedHandle] ~= nil then
			table.insert(issues, "controllersByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if eventState.bindingsByHandle[ownedHandle] ~= nil and tableCount(eventState.bindingsByHandle[ownedHandle]) > 0 then
			table.insert(issues, "bindingsByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if eventState.transitionCallbacksByHandle[ownedHandle] ~= nil and tableCount(eventState.transitionCallbacksByHandle[ownedHandle]) > 0 then
			table.insert(issues, "transitionCallbacksByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.childrenByHandle[ownedHandle] ~= nil and tableCount(listState.childrenByHandle[ownedHandle]) > 0 then
			table.insert(issues, "childrenByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.listItemHandlesByHandle[ownedHandle] ~= nil then
			table.insert(issues, "listItemHandlesByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.listItemIndexByHandle[ownedHandle] ~= nil then
			table.insert(issues, "listItemIndexByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.listDataByHandle[ownedHandle] ~= nil then
			table.insert(issues, "listDataByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.listRenderersByHandle[ownedHandle] ~= nil then
			table.insert(issues, "listRenderersByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.listVirtualByHandle[ownedHandle] ~= nil then
			table.insert(issues, "listVirtualByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.listVirtualOptionsByHandle[ownedHandle] ~= nil then
			table.insert(issues, "listVirtualOptionsByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.listVirtualStatsByHandle[ownedHandle] ~= nil then
			table.insert(issues, "listVirtualStatsByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.treeDataByHandle[ownedHandle] ~= nil then
			table.insert(issues, "treeDataByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.treeStateByHandle[ownedHandle] ~= nil then
			table.insert(issues, "treeStateByHandle[" .. tostring(ownedHandle) .. "]")
		end
		if listState.treeRenderersByHandle[ownedHandle] ~= nil then
			table.insert(issues, "treeRenderersByHandle[" .. tostring(ownedHandle) .. "]")
		end
	end

	for bindingId, binding in pairs(eventState.bindings) do
		if binding ~= nil and binding.handle ~= nil and handleSet[binding.handle] == true then
			table.insert(issues, "bindings[" .. tostring(bindingId) .. "]")
		end
	end
	for parentKey, childMap in pairs(objectState.childKeysByParentKey) do
		if parentKey == key then
			table.insert(issues, "childKeysParent[" .. tostring(parentKey) .. "]")
		elseif type(childMap) == "table" and childMap[key] == true then
			table.insert(issues, "childKeysRef[" .. tostring(parentKey) .. ":" .. tostring(key) .. "]")
		end
	end
	for childKey, parentKey in pairs(objectState.parentKeyByChildKey) do
		if childKey == key or parentKey == key then
			table.insert(issues, "parentKeyRef[" .. tostring(childKey) .. ":" .. tostring(parentKey) .. "]")
		end
	end
	for callbackId, callbackInfo in pairs(eventState.transitionCallbacks) do
		if callbackInfo ~= nil and callbackInfo.handle ~= nil and handleSet[callbackInfo.handle] == true then
			table.insert(issues, "transitionCallbacks[" .. tostring(callbackId) .. "]")
		end
	end
	for timerId, timerInfo in pairs(eventState.timers) do
		if timerInfo ~= nil and timerInfo.key == key then
			table.insert(issues, "timers[" .. tostring(timerId) .. "]")
		end
	end
	for parentHandle, childHandles in pairs(listState.childrenByHandle) do
		if handleSet[parentHandle] == true then
			table.insert(issues, "childrenByHandleParent[" .. tostring(parentHandle) .. "]")
		elseif type(childHandles) == "table" then
			for childPath, childHandle in pairs(childHandles) do
				if childHandle ~= nil and handleSet[childHandle] == true then
					table.insert(issues, "childrenByHandleRef[" .. tostring(parentHandle) .. ":" .. tostring(childPath) .. "]")
				end
			end
		end
	end
	for listHandle, itemHandles in pairs(listState.listItemHandlesByHandle) do
		if handleSet[listHandle] == true then
			table.insert(issues, "listItemHandlesParent[" .. tostring(listHandle) .. "]")
		elseif type(itemHandles) == "table" then
			for itemIndex, itemHandle in pairs(itemHandles) do
				if itemHandle ~= nil and handleSet[itemHandle] == true then
					table.insert(issues, "listItemHandlesRef[" .. tostring(listHandle) .. ":" .. tostring(itemIndex) .. "]")
				end
			end
		end
	end
	for listHandle, itemIndexes in pairs(listState.listItemIndexByHandle) do
		if handleSet[listHandle] == true then
			table.insert(issues, "listItemIndexParent[" .. tostring(listHandle) .. "]")
		elseif type(itemIndexes) == "table" then
			for itemHandle, itemIndex in pairs(itemIndexes) do
				if itemHandle ~= nil and handleSet[itemHandle] == true then
					table.insert(issues, "listItemIndexRef[" .. tostring(listHandle) .. ":" .. tostring(itemIndex) .. "]")
				end
			end
		end
	end
	for layerName, layerObjects in pairs(layerState.layerObjects) do
		for ownedHandle, _ in pairs(handleSet) do
			if layerObjects[ownedHandle] ~= nil then
				table.insert(issues, "layerObjects[" .. tostring(layerName) .. ":" .. tostring(ownedHandle) .. "]")
			end
		end
	end
	for index, entry in ipairs(layerState.uiStack) do
		if entry ~= nil and entry.key == key then
			table.insert(issues, "uiStack[" .. tostring(index) .. "]")
		end
	end
	for index, entry in ipairs(layerState.popupStack) do
		if entry ~= nil and entry.key == key then
			table.insert(issues, "popupStack[" .. tostring(index) .. "]")
		end
	end

	local focusedHandle = self:GetFocusedHandle()
	if focusedHandle ~= nil and handleSet[focusedHandle] == true then
		table.insert(issues, "focusedHandle[" .. tostring(focusedHandle) .. "]")
	end
	return issues
end

function FairyGuiLifecycle:ValidateClosedObject(objectInfo, ownedHandles, label, printWhenClean)
	local self = self.owner
	local issues = self:GetCloseResidue(objectInfo, ownedHandles)
	if #issues <= 0 then
		if printWhenClean == true then
			print("[FGUI] close residue ok:", label or "", "key=", objectInfo and objectInfo.key)
		end
		return true, issues
	end

	local message = table.concat(issues, "; ")
	print("[FGUI] close residue warning:", label or "", "key=", objectInfo and objectInfo.key, message)
	if self:IsStrictLifecycleEnabled() then
		error("[FGUI] close residue: " .. message)
	end
	return false, issues
end

function FairyGuiLifecycle:ClearFocusForHandles(ownedHandles)
	local self = self.owner
	local focusedHandle = self:GetFocusedHandle()
	if focusedHandle == nil or focusedHandle <= 0 then
		return false
	end
	if not containsHandle(ownedHandles, focusedHandle) then
		return false
	end
	return self:ClearFocus()
end

function FairyGuiLifecycle:AddTimer(keyOrHandle, duration, interval, tickFunc, finishFunc)
	local self = self.owner
	if threadpool == nil or threadpool.timer == nil then
		return nil
	end

	local objectInfo = self:GetObjectInfo(keyOrHandle)
	if objectInfo == nil then
		return nil
	end

	local key = objectInfo.key
	local eventState = self:GetStore():GetEventState()
	local seq = nil
	local function removeTimerRecord()
		if seq ~= nil then
			self:RemoveTimerRecord(seq)
		end
	end
	local function isOwnerAlive()
		return key ~= nil and self:GetStore():GetObjectState().objects[key] ~= nil
	end
	local function callTimerCallback(callback, ...)
		if type(callback) ~= "function" then
			return
		end
		local ok, err = pcall(callback, ...)
		if not ok then
			print("[FGUI] timer callback error:", key, err)
		end
	end

	local wrappedTick = nil
	if type(tickFunc) == "function" then
		wrappedTick = function(timerInfo, tick)
			if not isOwnerAlive() then
				removeTimerRecord()
				return
			end
			callTimerCallback(tickFunc, timerInfo, tick)
		end
	end
	local wrappedFinish = function(timerInfo)
		if isOwnerAlive() then
			callTimerCallback(finishFunc, timerInfo)
		end
		removeTimerRecord()
	end

	seq = threadpool:timer(duration, interval, wrappedTick, wrappedFinish)
	if seq == nil then
		return nil
	end

	eventState.timers[seq] = {
		seq = seq,
		key = key,
		handle = objectInfo.handle,
	}
	local timerList = eventState.timersByKey[key]
	if timerList == nil then
		timerList = {}
		eventState.timersByKey[key] = timerList
	end
	timerList[seq] = true
	return seq
end

function FairyGuiLifecycle:Delay(keyOrHandle, timeout, func)
	local self = self.owner
	return self:AddTimer(keyOrHandle, timeout, timeout, nil, func)
end

function FairyGuiLifecycle:RemoveTimerRecord(timerId)
	local self = self.owner
	local eventState = self:GetStore():GetEventState()
	local timerInfo = eventState.timers[timerId]
	if timerInfo == nil then
		return false
	end

	eventState.timers[timerId] = nil
	local timerList = eventState.timersByKey[timerInfo.key]
	if timerList ~= nil then
		timerList[timerId] = nil
		if tableCount(timerList) <= 0 then
			eventState.timersByKey[timerInfo.key] = nil
		end
	end
	return true
end

function FairyGuiLifecycle:CancelTimer(timerId)
	local self = self.owner
	if timerId == nil then
		return false
	end

	local removed = self:RemoveTimerRecord(timerId)
	if threadpool ~= nil and threadpool.cancel_timer ~= nil then
		return threadpool:cancel_timer(timerId) or removed
	end
	return removed
end

function FairyGuiLifecycle:ClearTimersForKey(key)
	local self = self.owner
	if key == nil then
		return 0
	end

	local timerList = self:GetStore():GetEventState().timersByKey[key]
	if timerList == nil then
		return 0
	end

	local timerIds = {}
	for timerId, _ in pairs(timerList) do
		table.insert(timerIds, timerId)
	end
	local removedCount = 0
	for _, timerId in ipairs(timerIds) do
		if self:CancelTimer(timerId) then
			removedCount = removedCount + 1
		end
	end
	return removedCount
end

function FairyGuiLifecycle:GetTimerCountForKey(key)
	local self = self.owner
	local timerList = self:GetStore():GetEventState().timersByKey[key]
	return tableCount(timerList)
end

function FairyGuiLifecycle:AddOwnedHandle(objectInfo, handle)
	if objectInfo == nil or handle == nil or handle <= 0 then
		return handle
	end
	if objectInfo.ownedHandles == nil then
		objectInfo.ownedHandles = {}
	end
	table.insert(objectInfo.ownedHandles, handle)
	return handle
end

function FairyGuiLifecycle:AddObjectHandleToParent(childHandle, parentHandle)
	if childHandle == nil or childHandle <= 0 or parentHandle == nil or parentHandle <= 0 then
		return false
	end
	if NativeApi == nil or NativeApi.addFairyGuiObjectToParent == nil then
		return false
	end
	return NativeApi:addFairyGuiObjectToParent(childHandle, parentHandle)
end

return FairyGuiLifecycle
