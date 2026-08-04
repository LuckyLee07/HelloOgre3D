#include "UIPolygon.h"
#include "Ogre.h"

UIPolygon::UIPolygon(Gorilla::Layer* pUILayer)
	: m_pUILayer(pUILayer), m_pPolygon(nullptr), m_visible(true),
	m_bgColor(1, 1, 1, 1), m_borderColor(0, 0, 0, 0), m_borderWidth(0)
{
}

UIPolygon::~UIPolygon()
{
	// 与 UIFrame 一致：底层图元随 Gorilla::Layer 销毁，这里只断引用，
	// 不主动 destroyPolygon（避免 shutdown 时层已销毁的二次释放）。
	m_pPolygon = nullptr;
	m_pUILayer = nullptr;
}

void UIPolygon::Initialize()
{
	m_pPolygon = m_pUILayer->createPolygon(0, 0, 10, 6);
	ApplyColors();
}

void UIPolygon::setPosition(const Ogre::Vector2& position)
{
	m_pPolygon->left(position.x);
	m_pPolygon->top(position.y);
}

void UIPolygon::setRadius(Ogre::Real radius)
{
	m_pPolygon->radius(radius);
}

void UIPolygon::setSides(int sides)
{
	if (sides < 3)
		sides = 3;
	m_pPolygon->sides((size_t)sides);
}

void UIPolygon::setAngleDegrees(Ogre::Real degrees)
{
	m_pPolygon->angle(Ogre::Radian(Ogre::Degree(degrees).valueRadians()));
}

void UIPolygon::setBackgroundColor(const Ogre::ColourValue& colorValue)
{
	m_bgColor = colorValue;
	if (m_visible)
		m_pPolygon->background_colour(colorValue);
}

void UIPolygon::setBorder(Ogre::Real width, const Ogre::ColourValue& colorValue)
{
	m_borderWidth = width;
	m_borderColor = colorValue;
	if (m_visible)
		m_pPolygon->border(width, colorValue);
}

void UIPolygon::setVisible(bool visible)
{
	m_visible = visible;
	ApplyColors();
}

void UIPolygon::ApplyColors()
{
	if (m_pPolygon == nullptr)
		return;

	if (m_visible)
	{
		m_pPolygon->background_colour(m_bgColor);
		m_pPolygon->border(m_borderWidth, m_borderColor);
	}
	else
	{
		// 背景 alpha=0 且描边宽 0：Polygon::_redraw 两处都跳过，不产生任何像素。
		m_pPolygon->background_colour(Ogre::ColourValue(0, 0, 0, 0));
		m_pPolygon->border(0, Ogre::ColourValue(0, 0, 0, 0));
	}
}
