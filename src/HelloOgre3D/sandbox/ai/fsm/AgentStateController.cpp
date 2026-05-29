#include "AgentStateController.h"

#include <utility>
#include <vector>

#include "AgentActionContext.h"
#include "core/SandboxServices.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "AgentStateEvaluators.h"
#include "AgentStateFactory.h"
#include "AgentFSM.h"
#include "states/AgentLuaState.h"
#include "LogSystem.h"
#include "systems/manager/ObjectManager.h"
#include "systems/manager/SandboxMgr.h"

namespace
{
	ObjectManager* ResolveObjectManager(const AgentObject* agent)
	{
		const SandboxServices* services = agent != nullptr ? agent->GetSandboxServices() : nullptr;
		if (services != nullptr && services->objects != nullptr)
			return services->objects;
		return g_ObjectManager;
	}

	SandboxMgr* ResolveSandboxMgr(const AgentObject* agent)
	{
		const SandboxServices* services = agent != nullptr ? agent->GetSandboxServices() : nullptr;
		if (services != nullptr && services->sandbox != nullptr)
			return services->sandbox;
		return g_SandboxMgr;
	}

	SoldierObject* AsSoldier(AgentObject* agent)
	{
		return dynamic_cast<SoldierObject*>(agent);
	}

	void AlignAgentToPath(AgentObject* agent)
	{
		if (!agent || !agent->HasPath())
			return;

		const Ogre::Vector3 nearest = agent->GetNearestPointOnPath(agent->GetPosition());
		const Ogre::Real distance = agent->GetDistanceAlongPath(nearest);
		Ogre::Vector3 pointOnPath = agent->GetPointOnPath(distance + 2.0f);
		Ogre::Vector3 forward = pointOnPath - agent->GetPosition();
		forward.y = 0.0f;

		if (forward.isZeroLength())
			return;

		if (forward.dotProduct(agent->GetForward()) < 0.0f)
		{
			agent->SetVelocity(forward * agent->GetSpeed());
			agent->SetForward(forward);
		}
	}
}

AgentStateController::AgentStateController(AgentObject* soldier)
	: m_fsm(new AgentFSM())
	, m_agent(soldier)
	, m_actionContext(new AgentActionContext(*this))
	, m_navMeshName("default")
{
}

AgentStateController::~AgentStateController()
{
	delete m_actionContext;
	m_actionContext = nullptr;

	delete m_fsm;
	m_fsm = nullptr;
}

#define USE_CPP_STATE 1
void AgentStateController::Init()
{
	AgentStateFactory::Init();

#if USE_CPP_STATE
	AddState("IdleState");
	AddState("ShootState");
	AddState("DeathState");
	AddState("MoveState");
#else
	AddStateExByLua("IdleState", "res/scripts/agent/states/IdleState.lua");
	AddStateExByLua("ShootState", "res/scripts/agent/states/ShootState.lua");
	AddStateExByLua("DeathState", "res/scripts/agent/states/DeathState.lua");
	AddStateExByLua("MoveState", "res/scripts/agent/states/MoveState.lua");
#endif // USE_CPP_STATE

	AddState("ReloadState");
	AddState("PursueState");
	AddState("FleeState");
	AddState("RandomMoveState");

	AgentStateEvaluators::ConfigureSoldierTransitions(*this);

	m_fsm->SetCurrentState("IdleState");
}

void AgentStateController::Tick(float deltaMs)
{
	if (!m_agent || !m_fsm)
		return;

	m_fsm->Update(deltaMs);
}

void AgentStateController::ChangeState(const std::string& stateName)
{
	if (m_fsm)
		m_fsm->RequestState(stateName);
}

AgentState* AgentStateController::GetCurrState() const
{
	if (!m_fsm)
		return nullptr;

	return m_fsm->GetCurrState();
}

bool AgentStateController::AddState(const std::string& name)
{
	AgentState* state = AgentStateFactory::Create(name.c_str(), m_agent);
	if (state == nullptr)
	{
		CCLOG_INFO("AgentStateController: failed to create state: %s\n", name.c_str());
		return false;
	}

	state->SetController(this);
	m_fsm->AddState(state);
	return true;
}

bool AgentStateController::AddStateExByLua(const std::string& name, const std::string& filepath)
{
	AgentState* state = AgentStateFactory::Create("AgentLuaState", m_agent);
	if (state == nullptr)
	{
		CCLOG_INFO("AgentStateController: failed to create state: %s\n", name.c_str());
		return false;
	}

	AgentLuaState* pState = static_cast<AgentLuaState*>(state);
	pState->SetStateId(name);
	pState->SetController(this);
	pState->BindToScript(filepath);

	m_fsm->AddState(pState);
	return true;
}

void AgentStateController::AddTransition(const std::string& from, const std::string& to)
{
	m_fsm->AddTransition(from, to);
}

void AgentStateController::AddTransitionByEvaluator(const std::string& from, const std::string& to, std::function<bool()> evaluator)
{
	m_fsm->AddTransitionEvaluator(from, to, std::move(evaluator));
}

bool AgentStateController::PlanPathTo(const Ogre::Vector3& target, bool updateMovePos)
{
	SandboxMgr* sandbox = ResolveSandboxMgr(m_agent);
	if (!m_agent || sandbox == nullptr)
		return false;

	std::vector<Ogre::Vector3> path;
	if (!sandbox->FindPath(m_navMeshName, m_agent->GetPosition(), target, path) || path.empty())
		return false;

	m_agent->SetPath(path, false);
	m_agent->SetTarget(target);
	AlignAgentToPath(m_agent); // �л�Ŀ��ʱֱ��ת��

	if (updateMovePos)
	{
		SoldierObject* soldier = AsSoldier(m_agent);
		if (soldier)
		{
			soldier->SetMovePosition(target);
		}
	}

	return true;
}

bool AgentStateController::PlanPathToEnemy()
{
	SoldierObject* soldier = AsSoldier(m_agent);
	if (!soldier)
		return false;

	if (!soldier->HasEnemy(m_navMeshName))
		return false;

	AgentObject* enemy = soldier->GetEnemy();
	if (!enemy)
		return false;

	return PlanPathTo(enemy->GetPosition(), true);
}

Ogre::Vector3 AgentStateController::RandomPoint() const
{
	SandboxMgr* sandbox = ResolveSandboxMgr(m_agent);
	if (sandbox == nullptr)
		return Ogre::Vector3::ZERO;

	return sandbox->RandomPoint(m_navMeshName);
}

void AgentStateController::ApplySteering(float deltaTimeInSeconds, bool slowMode)
{
	if (!m_agent || deltaTimeInSeconds <= 0.0f)
		return;

	Ogre::Vector3 avoidForce = m_agent->ForceToAvoidAgents(0.5f);
	Ogre::Vector3 avoidObjectForce = m_agent->ForceToAvoidObjects(0.5f);
	Ogre::Vector3 followForce = m_agent->ForceToFollowPath(0.5f);
	Ogre::Vector3 stayForce = m_agent->ForceToStayOnPath(0.5f);
	Ogre::Vector3 separationForce = Ogre::Vector3::ZERO;
	ObjectManager* objectManager = ResolveObjectManager(m_agent);
	if (objectManager != nullptr)
	{
		separationForce = m_agent->ForceToSeparate(objectManager->getAllAgents(), 1.25f, 180.0f);
	}

	Ogre::Vector3 totalForces =
		followForce * 1.5f +
		stayForce * 0.4f +
		avoidForce +
		separationForce * 1.25f +
		avoidObjectForce * 2.0f;

	const Ogre::Real targetSpeed = m_agent->GetMaxSpeed() * (slowMode ? 0.6f : 1.0f);
	if (m_agent->GetSpeed() < targetSpeed)
	{
		const Ogre::Vector3 speedForce = m_agent->ForceToTargetSpeed(targetSpeed);
		totalForces += speedForce * (slowMode ? 5.0f : 7.0f);
	}

	if (totalForces.squaredLength() < 0.1f)
		return;
	if (m_agent->GetMass() <= 0.0f)
		return;

	totalForces.y = 0.0f;
	if (totalForces.isZeroLength())
		return;

	totalForces.normalise();
	totalForces *= m_agent->GetMaxForce();

	m_agent->ApplyForce(totalForces);

	const Ogre::Vector3 acceleration = totalForces / m_agent->GetMass();
	const Ogre::Vector3 currentVelocity = m_agent->GetVelocity();
	Ogre::Vector3 velocity = currentVelocity + (acceleration * deltaTimeInSeconds);
	m_agent->SetVelocity(velocity);

	Ogre::Vector3 horizontalVelocity = velocity;
	horizontalVelocity.y = 0.0f;
	if (!horizontalVelocity.isZeroLength())
	{
		m_agent->SetForward(horizontalVelocity);
	}

	const Ogre::Real maxSpeed = m_agent->GetMaxSpeed() * (slowMode ? 0.6f : 1.0f);
	if (!horizontalVelocity.isZeroLength() && horizontalVelocity.squaredLength() > (maxSpeed * maxSpeed))
	{
		Ogre::Vector3 clampedHorizontal = horizontalVelocity.normalisedCopy() * maxSpeed;
		Ogre::Vector3 clamped = velocity;
		clamped.x = clampedHorizontal.x;
		clamped.z = clampedHorizontal.z;
		m_agent->SetVelocity(clamped);
	}
}
