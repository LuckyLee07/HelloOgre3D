#ifndef __IPLAYER_INPUT_H__  
#define __IPLAYER_INPUT_H__

#include "OISKeyboard.h"

class IPlayerInput
{
public:
	virtual ~IPlayerInput() = default;

	virtual bool isKeyDown(OIS::KeyCode key) const = 0;
	virtual bool isKeyPressed(OIS::KeyCode key) const = 0;
	virtual bool isKeyReleased(OIS::KeyCode key) const = 0;

	// 可扩展接口：鼠标按钮、方向轴等

};

#endif; // __IPLAYER_INPUT_H__