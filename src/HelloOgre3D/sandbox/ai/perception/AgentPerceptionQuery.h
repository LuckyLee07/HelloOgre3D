#ifndef __AGENT_PERCEPTION_QUERY_H__
#define __AGENT_PERCEPTION_QUERY_H__

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include "OgreString.h"
#include "OgreVector3.h"
#include "ai/perception/AgentSpatialQuery.h"
#include "objects/AgentObject.h"
#include "systems/manager/SandboxMgr.h"

struct AgentPerceptionOptions
{
	AgentPerceptionOptions()
		: navMeshName("default")
		, maxDistance(0.0f)
		, requirePath(true)
		, fieldOfViewDegrees(0.0f)
	{
	}

	Ogre::String navMeshName;
	float maxDistance;
	bool requirePath;
	float fieldOfViewDegrees;
};

struct AgentPerceptionResult
{
	AgentPerceptionResult()
		: target(nullptr)
		, targetId(-1)
		, targetPosition(Ogre::Vector3::ZERO)
		, distanceSquared(0.0f)
		, confidence(1.0f)
	{
	}

	AgentObject* target;
	int targetId;
	Ogre::Vector3 targetPosition;
	float distanceSquared;
	float confidence;
};

class IAgentPerceptionQuery
{
public:
	virtual ~IAgentPerceptionQuery() {}

	virtual bool IsEnemyValid(AgentObject* owner, AgentObject* enemy, const Ogre::String& navMeshName, bool requirePath) const = 0;
	virtual AgentObject* FindNearestEnemy(AgentObject* owner, const Ogre::String& navMeshName) const = 0;
	virtual bool TryGetEnemy(AgentObject* owner, AgentObject* enemy, const AgentPerceptionOptions& options, AgentPerceptionResult& result) const = 0;
	virtual bool FindNearestEnemy(AgentObject* owner, const AgentPerceptionOptions& options, AgentPerceptionResult& result) const = 0;
};

class AgentPerceptionQuery : public IAgentPerceptionQuery
{
public:
	AgentPerceptionQuery(ObjectManager* objectManager, SandboxMgr* sandbox)
		: m_defaultSpatialQuery(objectManager)
		, m_spatialQuery(&m_defaultSpatialQuery)
		, m_sandbox(sandbox)
	{
	}

	AgentPerceptionQuery(IAgentSpatialQuery* spatialQuery, SandboxMgr* sandbox)
		: m_defaultSpatialQuery(nullptr)
		, m_spatialQuery(spatialQuery)
		, m_sandbox(sandbox)
	{
	}

	virtual bool IsEnemyValid(AgentObject* owner, AgentObject* enemy, const Ogre::String& navMeshName, bool requirePath) const override
	{
		AgentPerceptionOptions options;
		options.navMeshName = navMeshName;
		options.requirePath = requirePath;
		AgentPerceptionResult result;
		return TryGetEnemy(owner, enemy, options, result);
	}

	virtual AgentObject* FindNearestEnemy(AgentObject* owner, const Ogre::String& navMeshName) const override
	{
		AgentPerceptionOptions options;
		options.navMeshName = navMeshName;
		AgentPerceptionResult result;
		return FindNearestEnemy(owner, options, result) ? result.target : nullptr;
	}

	virtual bool TryGetEnemy(AgentObject* owner, AgentObject* enemy, const AgentPerceptionOptions& options, AgentPerceptionResult& result) const override
	{
		result = AgentPerceptionResult();
		if (owner == nullptr || enemy == nullptr || enemy == owner)
		{
			return false;
		}

		if (enemy->GetHealth() <= 0.0f)
		{
			return false;
		}

		if (enemy->GetTeamId() == owner->GetTeamId())
		{
			return false;
		}

		const float distanceSquared = owner->GetPosition().squaredDistance(enemy->GetPosition());
		if (options.maxDistance > 0.0f && distanceSquared > options.maxDistance * options.maxDistance)
		{
			return false;
		}

		if (!PassesFieldOfView(owner, enemy, options.fieldOfViewDegrees))
		{
			return false;
		}

		if (options.requirePath && m_sandbox != nullptr)
		{
			std::vector<Ogre::Vector3> path;
			if (!m_sandbox->FindPath(options.navMeshName, owner->GetPosition(), enemy->GetPosition(), path) || path.empty())
			{
				return false;
			}
		}

		result.target = enemy;
		result.targetId = static_cast<int>(enemy->GetObjId());
		result.targetPosition = enemy->GetPosition();
		result.distanceSquared = distanceSquared;
		result.confidence = ComputeConfidence(distanceSquared, options.maxDistance);
		return true;
	}

	virtual bool FindNearestEnemy(AgentObject* owner, const AgentPerceptionOptions& options, AgentPerceptionResult& result) const override
	{
		result = AgentPerceptionResult();
		if (owner == nullptr || m_spatialQuery == nullptr)
		{
			return false;
		}

		std::vector<AgentObject*> agents;
		m_spatialQuery->QueryAgentsInRange(owner->GetPosition(), options.maxDistance, agents);
		float nearestDistance = std::numeric_limits<float>::max();

		for (AgentObject* candidate : agents)
		{
			AgentPerceptionResult candidateResult;
			if (!TryGetEnemy(owner, candidate, options, candidateResult))
			{
				continue;
			}

			if (candidateResult.distanceSquared >= nearestDistance)
			{
				continue;
			}

			result = candidateResult;
			nearestDistance = candidateResult.distanceSquared;
		}

		return result.target != nullptr;
	}

private:
	static float Clamp01(float value)
	{
		if (value < 0.0f)
			return 0.0f;
		if (value > 1.0f)
			return 1.0f;
		return value;
	}

	static bool PassesFieldOfView(AgentObject* owner, AgentObject* enemy, float fieldOfViewDegrees)
	{
		if (fieldOfViewDegrees <= 0.0f || fieldOfViewDegrees >= 360.0f)
			return true;

		Ogre::Vector3 toEnemy = enemy->GetPosition() - owner->GetPosition();
		toEnemy.y = 0.0f;
		if (toEnemy.squaredLength() <= 0.0001f)
			return true;

		Ogre::Vector3 forward = owner->GetForward();
		forward.y = 0.0f;
		if (forward.squaredLength() <= 0.0001f)
			return true;

		toEnemy.normalise();
		forward.normalise();
		const float halfRadians = fieldOfViewDegrees * 0.5f * 3.1415926535f / 180.0f;
		return forward.dotProduct(toEnemy) >= std::cos(halfRadians);
	}

	static float ComputeConfidence(float distanceSquared, float maxDistance)
	{
		if (maxDistance <= 0.0f)
			return 1.0f;
		const float distance = std::sqrt(std::max(0.0f, distanceSquared));
		return Clamp01(1.0f - distance / maxDistance);
	}

	ObjectManagerAgentSpatialQuery m_defaultSpatialQuery;
	IAgentSpatialQuery* m_spatialQuery;
	SandboxMgr* m_sandbox;
};

#endif // __AGENT_PERCEPTION_QUERY_H__
