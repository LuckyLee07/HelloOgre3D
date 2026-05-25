#include "FairyGuiLuaApiInternal.h"
#include "ClientManager.h"

FairyGuiLuaApi::FairyGuiLuaApi(ClientManager* clientManager)
	: m_clientManager(clientManager),
	m_lastPackageName(), m_lastStencilBackend(), m_lastStencilBackendDetail(),
	m_lastMaterialDetail(), m_lastTextureDetail(), m_lastFrameRenderDetail(),
	m_lastObjectText(), m_lastObjectValue(), m_lastControllerString(), m_lastImeDebug()
{
}

#if defined(HELLO_ENABLE_FGUI)
FairyGuiSystem* FairyGuiLuaApi::GetSystem() const
{
	return m_clientManager != nullptr ? m_clientManager->getFairyGuiSystem() : nullptr;
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
