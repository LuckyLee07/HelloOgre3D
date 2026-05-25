#include "ui/fairygui/FairyGuiSystemInternal.h"

int FairyGuiSystemImpl::AddObjectHandleEventListener(int objectHandle, const std::string& childPath, int eventType, int callbackId)
{
	fairygui::GObject* target = FindEventTarget(objectHandle, childPath);
	if (target == nullptr || callbackId <= 0)
		return 0;

	const int bindingId = m_nextListenerBindingId++;
	ListenerBinding binding;
	binding.rootHandle = objectHandle;
	binding.callbackId = callbackId;
	binding.eventType = eventType;
	binding.target = target;
	binding.targetObject = target;
	m_listenerBindings[bindingId] = binding;

	target->addEventListener(eventType, [this, callbackId, objectHandle, eventType, bindingId](fairygui::EventContext* context) {
		DispatchObjectHandleEvent(callbackId, objectHandle, eventType, bindingId, context);
	}, fairygui::EventTag(bindingId));
	return bindingId;
}

int FairyGuiSystemImpl::AddObjectHandleClickListener(int objectHandle, const std::string& childPath, int callbackId)
{
	return AddObjectHandleEventListener(objectHandle, childPath, fairygui::UIEventType::Click, callbackId);
}

int FairyGuiSystemImpl::AddObjectHandleControllerChangedListener(int objectHandle, const std::string& controllerName, int callbackId)
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	if (controller == nullptr || callbackId <= 0)
		return 0;

	const int bindingId = m_nextListenerBindingId++;
	ListenerBinding binding;
	binding.rootHandle = objectHandle;
	binding.callbackId = callbackId;
	binding.eventType = fairygui::UIEventType::Changed;
	binding.target = controller;
	binding.targetObject = FindObjectHandle(objectHandle);
	binding.controllerName = controllerName;
	m_listenerBindings[bindingId] = binding;

	controller->addEventListener(fairygui::UIEventType::Changed, [this, callbackId, objectHandle, bindingId](fairygui::EventContext* context) {
		DispatchObjectHandleEvent(callbackId, objectHandle, fairygui::UIEventType::Changed, bindingId, context);
	}, fairygui::EventTag(bindingId));
	return bindingId;
}

bool FairyGuiSystemImpl::RemoveObjectHandleListener(int bindingId)
{
	std::map<int, ListenerBinding>::iterator it = m_listenerBindings.find(bindingId);
	if (it == m_listenerBindings.end())
		return false;

	if (it->second.target != nullptr)
		it->second.target->removeEventListener(it->second.eventType, fairygui::EventTag(bindingId));
	m_listenerBindings.erase(it);
	return true;
}


fairygui::GObject* FairyGuiSystemImpl::FindEventTarget(int objectHandle, const std::string& childPath) const
{
	fairygui::GObject* root = FindObjectHandle(objectHandle);
	if (root == nullptr || childPath.empty())
		return root;

	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(root);
	if (component == nullptr)
		return nullptr;

	return component->getChildByPath(childPath);
}


void FairyGuiSystemImpl::RemoveObjectHandleListeners(int objectHandle)
{
	std::vector<int> bindingIds;
	for (std::map<int, ListenerBinding>::const_iterator it = m_listenerBindings.begin(); it != m_listenerBindings.end(); ++it)
	{
		if (it->second.rootHandle == objectHandle)
			bindingIds.push_back(it->first);
	}

	for (std::vector<int>::const_iterator it = bindingIds.begin(); it != bindingIds.end(); ++it)
		RemoveObjectHandleListener(*it);
}

void FairyGuiSystemImpl::DispatchObjectHandleEvent(int callbackId, int objectHandle, int eventType, int bindingId, fairygui::EventContext* context)
{
	H3D_PROFILE_SCOPE("FairyGuiSystemImpl::DispatchEvent");
	ScriptLuaVM* luaVM = GetScriptLuaVM();
	if (luaVM == nullptr)
		return;

	int senderHandle = 0;
	int itemHandle = 0;
	int itemIndex = -1;
	int x = 0;
	int y = 0;
	int button = -1;
	int touchId = -1;
	int wheelDelta = 0;
	int dragDeltaX = 0;
	int dragDeltaY = 0;

	if (context != nullptr)
	{
		fairygui::GObject* sender = dynamic_cast<fairygui::GObject*>(context->getSender());
		if (sender != nullptr)
			senderHandle = GetOrCreateObjectAlias(objectHandle, sender);

		if (eventType == fairygui::UIEventType::ClickItem || eventType == fairygui::UIEventType::RightClickItem)
		{
			fairygui::GObject* item = static_cast<fairygui::GObject*>(context->getData());
			if (item != nullptr)
			{
				itemHandle = GetOrCreateObjectAlias(objectHandle, item);

				std::map<int, ListenerBinding>::const_iterator bindingIt = m_listenerBindings.find(bindingId);
				fairygui::GList* list = bindingIt != m_listenerBindings.end() ? dynamic_cast<fairygui::GList*>(bindingIt->second.targetObject) : nullptr;
				if (list != nullptr)
				{
					const int childIndex = list->getChildIndex(item);
					if (childIndex >= 0)
						itemIndex = list->childIndexToItemIndex(childIndex);
				}
			}
		}

		fairygui::InputEvent* input = context->getInput();
		if (input != nullptr)
		{
			x = input->getX();
			y = input->getY();
			button = static_cast<int>(input->getButton());
			touchId = input->getTouchId();
			wheelDelta = input->getMouseWheelDelta();
			cocos2d::Touch* touch = input->getTouch();
			if (touch != nullptr)
			{
				const cocos2d::Vec2& location = touch->getLocation();
				const cocos2d::Vec2& previousLocation = touch->getPreviousLocation();
				dragDeltaX = static_cast<int>(location.x - previousLocation.x);
				dragDeltaY = static_cast<int>(location.y - previousLocation.y);
			}
		}
	}

	luaVM->callFunction(
		"FairyGuiManager_DispatchEvent",
		"iiiiiiiiiiiiii",
		callbackId,
		objectHandle,
		eventType,
		bindingId,
		senderHandle,
		itemHandle,
		itemIndex,
		x,
		y,
		button,
		touchId,
		wheelDelta,
		dragDeltaX,
		dragDeltaY);
}

