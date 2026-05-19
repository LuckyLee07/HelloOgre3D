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

	virtual bool OnMouseMoved(const OIS::MouseEvent&) { return false; }
	virtual bool OnMousePressed(const OIS::MouseEvent&, OIS::MouseButtonID) { return false; }
	virtual bool OnMouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID) { return false; }
};

#endif; // __IINPUT_HANDLER_H__
