#ifndef __UI_SERVICE_H__
#define __UI_SERVICE_H__

#include "OgreColourValue.h"

class UIComponent;

class UIService
{
public:
	static UIComponent* CreateUIComponent(unsigned int index);
	static void SetMarkupColor(unsigned int index, const Ogre::ColourValue& color);
};

#endif // __UI_SERVICE_H__