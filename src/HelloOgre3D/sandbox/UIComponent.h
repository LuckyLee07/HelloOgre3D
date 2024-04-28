#ifndef __UI_COMPONENT__
#define __UI_COMPONENT__

#include "BaseObject.h"
#include "ogre3d_gorilla/include/Gorilla.h"
#include "ogre3d/include/OgreSceneNode.h"
#include "ogre3d/include/OgreString.h"
#include "ogre3d/include/OgrePrerequisites.h"
#include <vector>

class UIComponent : public BaseObject //tolua_exports
{ //tolua_exports
public:
	//tolua_begin
	enum FontType
	{
		FONT_SMALL = 9,
		FONT_SMALL_MONO = 91,
		FONT_MEDIUM = 14,
		FONT_MEDIUM_MONO = 141,
		FONT_LARGE = 24,
		FONT_LARGE_MONO = 241,
		FONT_UNKNOWN = -1,
	};
	//tolua_end
public:
	UIComponent(Gorilla::Layer* pUILayer);
	~UIComponent();

	void Initialize();

	//tolua_begin
	void addChild(UIComponent* pChild);
	void setFontType(FontType fontType);

	void setText(const Ogre::String& pText);
	void setMarkupText(const Ogre::String& pText);

	void setPosition(const Ogre::Vector2& position);
	void setDimension(const Ogre::Vector2& dimension);
	void setTextMargin(Ogre::Real top, Ogre::Real left);

	void setBackgroundColor(const Ogre::ColourValue& colorValue);

	//tolua_end
private:
	std::vector<UIComponent*> m_children;

	Ogre::SceneNode* m_pSceneNode;
	Gorilla::Layer* m_pUILayer;
	Gorilla::ScreenRenderable* m_pScreen;
	Gorilla::Rectangle* m_pRectangle;
	Gorilla::Caption* m_pText;
	Gorilla::MarkupText* m_pMarkupText;

	FontType m_fontType;
	Ogre::Vector2 m_dimension;
	Ogre::Vector2 m_textMargin;
	Ogre::Vector2 m_topLeftPos;
	Ogre::Vector2 m_topLeftOffset;

}; //tolua_exports


#endif  // __UI_COMPONENT__
