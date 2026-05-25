#include "ui/fairygui/FairyGuiSystemInternal.h"

FairyGuiSystem::FairyGuiSystem()
	: m_impl(new FairyGuiSystemImpl())
{
}

FairyGuiSystem::~FairyGuiSystem()
{
	delete m_impl;
	m_impl = nullptr;
}

bool FairyGuiSystem::Initialize(const FairyGuiSystemStartupContext& context)
{
	return m_impl != nullptr ? m_impl->Initialize(static_cast<Ogre::RenderWindow*>(context.renderWindow), static_cast<Ogre::SceneManager*>(context.sceneManager)) : false;
}

void FairyGuiSystem::Shutdown()
{
	if (m_impl != nullptr)
		m_impl->Shutdown();
}

void FairyGuiSystem::Update(float deltaSeconds)
{
	if (m_impl != nullptr)
		m_impl->Update(deltaSeconds);
}

void FairyGuiSystem::Render()
{
	if (m_impl != nullptr)
		m_impl->Render();
}

void FairyGuiSystem::HandleWindowResized(unsigned int width, unsigned int height)
{
	if (m_impl != nullptr)
		m_impl->HandleWindowResized(width, height);
}

bool FairyGuiSystem::InjectMouseMove(int x, int y)
{
	return m_impl != nullptr ? m_impl->InjectMouseMove(x, y) : false;
}

bool FairyGuiSystem::InjectMouseDown(int x, int y, int button)
{
	return m_impl != nullptr ? m_impl->InjectMouseDown(x, y, button) : false;
}

bool FairyGuiSystem::InjectMouseUp(int x, int y, int button)
{
	return m_impl != nullptr ? m_impl->InjectMouseUp(x, y, button) : false;
}

bool FairyGuiSystem::InjectMouseWheel(int x, int y, int wheelDelta)
{
	return m_impl != nullptr ? m_impl->InjectMouseWheel(x, y, wheelDelta) : false;
}

bool FairyGuiSystem::InjectLogicalMouseMove(int x, int y)
{
	return m_impl != nullptr ? m_impl->InjectLogicalMouseMove(x, y) : false;
}

bool FairyGuiSystem::InjectLogicalMouseDown(int x, int y, int button)
{
	return m_impl != nullptr ? m_impl->InjectLogicalMouseDown(x, y, button) : false;
}

bool FairyGuiSystem::InjectLogicalMouseUp(int x, int y, int button)
{
	return m_impl != nullptr ? m_impl->InjectLogicalMouseUp(x, y, button) : false;
}

bool FairyGuiSystem::InjectLogicalMouseWheel(int x, int y, int wheelDelta)
{
	return m_impl != nullptr ? m_impl->InjectLogicalMouseWheel(x, y, wheelDelta) : false;
}

bool FairyGuiSystem::InjectKeyPressed(int keyCode, int keyText)
{
	return m_impl != nullptr ? m_impl->InjectKeyPressed(keyCode, keyText) : false;
}

bool FairyGuiSystem::InjectKeyReleased(int keyCode, int keyText)
{
	return m_impl != nullptr ? m_impl->InjectKeyReleased(keyCode, keyText) : false;
}

bool FairyGuiSystem::InjectImeCompositionText(const std::string& text)
{
	return m_impl != nullptr ? m_impl->InjectImeCompositionText(text) : false;
}

bool FairyGuiSystem::InjectImeCommitText(const std::string& text)
{
	return m_impl != nullptr ? m_impl->InjectImeCommitText(text) : false;
}

bool FairyGuiSystem::ClearImeCompositionText()
{
	return m_impl != nullptr ? m_impl->ClearImeCompositionText() : false;
}

bool FairyGuiSystem::HandleNativeImeMessage(unsigned int message, unsigned long long wParam, long long lParam, long long& result)
{
	return m_impl != nullptr ? m_impl->HandleNativeImeMessage(message, wParam, lParam, result) : false;
}

bool FairyGuiSystem::LoadPackage(const std::string& packagePath)
{
	return m_impl != nullptr ? m_impl->LoadPackage(packagePath) : false;
}

std::string FairyGuiSystem::LoadPackageAndGetName(const std::string& packagePath)
{
	return m_impl != nullptr ? m_impl->LoadPackageAndGetName(packagePath) : std::string();
}

bool FairyGuiSystem::RemovePackage(const std::string& packageName)
{
	return m_impl != nullptr ? m_impl->RemovePackage(packageName) : false;
}

int FairyGuiSystem::CreateObjectHandle(const std::string& packageName, const std::string& objectName)
{
	return m_impl != nullptr ? m_impl->CreateObjectHandle(packageName, objectName) : 0;
}

int FairyGuiSystem::CreateContainerHandle(const std::string& name)
{
	return m_impl != nullptr ? m_impl->CreateContainerHandle(name) : 0;
}

int FairyGuiSystem::CreateChildContainerHandle(int ownerHandle, const std::string& name)
{
	return m_impl != nullptr ? m_impl->CreateChildContainerHandle(ownerHandle, name) : 0;
}

int FairyGuiSystem::CreateLoaderHandle(int ownerHandle, const std::string& name, const std::string& url)
{
	return m_impl != nullptr ? m_impl->CreateLoaderHandle(ownerHandle, name, url) : 0;
}

int FairyGuiSystem::CreateTextHandle(int ownerHandle, const std::string& name, const std::string& text, float fontSize, float red, float green, float blue)
{
	return m_impl != nullptr ? m_impl->CreateTextHandle(ownerHandle, name, text, fontSize, red, green, blue) : 0;
}

int FairyGuiSystem::CreateTextInputHandle(int ownerHandle, const std::string& name, const std::string& text, float fontSize, float red, float green, float blue)
{
	return m_impl != nullptr ? m_impl->CreateTextInputHandle(ownerHandle, name, text, fontSize, red, green, blue) : 0;
}

int FairyGuiSystem::CreateGraphRectHandle(int ownerHandle, const std::string& name, float width, float height, float red, float green, float blue, float alpha)
{
	return m_impl != nullptr ? m_impl->CreateGraphRectHandle(ownerHandle, name, width, height, red, green, blue, alpha) : 0;
}

int FairyGuiSystem::CreateGraphRegularPolygonHandle(int ownerHandle, const std::string& name, float width, float height, int sides, float red, float green, float blue, float alpha)
{
	return m_impl != nullptr ? m_impl->CreateGraphRegularPolygonHandle(ownerHandle, name, width, height, sides, red, green, blue, alpha) : 0;
}

int FairyGuiSystem::CreateModalMaskHandle(float red, float green, float blue, float alpha)
{
	return m_impl != nullptr ? m_impl->CreateModalMaskHandle(red, green, blue, alpha) : 0;
}

int FairyGuiSystem::GetObjectHandleChild(int objectHandle, const std::string& childPath)
{
	return m_impl != nullptr ? m_impl->GetObjectHandleChild(objectHandle, childPath) : 0;
}

int FairyGuiSystem::GetObjectHandleListItem(int objectHandle, int itemIndex)
{
	return m_impl != nullptr ? m_impl->GetObjectHandleListItem(objectHandle, itemIndex) : 0;
}

int FairyGuiSystem::GetObjectHandleListItemCount(int objectHandle)
{
	return m_impl != nullptr ? m_impl->GetObjectHandleListItemCount(objectHandle) : 0;
}

bool FairyGuiSystem::AddObjectHandleToRoot(int objectHandle)
{
	return m_impl != nullptr ? m_impl->AddObjectHandleToRoot(objectHandle) : false;
}

bool FairyGuiSystem::AddObjectHandleToParent(int objectHandle, int parentHandle)
{
	return m_impl != nullptr ? m_impl->AddObjectHandleToParent(objectHandle, parentHandle) : false;
}

bool FairyGuiSystem::SetObjectHandlePosition(int objectHandle, float x, float y)
{
	return m_impl != nullptr ? m_impl->SetObjectHandlePosition(objectHandle, x, y) : false;
}

bool FairyGuiSystem::SetObjectHandleSize(int objectHandle, float width, float height)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleSize(objectHandle, width, height) : false;
}

bool FairyGuiSystem::SetObjectHandleVisible(int objectHandle, bool visible)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleVisible(objectHandle, visible) : false;
}

bool FairyGuiSystem::SetObjectHandleAlpha(int objectHandle, float alpha)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleAlpha(objectHandle, alpha) : false;
}

bool FairyGuiSystem::SetObjectHandleTouchable(int objectHandle, bool touchable)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleTouchable(objectHandle, touchable) : false;
}

bool FairyGuiSystem::SetObjectHandleMask(int objectHandle, int maskHandle, bool inverted)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleMask(objectHandle, maskHandle, inverted) : false;
}

bool FairyGuiSystem::SetObjectHandleSortingOrder(int objectHandle, int sortingOrder)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleSortingOrder(objectHandle, sortingOrder) : false;
}

bool FairyGuiSystem::SetObjectHandleText(int objectHandle, const std::string& text)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleText(objectHandle, text) : false;
}

std::string FairyGuiSystem::GetObjectHandleText(int objectHandle) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleText(objectHandle) : std::string();
}

bool FairyGuiSystem::SetObjectHandleIcon(int objectHandle, const std::string& icon)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleIcon(objectHandle, icon) : false;
}

bool FairyGuiSystem::SetObjectHandleLoaderUrl(int objectHandle, const std::string& url)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleLoaderUrl(objectHandle, url) : false;
}

bool FairyGuiSystem::SetObjectHandleControllerIndex(int objectHandle, const std::string& controllerName, int selectedIndex)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleControllerIndex(objectHandle, controllerName, selectedIndex) : false;
}

int FairyGuiSystem::GetObjectHandleControllerIndex(int objectHandle, const std::string& controllerName) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleControllerIndex(objectHandle, controllerName) : 0;
}

bool FairyGuiSystem::SetObjectHandleControllerPage(int objectHandle, const std::string& controllerName, const std::string& pageName)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleControllerPage(objectHandle, controllerName, pageName) : false;
}

std::string FairyGuiSystem::GetObjectHandleControllerPage(int objectHandle, const std::string& controllerName) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleControllerPage(objectHandle, controllerName) : std::string();
}

std::string FairyGuiSystem::GetObjectHandleControllerPageId(int objectHandle, const std::string& controllerName) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleControllerPageId(objectHandle, controllerName) : std::string();
}

int FairyGuiSystem::GetObjectHandleControllerPageCount(int objectHandle, const std::string& controllerName) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleControllerPageCount(objectHandle, controllerName) : 0;
}

std::string FairyGuiSystem::GetObjectHandleControllerPageNameAt(int objectHandle, const std::string& controllerName, int pageIndex) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleControllerPageNameAt(objectHandle, controllerName, pageIndex) : std::string();
}

std::string FairyGuiSystem::GetObjectHandleControllerPageIdAt(int objectHandle, const std::string& controllerName, int pageIndex) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleControllerPageIdAt(objectHandle, controllerName, pageIndex) : std::string();
}

bool FairyGuiSystem::SetObjectHandleValue(int objectHandle, float value)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleValue(objectHandle, value) : false;
}

float FairyGuiSystem::GetObjectHandleValue(int objectHandle) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleValue(objectHandle) : 0.0f;
}

bool FairyGuiSystem::SetObjectHandleMin(int objectHandle, float minValue)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleMin(objectHandle, minValue) : false;
}

float FairyGuiSystem::GetObjectHandleMin(int objectHandle) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleMin(objectHandle) : 0.0f;
}

bool FairyGuiSystem::SetObjectHandleMax(int objectHandle, float maxValue)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleMax(objectHandle, maxValue) : false;
}

float FairyGuiSystem::GetObjectHandleMax(int objectHandle) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleMax(objectHandle) : 0.0f;
}

bool FairyGuiSystem::SetObjectHandleComboBoxSelectedIndex(int objectHandle, int selectedIndex)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleComboBoxSelectedIndex(objectHandle, selectedIndex) : false;
}

int FairyGuiSystem::GetObjectHandleComboBoxSelectedIndex(int objectHandle) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleComboBoxSelectedIndex(objectHandle) : 0;
}

bool FairyGuiSystem::SetObjectHandleComboBoxValue(int objectHandle, const std::string& value)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleComboBoxValue(objectHandle, value) : false;
}

std::string FairyGuiSystem::GetObjectHandleComboBoxValue(int objectHandle) const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleComboBoxValue(objectHandle) : std::string();
}

bool FairyGuiSystem::PlayObjectHandleTransition(int objectHandle, const std::string& transitionName, int times, float delay, int callbackId)
{
	return m_impl != nullptr ? m_impl->PlayObjectHandleTransition(objectHandle, transitionName, times, delay, callbackId) : false;
}

bool FairyGuiSystem::StopObjectHandleTransition(int objectHandle, const std::string& transitionName, bool setToComplete, bool processCallback)
{
	return m_impl != nullptr ? m_impl->StopObjectHandleTransition(objectHandle, transitionName, setToComplete, processCallback) : false;
}

bool FairyGuiSystem::SetObjectHandleFocus(int objectHandle)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleFocus(objectHandle) : false;
}

bool FairyGuiSystem::ClearObjectHandleFocus()
{
	return m_impl != nullptr ? m_impl->ClearObjectHandleFocus() : false;
}

int FairyGuiSystem::GetFocusedObjectHandle() const
{
	return m_impl != nullptr ? m_impl->GetFocusedObjectHandle() : 0;
}

bool FairyGuiSystem::SetObjectHandleListItemCount(int objectHandle, int itemCount)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleListItemCount(objectHandle, itemCount) : false;
}

bool FairyGuiSystem::SetObjectHandleListSelectedIndex(int objectHandle, int selectedIndex)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleListSelectedIndex(objectHandle, selectedIndex) : false;
}

int FairyGuiSystem::GetObjectHandleListSelectedIndex(int objectHandle)
{
	return m_impl != nullptr ? m_impl->GetObjectHandleListSelectedIndex(objectHandle) : 0;
}

bool FairyGuiSystem::ScrollObjectHandleListToView(int objectHandle, int itemIndex)
{
	return m_impl != nullptr ? m_impl->ScrollObjectHandleListToView(objectHandle, itemIndex) : false;
}

bool FairyGuiSystem::SetObjectHandleListVirtual(int objectHandle, bool loop)
{
	return m_impl != nullptr ? m_impl->SetObjectHandleListVirtual(objectHandle, loop) : false;
}

bool FairyGuiSystem::RefreshObjectHandleList(int objectHandle)
{
	return m_impl != nullptr ? m_impl->RefreshObjectHandleList(objectHandle) : false;
}

bool FairyGuiSystem::CenterObjectHandle(int objectHandle, bool restraint)
{
	return m_impl != nullptr ? m_impl->CenterObjectHandle(objectHandle, restraint) : false;
}

int FairyGuiSystem::AddObjectHandleEventListener(int objectHandle, const std::string& childPath, int eventType, int callbackId)
{
	return m_impl != nullptr ? m_impl->AddObjectHandleEventListener(objectHandle, childPath, eventType, callbackId) : 0;
}

int FairyGuiSystem::AddObjectHandleClickListener(int objectHandle, const std::string& childPath, int callbackId)
{
	return m_impl != nullptr ? m_impl->AddObjectHandleClickListener(objectHandle, childPath, callbackId) : 0;
}

int FairyGuiSystem::AddObjectHandleControllerChangedListener(int objectHandle, const std::string& controllerName, int callbackId)
{
	return m_impl != nullptr ? m_impl->AddObjectHandleControllerChangedListener(objectHandle, controllerName, callbackId) : 0;
}

bool FairyGuiSystem::RemoveObjectHandleListener(int bindingId)
{
	return m_impl != nullptr ? m_impl->RemoveObjectHandleListener(bindingId) : false;
}

bool FairyGuiSystem::RemoveObjectHandle(int objectHandle)
{
	return m_impl != nullptr ? m_impl->RemoveObjectHandle(objectHandle) : false;
}

void FairyGuiSystem::ClearObjectHandles()
{
	if (m_impl != nullptr)
		m_impl->ClearObjectHandles();
}

bool FairyGuiSystem::CreateSmokeTestImage(const std::string& imagePath)
{
	return m_impl != nullptr ? m_impl->CreateSmokeTestImage(imagePath) : false;
}

bool FairyGuiSystem::IsInitialized() const
{
	return m_impl != nullptr ? m_impl->IsInitialized() : false;
}

int FairyGuiSystem::GetLastRenderCommandCount() const
{
	return m_impl != nullptr ? m_impl->GetLastRenderCommandCount() : 0;
}

int FairyGuiSystem::GetLastTriangleCount() const
{
	return m_impl != nullptr ? m_impl->GetLastTriangleCount() : 0;
}

int FairyGuiSystem::GetScreenWidth() const
{
	return m_impl != nullptr ? m_impl->GetScreenWidth() : 0;
}

int FairyGuiSystem::GetScreenHeight() const
{
	return m_impl != nullptr ? m_impl->GetScreenHeight() : 0;
}

int FairyGuiSystem::GetObjectHandleCount() const
{
	return m_impl != nullptr ? m_impl->GetObjectHandleCount() : 0;
}

int FairyGuiSystem::GetListenerBindingCount() const
{
	return m_impl != nullptr ? m_impl->GetListenerBindingCount() : 0;
}

int FairyGuiSystem::GetMaterialCount() const
{
	return m_impl != nullptr ? m_impl->GetMaterialCount() : 0;
}

int FairyGuiSystem::GetTextureCount() const
{
	return m_impl != nullptr ? m_impl->GetTextureCount() : 0;
}

int FairyGuiSystem::GetMaterialAliasCount() const
{
	return m_impl != nullptr ? m_impl->GetMaterialAliasCount() : 0;
}

int FairyGuiSystem::GetTextureAliasCount() const
{
	return m_impl != nullptr ? m_impl->GetTextureAliasCount() : 0;
}

int FairyGuiSystem::GetLastDrawCommandCount() const
{
	return m_impl != nullptr ? m_impl->GetLastDrawCommandCount() : 0;
}

int FairyGuiSystem::GetLastDrawTriangleCount() const
{
	return m_impl != nullptr ? m_impl->GetLastDrawTriangleCount() : 0;
}

int FairyGuiSystem::GetLastMaterialSwitchCount() const
{
	return m_impl != nullptr ? m_impl->GetLastMaterialSwitchCount() : 0;
}

int FairyGuiSystem::GetLastTextureSwitchCount() const
{
	return m_impl != nullptr ? m_impl->GetLastTextureSwitchCount() : 0;
}

int FairyGuiSystem::GetLastClippedCommandCount() const
{
	return m_impl != nullptr ? m_impl->GetLastClippedCommandCount() : 0;
}

int FairyGuiSystem::GetLastClippedTriangleCount() const
{
	return m_impl != nullptr ? m_impl->GetLastClippedTriangleCount() : 0;
}

int FairyGuiSystem::GetLastCulledCommandCount() const
{
	return m_impl != nullptr ? m_impl->GetLastCulledCommandCount() : 0;
}

int FairyGuiSystem::GetLastStencilCommandCount() const
{
	return m_impl != nullptr ? m_impl->GetLastStencilCommandCount() : 0;
}

int FairyGuiSystem::GetLastStencilTriangleCount() const
{
	return m_impl != nullptr ? m_impl->GetLastStencilTriangleCount() : 0;
}

int FairyGuiSystem::GetLastCpuClipSourceTriangleCount() const
{
	return m_impl != nullptr ? m_impl->GetLastCpuClipSourceTriangleCount() : 0;
}

int FairyGuiSystem::GetLastCpuClipOutputTriangleCount() const
{
	return m_impl != nullptr ? m_impl->GetLastCpuClipOutputTriangleCount() : 0;
}

int FairyGuiSystem::GetLastCpuClipFragmentCount() const
{
	return m_impl != nullptr ? m_impl->GetLastCpuClipFragmentCount() : 0;
}

int FairyGuiSystem::GetLastStencilClipScopeCount() const
{
	return m_impl != nullptr ? m_impl->GetLastStencilClipScopeCount() : 0;
}

int FairyGuiSystem::GetLastStencilClipPolygonCount() const
{
	return m_impl != nullptr ? m_impl->GetLastStencilClipPolygonCount() : 0;
}

int FairyGuiSystem::GetLastCustomCommandCount() const
{
	return m_impl != nullptr ? m_impl->GetLastCustomCommandCount() : 0;
}

int FairyGuiSystem::GetLastMaxBatchTriangles() const
{
	return m_impl != nullptr ? m_impl->GetLastMaxBatchTriangles() : 0;
}

int FairyGuiSystem::GetLastMaxBatchVertices() const
{
	return m_impl != nullptr ? m_impl->GetLastMaxBatchVertices() : 0;
}

bool FairyGuiSystem::IsHardwareStencilSupported() const
{
	return m_impl != nullptr ? m_impl->IsHardwareStencilSupported() : false;
}

std::string FairyGuiSystem::GetStencilBackendString() const
{
	return m_impl != nullptr ? m_impl->GetStencilBackendString() : std::string();
}

std::string FairyGuiSystem::GetStencilBackendDetailString() const
{
	return m_impl != nullptr ? m_impl->GetStencilBackendDetailString() : std::string();
}

std::string FairyGuiSystem::GetMaterialDetailString() const
{
	return m_impl != nullptr ? m_impl->GetMaterialDetailString() : std::string();
}

std::string FairyGuiSystem::GetTextureDetailString() const
{
	return m_impl != nullptr ? m_impl->GetTextureDetailString() : std::string();
}

std::string FairyGuiSystem::GetFrameRenderDetailString() const
{
	return m_impl != nullptr ? m_impl->GetFrameRenderDetailString() : std::string();
}

std::string FairyGuiSystem::GetImeDebugString() const
{
	return m_impl != nullptr ? m_impl->GetImeDebugString() : std::string();
}

bool FairyGuiSystem::IsImeCompositionActive() const
{
	return m_impl != nullptr ? m_impl->IsImeCompositionActive() : false;
}

bool FairyGuiSystem::IsImeCandidateOpen() const
{
	return m_impl != nullptr ? m_impl->IsImeCandidateOpen() : false;
}

int FairyGuiSystem::GetImeCompositionUpdateCount() const
{
	return m_impl != nullptr ? m_impl->GetImeCompositionUpdateCount() : 0;
}

int FairyGuiSystem::GetImeCompositionCommitCount() const
{
	return m_impl != nullptr ? m_impl->GetImeCompositionCommitCount() : 0;
}

int FairyGuiSystem::GetImeCompositionEndCount() const
{
	return m_impl != nullptr ? m_impl->GetImeCompositionEndCount() : 0;
}

int FairyGuiSystem::GetImeCandidateOpenCount() const
{
	return m_impl != nullptr ? m_impl->GetImeCandidateOpenCount() : 0;
}

int FairyGuiSystem::GetImeCandidateCloseCount() const
{
	return m_impl != nullptr ? m_impl->GetImeCandidateCloseCount() : 0;
}

int FairyGuiSystem::GetImeCandidateCount() const
{
	return m_impl != nullptr ? m_impl->GetImeCandidateCount() : 0;
}

int FairyGuiSystem::GetImeCandidateSelection() const
{
	return m_impl != nullptr ? m_impl->GetImeCandidateSelection() : 0;
}

FairyGuiSystemImpl::FairyGuiSystemImpl()
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

FairyGuiSystemImpl::~FairyGuiSystemImpl()
{
	Shutdown();
}

bool FairyGuiSystemImpl::Initialize(Ogre::RenderWindow* renderWindow, Ogre::SceneManager* sceneManager)
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

void FairyGuiSystemImpl::Shutdown()
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

void FairyGuiSystemImpl::Update(float deltaSeconds)
{
	H3D_PROFILE_SCOPE("FairyGuiSystemImpl::Update");
	if (!m_initialized)
		return;

	cocos2d::Director::getInstance()->mainLoop(deltaSeconds);
	H3D_PROFILE_PLOT("FGUIObjectHandles", static_cast<double>(m_objectHandles.size()));
	H3D_PROFILE_PLOT("FGUIListenerBindings", static_cast<double>(m_listenerBindings.size()));
}

void FairyGuiSystemImpl::Render()
{
	H3D_PROFILE_SCOPE("FairyGuiSystemImpl::Render");
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

void FairyGuiSystemImpl::HandleWindowResized(unsigned int width, unsigned int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
	cocos2d::Director::getInstance()->getOpenGLView()->setFrameSize((float)width, (float)height);
	if (m_pRoot != nullptr)
		m_pRoot->setSize((float)width, (float)height);
	UpdateNativeImeCandidatePosition();
}
