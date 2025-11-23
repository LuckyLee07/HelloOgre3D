#ifndef __UI_MANAGER_H__  
#define __UI_MANAGER_H__

#include <vector>
#include "OgreColourValue.h"

#define UI_LAYER_COUNT	16

namespace Gorilla
{
	class Layer;
	class Screen;
	class MarkupText;
}

class UIFrame;
class ClientManager;

class UIManager
{
public:
	explicit UIManager(ClientManager* gamaMnanager);
	~UIManager();

	void InitConfig();

	Ogre::Real GetScreenWidth();
	Ogre::Real GetScreenHeight();

	UIFrame* CreateUIFrame(unsigned int index);
	void SetMarkupColor(unsigned int index, const Ogre::ColourValue& color);

	void HandleWindowResized(unsigned int width, unsigned int height);

private:
	Gorilla::Layer* GetUILayer(unsigned int index = 0);
	
	Gorilla::Screen* m_pUIScene;
	Gorilla::MarkupText* m_pMarkupText;
	Gorilla::Layer* m_pUILayers[UI_LAYER_COUNT];

	ClientManager* m_pClientManager;

private:
	std::vector<UIFrame*> m_uiframes;
};

#endif; // __UI_MANAGER_H__