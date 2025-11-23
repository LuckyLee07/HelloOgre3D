#ifndef __UI_SERVICE_H__
#define __UI_SERVICE_H__

#include "OgreColourValue.h"

class UIFrame;
class UIManager;

class UIService
{
public:
	UIService(UIManager* pMananger);
	~UIService() { m_pUIManager = nullptr; }

	UIFrame* CreateUIFrame(unsigned int index);
	void SetMarkupColor(unsigned int index, const Ogre::ColourValue& color);

private:
	UIManager* m_pUIManager;
};

#endif // __UI_SERVICE_H__