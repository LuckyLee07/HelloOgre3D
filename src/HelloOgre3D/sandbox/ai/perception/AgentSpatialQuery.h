#ifndef __AGENT_SPATIAL_QUERY_H__
#define __AGENT_SPATIAL_QUERY_H__

#include <vector>

#include "OgreVector3.h"
#include "objects/AgentObject.h"
#include "systems/manager/ObjectManager.h"

class IAgentSpatialQuery
{
public:
	virtual ~IAgentSpatialQuery() {}

	virtual void QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents) const = 0;
};

class ObjectManagerAgentSpatialQuery : public IAgentSpatialQuery
{
public:
	explicit ObjectManagerAgentSpatialQuery(ObjectManager* objectManager)
		: m_objectManager(objectManager)
	{
	}

	virtual void QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents) const override
	{
		outAgents.clear();
		if (m_objectManager == nullptr)
		{
			return;
		}

		const std::vector<AgentObject*>& agents = m_objectManager->getAllAgents();
		if (radius <= 0.0f)
		{
			outAgents.insert(outAgents.end(), agents.begin(), agents.end());
			return;
		}

		const float radiusSquared = radius * radius;
		for (AgentObject* agent : agents)
		{
			if (agent == nullptr)
			{
				continue;
			}

			if (center.squaredDistance(agent->GetPosition()) <= radiusSquared)
			{
				outAgents.push_back(agent);
			}
		}
	}

private:
	ObjectManager* m_objectManager;
};

#endif // __AGENT_SPATIAL_QUERY_H__
