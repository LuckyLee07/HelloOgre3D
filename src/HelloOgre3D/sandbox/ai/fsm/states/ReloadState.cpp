#include "ReloadState.h"

#include "GameDefine.h"
#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"

namespace
{
	void StabilizeReloadMovement(AgentObject* agent)
	{
		if (!agent)
		{
			return;
		}

		Ogre::Vector3 velocity = agent->GetVelocity();
		const Ogre::Real yMovement = velocity.y;
		velocity.y = 0.0f;

		if (!velocity.isZeroLength())
		{
			velocity *= 0.55f;
			if (velocity.squaredLength() < 0.09f)
			{
				velocity = Ogre::Vector3::ZERO;
			}
		}

		velocity.y = yMovement;
		agent->SetVelocity(velocity);
	}
}

ReloadState::ReloadState(AgentObject* pAgent)
	: AgentState(pAgent)
	, m_elapsedMs(0.0f)
{
	m_stateId = "ReloadState";
}

ReloadState::~ReloadState()
{
}

void ReloadState::OnEnter()
{
	SetTerminated(false);
	m_elapsedMs = 0.0f;

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->EnterReload();
	}
	else if (m_pAgent)
	{
		m_pAgent->RequestState(SSTATE_RELOAD, true);
	}
}

void ReloadState::OnLeave()
{
	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->ExitReload();
	}
}

std::string ReloadState::OnUpdate(float dt)
{
	if (!m_pAgent)
		return "";

	if (m_pAgent->GetHealth() <= 0.0f)
	{
		SetTerminated(true);
		return "";
	}

	StabilizeReloadMovement(m_pAgent);

	m_elapsedMs += dt;
	if (m_elapsedMs < 120.0f)
		return "";

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		if (actions->IsReloadPresentationFinished() || (m_elapsedMs >= 2000.0f && !actions->HasPendingAnimation()))
		{
			actions->RestoreAmmo();
			SetTerminated(true);
		}
		return "";
	}

	if (!m_pAgent->HasNextAnim())
	{
		SetTerminated(true);
	}

	return "";
}
