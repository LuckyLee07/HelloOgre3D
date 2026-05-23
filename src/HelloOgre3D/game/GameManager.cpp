#include "GameManager.h"
#include "Ogre.h"
#include "OgreDpiHelper.h"
#include "ScriptLuaVM.h"
#include "tolua++.h"
#include "LuaInterface.h"
#include <algorithm>
#include "GlobalFuncs.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <winsock.h>
#else
#include <sys/time.h>
#endif
#include "ClientManager.h"
#if defined(HELLO_ENABLE_FGUI)
#include "ui/fairygui/FairyGuiSystem.h"
#endif
#include "systems/ui/UIManager.h"
#include "systems/manager/SandboxMgr.h"
#include "systems/manager/ObjectManager.h"
#include "debug/DebugDrawer.h"
#include "systems/physics/PhysicsWorld.h"
#include "core/SandboxMacros.h"
#include "profiling/Profile.h"

using namespace Ogre;

extern int tolua_SandboxToLua_open(lua_State* tolua_S);
extern int tolua_GameToLua_open(lua_State* tolua_S);
extern int tolua_SandboxToLua_Manual(lua_State* tolua_S);

GameManager* g_GameManager = nullptr;
GameManager* GetGameManager()
{
	return g_GameManager;
}

GameManager::GameManager(ClientManager* pClientMgr)
	: m_pClientManager(pClientMgr), m_SimulationTime(0), m_pScriptVM(nullptr),
	m_pPhysicsWorld(nullptr), m_pSandboxMgr(nullptr), m_pObjectManager(nullptr), m_pUIManager(nullptr),
	m_fairyGuiLastPackageName(), m_fairyGuiLastObjectText(), m_fairyGuiLastObjectValue(), m_fairyGuiLastControllerString(),
	m_fairyGuiLastMaterialDetail(), m_fairyGuiLastTextureDetail(), m_fairyGuiLastFrameRenderDetail(), m_fairyGuiLastImeDebug()
{
	
}

GameManager::~GameManager()
{
	SAFE_DELETE(m_pSandboxMgr);
	SAFE_DELETE(m_pObjectManager);
	SAFE_DELETE(m_pPhysicsWorld);
	SAFE_DELETE(m_pUIManager);

	g_SandboxMgr = nullptr;
	g_ObjectManager = nullptr;
	m_pClientManager = nullptr;
}

GameManager* GameManager::GetInstance()
{
	return g_GameManager;
}

bool GameManager::isFairyGuiAvailable()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->IsInitialized();
#else
	return false;
#endif
}

const char* GameManager::loadFairyGuiPackage(const char* packagePath)
{
	m_fairyGuiLastPackageName.clear();
#if defined(HELLO_ENABLE_FGUI)
	if (packagePath == nullptr)
		return "";

	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return "";

	m_fairyGuiLastPackageName = system->LoadPackageAndGetName(packagePath);
#endif
	return m_fairyGuiLastPackageName.c_str();
}

bool GameManager::removeFairyGuiPackage(const char* packageName)
{
#if defined(HELLO_ENABLE_FGUI)
	if (packageName == nullptr)
		return false;

	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->RemovePackage(packageName);
#else
	return false;
#endif
}

int GameManager::createFairyGuiObject(const char* packageName, const char* objectName)
{
#if defined(HELLO_ENABLE_FGUI)
	if (packageName == nullptr || objectName == nullptr)
		return 0;

	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->CreateObjectHandle(packageName, objectName);
#else
	return 0;
#endif
}

int GameManager::createFairyGuiContainer(const char* name)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->CreateContainerHandle(name != nullptr ? name : "");
#else
	return 0;
#endif
}

int GameManager::createFairyGuiChildContainer(int ownerHandle, const char* name)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->CreateChildContainerHandle(ownerHandle, name != nullptr ? name : "");
#else
	return 0;
#endif
}

int GameManager::createFairyGuiLoader(int ownerHandle, const char* name, const char* url)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->CreateLoaderHandle(ownerHandle, name != nullptr ? name : "", url != nullptr ? url : "");
#else
	return 0;
#endif
}

int GameManager::createFairyGuiText(int ownerHandle, const char* name, const char* text, Ogre::Real fontSize, Ogre::Real red, Ogre::Real green, Ogre::Real blue)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->CreateTextHandle(ownerHandle, name != nullptr ? name : "", text != nullptr ? text : "", fontSize, red, green, blue);
#else
	return 0;
#endif
}

int GameManager::createFairyGuiTextInput(int ownerHandle, const char* name, const char* text, Ogre::Real fontSize, Ogre::Real red, Ogre::Real green, Ogre::Real blue)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->CreateTextInputHandle(ownerHandle, name != nullptr ? name : "", text != nullptr ? text : "", fontSize, red, green, blue);
#else
	return 0;
#endif
}

int GameManager::createFairyGuiGraphRect(int ownerHandle, const char* name, Ogre::Real width, Ogre::Real height, Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->CreateGraphRectHandle(ownerHandle, name != nullptr ? name : "", width, height, red, green, blue, alpha);
#else
	return 0;
#endif
}

int GameManager::createFairyGuiGraphRegularPolygon(int ownerHandle, const char* name, Ogre::Real width, Ogre::Real height, int sides, Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->CreateGraphRegularPolygonHandle(ownerHandle, name != nullptr ? name : "", width, height, sides, red, green, blue, alpha);
#else
	return 0;
#endif
}

int GameManager::createFairyGuiModalMask(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->CreateModalMaskHandle(red, green, blue, alpha);
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastRenderCommandCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastRenderCommandCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastTriangleCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastTriangleCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiScreenWidth()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetScreenWidth() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiScreenHeight()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetScreenHeight() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiRuntimeObjectHandleCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetObjectHandleCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiRuntimeListenerBindingCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetListenerBindingCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiMaterialCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetMaterialCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiTextureCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetTextureCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiMaterialAliasCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetMaterialAliasCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiTextureAliasCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetTextureAliasCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastDrawCommandCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastDrawCommandCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastDrawTriangleCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastDrawTriangleCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastMaterialSwitchCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastMaterialSwitchCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastTextureSwitchCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastTextureSwitchCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastClippedCommandCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastClippedCommandCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastClippedTriangleCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastClippedTriangleCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastCulledCommandCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastCulledCommandCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastStencilCommandCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastStencilCommandCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastStencilTriangleCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastStencilTriangleCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastCustomCommandCount()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastCustomCommandCount() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastMaxBatchTriangles()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastMaxBatchTriangles() : 0;
#else
	return 0;
#endif
}

int GameManager::getFairyGuiLastMaxBatchVertices()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetLastMaxBatchVertices() : 0;
#else
	return 0;
#endif
}

const char* GameManager::getFairyGuiMaterialDetailString()
{
	m_fairyGuiLastMaterialDetail.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		m_fairyGuiLastMaterialDetail = system->GetMaterialDetailString();
#endif
	return m_fairyGuiLastMaterialDetail.c_str();
}

const char* GameManager::getFairyGuiTextureDetailString()
{
	m_fairyGuiLastTextureDetail.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		m_fairyGuiLastTextureDetail = system->GetTextureDetailString();
#endif
	return m_fairyGuiLastTextureDetail.c_str();
}

const char* GameManager::getFairyGuiFrameRenderDetailString()
{
	m_fairyGuiLastFrameRenderDetail.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		m_fairyGuiLastFrameRenderDetail = system->GetFrameRenderDetailString();
#endif
	return m_fairyGuiLastFrameRenderDetail.c_str();
}

bool GameManager::plotFairyGuiServiceStats(int serviceOpenTotal, int serviceKindCount, int toastQueueCount, int loadingRefTotal, int serviceCreatedTotal, int serviceClosedTotal, int serviceFailedTotal, int servicePeakOpen)
{
	H3D_PROFILE_PLOT("FGUIServiceOpen", static_cast<double>(serviceOpenTotal));
	H3D_PROFILE_PLOT("FGUIServiceKinds", static_cast<double>(serviceKindCount));
	H3D_PROFILE_PLOT("FGUIToastQueue", static_cast<double>(toastQueueCount));
	H3D_PROFILE_PLOT("FGUILoadingRefs", static_cast<double>(loadingRefTotal));
	H3D_PROFILE_PLOT("FGUIServiceCreated", static_cast<double>(serviceCreatedTotal));
	H3D_PROFILE_PLOT("FGUIServiceClosed", static_cast<double>(serviceClosedTotal));
	H3D_PROFILE_PLOT("FGUIServiceFailed", static_cast<double>(serviceFailedTotal));
	H3D_PROFILE_PLOT("FGUIServicePeakOpen", static_cast<double>(servicePeakOpen));
	return true;
}

int GameManager::getFairyGuiChild(int objectHandle, const char* childPath)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->GetObjectHandleChild(objectHandle, childPath != nullptr ? childPath : "");
#else
	return 0;
#endif
}

int GameManager::getFairyGuiListItem(int objectHandle, int itemIndex)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->GetObjectHandleListItem(objectHandle, itemIndex);
#else
	return 0;
#endif
}

int GameManager::getFairyGuiListItemCount(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->GetObjectHandleListItemCount(objectHandle);
#else
	return 0;
#endif
}

bool GameManager::addFairyGuiObjectToRoot(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->AddObjectHandleToRoot(objectHandle);
#else
	return false;
#endif
}

bool GameManager::addFairyGuiObjectToParent(int objectHandle, int parentHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->AddObjectHandleToParent(objectHandle, parentHandle);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectPosition(int objectHandle, Ogre::Real x, Ogre::Real y)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandlePosition(objectHandle, x, y);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectSize(int objectHandle, Ogre::Real width, Ogre::Real height)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleSize(objectHandle, width, height);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectVisible(int objectHandle, bool visible)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleVisible(objectHandle, visible);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectAlpha(int objectHandle, Ogre::Real alpha)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleAlpha(objectHandle, alpha);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectTouchable(int objectHandle, bool touchable)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleTouchable(objectHandle, touchable);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectMask(int objectHandle, int maskHandle, bool inverted)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleMask(objectHandle, maskHandle, inverted);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectSortingOrder(int objectHandle, int sortingOrder)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleSortingOrder(objectHandle, sortingOrder);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectText(int objectHandle, const char* text)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleText(objectHandle, text != nullptr ? text : "");
#else
	return false;
#endif
}

const char* GameManager::getFairyGuiObjectText(int objectHandle)
{
	m_fairyGuiLastObjectText.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		m_fairyGuiLastObjectText = system->GetObjectHandleText(objectHandle);
#endif
	return m_fairyGuiLastObjectText.c_str();
}

bool GameManager::setFairyGuiObjectIcon(int objectHandle, const char* icon)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleIcon(objectHandle, icon != nullptr ? icon : "");
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectLoaderUrl(int objectHandle, const char* url)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleLoaderUrl(objectHandle, url != nullptr ? url : "");
#else
	return false;
#endif
}

bool GameManager::setFairyGuiObjectControllerIndex(int objectHandle, const char* controllerName, int selectedIndex)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleControllerIndex(objectHandle, controllerName != nullptr ? controllerName : "", selectedIndex);
#else
	return false;
#endif
}

int GameManager::getFairyGuiObjectControllerIndex(int objectHandle, const char* controllerName)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetObjectHandleControllerIndex(objectHandle, controllerName != nullptr ? controllerName : "") : -1;
#else
	return -1;
#endif
}

bool GameManager::setFairyGuiObjectControllerPage(int objectHandle, const char* controllerName, const char* pageName)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleControllerPage(objectHandle, controllerName != nullptr ? controllerName : "", pageName != nullptr ? pageName : "");
#else
	return false;
#endif
}

const char* GameManager::getFairyGuiObjectControllerPage(int objectHandle, const char* controllerName)
{
	m_fairyGuiLastControllerString.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		m_fairyGuiLastControllerString = system->GetObjectHandleControllerPage(objectHandle, controllerName != nullptr ? controllerName : "");
#endif
	return m_fairyGuiLastControllerString.c_str();
}

const char* GameManager::getFairyGuiObjectControllerPageId(int objectHandle, const char* controllerName)
{
	m_fairyGuiLastControllerString.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		m_fairyGuiLastControllerString = system->GetObjectHandleControllerPageId(objectHandle, controllerName != nullptr ? controllerName : "");
#endif
	return m_fairyGuiLastControllerString.c_str();
}

int GameManager::getFairyGuiObjectControllerPageCount(int objectHandle, const char* controllerName)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetObjectHandleControllerPageCount(objectHandle, controllerName != nullptr ? controllerName : "") : 0;
#else
	return 0;
#endif
}

const char* GameManager::getFairyGuiObjectControllerPageNameAt(int objectHandle, const char* controllerName, int pageIndex)
{
	m_fairyGuiLastControllerString.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		m_fairyGuiLastControllerString = system->GetObjectHandleControllerPageNameAt(objectHandle, controllerName != nullptr ? controllerName : "", pageIndex);
#endif
	return m_fairyGuiLastControllerString.c_str();
}

const char* GameManager::getFairyGuiObjectControllerPageIdAt(int objectHandle, const char* controllerName, int pageIndex)
{
	m_fairyGuiLastControllerString.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		m_fairyGuiLastControllerString = system->GetObjectHandleControllerPageIdAt(objectHandle, controllerName != nullptr ? controllerName : "", pageIndex);
#endif
	return m_fairyGuiLastControllerString.c_str();
}

bool GameManager::setFairyGuiObjectValue(int objectHandle, Ogre::Real value)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleValue(objectHandle, value);
#else
	return false;
#endif
}

Ogre::Real GameManager::getFairyGuiObjectValue(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetObjectHandleValue(objectHandle) : 0;
#else
	return 0;
#endif
}

bool GameManager::setFairyGuiObjectMin(int objectHandle, Ogre::Real minValue)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleMin(objectHandle, minValue);
#else
	return false;
#endif
}

Ogre::Real GameManager::getFairyGuiObjectMin(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetObjectHandleMin(objectHandle) : 0;
#else
	return 0;
#endif
}

bool GameManager::setFairyGuiObjectMax(int objectHandle, Ogre::Real maxValue)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleMax(objectHandle, maxValue);
#else
	return false;
#endif
}

Ogre::Real GameManager::getFairyGuiObjectMax(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetObjectHandleMax(objectHandle) : 0;
#else
	return 0;
#endif
}

bool GameManager::setFairyGuiComboBoxSelectedIndex(int objectHandle, int selectedIndex)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleComboBoxSelectedIndex(objectHandle, selectedIndex);
#else
	return false;
#endif
}

int GameManager::getFairyGuiComboBoxSelectedIndex(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetObjectHandleComboBoxSelectedIndex(objectHandle) : -1;
#else
	return -1;
#endif
}

bool GameManager::setFairyGuiComboBoxValue(int objectHandle, const char* value)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleComboBoxValue(objectHandle, value != nullptr ? value : "");
#else
	return false;
#endif
}

const char* GameManager::getFairyGuiComboBoxValue(int objectHandle)
{
	m_fairyGuiLastObjectValue.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		m_fairyGuiLastObjectValue = system->GetObjectHandleComboBoxValue(objectHandle);
#endif
	return m_fairyGuiLastObjectValue.c_str();
}

bool GameManager::playFairyGuiTransition(int objectHandle, const char* transitionName, int times, Ogre::Real delay, int callbackId)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->PlayObjectHandleTransition(objectHandle, transitionName != nullptr ? transitionName : "", times, delay, callbackId);
#else
	return false;
#endif
}

bool GameManager::stopFairyGuiTransition(int objectHandle, const char* transitionName, bool setToComplete, bool processCallback)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->StopObjectHandleTransition(objectHandle, transitionName != nullptr ? transitionName : "", setToComplete, processCallback);
#else
	return false;
#endif
}

bool GameManager::focusFairyGuiObject(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleFocus(objectHandle);
#else
	return false;
#endif
}

bool GameManager::clearFairyGuiFocus()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->ClearObjectHandleFocus();
#else
	return false;
#endif
}

int GameManager::getFairyGuiFocusedObject()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetFocusedObjectHandle() : 0;
#else
	return 0;
#endif
}

bool GameManager::setFairyGuiListItemCount(int objectHandle, int itemCount)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleListItemCount(objectHandle, itemCount);
#else
	return false;
#endif
}

bool GameManager::setFairyGuiListSelectedIndex(int objectHandle, int selectedIndex)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleListSelectedIndex(objectHandle, selectedIndex);
#else
	return false;
#endif
}

int GameManager::getFairyGuiListSelectedIndex(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr ? system->GetObjectHandleListSelectedIndex(objectHandle) : -1;
#else
	return -1;
#endif
}

bool GameManager::setFairyGuiListVirtual(int objectHandle, bool loop)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->SetObjectHandleListVirtual(objectHandle, loop);
#else
	return false;
#endif
}

bool GameManager::refreshFairyGuiList(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->RefreshObjectHandleList(objectHandle);
#else
	return false;
#endif
}

bool GameManager::scrollFairyGuiListToView(int objectHandle, int itemIndex)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->ScrollObjectHandleListToView(objectHandle, itemIndex);
#else
	return false;
#endif
}

bool GameManager::centerFairyGuiObject(int objectHandle, bool restraint)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->CenterObjectHandle(objectHandle, restraint);
#else
	return false;
#endif
}

bool GameManager::injectFairyGuiMouseMove(int x, int y)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->InjectLogicalMouseMove(x, y);
#else
	return false;
#endif
}

bool GameManager::injectFairyGuiMouseDown(int x, int y, int button)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->InjectLogicalMouseDown(x, y, button);
#else
	return false;
#endif
}

bool GameManager::injectFairyGuiMouseUp(int x, int y, int button)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->InjectLogicalMouseUp(x, y, button);
#else
	return false;
#endif
}

bool GameManager::injectFairyGuiMouseWheel(int x, int y, int wheelDelta)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->InjectLogicalMouseWheel(x, y, wheelDelta);
#else
	return false;
#endif
}

bool GameManager::injectFairyGuiClick(int x, int y, int button)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
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

bool GameManager::injectFairyGuiKeyPressed(int keyCode, int keyText)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->InjectKeyPressed(keyCode, keyText);
#else
	return false;
#endif
}

bool GameManager::injectFairyGuiKeyReleased(int keyCode, int keyText)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->InjectKeyReleased(keyCode, keyText);
#else
	return false;
#endif
}

bool GameManager::injectFairyGuiImeCompositionText(const char* text)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->InjectImeCompositionText(text != nullptr ? text : "");
#else
	return false;
#endif
}

bool GameManager::injectFairyGuiImeCommitText(const char* text)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->InjectImeCommitText(text != nullptr ? text : "");
#else
	return false;
#endif
}

bool GameManager::clearFairyGuiImeComposition()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->ClearImeCompositionText();
#else
	return false;
#endif
}

const char* GameManager::getFairyGuiImeDebugString()
{
	m_fairyGuiLastImeDebug.clear();
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		m_fairyGuiLastImeDebug = system->GetImeDebugString();
#endif
	return m_fairyGuiLastImeDebug.c_str();
}

int GameManager::addFairyGuiEventListener(int objectHandle, const char* childPath, int eventType, int callbackId)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->AddObjectHandleEventListener(objectHandle, childPath != nullptr ? childPath : "", eventType, callbackId);
#else
	return 0;
#endif
}

int GameManager::addFairyGuiClickListener(int objectHandle, const char* childPath, int callbackId)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->AddObjectHandleClickListener(objectHandle, childPath != nullptr ? childPath : "", callbackId);
#else
	return 0;
#endif
}

int GameManager::addFairyGuiControllerChangedListener(int objectHandle, const char* controllerName, int callbackId)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system == nullptr)
		return 0;

	return system->AddObjectHandleControllerChangedListener(objectHandle, controllerName != nullptr ? controllerName : "", callbackId);
#else
	return 0;
#endif
}

bool GameManager::removeFairyGuiListener(int bindingId)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->RemoveObjectHandleListener(bindingId);
#else
	return false;
#endif
}

bool GameManager::removeFairyGuiObject(int objectHandle)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	return system != nullptr && system->RemoveObjectHandle(objectHandle);
#else
	return false;
#endif
}

void GameManager::clearFairyGuiObjects()
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
		system->ClearObjectHandles();
#endif
}

void GameManager::Initialize()
{
	m_pScriptVM = GetScriptLuaVM();

	m_pUIManager = new UIManager(this);
	m_pUIManager->InitConfig();

	m_pPhysicsWorld = new PhysicsWorld();
	m_pPhysicsWorld->initilize();

	m_pObjectManager = new ObjectManager(m_pPhysicsWorld);
	g_ObjectManager = m_pObjectManager;

	UIService uiservice(m_pUIManager);
	ObjectFactory objfactory(m_pObjectManager);
	CameraService camservice(m_pClientManager);

	Ogre::SceneManager* pSceneManager = m_pClientManager->getSceneManager();
	m_pSandboxMgr = new SandboxMgr(uiservice, camservice, objfactory, pSceneManager);
	g_SandboxMgr = m_pSandboxMgr;

	this->InitLuaEnv();
}

void GameManager::InitLuaEnv()
{
	// 设置ToLua对象 
	tolua_SandboxToLua_open(m_pScriptVM->getLuaState());
	tolua_GameToLua_open(m_pScriptVM->getLuaState());
	tolua_SandboxToLua_Manual(m_pScriptVM->getLuaState());

	// 设置lua可用的c++对象 
	m_pScriptVM->setUserTypePointer("Sandbox", "SandboxMgr", m_pSandboxMgr);
	m_pScriptVM->setUserTypePointer("GameManager", "GameManager", this);
	m_pScriptVM->setUserTypePointer("ObjectManager", "ObjectManager", m_pObjectManager);
	m_pScriptVM->setUserTypePointer("DebugDrawer", "DebugDrawer", DebugDrawer::GetInstance());
	m_pScriptVM->setUserTypePointer("LuaInterface", "LuaInterface", LuaInterface::GetInstance());
	
	m_pScriptVM->callFile("res/scripts/script_init.lua");

	struct timeval stNow;
	gettimeofday(&stNow, NULL);
	m_pScriptVM->callFunction("__init__", "ii", stNow.tv_sec, stNow.tv_usec / 1000);
}

void GameManager::Update(int deltaMilliseconds)
{
	H3D_PROFILE_SCOPE("GameManager::UpdateStages");
	//struct timeval stNow;
	//gettimeofday(&stNow, NULL);
	{
		H3D_PROFILE_SCOPE("Lua::__tick__");
		m_pScriptVM->callFunction("__tick__", "i", deltaMilliseconds);
	}

	m_SimulationTime += deltaMilliseconds;

	{
		H3D_PROFILE_SCOPE("ObjectManager::Update");
		m_pObjectManager->Update(deltaMilliseconds);
	}

	{
		H3D_PROFILE_SCOPE("PhysicsWorld::Step");
		m_pPhysicsWorld->stepWorld();
	}

	{
		H3D_PROFILE_SCOPE("Lua::Sandbox_Update");
		m_pScriptVM->callFunction("Sandbox_Update", "i", deltaMilliseconds);
	}
}

Ogre::Camera* GameManager::getCamera()
{
	return m_pClientManager->getCamera();
}

Ogre::SceneNode* GameManager::getRootSceneNode()
{
	return m_pClientManager->getRootSceneNode();
}

Ogre::SceneManager* GameManager::getSceneManager()
{
	return m_pClientManager->getSceneManager();
}

Ogre::Real GameManager::getScreenWidth()
{
	if (m_pUIManager != nullptr)
	{
		Ogre::Real uiWidth = m_pUIManager->GetScreenWidth();
		if (uiWidth > 0.0f)
			return uiWidth;
	}

	Ogre::RenderWindow* renderWindow = m_pClientManager->getRenderWindow();
	if (renderWindow != nullptr)
	{
		unsigned int width = 0, height = 0, depth = 0;
		int left = 0, top = 0;
		renderWindow->getMetrics(width, height, depth, left, top);
		return Ogre::Real(Ogre::DpiHelper::toLogicalPixels(width));
	}

	Ogre::Camera* camera = m_pClientManager->getCamera();
	if (camera && camera->getViewport())
		return Ogre::Real(Ogre::DpiHelper::toLogicalPixels(static_cast<unsigned int>(camera->getViewport()->getActualWidth())));
	return 0.0f;
}

Ogre::Real GameManager::getScreenHeight()
{
	if (m_pUIManager != nullptr)
	{
		Ogre::Real uiHeight = m_pUIManager->GetScreenHeight();
		if (uiHeight > 0.0f)
			return uiHeight;
	}

	Ogre::RenderWindow* renderWindow = m_pClientManager->getRenderWindow();
	if (renderWindow != nullptr)
	{
		unsigned int width = 0, height = 0, depth = 0;
		int left = 0, top = 0;
		renderWindow->getMetrics(width, height, depth, left, top);
		return Ogre::Real(Ogre::DpiHelper::toLogicalPixels(height));
	}

	Ogre::Camera* camera = m_pClientManager->getCamera();
	if (camera && camera->getViewport())
		return Ogre::Real(Ogre::DpiHelper::toLogicalPixels(static_cast<unsigned int>(camera->getViewport()->getActualHeight())));
	return 0.0f;
}

InputManager* GameManager::getInputManager()
{
	return m_pClientManager->getInputManager();
}

void GameManager::HandleWindowClosed()
{
	//m_pScriptVM->callFunction("EventHandle_WindowClosed", "");
}

void GameManager::HandleWindowResized(unsigned int width, unsigned int height)
{
	if (m_pUIManager != nullptr)
		m_pUIManager->HandleWindowResized(width, height);
	m_pScriptVM->callFunction("EventHandle_WindowResized", "ii", width, height);
	m_pScriptVM->callFunction("FairyGuiManager_HandleWindowResized", "ii", width, height);
}

bool GameManager::OnKeyPressed(OIS::KeyCode keycode, unsigned int key)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr && system->InjectKeyPressed(static_cast<int>(keycode), static_cast<int>(key)))
		return true;
#endif

	bool consumed = false;
	m_pScriptVM->callFunction("FairyGuiManager_HandleKeyPressed", "ii>b", keycode, static_cast<int>(key), &consumed);
	if (consumed)
		return true;

	m_pScriptVM->callFunction("EventHandle_Keyboard", "ib", keycode, true);
	return false;
}

bool GameManager::OnKeyReleased(OIS::KeyCode keycode, unsigned int key)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr && system->InjectKeyReleased(static_cast<int>(keycode), static_cast<int>(key)))
		return true;
#endif

	bool consumed = false;
	m_pScriptVM->callFunction("FairyGuiManager_HandleKeyReleased", "ii>b", keycode, static_cast<int>(key), &consumed);
	if (consumed)
		return true;

	m_pScriptVM->callFunction("EventHandle_Keyboard", "ib", keycode, false);

	m_pObjectManager->HandleKeyEvent(keycode, key);
	return false;
}

bool GameManager::OnMouseMoved(const OIS::MouseEvent& evt)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr)
	{
		if (evt.state.Z.rel != 0 && system->InjectMouseWheel(evt.state.X.abs, evt.state.Y.abs, evt.state.Z.rel))
			return true;
		if (system->InjectMouseMove(evt.state.X.abs, evt.state.Y.abs))
			return true;
	}
#endif
	return false;
}

bool GameManager::OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr && system->InjectMouseDown(evt.state.X.abs, evt.state.Y.abs, static_cast<int>(btn)))
		return true;
#endif
	return false;
}

bool GameManager::OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = m_pClientManager != nullptr ? m_pClientManager->getFairyGuiSystem() : nullptr;
	if (system != nullptr && system->InjectMouseUp(evt.state.X.abs, evt.state.Y.abs, static_cast<int>(btn)))
		return true;
#endif
	return false;
}

long long GameManager::getTimeInMillis()
{
	return m_SimulationTime;
}

Ogre::Real GameManager::getTimeInSeconds()
{
	return (Ogre::Real)(getTimeInMillis() / 1000);
}
