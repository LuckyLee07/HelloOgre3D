#include "AIController.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <string>
#include <vector>

#include "GameFunction.h"
#include "SandboxMacros.h"
#include "ai/behavior/BehaviorTreeDriver.h"
#include "ai/common/Blackboard.h"
#include "ai/common/IDecisionDriver.h"
#include "ai/decision/DecisionTreeDriver.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "profiling/Profile.h"
#include "systems/manager/ObjectManager.h"
#include "systems/manager/SandboxMgr.h"

AIController::AIController(SoldierObject* owner)
	: m_owner(owner)
	, m_driver(nullptr)
	, m_enemy(nullptr)
	, m_enemyId(-1)
	, m_hasMovePos(false)
	, m_movePos(Ogre::Vector3::ZERO)
	, m_tickInOwnerUpdateEnabled(true)
{
	InitDefaultDriver();
}

AIController::~AIController()
{
	SAFE_DELETE(m_driver);
}

void AIController::onAttach(GameObject* owner)
{
	IComponent::onAttach(owner);
	if (m_owner == nullptr)
	{
		m_owner = dynamic_cast<SoldierObject*>(getOwner());
	}
	InitDefaultDriver();
}

void AIController::onDetach()
{
	m_owner = nullptr;
	IComponent::onDetach();
}

void AIController::InitDefaultDriver()
{
	if (m_owner == nullptr || m_driver != nullptr)
	{
		return;
	}

	if (m_owner->GetUseCppFSM())
	{
		AgentStateController* fsm = new AgentStateController(m_owner);
		fsm->Init();
		m_driver = fsm;
	}
}

unsigned int AIController::GetAgentId() const
{
	return m_owner != nullptr ? m_owner->GetObjId() : 0;
}

void AIController::TickAI(int deltaMs)
{
	if (m_owner == nullptr)
	{
		return;
	}

	H3D_PROFILE_SCOPE("AIController::TickAI");
	static int totalMilisec = 0;
	totalMilisec += deltaMs;

	bool forceUpdate = true;
	if (forceUpdate || totalMilisec > 1000)
	{
		totalMilisec = 0;
		H3D_PROFILE_SCOPE("Lua::Agent_Update");
		m_owner->callFunction("Agent_Update", "u[SoldierObject]i", m_owner, deltaMs);
	}

	if (m_driver)
	{
		H3D_PROFILE_SCOPE("IDecisionDriver::Tick");
		m_driver->Tick((float)deltaMs);
	}
}

void AIController::SetTickInOwnerUpdateEnabled(bool enabled)
{
	m_tickInOwnerUpdateEnabled = enabled;
}

Blackboard* AIController::GetBlackboard() const
{
	BehaviorTreeDriver* behaviorDriver = GetBehaviorTreeDriver();
	if (behaviorDriver != nullptr)
	{
		return behaviorDriver->GetBlackboard();
	}

	DecisionTreeDriver* decisionDriver = GetDecisionTreeDriver();
	if (decisionDriver != nullptr)
	{
		return decisionDriver->GetBlackboard();
	}

	return nullptr;
}

bool AIController::IsEnemyValid(AgentObject* enemy, const Ogre::String& navMeshName, bool requirePath) const
{
	if (m_owner == nullptr || !enemy || enemy == m_owner)
	{
		return false;
	}

	if (enemy->GetHealth() <= 0.0f)
	{
		return false;
	}

	unsigned int enemyTeamId = enemy->GetTeamId();
	if (enemyTeamId == m_owner->GetTeamId())
	{
		return false;
	}

	if (!requirePath || !g_SandboxMgr)
	{
		return true;
	}

	std::vector<Ogre::Vector3> path;
	return g_SandboxMgr->FindPath(navMeshName, m_owner->GetPosition(), enemy->GetPosition(), path) && !path.empty();
}

AgentObject* AIController::FindNearestEnemy(const Ogre::String& navMeshName)
{
	if (m_owner == nullptr || !g_ObjectManager)
	{
		return nullptr;
	}

	const std::vector<AgentObject*>& agents = g_ObjectManager->getAllAgents();

	AgentObject* nearestEnemy = nullptr;
	float nearestDistance = std::numeric_limits<float>::max();

	for (AgentObject* candidate : agents)
	{
		if (!IsEnemyValid(candidate, navMeshName, true))
		{
			continue;
		}

		const float distSquared = m_owner->GetPosition().squaredDistance(candidate->GetPosition());
		if (distSquared >= nearestDistance)
		{
			continue;
		}

		nearestEnemy = candidate;
		nearestDistance = distSquared;
	}

	return nearestEnemy;
}

void AIController::SetEnemy(AgentObject* enemy)
{
	m_enemy = enemy;
	m_enemyId = enemy ? static_cast<int>(enemy->GetObjId()) : -1;
}

AgentObject* AIController::GetEnemy() const
{
	if (m_enemy == nullptr || m_enemyId < 0 || !g_ObjectManager)
	{
		return m_enemy;
	}

	BaseObject* object = g_ObjectManager->getObjectById(m_enemyId);
	return object == m_enemy ? dynamic_cast<AgentObject*>(object) : nullptr;
}

bool AIController::HasEnemy(const Ogre::String& navMeshName)
{
	AgentObject* enemy = GetEnemy();
	if (IsEnemyValid(enemy, navMeshName, true))
	{
		return true;
	}

	SetEnemy(FindNearestEnemy(navMeshName));
	return GetEnemy() != nullptr;
}

bool AIController::CanShootEnemy(const Ogre::String& navMeshName, float shootDistance)
{
	if (m_owner == nullptr)
	{
		return false;
	}

	AgentObject* enemy = GetEnemy();
	if (!IsEnemyValid(enemy, navMeshName, false))
	{
		return false;
	}

	const float distance = std::max(0.0f, shootDistance);
	const float shootSquared = distance * distance;
	const float distSquared = m_owner->GetPosition().squaredDistance(enemy->GetPosition());
	return distSquared < shootSquared;
}

bool AIController::HasMovePosition(float reachDistance) const
{
	if (m_owner == nullptr)
	{
		return false;
	}

	const float distance = std::max(0.0f, reachDistance);
	const float reachSquared = distance * distance;

	if (m_hasMovePos)
	{
		return m_owner->GetPosition().squaredDistance(m_movePos) > reachSquared;
	}

	if (!m_owner->HasPath())
	{
		return false;
	}

	const Ogre::Vector3 target = m_owner->GetTarget();
	return m_owner->GetPosition().squaredDistance(target) > reachSquared;
}

void AIController::SetMovePosition(const Ogre::Vector3& movePos)
{
	m_movePos = movePos;
	m_hasMovePos = true;
}

void AIController::ClearMovePosition()
{
	m_hasMovePos = false;
	m_movePos = Ogre::Vector3::ZERO;
}

bool AIController::IsTargetReached(float threshold) const
{
	if (m_owner == nullptr)
	{
		return false;
	}

	const float distance = std::max(0.0f, threshold);
	const float thresholdSquared = distance * distance;

	if (m_hasMovePos)
	{
		return m_owner->GetPosition().squaredDistance(m_movePos) < thresholdSquared;
	}

	if (!m_owner->HasPath())
	{
		return false;
	}

	const Ogre::Vector3 target = m_owner->GetTarget();
	return m_owner->GetPosition().squaredDistance(target) < thresholdSquared;
}

void AIController::UseDecisionTreeDriver()
{
	SetDriverByType("dt");
}

DecisionTreeDriver* AIController::GetDecisionTreeDriver() const
{
	return dynamic_cast<DecisionTreeDriver*>(m_driver);
}

void AIController::UseBehaviorTreeDriver()
{
	SetDriverByType("bt");
}

BehaviorTreeDriver* AIController::GetBehaviorTreeDriver() const
{
	return dynamic_cast<BehaviorTreeDriver*>(m_driver);
}

AgentStateController* AIController::GetFsmController() const
{
	return dynamic_cast<AgentStateController*>(m_driver);
}

void AIController::SetDriverByType(const char* type)
{
	if (m_owner == nullptr || type == nullptr)
	{
		return;
	}

	std::string key(type);
	std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});

	if (key == "fsm")
	{
		SetFsmDriver();
	}
	else if (key == "dt")
	{
		SetDecisionTreeDriver();
	}
	else if (key == "bt")
	{
		SetBehaviorTreeDriver();
	}
}

void AIController::SetFsmDriver()
{
	if (m_owner == nullptr)
	{
		return;
	}

	if (dynamic_cast<AgentStateController*>(m_driver) != nullptr)
	{
		return;
	}
	SAFE_DELETE(m_driver);

	AgentStateController* fsm = new AgentStateController(m_owner);
	fsm->Init();
	m_driver = fsm;
}

void AIController::SetDecisionTreeDriver()
{
	if (m_owner == nullptr)
	{
		return;
	}

	if (dynamic_cast<DecisionTreeDriver*>(m_driver) != nullptr)
	{
		return;
	}
	SAFE_DELETE(m_driver);

	DecisionTreeDriver* dt = new DecisionTreeDriver(m_owner);
	dt->Init();
	m_driver = dt;
}

void AIController::SetBehaviorTreeDriver()
{
	if (m_owner == nullptr)
	{
		return;
	}

	if (dynamic_cast<BehaviorTreeDriver*>(m_driver) != nullptr)
	{
		return;
	}
	SAFE_DELETE(m_driver);

	BehaviorTreeDriver* bt = new BehaviorTreeDriver(m_owner);
	bt->Init();
	m_driver = bt;
}
