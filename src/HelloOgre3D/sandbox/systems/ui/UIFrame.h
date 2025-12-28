#ifndef __UI_FRAME__
#define __UI_FRAME__

#include <vector>
#include "ogre3d_gorilla/include/Gorilla.h"
#include "OgreSceneNode.h"
#include "OgreString.h"
#include "OgrePrerequisites.h"

class UIFrame //tolua_exports
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
	UIFrame(Gorilla::Layer* pUILayer);
	virtual ~UIFrame();

	virtual void Initialize();

	//tolua_begin
	void addChild(UIFrame* pChild);
	void setFontType(FontType fontType);

	void setText(const Ogre::String& pText);
	void setMarkupText(const Ogre::String& pText);

	void setPosition(const Ogre::Vector2& position);
	void setDimension(const Ogre::Vector2& dimension);
	void setTextMargin(Ogre::Real top, Ogre::Real left);

	void setBackgroundColor(const Ogre::ColourValue& colorValue);
	
	void setGradientColor(Gorilla::Gradient direction, const Ogre::ColourValue& startColor, const Ogre::ColourValue& endColor);

	void setVisible(bool visible);
	bool isVisible() { return m_IsVisible; }

	Ogre::Vector2 GetDimension() { return m_dimension; }
	//tolua_end

private:
	bool m_IsVisible;
	std::vector<UIFrame*> m_children;

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

#endif  // __UI_FRAME__
