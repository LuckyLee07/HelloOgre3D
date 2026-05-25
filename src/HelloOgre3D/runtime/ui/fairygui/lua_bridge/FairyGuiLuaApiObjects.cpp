#include "FairyGuiLuaApiInternal.h"

int FairyGuiLuaApi::GetChild(int objectHandle, const char* childPath) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleChild(objectHandle, SafeString(childPath)) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetListItem(int objectHandle, int itemIndex) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleListItem(objectHandle, itemIndex) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetListItemCount(int objectHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleListItemCount(objectHandle) : 0;
#else
	return 0;
#endif
}

bool FairyGuiLuaApi::AddObjectToRoot(int objectHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->AddObjectHandleToRoot(objectHandle);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::AddObjectToParent(int objectHandle, int parentHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->AddObjectHandleToParent(objectHandle, parentHandle);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectPosition(int objectHandle, float x, float y) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandlePosition(objectHandle, x, y);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectSize(int objectHandle, float width, float height) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleSize(objectHandle, width, height);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectVisible(int objectHandle, bool visible) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleVisible(objectHandle, visible);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectAlpha(int objectHandle, float alpha) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleAlpha(objectHandle, alpha);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectTouchable(int objectHandle, bool touchable) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleTouchable(objectHandle, touchable);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectMask(int objectHandle, int maskHandle, bool inverted) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleMask(objectHandle, maskHandle, inverted);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectSortingOrder(int objectHandle, int sortingOrder) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleSortingOrder(objectHandle, sortingOrder);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectText(int objectHandle, const char* text) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleText(objectHandle, SafeString(text));
#else
	return false;
#endif
}

const char* FairyGuiLuaApi::GetObjectText(int objectHandle)
{
	m_lastObjectText.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastObjectText = system->GetObjectHandleText(objectHandle);
#endif
	return m_lastObjectText.c_str();
}

bool FairyGuiLuaApi::SetObjectIcon(int objectHandle, const char* icon) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleIcon(objectHandle, SafeString(icon));
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectLoaderUrl(int objectHandle, const char* url) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleLoaderUrl(objectHandle, SafeString(url));
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectControllerIndex(int objectHandle, const char* controllerName, int selectedIndex) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleControllerIndex(objectHandle, SafeString(controllerName), selectedIndex);
#else
	return false;
#endif
}

int FairyGuiLuaApi::GetObjectControllerIndex(int objectHandle, const char* controllerName) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleControllerIndex(objectHandle, SafeString(controllerName)) : -1;
#else
	return -1;
#endif
}

bool FairyGuiLuaApi::SetObjectControllerPage(int objectHandle, const char* controllerName, const char* pageName) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleControllerPage(objectHandle, SafeString(controllerName), SafeString(pageName));
#else
	return false;
#endif
}

const char* FairyGuiLuaApi::GetObjectControllerPage(int objectHandle, const char* controllerName)
{
	m_lastControllerString.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastControllerString = system->GetObjectHandleControllerPage(objectHandle, SafeString(controllerName));
#endif
	return m_lastControllerString.c_str();
}

const char* FairyGuiLuaApi::GetObjectControllerPageId(int objectHandle, const char* controllerName)
{
	m_lastControllerString.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastControllerString = system->GetObjectHandleControllerPageId(objectHandle, SafeString(controllerName));
#endif
	return m_lastControllerString.c_str();
}

int FairyGuiLuaApi::GetObjectControllerPageCount(int objectHandle, const char* controllerName) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleControllerPageCount(objectHandle, SafeString(controllerName)) : 0;
#else
	return 0;
#endif
}

const char* FairyGuiLuaApi::GetObjectControllerPageNameAt(int objectHandle, const char* controllerName, int pageIndex)
{
	m_lastControllerString.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastControllerString = system->GetObjectHandleControllerPageNameAt(objectHandle, SafeString(controllerName), pageIndex);
#endif
	return m_lastControllerString.c_str();
}

const char* FairyGuiLuaApi::GetObjectControllerPageIdAt(int objectHandle, const char* controllerName, int pageIndex)
{
	m_lastControllerString.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastControllerString = system->GetObjectHandleControllerPageIdAt(objectHandle, SafeString(controllerName), pageIndex);
#endif
	return m_lastControllerString.c_str();
}

bool FairyGuiLuaApi::SetObjectValue(int objectHandle, float value) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleValue(objectHandle, value);
#else
	return false;
#endif
}

float FairyGuiLuaApi::GetObjectValue(int objectHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleValue(objectHandle) : 0;
#else
	return 0;
#endif
}

bool FairyGuiLuaApi::SetObjectMin(int objectHandle, float minValue) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleMin(objectHandle, minValue);
#else
	return false;
#endif
}

float FairyGuiLuaApi::GetObjectMin(int objectHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleMin(objectHandle) : 0;
#else
	return 0;
#endif
}

bool FairyGuiLuaApi::SetObjectMax(int objectHandle, float maxValue) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleMax(objectHandle, maxValue);
#else
	return false;
#endif
}

float FairyGuiLuaApi::GetObjectMax(int objectHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleMax(objectHandle) : 0;
#else
	return 0;
#endif
}

bool FairyGuiLuaApi::SetComboBoxSelectedIndex(int objectHandle, int selectedIndex) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleComboBoxSelectedIndex(objectHandle, selectedIndex);
#else
	return false;
#endif
}

int FairyGuiLuaApi::GetComboBoxSelectedIndex(int objectHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleComboBoxSelectedIndex(objectHandle) : -1;
#else
	return -1;
#endif
}

bool FairyGuiLuaApi::SetComboBoxValue(int objectHandle, const char* value) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleComboBoxValue(objectHandle, SafeString(value));
#else
	return false;
#endif
}

const char* FairyGuiLuaApi::GetComboBoxValue(int objectHandle)
{
	m_lastObjectValue.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastObjectValue = system->GetObjectHandleComboBoxValue(objectHandle);
#endif
	return m_lastObjectValue.c_str();
}

bool FairyGuiLuaApi::PlayTransition(int objectHandle, const char* transitionName, int times, float delay, int callbackId) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->PlayObjectHandleTransition(objectHandle, SafeString(transitionName), times, delay, callbackId);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::StopTransition(int objectHandle, const char* transitionName, bool setToComplete, bool processCallback) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->StopObjectHandleTransition(objectHandle, SafeString(transitionName), setToComplete, processCallback);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::FocusObject(int objectHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleFocus(objectHandle);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::ClearFocus() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->ClearObjectHandleFocus();
#else
	return false;
#endif
}

int FairyGuiLuaApi::GetFocusedObject() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetFocusedObjectHandle() : 0;
#else
	return 0;
#endif
}

bool FairyGuiLuaApi::SetListItemCount(int objectHandle, int itemCount) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleListItemCount(objectHandle, itemCount);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetListSelectedIndex(int objectHandle, int selectedIndex) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleListSelectedIndex(objectHandle, selectedIndex);
#else
	return false;
#endif
}

int FairyGuiLuaApi::GetListSelectedIndex(int objectHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleListSelectedIndex(objectHandle) : -1;
#else
	return -1;
#endif
}

bool FairyGuiLuaApi::SetListVirtual(int objectHandle, bool loop) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleListVirtual(objectHandle, loop);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::RefreshList(int objectHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->RefreshObjectHandleList(objectHandle);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::ScrollListToView(int objectHandle, int itemIndex) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->ScrollObjectHandleListToView(objectHandle, itemIndex);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::CenterObject(int objectHandle, bool restraint) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->CenterObjectHandle(objectHandle, restraint);
#else
	return false;
#endif
}
