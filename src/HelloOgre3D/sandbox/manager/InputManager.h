#ifndef __INPUT_MANAGER_H__  
#define __INPUT_MANAGER_H__

#include <string>
#include "OISKeyboard.h"
#include "OISMouse.h"

namespace OIS
{
	class InputManager;
}
class GameManager;
class InputManager : public OIS::KeyListener, public OIS::MouseListener
{
public:
	InputManager();
	~InputManager();
	
	void Initialize();

	void capture();
	void closeWindow();
	void resizeMouseState(int width, int height);
	void setGameManager(GameManager* pGameManager);

private:
	// OIS::KeyListener
	virtual bool keyPressed(const OIS::KeyEvent& event);
	virtual bool keyReleased(const OIS::KeyEvent& event);

	// OIS::MouseListener
	virtual bool mouseMoved(const OIS::MouseEvent& event);
	virtual bool mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID btnId);
	virtual bool mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID btnId);

private:
	size_t m_windowHnd;
	GameManager* m_pGameManager;

	// OIS Input devices
	OIS::Mouse* m_pMouse;
	OIS::Keyboard* m_pKeyboard;
	OIS::InputManager * m_pOISInputMgr;
};

#endif; // __INPUT_MANAGER_H__