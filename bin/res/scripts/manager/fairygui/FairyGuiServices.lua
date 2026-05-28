local FairyGuiServiceCore = require("res.scripts.manager.fairygui.FairyGuiServiceCore")
local FairyGuiServiceToast = require("res.scripts.manager.fairygui.FairyGuiServiceToast")
local FairyGuiServiceLoading = require("res.scripts.manager.fairygui.FairyGuiServiceLoading")
local FairyGuiServiceTip = require("res.scripts.manager.fairygui.FairyGuiServiceTip")
local FairyGuiServiceGuideMask = require("res.scripts.manager.fairygui.FairyGuiServiceGuideMask")
local FairyGuiServiceMessageBox = require("res.scripts.manager.fairygui.FairyGuiServiceMessageBox")
local FairyGuiServicePopupMenu = require("res.scripts.manager.fairygui.FairyGuiServicePopupMenu")

local DEFAULT_SERVICE_SKINS = FairyGuiServiceCore.DEFAULT_SERVICE_SKINS

local FairyGuiServices = Class("FairyGuiServices")

local MANAGER_PROXY_METHODS = {
	"AddClick",
	"AddObjectHandleToParent",
	"AddOwnedHandle",
	"ApplyScreenAdapt",
	"AssignLayer",
	"AttachToLayer",
	"ClampLayoutRect",
	"Close",
	"CloseUI",
	"CreateContainer",
	"CreateGuideMaskSegments",
	"CreateLoader",
	"CreateModalMask",
	"CreateText",
	"Delay",
	"GetCurrentScene",
	"GetGuideMaskRects",
	"GetObjectInfo",
	"GetPopupGroup",
	"GetSceneName",
	"GetScreenHeight",
	"GetScreenWidth",
	"GetStore",
	"GetUIGroup",
	"PushStack",
	"RecordPerf",
	"RegisterObject",
	"SetAlpha",
	"SetPosition",
	"SetSize",
	"SetText",
	"SetTouchable",
	"SetVisible",
}

for _, methodName in ipairs(MANAGER_PROXY_METHODS) do
	FairyGuiServices[methodName] = FairyGuiServices[methodName] or function(self, ...)
		local owner = self.owner
		local method = owner ~= nil and owner[methodName] or nil
		if method == nil then
			return nil
		end
		return method(owner, ...)
	end
end
function FairyGuiServices:Init(owner)
	return FairyGuiServiceCore.Init(self, owner)
end

function FairyGuiServices:RegisterServiceSkin(serviceType, skinName, skin)
	return FairyGuiServiceCore.RegisterServiceSkin(self, serviceType, skinName, skin)
end

function FairyGuiServices:GetServiceSkin(serviceType, skinName)
	return FairyGuiServiceCore.GetServiceSkin(self, serviceType, skinName)
end

function FairyGuiServices:ResolveServiceSkin(serviceType, param)
	return FairyGuiServiceCore.ResolveServiceSkin(self, serviceType, param)
end

function FairyGuiServices:OpenServiceContainer(key, param)
	return FairyGuiServiceCore.OpenServiceContainer(self, key, param)
end

function FairyGuiServices:AddServiceText(objectInfo, name, text, x, y, width, height, fontSize, red, green, blue)
	return FairyGuiServiceCore.AddServiceText(self, objectInfo, name, text, x, y, width, height, fontSize, red, green, blue)
end

function FairyGuiServices:AddServiceImage(objectInfo, name, url, x, y, width, height, alpha)
	return FairyGuiServiceCore.AddServiceImage(self, objectInfo, name, url, x, y, width, height, alpha)
end

function FairyGuiServices:AddServiceButton(objectInfo, name, text, x, y, width, height, callback, style)
	return FairyGuiServiceCore.AddServiceButton(self, objectInfo, name, text, x, y, width, height, callback, style)
end

function FairyGuiServices:ApplyServiceLayout(objectInfo)
	return FairyGuiServiceCore.ApplyServiceLayout(self, objectInfo)
end

function FairyGuiServices:GetServiceObject(serviceKey)
	return FairyGuiServiceCore.GetServiceObject(self, serviceKey)
end

function FairyGuiServices:CloseService(serviceKey, reason)
	return FairyGuiServiceCore.CloseService(self, serviceKey, reason)
end

function FairyGuiServices:CloseServices(serviceType, reason)
	return FairyGuiServiceCore.CloseServices(self, serviceType, reason)
end

function FairyGuiServices:HandleServiceClosed(objectInfo, reason)
	return FairyGuiServiceCore.HandleServiceClosed(self, objectInfo, reason)
end

function FairyGuiServices:ClearToastQueue()
	return FairyGuiServiceToast.ClearToastQueue(self)
end

function FairyGuiServices:GetToastQueueCount()
	return FairyGuiServiceToast.GetToastQueueCount(self)
end

function FairyGuiServices:CreateToastRequest(text, duration, param)
	return FairyGuiServiceToast.CreateToastRequest(self, text, duration, param)
end

function FairyGuiServices:OpenToastRequest(request)
	return FairyGuiServiceToast.OpenToastRequest(self, request)
end

function FairyGuiServices:ShowNextToast()
	return FairyGuiServiceToast.ShowNextToast(self)
end

function FairyGuiServices:ShowToast(text, duration, param)
	return FairyGuiServiceToast.ShowToast(self, text, duration, param)
end

function FairyGuiServices:CloseToast(reason)
	return FairyGuiServiceToast.CloseToast(self, reason)
end

function FairyGuiServices:ShowTip(text, x, y, duration, param)
	return FairyGuiServiceTip.ShowTip(self, text, x, y, duration, param)
end

function FairyGuiServices:ShowHoverTip(text, anchorRect, param)
	return FairyGuiServiceTip.ShowHoverTip(self, text, anchorRect, param)
end

function FairyGuiServices:HideTip(reason)
	return FairyGuiServiceTip.HideTip(self, reason)
end

function FairyGuiServices:ShowLoading(text, param)
	return FairyGuiServiceLoading.ShowLoading(self, text, param)
end

function FairyGuiServices:GetLoadingRefCount()
	return FairyGuiServiceLoading.GetLoadingRefCount(self)
end

function FairyGuiServices:HideLoading(paramOrReason)
	return FairyGuiServiceLoading.HideLoading(self, paramOrReason)
end

function FairyGuiServices:ShowGuideMask(param)
	return FairyGuiServiceGuideMask.ShowGuideMask(self, param)
end

function FairyGuiServices:HideGuideMask(reason)
	return FairyGuiServiceGuideMask.HideGuideMask(self, reason)
end

function FairyGuiServices:ShowMessageBox(title, message, buttons, callback, param)
	return FairyGuiServiceMessageBox.ShowMessageBox(self, title, message, buttons, callback, param)
end

function FairyGuiServices:ShowDialog(title, message, buttons, callback, param)
	return FairyGuiServiceMessageBox.ShowDialog(self, title, message, buttons, callback, param)
end

function FairyGuiServices:ShowPopupMenu(items, x, y, callback, param)
	return FairyGuiServicePopupMenu.ShowPopupMenu(self, items, x, y, callback, param)
end

function FairyGuiServices:GetServiceStats()
	return FairyGuiServiceCore.GetServiceStats(self)
end

function FairyGuiServices:DumpServices()
	return FairyGuiServiceCore.DumpServices(self)
end
