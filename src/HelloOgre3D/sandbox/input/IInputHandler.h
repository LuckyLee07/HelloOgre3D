#ifndef __IINPUT_HANDLER_H__  
#define __IINPUT_HANDLER_H__

#include "OISKeyboard.h"
#include "OISMouse.h"

class IInputHandler
{
public:
	virtual ~IInputHandler() = default;

	virtual void OnKeyPressed(OIS::KeyCode keycode, unsigned int key) {}
	virtual void OnKeyReleased(OIS::KeyCode keycode, unsigned int key) {}

	virtual void OnMouseMoved(const OIS::MouseEvent& evt) {}
	virtual void OnMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID btn) {}
	virtual void OnMouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID btn) {}
};

#endif; // __IINPUT_HANDLER_H__