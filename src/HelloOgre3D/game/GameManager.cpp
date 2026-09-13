#include "GameManager.h"
#include "Ogre.h"
#include "OgreDpiHelper.h"
#include "ScriptLuaVM.h"
#include "tolua++.h"
#include "LuaInterface.h"
#include <algorithm>
#include <cstdlib>
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
#include "systems/input/InputManager.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/ObjectFactory.h"
#include "systems/service/AgentConfigService.h"
#include "systems/service/CameraService.h"
#include "systems/service/NavigationService.h"
#include "systems/service/RaycastService.h"
#include "systems/service/SceneService.h"
#include "systems/service/ScriptService.h"
#include "ai/common/AIScheduler.h"
#include "ai/tactics/TacticalService.h"
#include "ai/team/TeamBlackboardService.h"
#include "debug/DebugDrawer.h"
#include "systems/physics/PhysicsWorld.h"
#include "systems/service/SceneFactory.h"
#include "core/SandboxMacros.h"
#include "core/SandboxServices.h"
#include "profiling/Profile.h"
#include "profiling/RuntimeProfileCounters.h"
#include "diagnostics/RuntimeResourceDiagnostics.h"
#include "diagnostics/InputReplay.h"
#include "audio/RuntimeUiSound.h"
#include "components/control/PlayerController.h"
#include "components/anim/AnimComponent.h"
#include "objects/AgentObject.h"
#include "components/render/RenderComponent.h"
#include "components/combat/WeaponComponent.h"

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
	: m_pClientManager(pClientMgr), m_pScriptVM(nullptr), m_pPhysicsWorld(nullptr),
	m_pCameraService(nullptr), m_pAgentConfigService(nullptr), m_pNavigationService(nullptr), m_pRaycastService(nullptr), m_pSceneService(nullptr), m_pScriptService(nullptr), m_pUIManager(nullptr), m_pObjectManager(nullptr), m_pObjectFactory(nullptr), m_simulationPaused(false), m_SimulationTime(0),
	m_pFairyGuiLuaApi(new FairyGuiLuaApi(ResolveFairyGuiSystem(pClientMgr))), m_pUiSound(new RuntimeUiSound())
{
	
}

GameManager::~GameManager()
{
	if (getInputManager() != nullptr) getInputManager()->SetGameplayMouseLook(false);
	SAFE_DELETE(m_pObjectManager);
	SceneFactory::SetRootSceneNode(nullptr);

	SAFE_DELETE(m_pObjectFactory);
	SAFE_DELETE(m_pAgentConfigService);
	SAFE_DELETE(m_pNavigationService);
	SAFE_DELETE(m_pRaycastService);
	SAFE_DELETE(m_pSceneService);
	SAFE_DELETE(m_pScriptService);
	SAFE_DELETE(m_pPhysicsWorld);
	SAFE_DELETE(m_pUIManager);
	SAFE_DELETE(m_pCameraService);
	SAFE_DELETE(m_pFairyGuiLuaApi);
	SAFE_DELETE(m_pUiSound);

	m_pClientManager = nullptr;
}

GameManager* GameManager::GetInstance()
{
	return g_GameManager;
}

void GameManager::Initialize()
{
	m_pScriptVM = GetScriptLuaVM();

	Ogre::Camera* uiCamera = m_pClientManager != nullptr ? m_pClientManager->getCamera() : nullptr;
	m_pUIManager = new UIManager(uiCamera);
	m_pUIManager->InitConfig();

	m_pPhysicsWorld = new PhysicsWorld();
	m_pPhysicsWorld->initilize();

	m_pObjectManager = new ObjectManager(m_pPhysicsWorld);
	m_pObjectFactory = new ObjectFactory(m_pObjectManager);
	m_pAgentConfigService = new AgentConfigService();
	m_pNavigationService = new NavigationService(m_pObjectManager);
	m_pRaycastService = new RaycastService(m_pPhysicsWorld);
	m_pScriptService = new ScriptService(m_pScriptVM);

	Ogre::SceneManager* pSceneManager = m_pClientManager != nullptr ? m_pClientManager->getSceneManager() : nullptr;
	m_pCameraService = new CameraService(
		m_pClientManager != nullptr ? m_pClientManager->getCamera() : nullptr,
		pSceneManager,
		m_pClientManager != nullptr ? m_pClientManager->getCameraController() : nullptr,
		[this](CameraService::ProfileTimeKind kind) -> long long
		{
			if (m_pClientManager == nullptr)
				return 0;
			switch (kind)
			{
			case CameraService::PROFILE_RENDER_TIME:
				return m_pClientManager->GetProfileTime(ClientManager::P_RENDER_TIME);
			case CameraService::PROFILE_SIMULATE_TIME:
				return m_pClientManager->GetProfileTime(ClientManager::P_SIMULATE_TIME);
			case CameraService::PROFILE_TOTAL_SIMULATE_TIME:
				return m_pClientManager->GetProfileTime(ClientManager::P_TOTAL_SIMULATE_TIME);
			default:
				return 0;
		}
	});
	m_pCameraService->SetRaycastService(m_pRaycastService);
	m_pSceneService = new SceneService(pSceneManager, m_pCameraService);
	SceneFactory::SetRootSceneNode(pSceneManager != nullptr ? pSceneManager->getRootSceneNode() : nullptr);

	SandboxServices services;
	services.objects = m_pObjectManager;
	services.physics = m_pPhysicsWorld;
	services.input = m_pClientManager != nullptr ? m_pClientManager->getInputManager() : nullptr;
	services.camera = m_pCameraService;
	services.script = m_pScriptVM;
	services.objectFactory = m_pObjectFactory;
	services.agentConfig = m_pAgentConfigService;
	services.navigation = m_pNavigationService;
	services.raycast = m_pRaycastService;
	services.scene = m_pSceneService;
	services.scriptService = m_pScriptService;
	m_pObjectManager->SetSandboxServices(services);

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
	m_pScriptVM->setUserTypePointer("SandboxAgentConfig", "AgentConfigService", m_pAgentConfigService);
	m_pScriptVM->setUserTypePointer("SandboxCamera", "CameraService", m_pCameraService);
	m_pScriptVM->setUserTypePointer("SandboxNav", "NavigationService", m_pNavigationService);
	m_pScriptVM->setUserTypePointer("SandboxRaycast", "RaycastService", m_pRaycastService);
	m_pScriptVM->setUserTypePointer("SandboxScene", "SceneService", m_pSceneService);
	m_pScriptVM->setUserTypePointer("SandboxScript", "ScriptService", m_pScriptService);
	m_pScriptVM->setUserTypePointer("SandboxAIScheduler", "AIScheduler", m_pObjectManager->GetAIScheduler());
	m_pScriptVM->setUserTypePointer("SandboxPerception", "AgentPerceptionSystem", m_pObjectManager->GetAgentPerceptionSystem());
	m_pScriptVM->setUserTypePointer("SandboxTeam", "TeamBlackboardService", m_pObjectManager->GetTeamBlackboardService());
	m_pScriptVM->setUserTypePointer("SandboxTactics", "TacticalService", m_pObjectManager->GetTacticalService());
	m_pScriptVM->setUserTypePointer("SandboxUI", "UIManager", m_pUIManager);
	m_pScriptVM->setUserTypePointer("SandboxObjects", "ObjectFactory", m_pObjectFactory);
	m_pScriptVM->setUserTypePointer("GameManager", "GameManager", this);
	m_pScriptVM->setUserTypePointer("FairyGuiRuntime", "FairyGuiLuaApi", m_pFairyGuiLuaApi);
	m_pScriptVM->setUserTypePointer("SandboxAudio", "RuntimeUiSound", m_pUiSound);
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
	// Restore simulation poses before input/Lua/AI can request actions or resolve a muzzle.
	for (AgentObject* agent : m_pObjectManager->getAllAgents())
		if (agent != nullptr && agent->GetAnimComponent() != nullptr)
			agent->GetAnimComponent()->BeginSimulationPose();
	if (InputReplay::Update(*this, m_pObjectManager, getInputManager(), deltaMilliseconds))
		return;
	H3D_PROFILE_SCOPE("GameManager::UpdateStages");
	const bool perfEnabled = RuntimeStallProfiler::IsEnabled();
	RuntimeGameUpdateTiming perfTiming;
	long long stageStartMicros = 0;
	bool simulationAdvanced = false;
	if (!m_simulationPaused)
	{
		H3D_PROFILE_SCOPE("Lua::__tick__");
		if (perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		m_pScriptVM->callFunction("__tick__", "i", deltaMilliseconds);
		if (perfEnabled)
		{
			perfTiming.luaTickMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
			RuntimeStallProfiler::AddLuaCallbackTiming(perfTiming.luaTickMs);
		}
	}

	if (!m_simulationPaused)
	{
		simulationAdvanced = true;
		m_SimulationTime += deltaMilliseconds;

		{
			H3D_PROFILE_SCOPE("ObjectManager::Update");
			if (perfEnabled)
				stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
			m_pObjectManager->SetCurrentTimeMs(m_SimulationTime);
			m_pObjectManager->Update(deltaMilliseconds);
			if (perfEnabled)
				perfTiming.objectManagerMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
		}

		{
			H3D_PROFILE_SCOPE("PhysicsWorld::Step");
			if (perfEnabled)
				stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
			m_pPhysicsWorld->stepWorld(deltaMilliseconds / 1000.0f);
			if (perfEnabled)
				perfTiming.physicsMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
		}
	}

	{
		H3D_PROFILE_SCOPE("Lua::Sandbox_Update");
		if (perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		m_pScriptVM->callFunction("Sandbox_Update", "i", m_simulationPaused ? 0 : deltaMilliseconds);
		if (perfEnabled)
		{
			perfTiming.sandboxLuaMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
			RuntimeStallProfiler::AddLuaCallbackTiming(perfTiming.sandboxLuaMs);
		}
	}
	if (simulationAdvanced)
	{
		for (AgentObject* agent : m_pObjectManager->getAllAgents())
		{
			if (agent == nullptr) continue;
			if (agent->GetRenderComponent() != nullptr) agent->GetRenderComponent()->CaptureSimulationTransform();
			if (agent->GetAnimComponent() != nullptr) agent->GetAnimComponent()->CaptureSimulationPose(deltaMilliseconds);
		}
	}
	if (perfEnabled)
		RuntimeStallProfiler::SetGameUpdateTiming(perfTiming);
}

void GameManager::RenderPresentation(float alpha, float dtSec)
{
	UpdateMouseLookState();
	if (m_pObjectManager == nullptr || m_pCameraService == nullptr) return;
	H3D_PROFILE_SCOPE("GameManager::RenderPresentation");
	for (AgentObject* agent : m_pObjectManager->getAllAgents())
	{
		if (agent == nullptr) continue;
		RenderComponent* render = agent->GetRenderComponent();
		if (m_simulationPaused)
		{
			if (render != nullptr) render->FreezeInterpolation();
			if (agent->GetAnimComponent() != nullptr) agent->GetAnimComponent()->FreezePresentation();
		}
		if (render != nullptr) render->RenderInterpolated(m_simulationPaused ? 1.0f : alpha);
		if (agent->GetAnimComponent() != nullptr) agent->GetAnimComponent()->RenderPresentation(m_simulationPaused ? 1.0f : alpha);
		WeaponComponent* weapon = agent->GetWeaponComponent();
		if (weapon != nullptr) weapon->SyncToHandBone();
		if (render != nullptr) render->SyncOwnedBoneAttachments();
		if (m_pCameraService->IsFollowing() && agent->FindComponent<PlayerController>() != nullptr)
		{
			const Ogre::Vector3 displayed = render != nullptr
				? render->GetDerivedPosition() - render->GetVisualOffset() : agent->GetPosition();
			m_pCameraService->RenderFollow(displayed, m_simulationPaused ? 0.0f : dtSec);
			static const bool trace = std::getenv("HELLO_CAMERA_TRACE") != nullptr;
			if (trace)
				Ogre::LogManager::getSingleton().logMessage("[PresentationTrace] simMs=" + std::to_string(m_SimulationTime)
					+ " alpha=" + Ogre::StringConverter::toString(alpha) + " paused=" + Ogre::StringConverter::toString(m_simulationPaused)
					+ " physics=" + Ogre::StringConverter::toString(agent->GetPosition())
					+ " displayed=" + Ogre::StringConverter::toString(displayed)
					+ " bodyForward=" + Ogre::StringConverter::toString(agent->GetForward())
					+ " displayedForward=" + Ogre::StringConverter::toString(render != nullptr
						? render->GetOrientation().zAxis() : agent->GetForward()));
		}
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
	if (getInputManager() != nullptr) getInputManager()->SetGameplayMouseLook(false);
	//m_pScriptVM->callFunction("EventHandle_WindowClosed", "");
}

void GameManager::UpdateMouseLookState()
{
	InputManager* input = getInputManager();
	if (input == nullptr) return;
	const bool tacticalCursor = input->isKeyDown(OIS::KC_LMENU) || input->isKeyDown(OIS::KC_RMENU);
	input->SetGameplayMouseLook(!m_simulationPaused && !tacticalCursor
		&& m_pCameraService != nullptr && m_pCameraService->IsCameraRelativeMovement()
		&& m_pCameraService->IsFollowing());
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
	if (keycode == OIS::KC_LMENU || keycode == OIS::KC_RMENU)
	{
		InputManager* input = getInputManager();
		if (input != nullptr) input->SetGameplayMouseLook(false);
	}
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* fairyGuiSystem = ResolveFairyGuiSystem(m_pClientManager);
	if (fairyGuiSystem != nullptr && fairyGuiSystem->InjectKeyPressed(static_cast<int>(keycode), static_cast<int>(key)))
		return true;
#endif

	bool consumed = false;
	m_pScriptVM->callFunction("FairyGuiManager_HandleKeyPressed", "ii>b", keycode, static_cast<int>(key), &consumed);
	if (consumed)
		return true;

	m_pScriptVM->callFunction("EventHandle_Keyboard", "ib>B", keycode, true, &consumed);
	return consumed || m_simulationPaused;
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

	m_pScriptVM->callFunction("EventHandle_Keyboard", "ib>B", keycode, false, &consumed);

	if (!consumed && !m_simulationPaused)
		m_pObjectManager->HandleKeyEvent(keycode, key);
	return consumed || m_simulationPaused;
}

bool GameManager::OnMouseMoved(const OIS::MouseEvent& evt)
{
	UpdateMouseLookState();
	InputManager* input = getInputManager();
	if (input != nullptr && input->IsGameplayMouseLookActive())
	{
		m_pCameraService->RotateFollowView(static_cast<float>(evt.state.X.rel), static_cast<float>(evt.state.Y.rel));
		if (evt.state.Z.rel != 0)
			m_pCameraService->ZoomFollowCamera(-static_cast<float>(evt.state.Z.rel) / 120.0f);
		return true;
	}
	if (!m_simulationPaused && m_pCameraService != nullptr && m_pCameraService->IsFollowOrbiting())
	{
		m_pCameraService->DragFollowOrbit(static_cast<float>(evt.state.X.rel), static_cast<float>(evt.state.Y.rel));
		return true;
	}
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
	// 派发给 sample：ctype 0=move / 1=down / 2=up，button 沿用 OIS 数值（左 0 / 右 1），move 无按钮传 -1。
	// FGUI 消费掉的事件已在上面 return，保持 UI 优先于 sample 的既有次序。
	bool consumed = false;
	m_pScriptVM->callFunction("EventHandle_Mouse", "iiii>B", 0, evt.state.X.abs, evt.state.Y.abs, -1, &consumed);
	if (!consumed && !m_simulationPaused && evt.state.Z.rel != 0 && m_pCameraService != nullptr)
		consumed = m_pCameraService->ZoomFollowCamera(-static_cast<float>(evt.state.Z.rel) / 120.0f);
	return consumed || m_simulationPaused;
}

bool GameManager::OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
	UpdateMouseLookState();
	InputManager* input = getInputManager();
	if (input != nullptr && input->IsGameplayMouseLookActive())
		return btn != OIS::MB_Left;
	if (m_pCameraService != nullptr && m_pCameraService->IsFollowOrbiting()) return true;
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* fairyGuiSystem = ResolveFairyGuiSystem(m_pClientManager);
	if (fairyGuiSystem != nullptr && fairyGuiSystem->InjectMouseDown(evt.state.X.abs, evt.state.Y.abs, static_cast<int>(btn)))
		return true;
#endif
	bool consumed = false;
	m_pScriptVM->callFunction("EventHandle_Mouse", "iiii>B", 1, evt.state.X.abs, evt.state.Y.abs, static_cast<int>(btn), &consumed);
	if (!consumed && !m_simulationPaused && btn == OIS::MB_Middle && m_pCameraService != nullptr)
		consumed = m_pCameraService->BeginFollowOrbit();
	return consumed || m_simulationPaused;
}

bool GameManager::OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn)
{
	UpdateMouseLookState();
	InputManager* input = getInputManager();
	if (input != nullptr && input->IsGameplayMouseLookActive())
		return btn != OIS::MB_Left;
	if (btn == OIS::MB_Middle && m_pCameraService != nullptr && m_pCameraService->IsFollowOrbiting())
	{
		m_pCameraService->EndFollowOrbit();
		return true;
	}
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* fairyGuiSystem = ResolveFairyGuiSystem(m_pClientManager);
	if (fairyGuiSystem != nullptr && fairyGuiSystem->InjectMouseUp(evt.state.X.abs, evt.state.Y.abs, static_cast<int>(btn)))
		return true;
#endif
	bool consumed = false;
	m_pScriptVM->callFunction("EventHandle_Mouse", "iiii>B", 2, evt.state.X.abs, evt.state.Y.abs, static_cast<int>(btn), &consumed);
	return consumed || m_simulationPaused;
}

void GameManager::SetSimulationPaused(bool paused)
{
	if (paused && !m_simulationPaused && m_pObjectManager != nullptr)
	{
		for (AgentObject* agent : m_pObjectManager->getAllAgents())
		{
			PlayerController* player = agent != nullptr ? agent->FindComponent<PlayerController>() : nullptr;
			if (player != nullptr)
				player->ResetTransientInput();
		}
	}
	m_simulationPaused = paused;
	UpdateMouseLookState();
}

bool GameManager::IsSimulationPaused() const
{
	return m_simulationPaused;
}

void GameManager::RequestQuit()
{
	if (m_pClientManager != nullptr)
		m_pClientManager->SetShutdown(true);
}

bool GameManager::RequestWindowSize(int width, int height)
{
	return m_pClientManager != nullptr && width > 0 && height > 0
		&& m_pClientManager->RequestWindowSize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
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
