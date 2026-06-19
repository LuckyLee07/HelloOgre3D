#ifndef __TACTICAL_DEBUG_DRAW_SERVICE_H__
#define __TACTICAL_DEBUG_DRAW_SERVICE_H__

#include <string>
#include <unordered_map>

#include "OgreColourValue.h"
#include "OgreString.h"
#include "OgreVector3.h"

class InfluenceMapSystem;

namespace Ogre
{
	class ManualObject;
	class SceneNode;
}

class TacticalDebugDrawService
{
public:
	TacticalDebugDrawService();
	~TacticalDebugDrawService();

	int DrawLayer(const InfluenceMapSystem* influenceMap, const std::string& layerName, float yOffset, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue, const Ogre::ColourValue& gridColor, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName);
	int RebuildLayerDebugVisual(const InfluenceMapSystem* influenceMap, const std::string& layerName, float yOffset, const Ogre::ColourValue& positiveValue, const Ogre::ColourValue& zeroValue, const Ogre::ColourValue& negativeValue, const Ogre::ColourValue& gridColor, float threshold, int maxCells, bool drawNeutralCells, bool projectToNav, float maxProjectionDistance, const Ogre::String& navMeshName);
	void SetVisible(bool visible);
	void ClearVisuals();

private:
	struct DebugVisual
	{
		DebugVisual()
			: node(nullptr)
			, manualObject(nullptr)
		{
		}

		Ogre::SceneNode* node;
		Ogre::ManualObject* manualObject;
	};

	std::unordered_map<std::string, DebugVisual> m_visuals;
	bool m_visible;
};

#endif // __TACTICAL_DEBUG_DRAW_SERVICE_H__
