#ifndef __UI_MANAGER_H__  
#define __UI_MANAGER_H__

#include <vector>
#include "OgreColourValue.h"

#define UI_LAYER_COUNT	16

namespace Ogre
{
	class Camera;
}

namespace Gorilla
{
	class Layer;
	class Screen;
	class MarkupText;
}
class UIFrame;

class UIManager //tolua_exports
{ //tolua_exports
public:
	explicit UIManager(Ogre::Camera* camera);
	~UIManager();

	void InitConfig();

	Ogre::Real GetScreenWidth();
	Ogre::Real GetScreenHeight();

	//tolua_begin
	UIFrame* CreateUIFrame(unsigned int index = 1);
	void SetMarkupColor(unsigned int index, const Ogre::ColourValue& color);
	//tolua_end

	void HandleWindowResized(unsigned int width, unsigned int height);

private:
	Gorilla::Layer* GetUILayer(unsigned int index = 0);
	
	Gorilla::Screen* m_pUIScene;
	Gorilla::MarkupText* m_pMarkupText;
	Gorilla::Layer* m_pUILayers[UI_LAYER_COUNT];

private:
	Ogre::Camera* m_pCamera;
	std::vector<UIFrame*> m_uiframes;
}; //tolua_exports

#endif; // __UI_MANAGER_H__
