#ifndef __AGENT_SPATIAL_QUERY_H__
#define __AGENT_SPATIAL_QUERY_H__

#include <vector>

#include "OgreVector3.h"
#include "ai/perception/AgentSpatialIndexSystem.h"
#include "objects/AgentObject.h"
#include "profiling/RuntimeProfileCounters.h"
#include "systems/manager/ObjectManager.h"

class IAgentSpatialQuery
{
public:
	virtual ~IAgentSpatialQuery() {}

	void QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents, int maxResults = 0) const
	{
		AgentSpatialQueryOptions options;
		options.maxResults = maxResults;
		QueryAgentsInRange(center, radius, outAgents, options);
	}

	virtual void QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents, const AgentSpatialQueryOptions& options) const = 0;
};

class ObjectManagerAgentSpatialQuery : public IAgentSpatialQuery
{
public:
	explicit ObjectManagerAgentSpatialQuery(ObjectManager* objectManager)
		: m_objectManager(objectManager)
	{
	}

	using IAgentSpatialQuery::QueryAgentsInRange;

	virtual void QueryAgentsInRange(const Ogre::Vector3& center, float radius, std::vector<AgentObject*>& outAgents, const AgentSpatialQueryOptions& options) const override
	{
		const long long queryStartMicros = RuntimeStallProfiler::NowMicroseconds();
		outAgents.clear();
		if (m_objectManager == nullptr)
		{
			return;
		}

		const AgentSpatialIndexSystem* spatialIndex = m_objectManager->GetAgentSpatialIndexSystem();
		if (spatialIndex != nullptr && spatialIndex->IsEnabled() && spatialIndex->IsBuilt())
		{
			spatialIndex->QueryAgentsInRange(center, radius, outAgents, options);
			return;
		}

		const std::vector<AgentObject*>& agents = m_objectManager->getAllAgents();
		const float radiusSquared = radius * radius;
		int candidates = 0;
		int filteredCandidates = 0;
		int rejectedSelf = 0;
		int rejectedTeam = 0;
		int rejectedDead = 0;
		int rejectedType = 0;
		std::vector<float> resultDistances;
		if (options.maxResults > 0)
			resultDistances.reserve(options.maxResults);
		for (AgentObject* agent : agents)
		{
			++candidates;
			if (agent == nullptr)
			{
				continue;
			}

			const float distanceSquared = center.squaredDistance(agent->GetPosition());
			if (radius > 0.0f && distanceSquared > radiusSquared)
			{
				continue;
			}

			if (!PassesQueryOptions(agent, options, rejectedSelf, rejectedTeam, rejectedDead, rejectedType))
			{
				continue;
			}

			++filteredCandidates;
			AddResultCandidate(agent, distanceSquared, options, outAgents, resultDistances);
		}
		if (spatialIndex != nullptr)
			spatialIndex->RecordFallbackQueryStats(candidates, filteredCandidates, static_cast<int>(outAgents.size()), rejectedSelf, rejectedTeam, rejectedDead, rejectedType, RuntimeStallProfiler::ElapsedMsSince(queryStartMicros));
	}

private:
	static bool PassesQueryOptions(AgentObject* agent, const AgentSpatialQueryOptions& options, int& rejectedSelf, int& rejectedTeam, int& rejectedDead, int& rejectedType)
	{
		if (agent == nullptr)
			return false;

		if (!options.includeSelf && options.owner != nullptr && agent == options.owner)
		{
			++rejectedSelf;
			return false;
		}

		if (options.requireAlive && agent->GetHealth() <= 0.0f)
		{
			++rejectedDead;
			return false;
		}

		if (options.requiredTeamId >= 0 && static_cast<int>(agent->GetTeamId()) != options.requiredTeamId)
		{
			++rejectedTeam;
			return false;
		}

		if (options.excludedTeamId >= 0 && static_cast<int>(agent->GetTeamId()) == options.excludedTeamId)
		{
			++rejectedTeam;
			return false;
		}

		if (options.requiredObjectType > 0 && static_cast<int>(agent->GetObjType()) != options.requiredObjectType)
		{
			++rejectedType;
			return false;
		}

		return true;
	}

	static void AddResultCandidate(
		AgentObject* agent,
		float distanceSquared,
		const AgentSpatialQueryOptions& options,
		std::vector<AgentObject*>& outAgents,
		std::vector<float>& outDistances)
	{
		if (options.maxResults <= 0)
		{
			outAgents.push_back(agent);
			return;
		}

		if (static_cast<int>(outAgents.size()) < options.maxResults)
		{
			outAgents.push_back(agent);
			outDistances.push_back(distanceSquared);
			return;
		}

		int farthestIndex = -1;
		float farthestDistance = -1.0f;
		for (int i = 0; i < static_cast<int>(outDistances.size()); ++i)
		{
			if (outDistances[i] > farthestDistance)
			{
				farthestDistance = outDistances[i];
				farthestIndex = i;
			}
		}

		if (farthestIndex >= 0 && distanceSquared < farthestDistance)
		{
			outAgents[farthestIndex] = agent;
			outDistances[farthestIndex] = distanceSquared;
		}
	}

	ObjectManager* m_objectManager;
};

#endif // __AGENT_SPATIAL_QUERY_H__
