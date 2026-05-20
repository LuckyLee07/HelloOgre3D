#ifndef __IINPUT_HANDLER_H__  
#define __IINPUT_HANDLER_H__

#include "OISKeyboard.h"
#include "OISMouse.h"

class IInputHandler
{
public:
	virtual ~IInputHandler() = default;

	virtual bool OnKeyPressed(OIS::KeyCode, unsigned int) { return false; }
	virtual bool OnKeyReleased(OIS::KeyCode, unsigned int) { return false; }

	virtual bool OnMouseMoved(const OIS::MouseEvent&) { return false; }
	virtual bool OnMousePressed(const OIS::MouseEvent&, OIS::MouseButtonID) { return false; }
	virtual bool OnMouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID) { return false; }
};

#endif; // __IINPUT_HANDLER_H__
