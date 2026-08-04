#ifndef __UI_POLYGON__
#define __UI_POLYGON__

#include "ogre3d_gorilla/include/Gorilla.h"
#include "OgreVector2.h"
#include "OgreColourValue.h"

// 轻量 Gorilla::Polygon 包装：画 N 边正多边形（高边数≈圆盘、3 边=三角箭头），
// 带背景色 + 描边，可设旋转与可见性。用于 Sandbox19 圆形雷达（圆盘/圆点/箭头）。
// 与 UIFrame 同款：生命周期由 UIManager 持有（Create* 工厂），Lua 端不 new。
class UIPolygon //tolua_exports
{ //tolua_exports
public:
	UIPolygon(Gorilla::Layer* pUILayer);
	virtual ~UIPolygon();

	virtual void Initialize();

	//tolua_begin
	void setPosition(const Ogre::Vector2& position);  // 圆心（非左上角）
	void setRadius(Ogre::Real radius);
	void setSides(int sides);                          // ≥3；48≈圆，3=三角
	void setAngleDegrees(Ogre::Real degrees);          // 首顶点角度，用于箭头朝向
	void setBackgroundColor(const Ogre::ColourValue& colorValue);
	void setBorder(Ogre::Real width, const Ogre::ColourValue& colorValue);
	void setVisible(bool visible);
	bool isVisible() { return m_visible; }
	//tolua_end

private:
	void ApplyColors();

	Gorilla::Layer* m_pUILayer;
	Gorilla::Polygon* m_pPolygon;

	bool m_visible;
	Ogre::ColourValue m_bgColor;
	Ogre::ColourValue m_borderColor;
	Ogre::Real m_borderWidth;
}; //tolua_exports

#endif  // __UI_POLYGON__
