#ifndef __INPUT_MANAGER_H__  
#define __INPUT_MANAGER_H__

#include <map>
#include "systems/input/IInputHandler.h"

namespace OIS
{
	class InputManager;
}

class InputManager : public OIS::KeyListener, public OIS::MouseListener
{
public:
	InputManager();
	~InputManager();
	
	void Initialize();

	void capture();
	void closeWindow();
	void resizeMouseState(int width, int height);

	void registerHandler(IInputHandler* handler);
	void unregisterHandler(IInputHandler* handler);

private:
	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent& event);
	virtual bool keyReleased(const OIS::KeyEvent& event);

	// OIS::MouseListener
	virtual bool mouseMoved(const OIS::MouseEvent& event);
	virtual bool mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID btnId);
	virtual bool mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID btnId);

public:
	bool isKeyDown(OIS::KeyCode key) const;
	bool isKeyPressed(OIS::KeyCode key) const;
	bool isKeyReleased(OIS::KeyCode key) const;

	void update(int deltaMs); // 每帧调用以清除 KeyDown/KeyUp 状态

private:
	size_t m_windowHnd;

	// OIS Input devices
	OIS::Mouse* m_pMouse;
	OIS::Keyboard* m_pKeyboard;
	OIS::InputManager * m_pOISInputMgr;

	std::vector<IInputHandler*> m_inputHandlers;

	std::map<OIS::KeyCode, bool> m_KeyMap;	 //true: 键按下状态
	std::map<OIS::KeyCode, bool> m_KeyDownMap; //true: 键这一帧弹起
	std::map<OIS::KeyCode, bool> m_KeyUpMap; //true: 键这一帧按下
};

#endif; // __INPUT_MANAGER_H__