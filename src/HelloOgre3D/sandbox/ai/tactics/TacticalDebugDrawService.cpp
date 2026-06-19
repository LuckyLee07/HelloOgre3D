#include "ai/tactics/TacticalDebugDrawService.h"

#include "ai/tactics/InfluenceMapSystem.h"
#include "debug/DebugDrawer.h"
#include "systems/service/SceneFactory.h"
#include "OgreAxisAlignedBox.h"
#include "OgreManualObject.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace
{
	float Clamp01(float value)
	{
		if (value < 0.0f)
			return 0.0f;
		if (value > 1.0f)
			return 1.0f;
		return value;
	}

	Ogre::ColourValue BlendInfluenceColor(float value, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue)
	{
		const float amount = Clamp01(std::fabs(value));
		const Ogre::ColourValue& target = value >= 0.0f ? positiveValue : negativeValue;
		return Ogre::ColourValue(
			zeroValue.r + (target.r - zeroValue.r) * amount,
			zeroValue.g + (target.g - zeroValue.g) * amount,
			zeroValue.b + (target.b - zeroValue.b) * amount,
			zeroValue.a + (target.a - zeroValue.a) * amount);
	}

	void BuildSquareVertices(const Ogre::Vector3& position, float length, Ogre::Vector3* vertices)
	{
		const float halfLength = length * 0.5f;
		vertices[0] = Ogre::Vector3(position.x + halfLength, position.y, position.z + halfLength);
		vertices[1] = Ogre::Vector3(position.x - halfLength, position.y, position.z + halfLength);
		vertices[2] = Ogre::Vector3(position.x - halfLength, position.y, position.z - halfLength);
		vertices[3] = Ogre::Vector3(position.x + halfLength, position.y, position.z - halfLength);
	}

	void AddDegenerateTriangleSection(Ogre::ManualObject& manualObject)
	{
		manualObject.position(Ogre::Vector3::ZERO);
		manualObject.colour(Ogre::ColourValue::ZERO);
		manualObject.position(Ogre::Vector3::ZERO);
		manualObject.colour(Ogre::ColourValue::ZERO);
		manualObject.position(Ogre::Vector3::ZERO);
		manualObject.colour(Ogre::ColourValue::ZERO);
		manualObject.triangle(0, 1, 2);
	}

	void AddDegenerateLineSection(Ogre::ManualObject& manualObject)
	{
		manualObject.position(Ogre::Vector3::ZERO);
		manualObject.colour(Ogre::ColourValue::ZERO);
		manualObject.index(0);
	}
}

TacticalDebugDrawService::TacticalDebugDrawService()
	: m_visible(true)
{
}

TacticalDebugDrawService::~TacticalDebugDrawService()
{
	ClearVisuals();
}

int TacticalDebugDrawService::DrawLayer(const InfluenceMapSystem* influenceMap, const std::string& layerName, float yOffset, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue, const Ogre::ColourValue& gridColor, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName)
{
	if (influenceMap == nullptr)
		return 0;

	DebugDrawer* debugDrawer = DebugDrawer::GetInstance();
	if (debugDrawer == nullptr)
		return 0;

	(void)projectToNav;
	(void)maxProjectionDistance;
	(void)navMeshName;
	const float cellSize = influenceMap->GetCellSize();
	const float safeThreshold = std::max(0.0f, threshold);
	const int limit = maxCells > 0 ? maxCells : std::numeric_limits<int>::max();
	int drawn = 0;

	std::vector<Ogre::Vector3> cellPositions;
	std::vector<float> cellValues;
	influenceMap->CollectDebugCells(layerName, safeThreshold, limit, drawNeutralCells, cellPositions, cellValues);
	for (size_t i = 0; i < cellPositions.size(); ++i)
	{
		Ogre::Vector3 drawPosition = cellPositions[i];
		drawPosition.y += yOffset;
		const Ogre::ColourValue color = BlendInfluenceColor(cellValues[i], positiveValue, zeroValue, negativeValue);
		debugDrawer->drawSquare(drawPosition, cellSize, color, true);
		drawPosition.y += -0.5f;
		debugDrawer->drawSquare(drawPosition, cellSize, gridColor, false);
		++drawn;
	}
	return drawn;
}

int TacticalDebugDrawService::RebuildLayerDebugVisual(const InfluenceMapSystem* influenceMap, const std::string& layerName, float yOffset, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue, const Ogre::ColourValue& gridColor, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName)
{
	if (influenceMap == nullptr)
		return 0;

	Ogre::SceneManager* sceneManager = SceneFactory::GetSceneManager();
	if (sceneManager == nullptr)
		return 0;

	struct DrawCell
	{
		Ogre::Vector3 position;
		Ogre::ColourValue color;
	};

	(void)projectToNav;
	(void)maxProjectionDistance;
	(void)navMeshName;
	const float cellSize = influenceMap->GetCellSize();
	const float safeThreshold = std::max(0.0f, threshold);
	const int limit = maxCells > 0 ? maxCells : std::numeric_limits<int>::max();
	std::vector<DrawCell> cells;

	std::vector<Ogre::Vector3> positions;
	std::vector<float> cellValues;
	influenceMap->CollectDebugCells(layerName, safeThreshold, limit, drawNeutralCells, positions, cellValues);
	cells.reserve(positions.size());
	for (size_t i = 0; i < positions.size(); ++i)
	{
		DrawCell cell;
		cell.position = Ogre::Vector3(positions[i].x, positions[i].y + yOffset, positions[i].z);
		cell.color = BlendInfluenceColor(cellValues[i], positiveValue, zeroValue, negativeValue);
		cells.push_back(cell);
	}

	DebugVisual& visual = m_visuals[layerName];
	if (visual.manualObject == nullptr || visual.node == nullptr)
	{
		visual.node = sceneManager->getRootSceneNode()->createChildSceneNode();
		visual.manualObject = sceneManager->createManualObject();
		visual.manualObject->setDynamic(true);
		visual.manualObject->setRenderQueueGroupAndPriority(Ogre::RENDER_QUEUE_MAIN, 110);
		visual.manualObject->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
		visual.node->attachObject(visual.manualObject);
	}

	Ogre::ManualObject* manualObject = visual.manualObject;
	manualObject->clear();

	manualObject->begin("debug_overlay_draw", Ogre::RenderOperation::OT_TRIANGLE_LIST);
	if (cells.empty())
	{
		AddDegenerateTriangleSection(*manualObject);
	}
	else
	{
		manualObject->estimateVertexCount(cells.size() * 4);
		manualObject->estimateIndexCount(cells.size() * 6);
		int vertexIndex = 0;
		for (std::vector<DrawCell>::const_iterator iter = cells.begin(); iter != cells.end(); ++iter)
		{
			Ogre::Vector3 square[4];
			BuildSquareVertices(iter->position, cellSize, square);
			for (int index = 0; index < 4; ++index)
			{
				manualObject->position(square[index]);
				manualObject->colour(iter->color);
			}
			manualObject->triangle(vertexIndex, vertexIndex + 2, vertexIndex + 1);
			manualObject->triangle(vertexIndex, vertexIndex + 3, vertexIndex + 2);
			vertexIndex += 4;
		}
	}
	manualObject->end();

	manualObject->begin("debug_overlay_draw", Ogre::RenderOperation::OT_LINE_LIST);
	if (cells.empty())
	{
		AddDegenerateLineSection(*manualObject);
	}
	else
	{
		manualObject->estimateVertexCount(cells.size() * 4);
		manualObject->estimateIndexCount(cells.size() * 8);
		int vertexIndex = 0;
		for (std::vector<DrawCell>::const_iterator iter = cells.begin(); iter != cells.end(); ++iter)
		{
			Ogre::Vector3 square[4];
			Ogre::Vector3 outlinePosition = iter->position;
			BuildSquareVertices(outlinePosition, cellSize, square);
			for (int index = 0; index < 4; ++index)
			{
				manualObject->position(square[index]);
				manualObject->colour(gridColor);
			}
			manualObject->index(vertexIndex);
			manualObject->index(vertexIndex + 1);
			manualObject->index(vertexIndex + 1);
			manualObject->index(vertexIndex + 2);
			manualObject->index(vertexIndex + 2);
			manualObject->index(vertexIndex + 3);
			manualObject->index(vertexIndex + 3);
			manualObject->index(vertexIndex);
			vertexIndex += 4;
		}
	}
	manualObject->end();
	visual.node->setVisible(m_visible);
	visual.node->_updateBounds();
	return static_cast<int>(cells.size());
}

void TacticalDebugDrawService::SetVisible(bool visible)
{
	m_visible = visible;
	for (std::unordered_map<std::string, DebugVisual>::iterator iter = m_visuals.begin(); iter != m_visuals.end(); ++iter)
	{
		if (iter->second.node != nullptr)
			iter->second.node->setVisible(visible);
	}
}

void TacticalDebugDrawService::ClearVisuals()
{
	Ogre::SceneManager* sceneManager = SceneFactory::GetSceneManager();
	if (sceneManager == nullptr)
	{
		m_visuals.clear();
		return;
	}

	for (std::unordered_map<std::string, DebugVisual>::iterator iter = m_visuals.begin(); iter != m_visuals.end(); ++iter)
	{
		DebugVisual& visual = iter->second;
		if (visual.manualObject != nullptr)
			sceneManager->destroyManualObject(visual.manualObject);
		if (visual.node != nullptr)
			sceneManager->destroySceneNode(visual.node);
	}
	m_visuals.clear();
}
