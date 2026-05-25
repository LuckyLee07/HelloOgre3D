#include "FairyGuiLuaApiInternal.h"

bool FairyGuiLuaApi::InjectMouseMove(int x, int y) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectMouseMove(x, y);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectMouseDown(int x, int y, int button) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectMouseDown(x, y, button);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectMouseUp(int x, int y, int button) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectMouseUp(x, y, button);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectMouseWheel(int x, int y, int wheelDelta) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectMouseWheel(x, y, wheelDelta);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectLogicalMouseMove(int x, int y) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectLogicalMouseMove(x, y);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectLogicalMouseDown(int x, int y, int button) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectLogicalMouseDown(x, y, button);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectLogicalMouseUp(int x, int y, int button) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectLogicalMouseUp(x, y, button);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectLogicalMouseWheel(int x, int y, int wheelDelta) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectLogicalMouseWheel(x, y, wheelDelta);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectLogicalClick(int x, int y, int button) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return false;

	system->InjectLogicalMouseMove(x, y);
	const bool downConsumed = system->InjectLogicalMouseDown(x, y, button);
	const bool upConsumed = system->InjectLogicalMouseUp(x, y, button);
	return downConsumed || upConsumed;
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectKeyPressed(int keyCode, int keyText) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectKeyPressed(keyCode, keyText);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectKeyReleased(int keyCode, int keyText) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectKeyReleased(keyCode, keyText);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectImeCompositionText(const char* text) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectImeCompositionText(SafeString(text));
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectImeCommitText(const char* text) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectImeCommitText(SafeString(text));
#else
	return false;
#endif
}

bool FairyGuiLuaApi::ClearImeComposition() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->ClearImeCompositionText();
#else
	return false;
#endif
}

const char* FairyGuiLuaApi::GetImeDebugString()
{
	m_lastImeDebug.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastImeDebug = system->GetImeDebugString();
#endif
	return m_lastImeDebug.c_str();
}
