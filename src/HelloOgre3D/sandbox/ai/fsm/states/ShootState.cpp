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
	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	if (actions)
	{
		actions->ExitShoot();
	}
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
		AgentObject* enemy = actions->GetEnemy();
		if (enemy == nullptr || enemy->GetHealth() <= 0.0f)
		{
			SetTerminated(true);
			return "";
		}

		actions->SlowMovement();
		actions->StabilizeStationaryMovement(0.65f, 0.95f);
		actions->FaceEnemy();

		if (!m_shotConsumed && actions->ConsumeShootExecution())
		{
			actions->ConsumeAmmo(1);
			m_shotConsumed = true;
		}

		if (actions->IsShootPresentationFinished())
		{
			if (!m_shotConsumed)
			{
				actions->ConsumeAmmo(1);
				m_shotConsumed = true;
			}
			SetTerminated(true);
			return "";
		}

		if (m_elapsedMs >= 800.0f && !actions->HasPendingAnimation())
		{
			if (!m_shotConsumed)
			{
				actions->ConsumeAmmo(1);
				m_shotConsumed = true;
			}
			SetTerminated(true);
		}
		return "";
	}
	else if (m_pAgent->IsMoving())
	{
		m_pAgent->SlowMoving();
	}

	const bool shootReady = m_pAgent->IsAnimReadyForShoot();
	const bool hasPendingAnim = m_pAgent->HasNextAnim();
	if (!shootReady)
	{
		if (!hasPendingAnim)
		{
			m_pAgent->RequestState(SSTATE_FIRE, true);
		}
		return "";
	}

	if ((m_elapsedMs >= 120.0f && !hasPendingAnim) || m_elapsedMs >= 350.0f)
	{
		SetTerminated(true);
	}

	return "";
}
