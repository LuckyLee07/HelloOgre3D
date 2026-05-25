#include "ui/fairygui/FairyGuiSystemInternal.h"


FairyGuiSystem::FairyGuiSystem()
	: m_pRenderWindow(nullptr), m_pSceneManager(nullptr), m_pManualNode(nullptr), m_pManualObject(nullptr),
	m_pScene(nullptr), m_pRoot(nullptr), m_initialized(false), m_screenWidth(0), m_screenHeight(0),
	m_lastMouseX(0.0f), m_lastMouseY(0.0f), m_hasLastMousePosition(false), m_leftMouseDownOnUi(false),
	m_focusedObjectHandle(0),
	m_scissorEnabled(false), m_scissorRect(cocos2d::Rect::ZERO),
	m_stencilStage(cocos2d::STENCIL_STAGE_DISABLED), m_stencilDepth(0), m_stencilRevision(0),
	m_pendingStencilDepth(0), m_pendingStencilInverted(false), m_pendingStencilValid(false), m_pendingStencilRect(cocos2d::Rect::ZERO), m_pendingStencilPolygons(), m_stencilScopes(),
	m_lastRenderCommandCount(0), m_lastTriangleCount(0), m_materialCounter(0), m_nextObjectHandle(1), m_nextListenerBindingId(1),
	m_objectHandles(), m_listenerBindings(), m_textInputCarets(), m_materialNames(), m_textureNames(), m_materialNamesBySource(), m_textureNamesBySource(), m_textureDetailsBySource(),
	m_currentFrameStats(), m_lastFrameStats(), m_imeStats()
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	, m_nativeWindowHandle(nullptr), m_previousWindowProc(nullptr), m_nativeImeHookInstalled(false)
#endif
{
}

FairyGuiSystem::~FairyGuiSystem()
{
	Shutdown();
}

bool FairyGuiSystem::Initialize(Ogre::RenderWindow* renderWindow, Ogre::SceneManager* sceneManager)
{
	m_pRenderWindow = renderWindow;
	m_pSceneManager = sceneManager;
	if (m_pRenderWindow == nullptr || m_pSceneManager == nullptr)
		return false;

	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int colourDepth = 0;
	int left = 0;
	int top = 0;
	m_pRenderWindow->getMetrics(width, height, colourDepth, left, top);
	HandleWindowResized(ToLogicalWindowPixels(m_pRenderWindow, width), ToLogicalWindowPixels(m_pRenderWindow, height));

	m_pScene = cocos2d::Scene::create();
	m_pRoot = fairygui::GRoot::create(m_pScene);
	m_initialized = (m_pScene != nullptr && m_pRoot != nullptr);
	if (m_initialized)
	{
		m_pRoot->setSize((float)m_screenWidth, (float)m_screenHeight);
		m_pManualNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
		m_pManualObject = m_pSceneManager->createManualObject();
		m_pManualObject->setDynamic(true);
		m_pManualObject->setCastShadows(false);
		m_pManualObject->setUseIdentityProjection(true);
		m_pManualObject->setUseIdentityView(true);
		m_pManualObject->setKeepDeclarationOrder(true);
		m_pManualObject->setRenderQueueGroupAndPriority(Ogre::RENDER_QUEUE_OVERLAY, 100);
		m_pManualObject->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
		m_pManualNode->attachObject(m_pManualObject);
		m_pManualNode->_updateBounds();
		InstallNativeImeHook();
		if (!CreateConfiguredPackageObject() && IsEnvironmentEnabled("HELLO_FGUI_SMOKE_TEST"))
			CreateSmokeTestImage("media/fonts/dejavu/dejavu.png");
	}
	return m_initialized;
}

void FairyGuiSystem::Shutdown()
{
	RemoveNativeImeHook();
	ClearObjectHandles();

	if (m_pScene != nullptr)
		m_pScene->removeAllChildren();

	delete m_pRoot;
	delete m_pScene;
	DestroyOgreResources();
	m_pRoot = nullptr;
	m_pScene = nullptr;
	m_pManualObject = nullptr;
	m_pManualNode = nullptr;
	m_initialized = false;
	m_pRenderWindow = nullptr;
	m_pSceneManager = nullptr;
	m_screenWidth = 0;
	m_screenHeight = 0;
	m_lastMouseX = 0.0f;
	m_lastMouseY = 0.0f;
	m_hasLastMousePosition = false;
	m_leftMouseDownOnUi = false;
	m_focusedObjectHandle = 0;
	m_lastRenderCommandCount = 0;
	m_lastTriangleCount = 0;
	m_imeStats = ImeStats();
	ResetFrameRenderStats();
	m_lastFrameStats = FrameRenderStats();
	m_nextObjectHandle = 1;
	m_nextListenerBindingId = 1;
	m_textInputCarets.clear();
}

void FairyGuiSystem::Update(float deltaSeconds)
{
	H3D_PROFILE_SCOPE("FairyGuiSystem::Update");
	if (!m_initialized)
		return;

	cocos2d::Director::getInstance()->mainLoop(deltaSeconds);
	H3D_PROFILE_PLOT("FGUIObjectHandles", static_cast<double>(m_objectHandles.size()));
	H3D_PROFILE_PLOT("FGUIListenerBindings", static_cast<double>(m_listenerBindings.size()));
}

void FairyGuiSystem::Render()
{
	H3D_PROFILE_SCOPE("FairyGuiSystem::Render");
	if (!m_initialized)
		return;

	cocos2d::Renderer* renderer = cocos2d::Director::getInstance()->getRenderer();
	ResetFrameRenderStats();
	renderer->beginFrame();
	renderer->setCommandSink(this);
	BeginOgreRender();
	if (m_pScene != nullptr)
		m_pScene->visit(renderer, cocos2d::Mat4::IDENTITY, 0);
	EndOgreRender();
	renderer->setCommandSink(nullptr);
	m_lastRenderCommandCount = renderer->getTriangleCommandCount();
	m_lastTriangleCount = renderer->getSubmittedTriangleCount();
	FinalizeFrameRenderStats();
	H3D_PROFILE_PLOT("FGUIRenderCommands", static_cast<double>(m_lastRenderCommandCount));
	H3D_PROFILE_PLOT("FGUITriangles", static_cast<double>(m_lastTriangleCount));
	H3D_PROFILE_PLOT("FGUIDrawCommands", static_cast<double>(m_lastFrameStats.drawCommandCount));
	H3D_PROFILE_PLOT("FGUIDrawTriangles", static_cast<double>(m_lastFrameStats.drawTriangleCount));
	H3D_PROFILE_PLOT("FGUIMaterialSwitches", static_cast<double>(m_lastFrameStats.materialSwitchCount));
	H3D_PROFILE_PLOT("FGUITextureSwitches", static_cast<double>(m_lastFrameStats.textureSwitchCount));
	H3D_PROFILE_PLOT("FGUIClippedCommands", static_cast<double>(m_lastFrameStats.clippedCommandCount));
	H3D_PROFILE_PLOT("FGUIStencilCommands", static_cast<double>(m_lastFrameStats.stencilCommandCount));
	H3D_PROFILE_PLOT("FGUICpuClipSourceTriangles", static_cast<double>(m_lastFrameStats.cpuClipSourceTriangleCount));
	H3D_PROFILE_PLOT("FGUICpuClipOutputTriangles", static_cast<double>(m_lastFrameStats.cpuClipOutputTriangleCount));
	H3D_PROFILE_PLOT("FGUIStencilClipPolygons", static_cast<double>(m_lastFrameStats.stencilClipPolygonCount));
	H3D_PROFILE_PLOT("FGUIMaxBatchTriangles", static_cast<double>(m_lastFrameStats.maxBatchTriangles));
}

void FairyGuiSystem::HandleWindowResized(unsigned int width, unsigned int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
	cocos2d::Director::getInstance()->getOpenGLView()->setFrameSize((float)width, (float)height);
	if (m_pRoot != nullptr)
		m_pRoot->setSize((float)width, (float)height);
	UpdateNativeImeCandidatePosition();
}
