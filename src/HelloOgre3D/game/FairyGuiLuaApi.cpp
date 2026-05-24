#include "FairyGuiLuaApi.h"
#include "ClientManager.h"
#include "profiling/RuntimeProfileCounters.h"
#if defined(HELLO_ENABLE_FGUI)
#if defined(MIN)
#undef MIN
#endif
#if defined(MAX)
#undef MAX
#endif
#include "ui/fairygui/FairyGuiSystem.h"
#endif

FairyGuiLuaApi::FairyGuiLuaApi(ClientManager* clientManager)
	: m_clientManager(clientManager),
	m_lastPackageName(), m_lastStencilBackend(), m_lastStencilBackendDetail(),
	m_lastMaterialDetail(), m_lastTextureDetail(), m_lastFrameRenderDetail()
{
}

#if defined(HELLO_ENABLE_FGUI)
FairyGuiSystem* FairyGuiLuaApi::GetSystem() const
{
	return m_clientManager != nullptr ? m_clientManager->getFairyGuiSystem() : nullptr;
}
#endif

bool FairyGuiLuaApi::IsAvailable() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->IsInitialized();
#else
	return false;
#endif
}

const char* FairyGuiLuaApi::LoadPackage(const char* packagePath)
{
	m_lastPackageName.clear();
#if defined(HELLO_ENABLE_FGUI)
	if (packagePath == nullptr)
		return "";

	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return "";

	m_lastPackageName = system->LoadPackageAndGetName(packagePath);
#endif
	return m_lastPackageName.c_str();
}

bool FairyGuiLuaApi::RemovePackage(const char* packageName) const
{
#if defined(HELLO_ENABLE_FGUI)
	if (packageName == nullptr)
		return false;

	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->RemovePackage(packageName);
#else
	return false;
#endif
}

int FairyGuiLuaApi::CreateObject(const char* packageName, const char* objectName) const
{
#if defined(HELLO_ENABLE_FGUI)
	if (packageName == nullptr || objectName == nullptr)
		return 0;

	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->CreateObjectHandle(packageName, objectName);
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateContainer(const char* name) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->CreateContainerHandle(name != nullptr ? name : "");
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateChildContainer(int ownerHandle, const char* name) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->CreateChildContainerHandle(ownerHandle, name != nullptr ? name : "");
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateLoader(int ownerHandle, const char* name, const char* url) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->CreateLoaderHandle(ownerHandle, name != nullptr ? name : "", url != nullptr ? url : "");
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateText(int ownerHandle, const char* name, const char* text, float fontSize, float red, float green, float blue) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->CreateTextHandle(ownerHandle, name != nullptr ? name : "", text != nullptr ? text : "", fontSize, red, green, blue);
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateTextInput(int ownerHandle, const char* name, const char* text, float fontSize, float red, float green, float blue) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->CreateTextInputHandle(ownerHandle, name != nullptr ? name : "", text != nullptr ? text : "", fontSize, red, green, blue);
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateGraphRect(int ownerHandle, const char* name, float width, float height, float red, float green, float blue, float alpha) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->CreateGraphRectHandle(ownerHandle, name != nullptr ? name : "", width, height, red, green, blue, alpha);
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateGraphRegularPolygon(int ownerHandle, const char* name, float width, float height, int sides, float red, float green, float blue, float alpha) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->CreateGraphRegularPolygonHandle(ownerHandle, name != nullptr ? name : "", width, height, sides, red, green, blue, alpha);
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateModalMask(float red, float green, float blue, float alpha) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->CreateModalMaskHandle(red, green, blue, alpha);
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastRenderCommandCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastRenderCommandCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastTriangleCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastTriangleCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetScreenWidth() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetScreenWidth() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetScreenHeight() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetScreenHeight() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetRuntimeObjectHandleCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetRuntimeListenerBindingCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetListenerBindingCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetMaterialCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetMaterialCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetTextureCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetTextureCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetMaterialAliasCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetMaterialAliasCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetTextureAliasCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetTextureAliasCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastDrawCommandCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastDrawCommandCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastDrawTriangleCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastDrawTriangleCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastMaterialSwitchCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastMaterialSwitchCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastTextureSwitchCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastTextureSwitchCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastClippedCommandCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastClippedCommandCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastClippedTriangleCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastClippedTriangleCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastCulledCommandCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastCulledCommandCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastStencilCommandCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastStencilCommandCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastStencilTriangleCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastStencilTriangleCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastCpuClipSourceTriangleCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastCpuClipSourceTriangleCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastCpuClipOutputTriangleCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastCpuClipOutputTriangleCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastCpuClipFragmentCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastCpuClipFragmentCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastStencilClipScopeCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastStencilClipScopeCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastStencilClipPolygonCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastStencilClipPolygonCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastCustomCommandCount() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastCustomCommandCount() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastMaxBatchTriangles() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastMaxBatchTriangles() : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::GetLastMaxBatchVertices() const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetLastMaxBatchVertices() : 0;
#else
	return 0;
#endif
}

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
