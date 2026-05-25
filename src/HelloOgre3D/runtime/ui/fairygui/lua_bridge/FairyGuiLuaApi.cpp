#include "FairyGuiLuaApiInternal.h"

FairyGuiLuaApi::FairyGuiLuaApi(FairyGuiSystem* system)
	: m_system(system),
	m_lastPackageName(), m_lastStencilBackend(), m_lastStencilBackendDetail(),
	m_lastMaterialDetail(), m_lastTextureDetail(), m_lastFrameRenderDetail(),
	m_lastObjectText(), m_lastObjectValue(), m_lastControllerString(), m_lastImeDebug()
{
}

#if defined(HELLO_ENABLE_FGUI)
FairyGuiSystem* FairyGuiLuaApi::GetSystem() const
{
	return m_system;
}
#endif

const char* FairyGuiLuaApi::SafeString(const char* value)
{
	return value != nullptr ? value : "";
}

bool FairyGuiLuaApi::IsAvailable() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->IsInitialized();
#else
	return false;
#endif
}
