#ifndef __UI_SERVICE_H__
#define __UI_SERVICE_H__

#include "OgreColourValue.h"

class UIComponent;
class GameManager;

class UIService
{
public:
	UIService(GameManager* pMananger);
	~UIService() { m_gameManager = nullptr; }

	UIComponent* CreateUIComponent(unsigned int index);
	void SetMarkupColor(unsigned int index, const Ogre::ColourValue& color);

private:
	GameManager* m_gameManager;
};

#endif // __UI_SERVICE_H__