#include "AgentStateController.h"

#include <utility>
#include <vector>

#include "AgentActionContext.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "AgentStateEvaluators.h"
#include "AgentStateFactory.h"
#include "AgentFSM.h"
#include "states/AgentLuaState.h"
#include "LogSystem.h"
#include "systems/manager/SandboxMgr.h"

namespace
{
	SoldierObject* AsSoldier(AgentObject* agent)
	{
		return dynamic_cast<SoldierObject*>(agent);
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

void AgentStateController::Init()
{
	AgentStateFactory::Init();

	AddState("IdleState");
	AddState("ShootState");
	AddState("DeathState");
	AddState("MoveState");
	AddState("ReloadState");
	AddState("PursueState");
	AddState("FleeState");
	AddState("RandomMoveState");

	AgentStateEvaluators::ConfigureSoldierTransitions(*this);

	m_fsm->SetCurrentState("IdleState");
}

void AgentStateController::Update(float dtime)
{
	if (!m_agent || !m_fsm)
		return;

	m_fsm->Update(dtime);
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
	if (!m_agent || !g_SandboxMgr)
		return false;

	std::vector<Ogre::Vector3> path;
	if (!g_SandboxMgr->FindPath(m_navMeshName, m_agent->GetPosition(), target, path) || path.empty())
		return false;

	m_agent->SetPath(path, false);
	m_agent->SetTarget(target);

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
	if (!g_SandboxMgr)
		return Ogre::Vector3::ZERO;

	return g_SandboxMgr->RandomPoint(m_navMeshName);
}

void AgentStateController::ApplySteering(float deltaTimeInSeconds, bool slowMode)
{
	if (!m_agent || deltaTimeInSeconds <= 0.0f)
		return;

	Ogre::Vector3 avoidForce = m_agent->ForceToAvoidAgents(0.5f);
	Ogre::Vector3 avoidObjectForce = m_agent->ForceToAvoidObjects(0.5f);
	Ogre::Vector3 followForce = m_agent->ForceToFollowPath(0.5f);
	Ogre::Vector3 stayForce = m_agent->ForceToStayOnPath(0.5f);

	Ogre::Vector3 totalForces = followForce * 1.5f + stayForce * 0.4f + avoidForce + avoidObjectForce * 2.0f;

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
	Ogre::Vector3 velocity = m_agent->GetVelocity() + (acceleration * deltaTimeInSeconds);
	velocity.y = 0.0f;

	m_agent->SetVelocity(velocity);

	if (!velocity.isZeroLength())
	{
		m_agent->SetForward(velocity);
	}

	const Ogre::Real maxSpeed = m_agent->GetMaxSpeed() * (slowMode ? 0.6f : 1.0f);
	if (!velocity.isZeroLength() && velocity.squaredLength() > (maxSpeed * maxSpeed))
	{
		Ogre::Vector3 clamped = velocity.normalisedCopy() * maxSpeed;
		clamped.y = 0.0f;
		m_agent->SetVelocity(clamped);
	}
}