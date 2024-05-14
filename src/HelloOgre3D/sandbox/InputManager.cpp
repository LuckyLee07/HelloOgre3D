#include "InputManager.h"
#include "OISInputManager.h"
#include "ClientManager.h"
#include "GameManager.h"
#include "OgreRenderWindow.h"
#include "Samples/SdkCameraMan.h"

InputManager::InputManager() : m_pGameManager(nullptr), 
	m_pMouse(nullptr), m_pKeyboard(nullptr), m_pOISInputMgr(nullptr), m_windowHnd(0)
{
	Ogre::RenderWindow* pRenderWindow = GetClientMgr()->getRenderWindow();
	pRenderWindow->getCustomAttribute("WINDOW", &m_windowHnd);
}

InputManager::~InputManager()
{
	m_pGameManager = nullptr;

	this->closeWindow();
}

void InputManager::Initialize()
{
	std::ostringstream windowHndStr;
	windowHndStr << m_windowHnd;

	OIS::ParamList oispl;
	oispl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
#if defined OIS_WIN32_PLATFORM
	oispl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
	oispl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
	oispl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
	oispl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM
	oispl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
	oispl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
	oispl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
	oispl.insert(std::make_pair(std::string("XAutorepeatOn"), std::string("true")));
#endif
	m_pOISInputMgr = OIS::InputManager::createInputSystem(oispl);

	m_pMouse = static_cast<OIS::Mouse*>(m_pOISInputMgr->createInputObject(OIS::OISMouse, true));
	m_pKeyboard = static_cast<OIS::Keyboard*>(m_pOISInputMgr->createInputObject(OIS::OISKeyboard, true));

	m_pMouse->setEventCallback(this);
	m_pKeyboard->setEventCallback(this);
}

void InputManager::capture()
{
	m_pMouse->capture();
	m_pKeyboard->capture();
}

void InputManager::closeWindow()
{
	m_pOISInputMgr->destroyInputObject(m_pMouse);
	m_pOISInputMgr->destroyInputObject(m_pKeyboard);

	OIS::InputManager::destroyInputSystem(m_pOISInputMgr);

	m_pOISInputMgr = nullptr;
}

void InputManager::resizeMouseState(int width, int height)
{
	const OIS::MouseState& mouseState = m_pMouse->getMouseState();
	mouseState.width = width;
	mouseState.height = height;
}

void InputManager::setGameManager(GameManager* pGameManager)
{
	m_pGameManager = pGameManager;
}

bool InputManager::keyPressed(const OIS::KeyEvent& event)
{
	GetClientMgr()->getCameraMan()->injectKeyDown(event);

	if (m_pGameManager != nullptr)
		m_pGameManager->HandleKeyPress(event.key, event.text);

	return true;
}

bool InputManager::keyReleased(const OIS::KeyEvent& event)
{
	GetClientMgr()->getCameraMan()->injectKeyUp(event);

	if (m_pGameManager != nullptr)
		m_pGameManager->HandleKeyRelease(event.key, event.text);

	return true;
}

bool InputManager::mouseMoved(const OIS::MouseEvent& event)
{
	if (event.state.buttonDown(OIS::MB_Right))
	{
		GetClientMgr()->getCameraMan()->injectMouseMove(event);
	}

	GetClientMgr()->SetWindowActive(true);

	if (m_pGameManager != nullptr)
		m_pGameManager->HandleMouseMove(event.state.width, event.state.height);

	return true;
}

bool InputManager::mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID btnId)
{
	GetClientMgr()->getCameraMan()->injectMouseDown(event, btnId);
	
	if (m_pGameManager != nullptr)
		m_pGameManager->HandleMousePress(event.state.width, event.state.height, btnId);
	
	return true;
}

bool InputManager::mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID btnId)
{
	GetClientMgr()->getCameraMan()->injectMouseUp(event, btnId);

	if (m_pGameManager != nullptr)
		m_pGameManager->HandleMouseRelease(event.state.width, event.state.height, btnId);
	
	return true;
}
