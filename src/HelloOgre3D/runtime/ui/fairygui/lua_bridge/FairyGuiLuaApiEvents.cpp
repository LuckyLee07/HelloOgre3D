#include "FairyGuiLuaApiInternal.h"

int FairyGuiLuaApi::AddEventListener(int objectHandle, const char* childPath, int eventType, int callbackId) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->AddObjectHandleEventListener(objectHandle, SafeString(childPath), eventType, callbackId) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::AddClickListener(int objectHandle, const char* childPath, int callbackId) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->AddObjectHandleClickListener(objectHandle, SafeString(childPath), callbackId) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::AddControllerChangedListener(int objectHandle, const char* controllerName, int callbackId) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->AddObjectHandleControllerChangedListener(objectHandle, SafeString(controllerName), callbackId) : 0;
#else
	return 0;
#endif
}

bool FairyGuiLuaApi::RemoveListener(int bindingId) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->RemoveObjectHandleListener(bindingId);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::RemoveObject(int objectHandle) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->RemoveObjectHandle(objectHandle);
#else
	return false;
#endif
}

void FairyGuiLuaApi::ClearObjects() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		system->ClearObjectHandles();
#endif
}
