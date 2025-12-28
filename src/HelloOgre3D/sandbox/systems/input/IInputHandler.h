#ifndef __IINPUT_HANDLER_H__  
#define __IINPUT_HANDLER_H__

#include "OISKeyboard.h"
#include "OISMouse.h"

class IInputHandler
{
public:
	virtual ~IInputHandler() = default;

	virtual void OnKeyPressed(OIS::KeyCode, unsigned int) {}
	virtual void OnKeyReleased(OIS::KeyCode, unsigned int) {}

	virtual void OnMouseMoved(const OIS::MouseEvent&) {}
	virtual void OnMousePressed(const OIS::MouseEvent&, OIS::MouseButtonID) {}
	virtual void OnMouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID) {}
};

#endif; // __IINPUT_HANDLER_H__