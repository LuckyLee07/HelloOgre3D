#include "InputManager.h"
#include "OISInputManager.h"
#include "ClientManager.h"
#include "GameManager.h"
#include "OgreRenderWindow.h"
#include "Samples/SdkCameraMan.h"

InputManager::InputManager() : m_pOISInputMgr(nullptr),
	m_pMouse(nullptr), m_pKeyboard(nullptr), m_windowHnd(0)
{
	Ogre::RenderWindow* pRenderWindow = GetClientMgr()->getRenderWindow();
	pRenderWindow->getCustomAttribute("WINDOW", &m_windowHnd);
}

InputManager::~InputManager()
{
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
	if (m_pMouse != nullptr) m_pMouse->capture();
	if (m_pKeyboard != nullptr) m_pKeyboard->capture();
}

void InputManager::closeWindow()
{
	m_pOISInputMgr->destroyInputObject(m_pMouse);
	m_pOISInputMgr->destroyInputObject(m_pKeyboard);
	m_pMouse = nullptr;
	m_pKeyboard = nullptr;

	OIS::InputManager::destroyInputSystem(m_pOISInputMgr);

	m_pOISInputMgr = nullptr;
}

void InputManager::resizeMouseState(int width, int height)
{
	const OIS::MouseState& mouseState = m_pMouse->getMouseState();
	mouseState.width = width;
	mouseState.height = height;
}

void InputManager::registerHandler(IInputHandler* handler)
{
	if (handler) m_inputHandlers.push_back(handler);
}

void InputManager::unregisterHandler(IInputHandler* handler)
{
	m_inputHandlers.erase(std::remove(m_inputHandlers.begin(), m_inputHandlers.end(), handler), m_inputHandlers.end());
}

bool InputManager::keyPressed(const OIS::KeyEvent& event)
{
	if (event.key == OIS::KC_ESCAPE)
		GetClientMgr()->SetShutdown(true);

	GetClientMgr()->getCameraMan()->injectKeyDown(event);

	for (auto* handler : m_inputHandlers)
		handler->OnKeyPressed(event.key, event.text);

	m_KeyMap[event.key] = true;
	m_KeyDownMap[event.key] = true;

	return true;
}

bool InputManager::keyReleased(const OIS::KeyEvent& event)
{
	GetClientMgr()->getCameraMan()->injectKeyUp(event);

	for (auto* handler : m_inputHandlers)
		handler->OnKeyReleased(event.key, event.text);

	m_KeyMap[event.key] = false;
	m_KeyUpMap[event.key] = true;

	return true;
}

bool InputManager::mouseMoved(const OIS::MouseEvent& event)
{
	if (event.state.buttonDown(OIS::MB_Right))
		GetClientMgr()->getCameraMan()->injectMouseMove(event);

	GetClientMgr()->SetWindowActive(true);

	for (auto* handler : m_inputHandlers)
		handler->OnMouseMoved(event);

	return true;
}

bool InputManager::mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID btnId)
{
	GetClientMgr()->getCameraMan()->injectMouseDown(event, btnId);
	
	for (auto* handler : m_inputHandlers)
		handler->OnMousePressed(event, btnId);
	
	return true;
}

bool InputManager::mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID btnId)
{
	GetClientMgr()->getCameraMan()->injectMouseUp(event, btnId);

	for (auto* handler : m_inputHandlers)
		handler->OnMouseReleased(event, btnId);
	
	return true;
}

bool InputManager::isKeyDown(OIS::KeyCode key) const
{
	auto iter = m_KeyMap.find(key);
	return iter != m_KeyMap.end() && iter->second;
}

bool InputManager::isKeyPressed(OIS::KeyCode key) const
{
	auto iter = m_KeyDownMap.find(key);
	return iter != m_KeyDownMap.end() && iter->second;
}

bool InputManager::isKeyReleased(OIS::KeyCode key) const
{
	auto iter = m_KeyUpMap.find(key);
	return iter != m_KeyUpMap.end() && iter->second;
}

void InputManager::update(float dtime)
{
	for (auto& kv : m_KeyDownMap)
		kv.second = false;

	for (auto& kv : m_KeyUpMap)
		kv.second = false;
}