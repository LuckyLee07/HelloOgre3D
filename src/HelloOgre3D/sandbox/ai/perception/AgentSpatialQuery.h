#ifndef __AGENT_SPATIAL_QUERY_H__
#define __AGENT_SPATIAL_QUERY_H__

#include <vector>

#include "OgreVector3.h"
#include "ai/perception/AgentSpatialIndexSystem.h"
#include "objects/AgentObject.h"
#include "systems/manager/ObjectManager.h"

class IAgentSpatialQuery
{
public:
	virtual ~IAgentSpatialQuery() {}

	virtual void QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents, int maxResults = 0) const = 0;
};

class ObjectManagerAgentSpatialQuery : public IAgentSpatialQuery
{
public:
	explicit ObjectManagerAgentSpatialQuery(ObjectManager* objectManager)
		: m_objectManager(objectManager)
	{
	}

	virtual void QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents, int maxResults = 0) const override
	{
		outAgents.clear();
		if (m_objectManager == nullptr)
		{
			return;
		}

		const AgentSpatialIndexSystem* spatialIndex = m_objectManager->GetAgentSpatialIndexSystem();
		if (spatialIndex != nullptr && spatialIndex->IsEnabled() && spatialIndex->IsBuilt())
		{
			spatialIndex->QueryAgentsInRange(center, radius, outAgents, maxResults);
			return;
		}

		const std::vector<AgentObject*>& agents = m_objectManager->getAllAgents();
		if (radius <= 0.0f)
		{
			if (maxResults > 0 && maxResults < static_cast<int>(agents.size()))
			{
				outAgents.insert(outAgents.end(), agents.begin(), agents.begin() + maxResults);
			}
			else
			{
				outAgents.insert(outAgents.end(), agents.begin(), agents.end());
			}
			if (spatialIndex != nullptr)
				spatialIndex->RecordFallbackQueryStats(static_cast<int>(agents.size()), static_cast<int>(outAgents.size()));
			return;
		}

		const float radiusSquared = radius * radius;
		int candidates = 0;
		for (AgentObject* agent : agents)
		{
			++candidates;
			if (agent == nullptr)
			{
				continue;
			}

			if (center.squaredDistance(agent->GetPosition()) <= radiusSquared)
			{
				outAgents.push_back(agent);
				if (maxResults > 0 && static_cast<int>(outAgents.size()) >= maxResults)
				{
					if (spatialIndex != nullptr)
						spatialIndex->RecordFallbackQueryStats(candidates, static_cast<int>(outAgents.size()));
					return;
				}
			}
		}
		if (spatialIndex != nullptr)
			spatialIndex->RecordFallbackQueryStats(candidates, static_cast<int>(outAgents.size()));
	}

private:
	ObjectManager* m_objectManager;
};

#endif // __AGENT_SPATIAL_QUERY_H__
