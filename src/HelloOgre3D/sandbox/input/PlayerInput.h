#ifndef __PLAYER_INPUT_H__  
#define __PLAYER_INPUT_H__

#include "IPlayerInput.h"
#include "manager/InputManager.h"

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

	// ����չ�ӿڣ���갴ť���������

private:
	InputManager* m_inputMgr;
};

#endif; // __PLAYER_INPUT_H__