#include "ui/fairygui/FairyGuiSystem.h"

#include "cocos2d.h"
#include "GObject.h"
#include "GRoot.h"
#include "UIPackage.h"

#include "OgreRenderWindow.h"

FairyGuiSystem::FairyGuiSystem()
	: m_pRenderWindow(nullptr), m_pSceneManager(nullptr), m_pScene(nullptr), m_pRoot(nullptr), m_initialized(false),
	m_screenWidth(0), m_screenHeight(0)
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
	HandleWindowResized(width, height);

	m_pScene = cocos2d::Scene::create();
	m_pRoot = fairygui::GRoot::create(m_pScene);
	m_initialized = (m_pScene != nullptr && m_pRoot != nullptr);
	if (m_initialized)
		m_pRoot->setSize((float)m_screenWidth, (float)m_screenHeight);
	return m_initialized;
}

void FairyGuiSystem::Shutdown()
{
	if (m_pScene != nullptr)
		m_pScene->removeAllChildren();

	delete m_pRoot;
	delete m_pScene;
	m_pRoot = nullptr;
	m_pScene = nullptr;
	m_initialized = false;
	m_pRenderWindow = nullptr;
	m_pSceneManager = nullptr;
	m_screenWidth = 0;
	m_screenHeight = 0;
}

void FairyGuiSystem::Update(float deltaSeconds)
{
	if (!m_initialized)
		return;

	cocos2d::Director::getInstance()->mainLoop(deltaSeconds);
}

void FairyGuiSystem::Render()
{
	if (!m_initialized)
		return;

	if (m_pScene != nullptr)
		m_pScene->visit(cocos2d::Director::getInstance()->getRenderer(), cocos2d::Mat4::IDENTITY, 0);
}

void FairyGuiSystem::HandleWindowResized(unsigned int width, unsigned int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
	cocos2d::Director::getInstance()->getOpenGLView()->setFrameSize((float)width, (float)height);
	if (m_pRoot != nullptr)
		m_pRoot->setSize((float)width, (float)height);
}

bool FairyGuiSystem::LoadPackage(const std::string& packagePath)
{
	if (!m_initialized || packagePath.empty())
		return false;

	return fairygui::UIPackage::addPackage(packagePath) != nullptr;
}

fairygui::GObject* FairyGuiSystem::CreateObject(const std::string& packageName, const std::string& objectName)
{
	if (!m_initialized || packageName.empty() || objectName.empty())
		return nullptr;

	return fairygui::UIPackage::createObject(packageName, objectName);
}

bool FairyGuiSystem::AddToRoot(fairygui::GObject* object)
{
	if (!m_initialized || m_pRoot == nullptr || object == nullptr)
		return false;

	m_pRoot->addChild(object);
	return true;
}
