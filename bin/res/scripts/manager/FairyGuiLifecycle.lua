local FairyGuiLifecycle = Class("FairyGuiLifecycle")

local function bindOwnerState(owner, state)
	if owner == nil or state == nil then
		return
	end

	owner.objects = state.objects
	owner.objectsByHandle = state.objectsByHandle
	owner.uiRegistry = state.uiRegistry
	owner.uiNameToKey = state.uiNameToKey
	owner.hiddenObjects = state.hiddenObjects
	owner.views = state.views
	owner.viewsByHandle = state.viewsByHandle
	owner.controllers = state.controllers
	owner.controllersByHandle = state.controllersByHandle
	owner.childKeysByParentKey = state.childKeysByParentKey
	owner.parentKeyByChildKey = state.parentKeyByChildKey
end

function FairyGuiLifecycle:Init(owner)
	self.owner = owner

	self.objects = owner ~= nil and owner.objects or {}
	self.objectsByHandle = owner ~= nil and owner.objectsByHandle or {}
	self.uiRegistry = owner ~= nil and owner.uiRegistry or {}
	self.uiNameToKey = owner ~= nil and owner.uiNameToKey or {}
	self.hiddenObjects = owner ~= nil and owner.hiddenObjects or {}
	self.views = owner ~= nil and owner.views or {}
	self.viewsByHandle = owner ~= nil and owner.viewsByHandle or {}
	self.controllers = owner ~= nil and owner.controllers or {}
	self.controllersByHandle = owner ~= nil and owner.controllersByHandle or {}
	self.childKeysByParentKey = owner ~= nil and owner.childKeysByParentKey or {}
	self.parentKeyByChildKey = owner ~= nil and owner.parentKeyByChildKey or {}

	bindOwnerState(owner, self)
end

function FairyGuiLifecycle:BindOwnerState()
	bindOwnerState(self.owner, self)
end

return FairyGuiLifecycle
