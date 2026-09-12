#include "InputManager.h"
#include "OISInputManager.h"
#include "OgreRenderWindow.h"
#include "OgreLogManager.h"
#include "ogre/OgreCameraController.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#if defined(OIS_WIN32_PLATFORM)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

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

	bool IsBackgroundWindowRequested()
	{
		const char* value = std::getenv("HELLO_WINDOW_BACKGROUND");
		if (value == nullptr || value[0] == '\0')
			return false;

		std::string normalized(value);
		std::transform(normalized.begin(), normalized.end(), normalized.begin(),
			[](unsigned char character) { return static_cast<char>(std::tolower(character)); });
		return normalized != "0" && normalized != "false" && normalized != "no" && normalized != "off";
	}
}

InputManager::InputManager(Ogre::RenderWindow* renderWindow, OgreCameraController* cameraController,
	const WindowStateSetter& shutdownSetter, const WindowStateSetter& windowActiveSetter)
	: m_windowHnd(0), m_renderWindow(renderWindow), m_cameraController(cameraController),
	m_shutdownSetter(shutdownSetter), m_windowActiveSetter(windowActiveSetter),
	m_pMouse(nullptr), m_pKeyboard(nullptr), m_pOISInputMgr(nullptr),
	m_gameplayMouseLookEnabled(false), m_mouseCaptured(false), m_capturePendingClick(false), m_captureClickButton(-1)
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
	// Background automation renders and accepts internal replay events only. It must
	// never acquire the user's hardware input while another application is active.
	const char* replay = std::getenv("HELLO_INPUT_REPLAY");
	if (IsBackgroundWindowRequested() || (replay != nullptr && replay[0] != '\0'))
	{
		Ogre::LogManager::getSingleton().logMessage(IsBackgroundWindowRequested()
			? "[WindowMode] background=true physical-input=disabled" : "[InputReplay] physical-input=disabled");
		return;
	}
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
	if (m_mouseCaptured && m_renderWindow != nullptr && !m_renderWindow->isActive())
		SuspendGameplayMouseLook();
#if defined(OIS_APPLE_PLATFORM)
	if (m_pKeyboard == nullptr) PumpNativeWindowEvents();
#endif
	if (m_pMouse != nullptr)
		m_pMouse->capture();

	if (m_pKeyboard != nullptr)
		m_pKeyboard->capture();
}

void InputManager::closeWindow()
{
	SetGameplayMouseLook(false);
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

void InputManager::SetGameplayMouseLook(bool enabled)
{
	if (m_gameplayMouseLookEnabled == enabled && (m_mouseCaptured || !enabled || m_capturePendingClick))
		return;
	m_gameplayMouseLookEnabled = enabled;
	if (!enabled)
	{
		if (m_mouseCaptured)
		{
			SetNativeMouseCapture(false);
			Ogre::LogManager::getSingleton().logMessage("[MouseLook] capture=off reason=mode-change");
		}
		m_mouseCaptured = false;
		return;
	}
	if (m_capturePendingClick || m_pOISInputMgr == nullptr ||
		(m_renderWindow != nullptr && !m_renderWindow->isActive()))
		return;
	m_mouseCaptured = SetNativeMouseCapture(true);
	if (m_mouseCaptured)
		Ogre::LogManager::getSingleton().logMessage("[MouseLook] capture=on");
}

bool InputManager::IsGameplayMouseLookActive() const
{
	return m_gameplayMouseLookEnabled &&
		(m_pOISInputMgr == nullptr || m_mouseCaptured);
}

void InputManager::SuspendGameplayMouseLook()
{
	if (m_mouseCaptured)
	{
		SetNativeMouseCapture(false);
		Ogre::LogManager::getSingleton().logMessage("[MouseLook] capture=off reason=focus-lost");
	}
	m_mouseCaptured = false;
	m_capturePendingClick = true;
	ResetHeldKeys();
}

bool InputManager::SetNativeMouseCapture(bool captured)
{
#if defined(OIS_APPLE_PLATFORM)
	return ApplyNativeMouseCapture(captured);
#elif defined(OIS_WIN32_PLATFORM)
	if (!captured)
	{
		ClipCursor(nullptr);
		ShowCursor(TRUE);
		return true;
	}
	HWND window = reinterpret_cast<HWND>(m_windowHnd);
	RECT bounds;
	if (window == nullptr || !GetClientRect(window, &bounds)) return false;
	POINT topLeft = {bounds.left, bounds.top};
	POINT bottomRight = {bounds.right, bounds.bottom};
	if (!ClientToScreen(window, &topLeft) || !ClientToScreen(window, &bottomRight)) return false;
	bounds.left = topLeft.x;
	bounds.top = topLeft.y;
	bounds.right = bottomRight.x;
	bounds.bottom = bottomRight.y;
	if (!ClipCursor(&bounds)) return false;
	if (!SetCursorPos((bounds.left + bounds.right) / 2, (bounds.top + bounds.bottom) / 2))
	{
		ClipCursor(nullptr);
		return false;
	}
	ShowCursor(FALSE);
	return true;
#else
	return !captured;
#endif
}

void InputManager::resizeMouseState(int width, int height)
{
#if defined(OIS_APPLE_PLATFORM)
	m_nativeMouseState.width = width;
	m_nativeMouseState.height = height;
#elif defined(OIS_WIN32_PLATFORM)
	if (m_mouseCaptured)
	{
		SetNativeMouseCapture(false);
		m_mouseCaptured = SetNativeMouseCapture(true);
	}
#endif
	if (m_pMouse == nullptr)
		return;
	const OIS::MouseState& mouseState = m_pMouse->getMouseState();
	mouseState.width = width;
	mouseState.height = height;
}

void InputManager::registerHandler(IInputHandler* handler)
{
	if (handler != nullptr && std::find(m_inputHandlers.begin(), m_inputHandlers.end(), handler) == m_inputHandlers.end())
		m_inputHandlers.push_back(handler);
}

void InputManager::unregisterHandler(IInputHandler* handler)
{
	m_inputHandlers.erase(std::remove(m_inputHandlers.begin(), m_inputHandlers.end(), handler), m_inputHandlers.end());
}

bool InputManager::keyPressed(const OIS::KeyEvent& event)
{
	bool consumed = false;
	for (auto* handler : m_inputHandlers)
	{
		if (handler->OnKeyPressed(event.key, event.text))
		{
			consumed = true;
			break;
		}
	}

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
	{
		if (handler->OnMouseMoved(event))
		{
			consumed = true;
			break;
		}
	}

	if (!consumed && event.state.buttonDown(OIS::MB_Right) && m_cameraController != nullptr)
		m_cameraController->injectMouseMove(event);

	if (m_windowActiveSetter)
		m_windowActiveSetter(true);

	return true;
}

bool InputManager::mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID btnId)
{
	if (m_capturePendingClick && m_gameplayMouseLookEnabled)
	{
		m_mouseCaptured = SetNativeMouseCapture(true);
		m_capturePendingClick = !m_mouseCaptured;
		if (m_mouseCaptured)
			Ogre::LogManager::getSingleton().logMessage("[MouseLook] capture=on reason=resume-click");
		m_captureClickButton = static_cast<int>(btnId);
		return true;
	}
	bool consumed = false;
	for (auto* handler : m_inputHandlers)
	{
		if (handler->OnMousePressed(event, btnId))
		{
			consumed = true;
			break;
		}
	}

	if (!consumed && m_cameraController != nullptr)
		m_cameraController->injectMouseDown(event, btnId);
	
	return true;
}

bool InputManager::mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID btnId)
{
	if (m_captureClickButton == static_cast<int>(btnId))
	{
		m_captureClickButton = -1;
		return true;
	}
	bool consumed = false;
	for (auto* handler : m_inputHandlers)
		consumed = handler->OnMouseReleased(event, btnId) || consumed;

	if (!consumed && m_cameraController != nullptr)
		m_cameraController->injectMouseUp(event, btnId);
	
	return true;
}

bool InputManager::IsFollowCamera() const
{
	return m_cameraController != nullptr && m_cameraController->getStyle() == OgreCameraController::CS_FOLLOW;
}

void InputManager::ResetHeldKeys()
{
	std::vector<OIS::KeyCode> held;
	for (const auto& entry : m_KeyMap)
		if (entry.second) held.push_back(entry.first);
	for (OIS::KeyCode key : held) keyReleased(OIS::KeyEvent(nullptr, key, 0));
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
