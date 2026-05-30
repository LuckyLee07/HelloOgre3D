#ifndef __AGENT_PERCEPTION_QUERY_H__
#define __AGENT_PERCEPTION_QUERY_H__

#include <limits>
#include <vector>

#include "OgreString.h"
#include "OgreVector3.h"
#include "objects/AgentObject.h"
#include "systems/manager/ObjectManager.h"
#include "systems/manager/SandboxMgr.h"

class IAgentPerceptionQuery
{
public:
	virtual ~IAgentPerceptionQuery() {}

	virtual bool IsEnemyValid(AgentObject* owner, AgentObject* enemy, const Ogre::String& navMeshName, bool requirePath) const = 0;
	virtual AgentObject* FindNearestEnemy(AgentObject* owner, const Ogre::String& navMeshName) const = 0;
};

class AgentPerceptionQuery : public IAgentPerceptionQuery
{
public:
	AgentPerceptionQuery(ObjectManager* objectManager, SandboxMgr* sandbox)
		: m_objectManager(objectManager)
		, m_sandbox(sandbox)
	{
	}

	virtual bool IsEnemyValid(AgentObject* owner, AgentObject* enemy, const Ogre::String& navMeshName, bool requirePath) const override
	{
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

		if (!requirePath || m_sandbox == nullptr)
		{
			return true;
		}

		std::vector<Ogre::Vector3> path;
		return m_sandbox->FindPath(navMeshName, owner->GetPosition(), enemy->GetPosition(), path) && !path.empty();
	}

	virtual AgentObject* FindNearestEnemy(AgentObject* owner, const Ogre::String& navMeshName) const override
	{
		if (owner == nullptr || m_objectManager == nullptr)
		{
			return nullptr;
		}

		const std::vector<AgentObject*>& agents = m_objectManager->getAllAgents();
		AgentObject* nearestEnemy = nullptr;
		float nearestDistance = std::numeric_limits<float>::max();

		for (AgentObject* candidate : agents)
		{
			if (!IsEnemyValid(owner, candidate, navMeshName, true))
			{
				continue;
			}

			const float distSquared = owner->GetPosition().squaredDistance(candidate->GetPosition());
			if (distSquared >= nearestDistance)
			{
				continue;
			}

			nearestEnemy = candidate;
			nearestDistance = distSquared;
		}

		return nearestEnemy;
	}

private:
	ObjectManager* m_objectManager;
	SandboxMgr* m_sandbox;
};

#endif // __AGENT_PERCEPTION_QUERY_H__
