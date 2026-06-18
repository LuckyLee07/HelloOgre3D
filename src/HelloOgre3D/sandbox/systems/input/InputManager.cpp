#include "InputManager.h"
#include "OISInputManager.h"
#include "OgreRenderWindow.h"
#include "ogre/OgreCameraController.h"

namespace
{
	int ClampMouseCoordinate(int value, int maxValue)
	{
		if (maxValue <= 0)
			return value;
		if (value < 0)
			return 0;
		return value > maxValue ? maxValue : value;
	}
}

InputManager::InputManager(Ogre::RenderWindow* renderWindow, OgreCameraController* cameraController,
	const WindowStateSetter& shutdownSetter, const WindowStateSetter& windowActiveSetter)
	: m_windowHnd(0), m_renderWindow(renderWindow), m_cameraController(cameraController),
	m_shutdownSetter(shutdownSetter), m_windowActiveSetter(windowActiveSetter),
	m_pMouse(nullptr), m_pKeyboard(nullptr), m_pOISInputMgr(nullptr)
#if defined(OIS_APPLE_PLATFORM)
	, m_nativeMouseBridge(nullptr), m_nativeMouseState()
#endif
{
	if (m_renderWindow != nullptr)
		m_renderWindow->getCustomAttribute("WINDOW", &m_windowHnd);
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
#elif defined OIS_APPLE_PLATFORM
	oispl.insert(std::make_pair(std::string("MacAutoRepeatOn"), std::string("true")));
#endif
	m_pOISInputMgr = OIS::InputManager::createInputSystem(oispl);

#if defined(OIS_APPLE_PLATFORM)
	// Cocoa mouse path in OIS 1.5 can stall on some runtime combinations.
	// Keep keyboard enabled for gameplay input (F-keys) and skip mouse temporarily.
	m_pMouse = nullptr;
#else
	m_pMouse = static_cast<OIS::Mouse*>(m_pOISInputMgr->createInputObject(OIS::OISMouse, true));
#endif
	m_pKeyboard = static_cast<OIS::Keyboard*>(m_pOISInputMgr->createInputObject(OIS::OISKeyboard, true));

	if (m_pMouse != nullptr)
		m_pMouse->setEventCallback(this);
	if (m_pKeyboard != nullptr)
		m_pKeyboard->setEventCallback(this);

	if (m_renderWindow != nullptr)
	{
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int colourDepth = 0;
		int left = 0;
		int top = 0;
		m_renderWindow->getMetrics(width, height, colourDepth, left, top);
		resizeMouseState(width, height);
	}

#if defined(OIS_APPLE_PLATFORM)
	InstallNativeMouseBridge();
#endif
}

void InputManager::capture()
{
	if (m_pMouse != nullptr)
		m_pMouse->capture();

	if (m_pKeyboard != nullptr)
		m_pKeyboard->capture();
}

void InputManager::closeWindow()
{
#if defined(OIS_APPLE_PLATFORM)
	UninstallNativeMouseBridge();
#endif

	if (m_pOISInputMgr == nullptr) return;

	if (m_pMouse != nullptr)
		m_pOISInputMgr->destroyInputObject(m_pMouse);
	if (m_pKeyboard != nullptr)
		m_pOISInputMgr->destroyInputObject(m_pKeyboard);
	m_pMouse = nullptr;
	m_pKeyboard = nullptr;

	OIS::InputManager::destroyInputSystem(m_pOISInputMgr);
	m_pOISInputMgr = nullptr;
	m_pMouse = nullptr;
	m_pKeyboard = nullptr;
}

void InputManager::resizeMouseState(int width, int height)
{
#if defined(OIS_APPLE_PLATFORM)
	m_nativeMouseState.width = width;
	m_nativeMouseState.height = height;
#endif
	if (m_pMouse == nullptr)
		return;
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
	bool consumed = false;
	for (auto* handler : m_inputHandlers)
		consumed = handler->OnKeyPressed(event.key, event.text) || consumed;

	if (!consumed && event.key == OIS::KC_ESCAPE && m_shutdownSetter)
		m_shutdownSetter(true);

	if (!consumed && m_cameraController != nullptr)
		m_cameraController->injectKeyDown(event);

	m_KeyMap[event.key] = true;
	m_KeyDownMap[event.key] = true;

	return true;
}

bool InputManager::keyReleased(const OIS::KeyEvent& event)
{
	bool consumed = false;
	for (auto* handler : m_inputHandlers)
		consumed = handler->OnKeyReleased(event.key, event.text) || consumed;

	if (!consumed && m_cameraController != nullptr)
		m_cameraController->injectKeyUp(event);

	m_KeyMap[event.key] = false;
	m_KeyUpMap[event.key] = true;

	return true;
}

bool InputManager::mouseMoved(const OIS::MouseEvent& event)
{
	bool consumed = false;
	for (auto* handler : m_inputHandlers)
		consumed = handler->OnMouseMoved(event) || consumed;

	if (!consumed && event.state.buttonDown(OIS::MB_Right) && m_cameraController != nullptr)
		m_cameraController->injectMouseMove(event);

	if (m_windowActiveSetter)
		m_windowActiveSetter(true);

	return true;
}

bool InputManager::mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID btnId)
{
	bool consumed = false;
	for (auto* handler : m_inputHandlers)
		consumed = handler->OnMousePressed(event, btnId) || consumed;

	if (!consumed && m_cameraController != nullptr)
		m_cameraController->injectMouseDown(event, btnId);
	
	return true;
}

bool InputManager::mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID btnId)
{
	bool consumed = false;
	for (auto* handler : m_inputHandlers)
		consumed = handler->OnMouseReleased(event, btnId) || consumed;

	if (!consumed && m_cameraController != nullptr)
		m_cameraController->injectMouseUp(event, btnId);
	
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

#if defined(OIS_APPLE_PLATFORM)
void InputManager::HandleNativeMouseMove(int x, int y, int relX, int relY, int wheelDelta, int buttons)
{
	m_nativeMouseState.X.abs = ClampMouseCoordinate(x, m_nativeMouseState.width);
	m_nativeMouseState.Y.abs = ClampMouseCoordinate(y, m_nativeMouseState.height);
	m_nativeMouseState.X.rel = relX;
	m_nativeMouseState.Y.rel = relY;
	m_nativeMouseState.Z.rel = wheelDelta;
	m_nativeMouseState.Z.abs += wheelDelta;
	m_nativeMouseState.buttons = buttons;

	OIS::MouseEvent event(nullptr, m_nativeMouseState);
	mouseMoved(event);

	m_nativeMouseState.X.rel = 0;
	m_nativeMouseState.Y.rel = 0;
	m_nativeMouseState.Z.rel = 0;
}

void InputManager::HandleNativeMouseButton(int x, int y, int relX, int relY, OIS::MouseButtonID button, bool pressed, int buttons)
{
	m_nativeMouseState.X.abs = ClampMouseCoordinate(x, m_nativeMouseState.width);
	m_nativeMouseState.Y.abs = ClampMouseCoordinate(y, m_nativeMouseState.height);
	m_nativeMouseState.X.rel = relX;
	m_nativeMouseState.Y.rel = relY;
	m_nativeMouseState.Z.rel = 0;
	m_nativeMouseState.buttons = buttons;

	OIS::MouseEvent event(nullptr, m_nativeMouseState);
	if (pressed)
		mousePressed(event, button);
	else
		mouseReleased(event, button);

	m_nativeMouseState.X.rel = 0;
	m_nativeMouseState.Y.rel = 0;
}
#endif

void InputManager::update(int deltaMs)
{
	for (auto& kv : m_KeyDownMap)
		kv.second = false;

	for (auto& kv : m_KeyUpMap)
		kv.second = false;
}
