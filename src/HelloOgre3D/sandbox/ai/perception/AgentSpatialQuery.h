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
		for (AgentObject* agent : agents)
		{
			++candidates;
			if (agent == nullptr)
			{
				continue;
			}

			if (radius > 0.0f && center.squaredDistance(agent->GetPosition()) > radiusSquared)
			{
				continue;
			}

			if (!PassesQueryOptions(agent, options, rejectedSelf, rejectedTeam, rejectedDead, rejectedType))
			{
				continue;
			}

			++filteredCandidates;
			outAgents.push_back(agent);
			if (options.maxResults > 0 && static_cast<int>(outAgents.size()) >= options.maxResults)
			{
				if (spatialIndex != nullptr)
					spatialIndex->RecordFallbackQueryStats(candidates, filteredCandidates, static_cast<int>(outAgents.size()), rejectedSelf, rejectedTeam, rejectedDead, rejectedType, RuntimeStallProfiler::ElapsedMsSince(queryStartMicros));
				return;
			}
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

	ObjectManager* m_objectManager;
};

#endif // __AGENT_SPATIAL_QUERY_H__
