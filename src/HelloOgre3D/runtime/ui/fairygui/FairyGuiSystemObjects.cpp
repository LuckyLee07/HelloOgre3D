#include "ui/fairygui/FairyGuiSystemInternal.h"

bool FairyGuiSystemImpl::LoadPackage(const std::string& packagePath)
{
	H3D_PROFILE_SCOPE_NAMED(loadPackageZone, "FairyGuiSystemImpl::LoadPackage");
	if (!packagePath.empty())
	{
		H3D_PROFILE_TEXT(loadPackageZone, packagePath.c_str(), packagePath.size());
	}
	if (!m_initialized || packagePath.empty())
		return false;

	return AddPackage(packagePath) != nullptr;
}

std::string FairyGuiSystemImpl::LoadPackageAndGetName(const std::string& packagePath)
{
	H3D_PROFILE_SCOPE_NAMED(loadPackageZone, "FairyGuiSystemImpl::LoadPackageAndGetName");
	if (!packagePath.empty())
	{
		H3D_PROFILE_TEXT(loadPackageZone, packagePath.c_str(), packagePath.size());
	}
	if (!m_initialized || packagePath.empty())
		return std::string();

	fairygui::UIPackage* package = AddPackage(packagePath);
	return package != nullptr ? package->getName() : std::string();
}

bool FairyGuiSystemImpl::RemovePackage(const std::string& packageName)
{
	H3D_PROFILE_SCOPE_NAMED(removePackageZone, "FairyGuiSystemImpl::RemovePackage");
	if (!packageName.empty())
	{
		H3D_PROFILE_TEXT(removePackageZone, packageName.c_str(), packageName.size());
	}
	if (!m_initialized || packageName.empty())
		return false;

	fairygui::UIPackage::removePackage(packageName);
	return true;
}

fairygui::GObject* FairyGuiSystemImpl::CreateObject(const std::string& packageName, const std::string& objectName)
{
	H3D_PROFILE_SCOPE_NAMED(createObjectZone, "FairyGuiSystemImpl::CreateObject");
	if (!objectName.empty())
	{
		H3D_PROFILE_TEXT(createObjectZone, objectName.c_str(), objectName.size());
	}
	if (!m_initialized || packageName.empty() || objectName.empty())
		return nullptr;

	return fairygui::UIPackage::createObject(packageName, objectName);
}

bool FairyGuiSystemImpl::AddToRoot(fairygui::GObject* object)
{
	if (!m_initialized || m_pRoot == nullptr || object == nullptr)
		return false;

	m_pRoot->addChild(object);
	return true;
}

int FairyGuiSystemImpl::CreateObjectHandle(const std::string& packageName, const std::string& objectName)
{
	fairygui::GObject* object = CreateObject(packageName, objectName);
	if (object == nullptr)
		return 0;

	const int objectHandle = m_nextObjectHandle++;
	object->retain();

	ObjectHandleInfo handleInfo;
	handleInfo.object = object;
	handleInfo.ownerHandle = 0;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystemImpl::CreateContainerHandle(const std::string& name)
{
	if (!m_initialized)
		return 0;

	fairygui::GComponent* container = fairygui::GComponent::create();
	if (container == nullptr)
		return 0;

	container->name = name;
	container->setSize(static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight));
	container->setOpaque(false);
	container->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = container;
	handleInfo.ownerHandle = 0;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystemImpl::CreateChildContainerHandle(int ownerHandle, const std::string& name)
{
	if (!m_initialized)
		return 0;

	fairygui::GComponent* container = fairygui::GComponent::create();
	if (container == nullptr)
		return 0;

	container->name = name;
	container->setOpaque(false);
	container->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = container;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystemImpl::CreateLoaderHandle(int ownerHandle, const std::string& name, const std::string& url)
{
	if (!m_initialized)
		return 0;

	fairygui::GLoader* loader = fairygui::GLoader::create();
	if (loader == nullptr)
		return 0;

	loader->name = name;
	loader->setURL(url);
	loader->setFill(fairygui::LoaderFillType::SCALE_FREE);
	loader->setTouchable(false);
	loader->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = loader;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystemImpl::CreateTextHandle(int ownerHandle, const std::string& name, const std::string& text, float fontSize, float red, float green, float blue)
{
	if (!m_initialized)
		return 0;

	fairygui::GBasicTextField* label = fairygui::GBasicTextField::create();
	if (label == nullptr)
		return 0;

	label->name = name;
	label->setText(text);
	label->setFontSize(fontSize);
	label->setColor(cocos2d::Color3B(
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, red))),
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, green))),
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, blue)))));
	label->setTouchable(false);
	label->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = label;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystemImpl::CreateTextInputHandle(int ownerHandle, const std::string& name, const std::string& text, float fontSize, float red, float green, float blue)
{
	if (!m_initialized)
		return 0;

	fairygui::GTextInput* input = fairygui::GTextInput::create();
	if (input == nullptr)
		return 0;

	input->name = name;
	input->setText(text);
	input->setFontSize(fontSize);
	input->setColor(cocos2d::Color3B(
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, red))),
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, green))),
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, blue)))));
	input->setTouchable(true);
	input->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = input;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystemImpl::CreateGraphRectHandle(int ownerHandle, const std::string& name, float width, float height, float red, float green, float blue, float alpha)
{
	if (!m_initialized || width <= 0.0f || height <= 0.0f)
		return 0;

	fairygui::GGraph* graph = fairygui::GGraph::create();
	if (graph == nullptr)
		return 0;

	graph->name = name;
	graph->drawRect(width, height, 0, cocos2d::Color4F(red, green, blue, alpha), cocos2d::Color4F(red, green, blue, alpha));
	graph->setTouchable(false);
	graph->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = graph;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystemImpl::CreateGraphRegularPolygonHandle(int ownerHandle, const std::string& name, float width, float height, int sides, float red, float green, float blue, float alpha)
{
	if (!m_initialized || width <= 0.0f || height <= 0.0f || sides < 3)
		return 0;

	fairygui::GGraph* graph = fairygui::GGraph::create();
	if (graph == nullptr)
		return 0;

	graph->name = name;
	graph->setSize(width, height);
	graph->drawRegularPolygon(0, cocos2d::Color4F(red, green, blue, alpha), cocos2d::Color4F(red, green, blue, alpha), sides, -90.0f);
	graph->setTouchable(false);
	graph->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = graph;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystemImpl::CreateModalMaskHandle(float red, float green, float blue, float alpha)
{
	if (!m_initialized || m_screenWidth == 0 || m_screenHeight == 0)
		return 0;

	fairygui::GGraph* graph = fairygui::GGraph::create();
	if (graph == nullptr)
		return 0;

	graph->drawRect(
		static_cast<float>(m_screenWidth),
		static_cast<float>(m_screenHeight),
		0,
		cocos2d::Color4F(red, green, blue, alpha),
		cocos2d::Color4F(red, green, blue, alpha));
	graph->setTouchable(true);
	graph->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = graph;
	handleInfo.ownerHandle = 0;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystemImpl::GetObjectHandleChild(int objectHandle, const std::string& childPath)
{
	if (childPath.empty())
		return objectHandle;

	fairygui::GObject* child = FindEventTarget(objectHandle, childPath);
	if (child == nullptr)
		return 0;

	return GetOrCreateObjectAlias(GetObjectHandleOwner(objectHandle), child);
}

int FairyGuiSystemImpl::GetObjectHandleListItem(int objectHandle, int itemIndex)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr || itemIndex < 0 || itemIndex >= list->getNumItems())
		return 0;

	const int childIndex = list->itemIndexToChildIndex(itemIndex);
	if (childIndex < 0 || childIndex >= list->numChildren())
		return 0;

	fairygui::GObject* item = list->getChildAt(childIndex);
	return GetOrCreateObjectAlias(GetObjectHandleOwner(objectHandle), item);
}

int FairyGuiSystemImpl::GetObjectHandleListItemCount(int objectHandle)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr)
		return 0;

	return list->getNumItems();
}

bool FairyGuiSystemImpl::AddObjectHandleToRoot(int objectHandle)
{
	return AddToRoot(FindObjectHandle(objectHandle));
}

bool FairyGuiSystemImpl::AddObjectHandleToParent(int objectHandle, int parentHandle)
{
	if (parentHandle <= 0)
		return AddObjectHandleToRoot(objectHandle);

	fairygui::GObject* object = FindObjectHandle(objectHandle);
	fairygui::GComponent* parent = dynamic_cast<fairygui::GComponent*>(FindObjectHandle(parentHandle));
	if (object == nullptr || parent == nullptr)
		return false;

	parent->addChild(object);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandlePosition(int objectHandle, float x, float y)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setPosition(x, y);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleSize(int objectHandle, float width, float height)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setSize(width, height);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleVisible(int objectHandle, bool visible)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setVisible(visible);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleAlpha(int objectHandle, float alpha)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setAlpha(alpha);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleTouchable(int objectHandle, bool touchable)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setTouchable(touchable);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleMask(int objectHandle, int maskHandle, bool inverted)
{
	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(FindObjectHandle(objectHandle));
	fairygui::GObject* mask = FindObjectHandle(maskHandle);
	if (component == nullptr || mask == nullptr)
		return false;

	component->setMask(mask->displayObject(), inverted);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleSortingOrder(int objectHandle, int sortingOrder)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setSortingOrder(sortingOrder);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleText(int objectHandle, const std::string& text)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setText(text);
	if (dynamic_cast<fairygui::GTextInput*>(object) != nullptr)
		m_textInputCarets[objectHandle] = GetUtf8CodepointCount(text);
	return true;
}

std::string FairyGuiSystemImpl::GetObjectHandleText(int objectHandle) const
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	return object != nullptr ? object->getText() : std::string();
}

bool FairyGuiSystemImpl::SetObjectHandleIcon(int objectHandle, const std::string& icon)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setIcon(icon);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleLoaderUrl(int objectHandle, const std::string& url)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	fairygui::GLoader* loader = dynamic_cast<fairygui::GLoader*>(object);
	if (loader == nullptr)
		return false;

	loader->setURL(url);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleControllerIndex(int objectHandle, const std::string& controllerName, int selectedIndex)
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	if (controller == nullptr)
		return false;

	controller->setSelectedIndex(selectedIndex);
	return true;
}

int FairyGuiSystemImpl::GetObjectHandleControllerIndex(int objectHandle, const std::string& controllerName) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	return controller != nullptr ? controller->getSelectedIndex() : -1;
}

bool FairyGuiSystemImpl::SetObjectHandleControllerPage(int objectHandle, const std::string& controllerName, const std::string& pageName)
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	if (controller == nullptr)
		return false;

	controller->setSelectedPage(pageName);
	return true;
}

std::string FairyGuiSystemImpl::GetObjectHandleControllerPage(int objectHandle, const std::string& controllerName) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	return controller != nullptr ? controller->getSelectedPage() : std::string();
}

std::string FairyGuiSystemImpl::GetObjectHandleControllerPageId(int objectHandle, const std::string& controllerName) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	return controller != nullptr ? controller->getSelectedPageId() : std::string();
}

int FairyGuiSystemImpl::GetObjectHandleControllerPageCount(int objectHandle, const std::string& controllerName) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	return controller != nullptr ? controller->getPageCount() : 0;
}

std::string FairyGuiSystemImpl::GetObjectHandleControllerPageNameAt(int objectHandle, const std::string& controllerName, int pageIndex) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	if (controller == nullptr || pageIndex < 0 || pageIndex >= controller->getPageCount())
		return std::string();

	const std::string pageId = controller->getPageId(pageIndex);
	return controller->getPageNameById(pageId);
}

std::string FairyGuiSystemImpl::GetObjectHandleControllerPageIdAt(int objectHandle, const std::string& controllerName, int pageIndex) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	if (controller == nullptr || pageIndex < 0 || pageIndex >= controller->getPageCount())
		return std::string();

	return controller->getPageId(pageIndex);
}

bool FairyGuiSystemImpl::SetObjectHandleValue(int objectHandle, float value)
{
	return SetRangeObjectValue(FindObjectHandle(objectHandle), value);
}

float FairyGuiSystemImpl::GetObjectHandleValue(int objectHandle) const
{
	return GetRangeObjectValue(FindObjectHandle(objectHandle));
}

bool FairyGuiSystemImpl::SetObjectHandleMin(int objectHandle, float minValue)
{
	return SetRangeObjectMin(FindObjectHandle(objectHandle), minValue);
}

float FairyGuiSystemImpl::GetObjectHandleMin(int objectHandle) const
{
	return GetRangeObjectMin(FindObjectHandle(objectHandle));
}

bool FairyGuiSystemImpl::SetObjectHandleMax(int objectHandle, float maxValue)
{
	return SetRangeObjectMax(FindObjectHandle(objectHandle), maxValue);
}

float FairyGuiSystemImpl::GetObjectHandleMax(int objectHandle) const
{
	return GetRangeObjectMax(FindObjectHandle(objectHandle));
}

bool FairyGuiSystemImpl::SetObjectHandleComboBoxSelectedIndex(int objectHandle, int selectedIndex)
{
	fairygui::GComboBox* comboBox = dynamic_cast<fairygui::GComboBox*>(FindObjectHandle(objectHandle));
	if (comboBox == nullptr)
		return false;

	comboBox->setSelectedIndex(selectedIndex);
	return true;
}

int FairyGuiSystemImpl::GetObjectHandleComboBoxSelectedIndex(int objectHandle) const
{
	fairygui::GComboBox* comboBox = dynamic_cast<fairygui::GComboBox*>(FindObjectHandle(objectHandle));
	return comboBox != nullptr ? comboBox->getSelectedIndex() : -1;
}

bool FairyGuiSystemImpl::SetObjectHandleComboBoxValue(int objectHandle, const std::string& value)
{
	fairygui::GComboBox* comboBox = dynamic_cast<fairygui::GComboBox*>(FindObjectHandle(objectHandle));
	if (comboBox == nullptr)
		return false;

	comboBox->setValue(value);
	return true;
}

std::string FairyGuiSystemImpl::GetObjectHandleComboBoxValue(int objectHandle) const
{
	fairygui::GComboBox* comboBox = dynamic_cast<fairygui::GComboBox*>(FindObjectHandle(objectHandle));
	return comboBox != nullptr ? comboBox->getValue() : std::string();
}

bool FairyGuiSystemImpl::PlayObjectHandleTransition(int objectHandle, const std::string& transitionName, int times, float delay, int callbackId)
{
	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(FindObjectHandle(objectHandle));
	fairygui::Transition* transition = FindTransition(component, transitionName);
	if (transition == nullptr)
		return false;

	if (callbackId > 0)
	{
		const std::string callbackTransitionName = transitionName.empty() ? transition->name : transitionName;
		transition->play(times, delay, [callbackId, objectHandle, callbackTransitionName]() {
			ScriptLuaVM* luaVM = GetScriptLuaVM();
			if (luaVM == nullptr)
				return;

			luaVM->callFunction(
				"FairyGuiManager_DispatchTransition",
				"iis",
				callbackId,
				objectHandle,
				const_cast<char*>(callbackTransitionName.c_str()));
		});
		return true;
	}

	transition->play(times, delay);
	return true;
}

bool FairyGuiSystemImpl::StopObjectHandleTransition(int objectHandle, const std::string& transitionName, bool setToComplete, bool processCallback)
{
	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(FindObjectHandle(objectHandle));
	fairygui::Transition* transition = FindTransition(component, transitionName);
	if (transition == nullptr)
		return false;

	transition->stop(setToComplete, processCallback);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleFocus(int objectHandle)
{
	fairygui::GTextInput* input = FindTextInput(objectHandle);
	return FocusTextInput(input);
}

bool FairyGuiSystemImpl::ClearObjectHandleFocus()
{
	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input != nullptr)
		input->dispatchEvent(fairygui::UIEventType::Exit);
	CancelNativeImeComposition();
	m_focusedObjectHandle = 0;
	EndImeComposition(true);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleListItemCount(int objectHandle, int itemCount)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr || itemCount < 0)
		return false;

	list->setNumItems(itemCount);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleListSelectedIndex(int objectHandle, int selectedIndex)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr)
		return false;

	list->setSelectedIndex(selectedIndex);
	return true;
}

int FairyGuiSystemImpl::GetObjectHandleListSelectedIndex(int objectHandle)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr)
		return -1;

	return list->getSelectedIndex();
}

bool FairyGuiSystemImpl::ScrollObjectHandleListToView(int objectHandle, int itemIndex)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr || itemIndex < 0 || itemIndex >= list->getNumItems())
		return false;

	list->scrollToView(itemIndex);
	return true;
}

bool FairyGuiSystemImpl::SetObjectHandleListVirtual(int objectHandle, bool loop)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr)
		return false;
	if (list->getScrollPane() == nullptr || list->getDefaultItem().empty())
		return false;

	if (list->itemRenderer == nullptr)
	{
		list->itemRenderer = [this, objectHandle](int itemIndex, fairygui::GObject* item) {
			ScriptLuaVM* luaVM = GetScriptLuaVM();
			if (luaVM == nullptr || item == nullptr)
				return;

			const int itemHandle = GetOrCreateObjectAlias(GetObjectHandleOwner(objectHandle), item);
			luaVM->callFunction(
				"FairyGuiManager_RenderVirtualListItem",
				"iii",
				objectHandle,
				itemIndex + 1,
				itemHandle);
		};
	}
	if (!list->isVirtual())
	{
		if (loop)
			list->setVirtualAndLoop();
		else
			list->setVirtual();
	}
	return true;
}

bool FairyGuiSystemImpl::RefreshObjectHandleList(int objectHandle)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr)
		return false;

	if (list->isVirtual())
		list->refreshVirtualList();
	return true;
}

bool FairyGuiSystemImpl::CenterObjectHandle(int objectHandle, bool restraint)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->center(restraint);
	return true;
}

bool FairyGuiSystemImpl::RemoveObjectHandle(int objectHandle)
{
	std::map<int, ObjectHandleInfo>::iterator it = m_objectHandles.find(objectHandle);
	if (it == m_objectHandles.end() || it->second.object == nullptr)
		return false;

	if (objectHandle == m_focusedObjectHandle || GetObjectHandleOwner(m_focusedObjectHandle) == objectHandle)
		ClearObjectHandleFocus();
	RemoveObjectHandleAliases(objectHandle);
	RemoveObjectHandleListeners(objectHandle);
	m_textInputCarets.erase(objectHandle);
	if (it->second.retained)
	{
		it->second.object->removeFromParent();
		it->second.object->release();
	}
	m_objectHandles.erase(it);
	return true;
}

void FairyGuiSystemImpl::ClearObjectHandles()
{
	m_listenerBindings.clear();

	std::vector<int> rootHandles;
	for (std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->second.ownerHandle == 0)
			rootHandles.push_back(it->first);
	}

	for (std::vector<int>::const_iterator it = rootHandles.begin(); it != rootHandles.end(); ++it)
		RemoveObjectHandle(*it);

	while (!m_objectHandles.empty())
		RemoveObjectHandle(m_objectHandles.begin()->first);
	m_textInputCarets.clear();
	m_focusedObjectHandle = 0;
}

bool FairyGuiSystemImpl::CreateSmokeTestImage(const std::string& imagePath)
{
	if (!m_initialized || m_pRoot == nullptr || imagePath.empty())
		return false;

	fairygui::GLoader* loader = fairygui::GLoader::create();
	if (loader == nullptr)
		return false;

	loader->setPosition(16.0f, 16.0f);
	loader->setSize(256.0f, 64.0f);
	loader->setFill(fairygui::LoaderFillType::SCALE);
	loader->setURL(imagePath);
	m_pRoot->addChild(loader);
	return true;
}

fairygui::GObject* FairyGuiSystemImpl::FindObjectHandle(int objectHandle) const
{
	std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.find(objectHandle);
	return it != m_objectHandles.end() ? it->second.object : nullptr;
}


fairygui::GController* FairyGuiSystemImpl::FindController(int objectHandle, const std::string& controllerName) const
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(object);
	if (component == nullptr)
		return nullptr;

	if (!controllerName.empty())
		return component->getController(controllerName);
	if (!component->getControllers().empty())
		return component->getControllerAt(0);
	return nullptr;
}

fairygui::GTextInput* FairyGuiSystemImpl::FindTextInput(int objectHandle) const
{
	return dynamic_cast<fairygui::GTextInput*>(FindObjectHandle(objectHandle));
}

fairygui::GTextInput* FairyGuiSystemImpl::FindTextInputTarget(fairygui::GObject* target) const
{
	while (target != nullptr && target != m_pRoot)
	{
		fairygui::GTextInput* input = dynamic_cast<fairygui::GTextInput*>(target);
		if (input != nullptr)
			return input;
		target = target->findParent();
	}
	return nullptr;
}

int FairyGuiSystemImpl::FindOwnerHandleForObject(fairygui::GObject* object) const
{
	if (object == nullptr)
		return 0;

	for (std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->second.object == object)
			return GetObjectHandleOwner(it->first);
	}

	for (std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->second.ownerHandle != 0)
			continue;
		fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(it->second.object);
		if (component != nullptr && component->isAncestorOf(object))
			return it->first;
	}
	return 0;
}

int FairyGuiSystemImpl::GetOrCreateObjectAlias(int ownerHandle, fairygui::GObject* object)
{
	if (object == nullptr)
		return 0;

	ownerHandle = GetObjectHandleOwner(ownerHandle);
	for (std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->second.object == object && (it->first == ownerHandle || it->second.ownerHandle == ownerHandle))
			return it->first;
	}

	const int aliasHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = object;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = false;
	m_objectHandles[aliasHandle] = handleInfo;
	return aliasHandle;
}

int FairyGuiSystemImpl::GetObjectHandleOwner(int objectHandle) const
{
	std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.find(objectHandle);
	if (it == m_objectHandles.end())
		return objectHandle;
	return it->second.ownerHandle != 0 ? it->second.ownerHandle : objectHandle;
}

void FairyGuiSystemImpl::RemoveObjectHandleAliases(int objectHandle)
{
	std::vector<int> aliasHandles;
	for (std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->first != objectHandle && it->second.ownerHandle == objectHandle)
			aliasHandles.push_back(it->first);
	}

	for (std::vector<int>::const_iterator it = aliasHandles.begin(); it != aliasHandles.end(); ++it)
		RemoveObjectHandle(*it);
}


bool FairyGuiSystemImpl::CreateConfiguredPackageObject()
{
	if (!m_initialized || m_pRoot == nullptr)
		return false;

	const std::string packagePath = GetEnvironmentString("HELLO_FGUI_PACKAGE_PATH");
	const std::string objectName = GetEnvironmentString("HELLO_FGUI_OBJECT_NAME");
	if (packagePath.empty() || objectName.empty())
		return false;

	fairygui::UIPackage* package = AddPackage(packagePath);
	if (package == nullptr)
		return false;

	std::string packageName = GetEnvironmentString("HELLO_FGUI_PACKAGE_NAME");
	if (packageName.empty())
		packageName = package->getName();

	fairygui::GObject* object = fairygui::UIPackage::createObject(packageName, objectName);
	if (object == nullptr)
		return false;

	object->setPosition(
		GetEnvironmentFloat("HELLO_FGUI_OBJECT_X", 16.0f),
		GetEnvironmentFloat("HELLO_FGUI_OBJECT_Y", 16.0f));
	m_pRoot->addChild(object);
	return true;
}


