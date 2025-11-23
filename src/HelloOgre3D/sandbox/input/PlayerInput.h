#ifndef __PLAYER_INPUT_H__  
#define __PLAYER_INPUT_H__

#include "IPlayerInput.h"
#include "InputManager.h"

class PlayerInput : public IPlayerInput
{
public:
	PlayerInput(InputManager* inputMgr) : m_inputMgr(inputMgr) {}

	virtual bool isKeyDown(OIS::KeyCode key) const override
	{
		return m_inputMgr->isKeyDown(key);
	}

	virtual bool isKeyPressed(OIS::KeyCode key) const override
	{
		return m_inputMgr->isKeyPressed(key);
	}

	virtual bool isKeyReleased(OIS::KeyCode key) const override
	{
		return m_inputMgr->isKeyReleased(key);
	}

	// 可扩展接口：鼠标按钮、方向轴等

private:
	InputManager* m_inputMgr;
};

#endif; // __PLAYER_INPUT_H__