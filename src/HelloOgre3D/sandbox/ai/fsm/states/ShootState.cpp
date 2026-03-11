#include "ShootState.h"

#include "GameDefine.h"
#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"

ShootState::ShootState(AgentObject* pAgent)
	: AgentState(pAgent)
	, m_elapsedMs(0.0f)
	, m_shotConsumed(false)
{
	m_stateId = "ShootState";
}

ShootState::~ShootState()
{
}

void ShootState::OnEnter()
{
	SetTerminated(false);
	m_elapsedMs = 0.0f;
	m_shotConsumed = false;

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->EnterShoot();
	}
	else if (m_pAgent)
	{
		m_pAgent->RequestState(SSTATE_FIRE, true);
	}
}

void ShootState::OnLeave()
{
}

std::string ShootState::OnUpdate(float dt)
{
	if (!m_pAgent)
		return "";

	if (m_pAgent->GetHealth() <= 0.0f)
	{
		SetTerminated(true);
		return "";
	}

	m_elapsedMs += dt;

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->SlowMovement();
		actions->FaceEnemy();
	}
	else if (m_pAgent->IsMoving())
	{
		m_pAgent->SlowMoving();
	}

	const bool shootReady = actions ? actions->IsShootAnimationReady() : m_pAgent->IsAnimReadyForShoot();
	const bool hasPendingAnim = actions ? actions->HasPendingAnimation() : m_pAgent->HasNextAnim();
	if (!shootReady)
	{
		if (!hasPendingAnim)
		{
			if (actions)
			{
				actions->EnterShoot();
			}
			else
			{
				m_pAgent->RequestState(SSTATE_FIRE, true);
			}
		}
		return "";
	}

	if ((m_elapsedMs >= 120.0f && !hasPendingAnim) || m_elapsedMs >= 350.0f)
	{
		if (!m_shotConsumed)
		{
			if (actions)
			{
				actions->ConsumeAmmo(1);
			}
			m_shotConsumed = true;
		}

		SetTerminated(true);
	}

	return "";
}