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

int FairyGuiLuaApi::GetChild(int objectHandle, const char* childPath) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleChild(objectHandle, childPath != nullptr ? childPath : "") : 0;
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
	return system != nullptr && system->SetObjectHandleText(objectHandle, text != nullptr ? text : "");
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
	return system != nullptr && system->SetObjectHandleIcon(objectHandle, icon != nullptr ? icon : "");
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectLoaderUrl(int objectHandle, const char* url) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleLoaderUrl(objectHandle, url != nullptr ? url : "");
#else
	return false;
#endif
}

bool FairyGuiLuaApi::SetObjectControllerIndex(int objectHandle, const char* controllerName, int selectedIndex) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleControllerIndex(objectHandle, controllerName != nullptr ? controllerName : "", selectedIndex);
#else
	return false;
#endif
}

int FairyGuiLuaApi::GetObjectControllerIndex(int objectHandle, const char* controllerName) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleControllerIndex(objectHandle, controllerName != nullptr ? controllerName : "") : -1;
#else
	return -1;
#endif
}

bool FairyGuiLuaApi::SetObjectControllerPage(int objectHandle, const char* controllerName, const char* pageName) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->SetObjectHandleControllerPage(objectHandle, controllerName != nullptr ? controllerName : "", pageName != nullptr ? pageName : "");
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
		m_lastControllerString = system->GetObjectHandleControllerPage(objectHandle, controllerName != nullptr ? controllerName : "");
#endif
	return m_lastControllerString.c_str();
}

const char* FairyGuiLuaApi::GetObjectControllerPageId(int objectHandle, const char* controllerName)
{
	m_lastControllerString.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastControllerString = system->GetObjectHandleControllerPageId(objectHandle, controllerName != nullptr ? controllerName : "");
#endif
	return m_lastControllerString.c_str();
}

int FairyGuiLuaApi::GetObjectControllerPageCount(int objectHandle, const char* controllerName) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->GetObjectHandleControllerPageCount(objectHandle, controllerName != nullptr ? controllerName : "") : 0;
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
		m_lastControllerString = system->GetObjectHandleControllerPageNameAt(objectHandle, controllerName != nullptr ? controllerName : "", pageIndex);
#endif
	return m_lastControllerString.c_str();
}

const char* FairyGuiLuaApi::GetObjectControllerPageIdAt(int objectHandle, const char* controllerName, int pageIndex)
{
	m_lastControllerString.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system != nullptr)
		m_lastControllerString = system->GetObjectHandleControllerPageIdAt(objectHandle, controllerName != nullptr ? controllerName : "", pageIndex);
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
	return system != nullptr && system->SetObjectHandleComboBoxValue(objectHandle, value != nullptr ? value : "");
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
	return system != nullptr && system->PlayObjectHandleTransition(objectHandle, transitionName != nullptr ? transitionName : "", times, delay, callbackId);
#else
	return false;
#endif
}

bool FairyGuiLuaApi::StopTransition(int objectHandle, const char* transitionName, bool setToComplete, bool processCallback) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->StopObjectHandleTransition(objectHandle, transitionName != nullptr ? transitionName : "", setToComplete, processCallback);
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
	return system != nullptr && system->InjectImeCompositionText(text != nullptr ? text : "");
#else
	return false;
#endif
}

bool FairyGuiLuaApi::InjectImeCommitText(const char* text) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->InjectImeCommitText(text != nullptr ? text : "");
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

int FairyGuiLuaApi::AddEventListener(int objectHandle, const char* childPath, int eventType, int callbackId) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->AddObjectHandleEventListener(objectHandle, childPath != nullptr ? childPath : "", eventType, callbackId);
#else
	return 0;
#endif
}

int FairyGuiLuaApi::AddClickListener(int objectHandle, const char* childPath, int callbackId) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->AddObjectHandleClickListener(objectHandle, childPath != nullptr ? childPath : "", callbackId);
#else
	return 0;
#endif
}

int FairyGuiLuaApi::AddControllerChangedListener(int objectHandle, const char* controllerName, int callbackId) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->AddObjectHandleControllerChangedListener(objectHandle, controllerName != nullptr ? controllerName : "", callbackId);
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
