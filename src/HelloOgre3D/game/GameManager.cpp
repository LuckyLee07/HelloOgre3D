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
#include "ui/fairygui/lua_bridge/FairyGuiLuaApi.h"
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
#include "diagnostics/RuntimeResourceDiagnostics.h"

using namespace Ogre;

extern int tolua_SandboxToLua_open(lua_State* tolua_S);
extern int tolua_GameToLua_open(lua_State* tolua_S);
extern int tolua_RuntimeToLua_open(lua_State* tolua_S);
extern int tolua_SandboxToLua_Manual(lua_State* tolua_S);

GameManager* g_GameManager = nullptr;
GameManager* GetGameManager()
{
	return g_GameManager;
}

namespace
{
FairyGuiSystem* ResolveFairyGuiSystem(ClientManager* clientManager)
{
#if defined(HELLO_ENABLE_FGUI)
	return clientManager != nullptr ? clientManager->getFairyGuiSystem() : nullptr;
#else
	(void)clientManager;
	return nullptr;
#endif
}
}

GameManager::GameManager(ClientManager* pClientMgr)
	: m_pClientManager(pClientMgr), m_SimulationTime(0), m_pScriptVM(nullptr),
	m_pPhysicsWorld(nullptr), m_pSandboxMgr(nullptr), m_pObjectManager(nullptr), m_pUIManager(nullptr),
	m_pFairyGuiLuaApi(new FairyGuiLuaApi(ResolveFairyGuiSystem(pClientMgr)))
{
	
}

GameManager::~GameManager()
{
	SAFE_DELETE(m_pSandboxMgr);
	SAFE_DELETE(m_pObjectManager);
	SAFE_DELETE(m_pPhysicsWorld);
	SAFE_DELETE(m_pUIManager);
	SAFE_DELETE(m_pFairyGuiLuaApi);

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
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->IsAvailable();
}

const char* GameManager::loadFairyGuiPackage(const char* packagePath)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->LoadPackage(packagePath) : "";
}

bool GameManager::removeFairyGuiPackage(const char* packageName)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->RemovePackage(packageName);
}

int GameManager::createFairyGuiObject(const char* packageName, const char* objectName)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->CreateObject(packageName, objectName) : 0;
}

int GameManager::createFairyGuiContainer(const char* name)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->CreateContainer(name) : 0;
}

int GameManager::createFairyGuiChildContainer(int ownerHandle, const char* name)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->CreateChildContainer(ownerHandle, name) : 0;
}

int GameManager::createFairyGuiLoader(int ownerHandle, const char* name, const char* url)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->CreateLoader(ownerHandle, name, url) : 0;
}

int GameManager::createFairyGuiText(int ownerHandle, const char* name, const char* text, Ogre::Real fontSize, Ogre::Real red, Ogre::Real green, Ogre::Real blue)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->CreateText(ownerHandle, name, text, fontSize, red, green, blue) : 0;
}

int GameManager::createFairyGuiTextInput(int ownerHandle, const char* name, const char* text, Ogre::Real fontSize, Ogre::Real red, Ogre::Real green, Ogre::Real blue)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->CreateTextInput(ownerHandle, name, text, fontSize, red, green, blue) : 0;
}

int GameManager::createFairyGuiGraphRect(int ownerHandle, const char* name, Ogre::Real width, Ogre::Real height, Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->CreateGraphRect(ownerHandle, name, width, height, red, green, blue, alpha) : 0;
}

int GameManager::createFairyGuiGraphRegularPolygon(int ownerHandle, const char* name, Ogre::Real width, Ogre::Real height, int sides, Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->CreateGraphRegularPolygon(ownerHandle, name, width, height, sides, red, green, blue, alpha) : 0;
}

int GameManager::createFairyGuiModalMask(Ogre::Real red, Ogre::Real green, Ogre::Real blue, Ogre::Real alpha)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->CreateModalMask(red, green, blue, alpha) : 0;
}

int GameManager::getFairyGuiLastRenderCommandCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastRenderCommandCount() : 0;
}

int GameManager::getFairyGuiLastTriangleCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastTriangleCount() : 0;
}

int GameManager::getFairyGuiScreenWidth()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetScreenWidth() : 0;
}

int GameManager::getFairyGuiScreenHeight()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetScreenHeight() : 0;
}

int GameManager::getFairyGuiRuntimeObjectHandleCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetRuntimeObjectHandleCount() : 0;
}

int GameManager::getFairyGuiRuntimeListenerBindingCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetRuntimeListenerBindingCount() : 0;
}

int GameManager::getFairyGuiMaterialCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetMaterialCount() : 0;
}

int GameManager::getFairyGuiTextureCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetTextureCount() : 0;
}

int GameManager::getFairyGuiMaterialAliasCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetMaterialAliasCount() : 0;
}

int GameManager::getFairyGuiTextureAliasCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetTextureAliasCount() : 0;
}

int GameManager::getFairyGuiLastDrawCommandCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastDrawCommandCount() : 0;
}

int GameManager::getFairyGuiLastDrawTriangleCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastDrawTriangleCount() : 0;
}

int GameManager::getFairyGuiLastMaterialSwitchCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastMaterialSwitchCount() : 0;
}

int GameManager::getFairyGuiLastTextureSwitchCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastTextureSwitchCount() : 0;
}

int GameManager::getFairyGuiLastClippedCommandCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastClippedCommandCount() : 0;
}

int GameManager::getFairyGuiLastClippedTriangleCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastClippedTriangleCount() : 0;
}

int GameManager::getFairyGuiLastCulledCommandCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastCulledCommandCount() : 0;
}

int GameManager::getFairyGuiLastStencilCommandCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastStencilCommandCount() : 0;
}

int GameManager::getFairyGuiLastStencilTriangleCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastStencilTriangleCount() : 0;
}

int GameManager::getFairyGuiLastCpuClipSourceTriangleCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastCpuClipSourceTriangleCount() : 0;
}

int GameManager::getFairyGuiLastCpuClipOutputTriangleCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastCpuClipOutputTriangleCount() : 0;
}

int GameManager::getFairyGuiLastCpuClipFragmentCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastCpuClipFragmentCount() : 0;
}

int GameManager::getFairyGuiLastStencilClipScopeCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastStencilClipScopeCount() : 0;
}

int GameManager::getFairyGuiLastStencilClipPolygonCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastStencilClipPolygonCount() : 0;
}

int GameManager::getFairyGuiLastCustomCommandCount()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastCustomCommandCount() : 0;
}

int GameManager::getFairyGuiLastMaxBatchTriangles()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastMaxBatchTriangles() : 0;
}

int GameManager::getFairyGuiLastMaxBatchVertices()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetLastMaxBatchVertices() : 0;
}

bool GameManager::isFairyGuiHardwareStencilSupported()
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->IsHardwareStencilSupported();
}

const char* GameManager::getFairyGuiStencilBackendString()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetStencilBackendString() : "";
}

const char* GameManager::getFairyGuiStencilBackendDetailString()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetStencilBackendDetailString() : "";
}

const char* GameManager::getFairyGuiMaterialDetailString()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetMaterialDetailString() : "";
}

const char* GameManager::getFairyGuiTextureDetailString()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetTextureDetailString() : "";
}

const char* GameManager::getFairyGuiFrameRenderDetailString()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetFrameRenderDetailString() : "";
}

bool GameManager::plotFairyGuiServiceStats(int serviceOpenTotal, int serviceKindCount, int toastQueueCount, int loadingRefTotal, int serviceCreatedTotal, int serviceClosedTotal, int serviceFailedTotal, int servicePeakOpen)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->PlotServiceStats(serviceOpenTotal, serviceKindCount, toastQueueCount, loadingRefTotal, serviceCreatedTotal, serviceClosedTotal, serviceFailedTotal, servicePeakOpen);
}

int GameManager::getFairyGuiChild(int objectHandle, const char* childPath)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetChild(objectHandle, childPath) : 0;
}

int GameManager::getFairyGuiListItem(int objectHandle, int itemIndex)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetListItem(objectHandle, itemIndex) : 0;
}

int GameManager::getFairyGuiListItemCount(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetListItemCount(objectHandle) : 0;
}

bool GameManager::addFairyGuiObjectToRoot(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->AddObjectToRoot(objectHandle);
}

bool GameManager::addFairyGuiObjectToParent(int objectHandle, int parentHandle)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->AddObjectToParent(objectHandle, parentHandle);
}

bool GameManager::setFairyGuiObjectPosition(int objectHandle, Ogre::Real x, Ogre::Real y)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectPosition(objectHandle, x, y);
}

bool GameManager::setFairyGuiObjectSize(int objectHandle, Ogre::Real width, Ogre::Real height)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectSize(objectHandle, width, height);
}

bool GameManager::setFairyGuiObjectVisible(int objectHandle, bool visible)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectVisible(objectHandle, visible);
}

bool GameManager::setFairyGuiObjectAlpha(int objectHandle, Ogre::Real alpha)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectAlpha(objectHandle, alpha);
}

bool GameManager::setFairyGuiObjectTouchable(int objectHandle, bool touchable)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectTouchable(objectHandle, touchable);
}

bool GameManager::setFairyGuiObjectMask(int objectHandle, int maskHandle, bool inverted)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectMask(objectHandle, maskHandle, inverted);
}

bool GameManager::setFairyGuiObjectSortingOrder(int objectHandle, int sortingOrder)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectSortingOrder(objectHandle, sortingOrder);
}

bool GameManager::setFairyGuiObjectText(int objectHandle, const char* text)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectText(objectHandle, text);
}

const char* GameManager::getFairyGuiObjectText(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetObjectText(objectHandle) : "";
}

bool GameManager::setFairyGuiObjectIcon(int objectHandle, const char* icon)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectIcon(objectHandle, icon);
}

bool GameManager::setFairyGuiObjectLoaderUrl(int objectHandle, const char* url)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectLoaderUrl(objectHandle, url);
}

bool GameManager::setFairyGuiObjectControllerIndex(int objectHandle, const char* controllerName, int selectedIndex)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectControllerIndex(objectHandle, controllerName, selectedIndex);
}

int GameManager::getFairyGuiObjectControllerIndex(int objectHandle, const char* controllerName)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetObjectControllerIndex(objectHandle, controllerName) : -1;
}

bool GameManager::setFairyGuiObjectControllerPage(int objectHandle, const char* controllerName, const char* pageName)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectControllerPage(objectHandle, controllerName, pageName);
}

const char* GameManager::getFairyGuiObjectControllerPage(int objectHandle, const char* controllerName)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetObjectControllerPage(objectHandle, controllerName) : "";
}

const char* GameManager::getFairyGuiObjectControllerPageId(int objectHandle, const char* controllerName)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetObjectControllerPageId(objectHandle, controllerName) : "";
}

int GameManager::getFairyGuiObjectControllerPageCount(int objectHandle, const char* controllerName)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetObjectControllerPageCount(objectHandle, controllerName) : 0;
}

const char* GameManager::getFairyGuiObjectControllerPageNameAt(int objectHandle, const char* controllerName, int pageIndex)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetObjectControllerPageNameAt(objectHandle, controllerName, pageIndex) : "";
}

const char* GameManager::getFairyGuiObjectControllerPageIdAt(int objectHandle, const char* controllerName, int pageIndex)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetObjectControllerPageIdAt(objectHandle, controllerName, pageIndex) : "";
}

bool GameManager::setFairyGuiObjectValue(int objectHandle, Ogre::Real value)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectValue(objectHandle, value);
}

Ogre::Real GameManager::getFairyGuiObjectValue(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetObjectValue(objectHandle) : 0;
}

bool GameManager::setFairyGuiObjectMin(int objectHandle, Ogre::Real minValue)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectMin(objectHandle, minValue);
}

Ogre::Real GameManager::getFairyGuiObjectMin(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetObjectMin(objectHandle) : 0;
}

bool GameManager::setFairyGuiObjectMax(int objectHandle, Ogre::Real maxValue)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetObjectMax(objectHandle, maxValue);
}

Ogre::Real GameManager::getFairyGuiObjectMax(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetObjectMax(objectHandle) : 0;
}

bool GameManager::setFairyGuiComboBoxSelectedIndex(int objectHandle, int selectedIndex)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetComboBoxSelectedIndex(objectHandle, selectedIndex);
}

int GameManager::getFairyGuiComboBoxSelectedIndex(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetComboBoxSelectedIndex(objectHandle) : -1;
}

bool GameManager::setFairyGuiComboBoxValue(int objectHandle, const char* value)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetComboBoxValue(objectHandle, value);
}

const char* GameManager::getFairyGuiComboBoxValue(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetComboBoxValue(objectHandle) : "";
}

bool GameManager::playFairyGuiTransition(int objectHandle, const char* transitionName, int times, Ogre::Real delay, int callbackId)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->PlayTransition(objectHandle, transitionName, times, delay, callbackId);
}

bool GameManager::stopFairyGuiTransition(int objectHandle, const char* transitionName, bool setToComplete, bool processCallback)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->StopTransition(objectHandle, transitionName, setToComplete, processCallback);
}

bool GameManager::focusFairyGuiObject(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->FocusObject(objectHandle);
}

bool GameManager::clearFairyGuiFocus()
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->ClearFocus();
}

int GameManager::getFairyGuiFocusedObject()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetFocusedObject() : 0;
}

bool GameManager::setFairyGuiListItemCount(int objectHandle, int itemCount)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetListItemCount(objectHandle, itemCount);
}

bool GameManager::setFairyGuiListSelectedIndex(int objectHandle, int selectedIndex)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetListSelectedIndex(objectHandle, selectedIndex);
}

int GameManager::getFairyGuiListSelectedIndex(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetListSelectedIndex(objectHandle) : -1;
}

bool GameManager::setFairyGuiListVirtual(int objectHandle, bool loop)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->SetListVirtual(objectHandle, loop);
}

bool GameManager::refreshFairyGuiList(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->RefreshList(objectHandle);
}

bool GameManager::scrollFairyGuiListToView(int objectHandle, int itemIndex)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->ScrollListToView(objectHandle, itemIndex);
}

bool GameManager::centerFairyGuiObject(int objectHandle, bool restraint)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->CenterObject(objectHandle, restraint);
}

bool GameManager::injectFairyGuiMouseMove(int x, int y)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->InjectLogicalMouseMove(x, y);
}

bool GameManager::injectFairyGuiMouseDown(int x, int y, int button)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->InjectLogicalMouseDown(x, y, button);
}

bool GameManager::injectFairyGuiMouseUp(int x, int y, int button)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->InjectLogicalMouseUp(x, y, button);
}

bool GameManager::injectFairyGuiMouseWheel(int x, int y, int wheelDelta)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->InjectLogicalMouseWheel(x, y, wheelDelta);
}

bool GameManager::injectFairyGuiClick(int x, int y, int button)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->InjectLogicalClick(x, y, button);
}

bool GameManager::injectFairyGuiKeyPressed(int keyCode, int keyText)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->InjectKeyPressed(keyCode, keyText);
}

bool GameManager::injectFairyGuiKeyReleased(int keyCode, int keyText)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->InjectKeyReleased(keyCode, keyText);
}

bool GameManager::injectFairyGuiImeCompositionText(const char* text)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->InjectImeCompositionText(text);
}

bool GameManager::injectFairyGuiImeCommitText(const char* text)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->InjectImeCommitText(text);
}

bool GameManager::clearFairyGuiImeComposition()
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->ClearImeComposition();
}

const char* GameManager::getFairyGuiImeDebugString()
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->GetImeDebugString() : "";
}

int GameManager::addFairyGuiEventListener(int objectHandle, const char* childPath, int eventType, int callbackId)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->AddEventListener(objectHandle, childPath, eventType, callbackId) : 0;
}

int GameManager::addFairyGuiClickListener(int objectHandle, const char* childPath, int callbackId)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->AddClickListener(objectHandle, childPath, callbackId) : 0;
}

int GameManager::addFairyGuiControllerChangedListener(int objectHandle, const char* controllerName, int callbackId)
{
	return m_pFairyGuiLuaApi != nullptr ? m_pFairyGuiLuaApi->AddControllerChangedListener(objectHandle, controllerName, callbackId) : 0;
}

bool GameManager::removeFairyGuiListener(int bindingId)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->RemoveListener(bindingId);
}

bool GameManager::removeFairyGuiObject(int objectHandle)
{
	return m_pFairyGuiLuaApi != nullptr && m_pFairyGuiLuaApi->RemoveObject(objectHandle);
}

void GameManager::clearFairyGuiObjects()
{
	if (m_pFairyGuiLuaApi != nullptr)
		m_pFairyGuiLuaApi->ClearObjects();
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
	tolua_RuntimeToLua_open(m_pScriptVM->getLuaState());
	tolua_SandboxToLua_Manual(m_pScriptVM->getLuaState());

	// 设置lua可用的c++对象 
	m_pScriptVM->setUserTypePointer("Sandbox", "SandboxMgr", m_pSandboxMgr);
	m_pScriptVM->setUserTypePointer("GameManager", "GameManager", this);
	m_pScriptVM->setUserTypePointer("FairyGuiRuntime", "FairyGuiLuaApi", m_pFairyGuiLuaApi);
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
	FairyGuiSystem* fairyGuiSystem = ResolveFairyGuiSystem(m_pClientManager);
	if (fairyGuiSystem != nullptr && fairyGuiSystem->InjectKeyPressed(static_cast<int>(keycode), static_cast<int>(key)))
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
	FairyGuiSystem* fairyGuiSystem = ResolveFairyGuiSystem(m_pClientManager);
	if (fairyGuiSystem != nullptr && fairyGuiSystem->InjectKeyReleased(static_cast<int>(keycode), static_cast<int>(key)))
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
	FairyGuiSystem* fairyGuiSystem = ResolveFairyGuiSystem(m_pClientManager);
	if (fairyGuiSystem != nullptr)
	{
		if (evt.state.Z.rel != 0 && fairyGuiSystem->InjectMouseWheel(evt.state.X.abs, evt.state.Y.abs, evt.state.Z.rel))
			return true;
		if (fairyGuiSystem->InjectMouseMove(evt.state.X.abs, evt.state.Y.abs))
			return true;
	}
#endif
	return false;
}

bool GameManager::OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* fairyGuiSystem = ResolveFairyGuiSystem(m_pClientManager);
	if (fairyGuiSystem != nullptr && fairyGuiSystem->InjectMouseDown(evt.state.X.abs, evt.state.Y.abs, static_cast<int>(btn)))
		return true;
#endif
	return false;
}

bool GameManager::OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* fairyGuiSystem = ResolveFairyGuiSystem(m_pClientManager);
	if (fairyGuiSystem != nullptr && fairyGuiSystem->InjectMouseUp(evt.state.X.abs, evt.state.Y.abs, static_cast<int>(btn)))
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

std::string GameManager::buildRuntimeResourceDump(int maxEntriesPerType)
{
	return RuntimeResourceDiagnostics::BuildResourceDump(maxEntriesPerType);
}
