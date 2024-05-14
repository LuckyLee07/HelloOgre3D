#ifndef __DEBUG_DRAWER_H__  
#define __DEBUG_DRAWER_H__

#include "Singleton.h"
#include "OgreVector3.h"
#include "OgreColourValue.h"

#include <vector>

namespace Ogre
{
	class SceneNode;
	class ManualObject;
};

typedef std::pair<Ogre::Vector3, Ogre::ColourValue> VertexPair;

class DebugDrawer //tolua_exports
	: public Fancy::Singleton<DebugDrawer>
{ //tolua_exports
public:
	DebugDrawer();
	~DebugDrawer();

	void Initialize();

	void build();
	void clear();
	
	//tolua_begin
	bool getIsEnable() { return m_isEnable; }
	void setIsEnable(bool isEnable) { m_isEnable = isEnable; }
	
	void drawLine(const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::ColourValue& color);
	void drawCircle(const Ogre::Vector3& centre, float radius, int segmentsCount, const Ogre::ColourValue& color, bool isFilled = false);
	void drawSquare(const Ogre::Vector3& position, Ogre::Real length, const Ogre::ColourValue& color, bool isFilled = false);
	//tolua_end

private:
	int addLineVertex(const Ogre::Vector3& vertex, const Ogre::ColourValue& color);
	void addLineIndices(int index1, int index2);

	void buildLine(const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::ColourValue& color, float alpha = 1.0f);
	void buildCircle(const Ogre::Vector3& centre, float radius, int segmentsCount, const Ogre::ColourValue& color, float alpha = 1.0f);
	void buildQuad(const Ogre::Vector3* vertices, const Ogre::ColourValue& color, float alpha = 1.0f);

private:
	Ogre::SceneNode* m_pManualNode;
	Ogre::ManualObject* m_pManualObject;

	bool m_isEnable;

	std::vector<VertexPair> m_lineVertices;
	std::vector<int> m_lineIndices;

	int  m_linesIndex;
}; //tolua_exports

#endif; // __DEBUG_DRAWER_H__