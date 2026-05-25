#include "FairyGuiLuaApiInternal.h"
#include "profiling/RuntimeProfileCounters.h"

#if defined(HELLO_ENABLE_FGUI)
#define HELLO_FGUI_INT_GETTER(ApiMethod, SystemMethod) \
int FairyGuiLuaApi::ApiMethod() const \
{ \
	FairyGuiSystem* system = GetSystem(); \
	return system != nullptr ? system->SystemMethod() : 0; \
}
#else
#define HELLO_FGUI_INT_GETTER(ApiMethod, SystemMethod) \
int FairyGuiLuaApi::ApiMethod() const \
{ \
	return 0; \
}
#endif

HELLO_FGUI_INT_GETTER(GetLastRenderCommandCount, GetLastRenderCommandCount)
HELLO_FGUI_INT_GETTER(GetLastTriangleCount, GetLastTriangleCount)
HELLO_FGUI_INT_GETTER(GetScreenWidth, GetScreenWidth)
HELLO_FGUI_INT_GETTER(GetScreenHeight, GetScreenHeight)
HELLO_FGUI_INT_GETTER(GetRuntimeObjectHandleCount, GetObjectHandleCount)
HELLO_FGUI_INT_GETTER(GetRuntimeListenerBindingCount, GetListenerBindingCount)
HELLO_FGUI_INT_GETTER(GetMaterialCount, GetMaterialCount)
HELLO_FGUI_INT_GETTER(GetTextureCount, GetTextureCount)
HELLO_FGUI_INT_GETTER(GetMaterialAliasCount, GetMaterialAliasCount)
HELLO_FGUI_INT_GETTER(GetTextureAliasCount, GetTextureAliasCount)
HELLO_FGUI_INT_GETTER(GetLastDrawCommandCount, GetLastDrawCommandCount)
HELLO_FGUI_INT_GETTER(GetLastDrawTriangleCount, GetLastDrawTriangleCount)
HELLO_FGUI_INT_GETTER(GetLastMaterialSwitchCount, GetLastMaterialSwitchCount)
HELLO_FGUI_INT_GETTER(GetLastTextureSwitchCount, GetLastTextureSwitchCount)
HELLO_FGUI_INT_GETTER(GetLastClippedCommandCount, GetLastClippedCommandCount)
HELLO_FGUI_INT_GETTER(GetLastClippedTriangleCount, GetLastClippedTriangleCount)
HELLO_FGUI_INT_GETTER(GetLastCulledCommandCount, GetLastCulledCommandCount)
HELLO_FGUI_INT_GETTER(GetLastStencilCommandCount, GetLastStencilCommandCount)
HELLO_FGUI_INT_GETTER(GetLastStencilTriangleCount, GetLastStencilTriangleCount)
HELLO_FGUI_INT_GETTER(GetLastCpuClipSourceTriangleCount, GetLastCpuClipSourceTriangleCount)
HELLO_FGUI_INT_GETTER(GetLastCpuClipOutputTriangleCount, GetLastCpuClipOutputTriangleCount)
HELLO_FGUI_INT_GETTER(GetLastCpuClipFragmentCount, GetLastCpuClipFragmentCount)
HELLO_FGUI_INT_GETTER(GetLastStencilClipScopeCount, GetLastStencilClipScopeCount)
HELLO_FGUI_INT_GETTER(GetLastStencilClipPolygonCount, GetLastStencilClipPolygonCount)
HELLO_FGUI_INT_GETTER(GetLastCustomCommandCount, GetLastCustomCommandCount)
HELLO_FGUI_INT_GETTER(GetLastMaxBatchTriangles, GetLastMaxBatchTriangles)
HELLO_FGUI_INT_GETTER(GetLastMaxBatchVertices, GetLastMaxBatchVertices)

#undef HELLO_FGUI_INT_GETTER

bool FairyGuiLuaApi::IsHardwareStencilSupported() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->IsHardwareStencilSupported();
#else
	return false;
#endif
}

const char* FairyGuiLuaApi::GetStencilBackendString()
{
	m_lastStencilBackend.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastStencilBackend = system->GetStencilBackendString();
#endif
	return m_lastStencilBackend.c_str();
}

const char* FairyGuiLuaApi::GetStencilBackendDetailString()
{
	m_lastStencilBackendDetail.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastStencilBackendDetail = system->GetStencilBackendDetailString();
#endif
	return m_lastStencilBackendDetail.c_str();
}

const char* FairyGuiLuaApi::GetMaterialDetailString()
{
	m_lastMaterialDetail.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastMaterialDetail = system->GetMaterialDetailString();
#endif
	return m_lastMaterialDetail.c_str();
}

const char* FairyGuiLuaApi::GetTextureDetailString()
{
	m_lastTextureDetail.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastTextureDetail = system->GetTextureDetailString();
#endif
	return m_lastTextureDetail.c_str();
}

const char* FairyGuiLuaApi::GetFrameRenderDetailString()
{
	m_lastFrameRenderDetail.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastFrameRenderDetail = system->GetFrameRenderDetailString();
#endif
	return m_lastFrameRenderDetail.c_str();
}

bool FairyGuiLuaApi::PlotServiceStats(int serviceOpenTotal, int serviceKindCount, int toastQueueCount, int loadingRefTotal, int serviceCreatedTotal, int serviceClosedTotal, int serviceFailedTotal, int servicePeakOpen) const
{
	RuntimeProfileCounters::PlotFairyGuiServiceStats(serviceOpenTotal, serviceKindCount, toastQueueCount, loadingRefTotal, serviceCreatedTotal, serviceClosedTotal, serviceFailedTotal, servicePeakOpen);
	return true;
}
