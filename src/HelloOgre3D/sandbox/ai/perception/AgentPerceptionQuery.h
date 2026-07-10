#ifndef __AGENT_PERCEPTION_QUERY_H__
#define __AGENT_PERCEPTION_QUERY_H__

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include "OgreAxisAlignedBox.h"
#include "OgreEntity.h"
#include "OgreMath.h"
#include "OgreRay.h"
#include "OgreString.h"
#include "OgreVector3.h"
#include "ai/perception/AgentSpatialQuery.h"
#include "components/agent/AgentLocomotion.h"
#include "objects/AgentObject.h"
#include "objects/BlockObject.h"
#include "systems/service/NavigationService.h"

struct AgentPerceptionOptions
{
	AgentPerceptionOptions()
		: navMeshName("default")
		, maxDistance(0.0f)
		, requirePath(true)
		, fieldOfViewDegrees(0.0f)
		, maxSpatialResults(0)
	{
	}

	Ogre::String navMeshName;
	float maxDistance;
	bool requirePath;
	float fieldOfViewDegrees;
	int maxSpatialResults;
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
	AgentPerceptionQuery(ObjectManager* objectManager, NavigationService* navigation)
		: m_defaultSpatialQuery(objectManager)
		, m_spatialQuery(&m_defaultSpatialQuery)
		, m_objectManager(objectManager)
		, m_navigation(navigation)
	{
	}

	AgentPerceptionQuery(IAgentSpatialQuery* spatialQuery, NavigationService* navigation)
		: m_defaultSpatialQuery(nullptr)
		, m_spatialQuery(spatialQuery)
		, m_objectManager(nullptr)
		, m_navigation(navigation)
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

		if (!HasLineOfSight(owner, enemy))
		{
			return false;
		}

		if (options.requirePath && m_navigation != nullptr)
		{
			std::vector<Ogre::Vector3> path;
			if (!m_navigation->FindPath(options.navMeshName, owner->GetPosition(), enemy->GetPosition(), path) || path.empty())
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
		AgentSpatialQueryOptions spatialOptions;
		spatialOptions.owner = owner;
		spatialOptions.includeSelf = false;
		spatialOptions.requireAlive = true;
		spatialOptions.excludedTeamId = static_cast<int>(owner->GetTeamId());
		spatialOptions.maxResults = options.maxSpatialResults;
		m_spatialQuery->QueryAgentsInRange(owner->GetPosition(), options.maxDistance, agents, spatialOptions);
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

	bool HasLineOfSight(AgentObject* owner, AgentObject* enemy) const
	{
		if (m_objectManager == nullptr || owner == nullptr || enemy == nullptr)
			return true;

		const Ogre::Vector3 origin = GetSightOrigin(owner);
		const Ogre::Vector3 target = GetSightTarget(enemy);
		Ogre::Vector3 direction = target - origin;
		const float length = direction.length();
		if (length <= 0.001f)
			return true;

		direction /= length;
		const Ogre::Ray ray(origin, direction);
		const std::vector<BlockObject*>& blocks = m_objectManager->getAllBlocks();
		for (BlockObject* block : blocks)
		{
			if (block == nullptr)
				continue;
			if (block->GetObjType() != BaseObject::OBJ_TYPE_BLOCK)
				continue;

			Ogre::Entity* entity = block->GetEntity();
			if (entity == nullptr)
				continue;

			const Ogre::AxisAlignedBox box = entity->getWorldBoundingBox(true);
			if (box.isNull())
				continue;

			const std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, box);
			if (hit.first && hit.second > 0.05f && hit.second < length - 0.05f)
				return false;
		}

		return true;
	}

	static Ogre::Vector3 GetSightOrigin(const AgentObject* agent)
	{
		if (agent == nullptr)
			return Ogre::Vector3::ZERO;

		const AgentLocomotion* locomotion = agent->GetLocomotionComponent();
		const Ogre::Real height = locomotion != nullptr ? locomotion->GetHeight() : AgentLocomotion::DEFAULT_AGENT_HEIGHT;
		return agent->GetPosition() + Ogre::Vector3(0.0f, height * 0.5f, 0.0f);
	}

	static Ogre::Vector3 GetSightTarget(const AgentObject* agent)
	{
		return agent != nullptr ? agent->GetPosition() : Ogre::Vector3::ZERO;
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
	ObjectManager* m_objectManager;
	NavigationService* m_navigation;
};

#endif // __AGENT_PERCEPTION_QUERY_H__
