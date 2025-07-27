#ifndef __IPLAYER_INPUT_H__  
#define __IPLAYER_INPUT_H__

#include "OISKeyboard.h"

class IPlayerInput //tolua_exports
{ //tolua_exports
public:
	virtual ~IPlayerInput() = default;

	//tolua_begin
	virtual bool isKeyDown(OIS::KeyCode key) const = 0;
	virtual bool isKeyPressed(OIS::KeyCode key) const = 0;
	virtual bool isKeyReleased(OIS::KeyCode key) const = 0;
	//tolua_end

	// ����չ�ӿڣ���갴ť���������

}; //tolua_exports

#endif; // __IPLAYER_INPUT_H__