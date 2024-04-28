#include "UIComponent.h"
#include "Ogre.h"

static const int markupTextTopOffset = 1;
static const int markupTextLeftOffset = 1;

UIComponent::UIComponent(Gorilla::Layer* pUILayer) 
	: BaseObject(0, BaseObject::OBJ_SANDBOX_UI),
	m_pSceneNode(nullptr), m_pUILayer(pUILayer), 
	m_pScreen(nullptr), m_pRectangle(nullptr), m_pText(nullptr), m_pMarkupText(nullptr),
	m_dimension(0, 0), m_textMargin(0, 0), m_topLeftPos(0, 0), m_topLeftOffset(0, 0)
{
	this->Initialize();
}

UIComponent::~UIComponent()
{

}

void UIComponent::Initialize()
{
	m_pRectangle = m_pUILayer->createRectangle(m_topLeftPos + m_topLeftOffset, m_dimension);

	Ogre::Real textPosx = m_textMargin.x + m_topLeftOffset.x;
	Ogre::Real textPosy = m_textMargin.y + m_topLeftOffset.y;

	m_pText = m_pUILayer->createCaption(m_fontType, textPosx, textPosy, "");

	m_pMarkupText = m_pUILayer->createMarkupText(
		UIComponent::FONT_SMALL,
		textPosx + markupTextLeftOffset,
		textPosy + markupTextTopOffset,
		"");

	setDimension(Ogre::Vector2(100, 100));
	setBackgroundColor(Ogre::ColourValue(0, 0, 0, 0));
}

void UIComponent::addChild(UIComponent* pChild)
{

}

void UIComponent::setFontType(FontType fontType)
{
	m_fontType = fontType;
}

void UIComponent::setText(const Ogre::String& pText)
{
	m_pText->text(pText);
}

void UIComponent::setMarkupText(const Ogre::String& pText)
{
	m_pMarkupText->text(pText);
}

void UIComponent::setTextMargin(Ogre::Real top, Ogre::Real left)
{
	m_textMargin.x = left;
	m_textMargin.y = top;

	Ogre::Real textTop = m_topLeftOffset.y + m_topLeftPos.y + m_textMargin.y;
	Ogre::Real textLeft = m_topLeftOffset.x + m_topLeftPos.x + m_textMargin.x;

	m_pText->top(textTop);
	m_pText->left(textLeft);
	m_pMarkupText->top(textTop );
	m_pMarkupText->left(textLeft);

	Ogre::Real textWidth = m_dimension.x - m_textMargin.x * 2;
	Ogre::Real textHeight = m_dimension.y - m_textMargin.y * 2;

	m_pText->size(textWidth, textHeight);
	m_pMarkupText->size(textWidth, textHeight);
}

void UIComponent::setPosition(const Ogre::Vector2& position)
{
	m_topLeftPos = position;

	m_pRectangle->position(m_topLeftOffset + m_topLeftPos);

	Ogre::Real textTop = m_topLeftOffset.y + m_topLeftPos.y + m_textMargin.y;
	Ogre::Real textLeft = m_topLeftOffset.x + m_topLeftPos.x + m_textMargin.x;
	m_pText->top(textTop);
	m_pText->left(textLeft);

	m_pMarkupText->top(textTop + markupTextTopOffset);
	m_pMarkupText->left(textLeft + markupTextLeftOffset);
}

void UIComponent::setDimension(const Ogre::Vector2& dimension)
{
	m_dimension = dimension;

	m_pRectangle->width(dimension.x);
	m_pRectangle->height(dimension.y);

	Ogre::Real textWidth = m_dimension.x - m_textMargin.x * 2;
	Ogre::Real textHeight = m_dimension.y - m_textMargin.y * 2;

	m_pText->size(textWidth, textHeight);
	m_pMarkupText->size(textWidth, textHeight);
}

void UIComponent::setBackgroundColor(const Ogre::ColourValue& colorValue)
{
	m_pRectangle->background_colour(colorValue);
}