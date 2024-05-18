#include "DebugDrawer.h"
#include "Ogre.h"
#include "ClientManager.h"

DebugDrawer::DebugDrawer() : m_pManualNode(nullptr), m_pManualObject(nullptr),
	m_isEnable(true), m_linesIndex(0)
{
	//Initialize();
}

DebugDrawer::~DebugDrawer()
{

}

void DebugDrawer::Initialize()
{
	Ogre::SceneManager* pSceneManager = GetClientMgr()->getSceneManager();
	Ogre::SceneNode* pRootScenNode = pSceneManager->getRootSceneNode();

	m_pManualNode = pRootScenNode->createChildSceneNode();
	m_pManualObject = pSceneManager->createManualObject();
	m_pManualNode->attachObject(m_pManualObject);
	m_pManualObject->setDynamic(true);

	m_pManualObject->begin("debug_draw", Ogre::RenderOperation::OT_LINE_LIST);
	m_pManualObject->position(Ogre::Vector3::ZERO);
	m_pManualObject->colour(Ogre::ColourValue::ZERO);
	m_pManualObject->index(0);
	m_pManualObject->end();

	m_pManualObject->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
	m_pManualNode->_updateBounds();
}

void DebugDrawer::build()
{
	m_pManualObject->beginUpdate(0);
	if (m_isEnable && !m_lineVertices.empty())
	{
		m_pManualObject->estimateVertexCount(m_lineVertices.size());
		m_pManualObject->estimateIndexCount(m_lineIndices.size());

		auto vIter = m_lineVertices.begin();
		for (; vIter != m_lineVertices.end(); vIter++)
		{
			m_pManualObject->position(vIter->first);
			m_pManualObject->colour(vIter->second);
		}
		auto iIter = m_lineIndices.begin();
		for (; iIter != m_lineIndices.end(); iIter++)
		{
			m_pManualObject->index(*iIter);
		}
	}
	else
	{
		m_pManualObject->position(Ogre::Vector3::ZERO);
		m_pManualObject->colour(Ogre::ColourValue::ZERO);
		m_pManualObject->index(0);
	}
	m_pManualObject->end();

	//m_pManualObject->beginUpdate(1);
	//m_pManualObject->end();
	m_pManualNode->_updateBounds();
}

void DebugDrawer::clear()
{
	m_linesIndex = 0;
	m_lineVertices.clear();
	m_lineIndices.clear();
}

void DebugDrawer::drawLine(const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::ColourValue& color)
{
	this->buildLine(start, end, color);
}

void DebugDrawer::drawCircle(const Ogre::Vector3& centre, float radius, int segmentsCount, const Ogre::ColourValue& color, bool isFilled)
{
	this->buildCircle(centre, radius, segmentsCount, color);
}

void DebugDrawer::drawSquare(const Ogre::Vector3& position, Ogre::Real length, const Ogre::ColourValue& color, bool isFilled)
{
	float halfLength = length / 2.0f;
	Ogre::Vector3 square[4];
	square[0].x = position.x + halfLength;
	square[0].y = position.y;
	square[0].z = position.z + halfLength;

	square[1].x = position.x - halfLength;
	square[1].y = position.y;
	square[1].z = position.z + halfLength;

	square[2].x = position.x - halfLength;
	square[2].y = position.y;
	square[2].z = position.z - halfLength;

	square[3].x = position.x + halfLength;
	square[3].y = position.y;
	square[3].z = position.z - halfLength;
	this->buildQuad(square, color);
}

void DebugDrawer::buildLine(const Ogre::Vector3& start, const Ogre::Vector3& end, const Ogre::ColourValue& color, float alpha)
{
	Ogre::ColourValue colorWithAlpha = color;
	colorWithAlpha.a = alpha;

	int index1 = addLineVertex(start, colorWithAlpha);
	addLineVertex(end, colorWithAlpha);

	this->addLineIndices(index1, index1+1);
}

void DebugDrawer::buildCircle(const Ogre::Vector3& centre, float radius, int segmentsCount, const Ogre::ColourValue& color, float alpha)
{
	int index = m_linesIndex;
	float increment = 2 * Ogre::Math::PI / segmentsCount;
	float angle = 0.0f;

	Ogre::ColourValue colorWithAlpha = color;
	colorWithAlpha.r = alpha;
	for (int i = 0; i < segmentsCount; i++)
	{
		Ogre::Real posx = centre.x + radius * Ogre::Math::Cos(angle);
		Ogre::Real posz = centre.z + radius * Ogre::Math::Sin(angle);
		addLineVertex(Ogre::Vector3(posx, centre.y, posz), colorWithAlpha);
		angle += increment;

		addLineIndices(index + i, (i + 1) == segmentsCount ? index : (index + i + 1));
	}
}

void DebugDrawer::buildQuad(const Ogre::Vector3* vertices, const Ogre::ColourValue& color, float alpha)
{
	int index = m_linesIndex;
	for (size_t i = 0; i < 4; i++)
	{
		addLineVertex(vertices[i], Ogre::ColourValue(color.r, color.g, color.b, alpha));
		addLineIndices(index + i, (i + 1) == 4 ? index : (index + i + 1));
	}
}

int DebugDrawer::addLineVertex(const Ogre::Vector3& vertex, const Ogre::ColourValue& color)
{
	m_lineVertices.push_back(VertexPair(vertex, color));
	return m_linesIndex++;
}

void DebugDrawer::addLineIndices(int index1, int index2)
{
	m_lineIndices.push_back(index1);
	m_lineIndices.push_back(index2);
}