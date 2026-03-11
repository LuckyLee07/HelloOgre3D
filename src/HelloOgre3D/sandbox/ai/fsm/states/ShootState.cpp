#include "ShootState.h"

#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "GameDefine.h"

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

	if (m_pAgent)
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

	if (m_pAgent->IsMoving())
	{
		m_pAgent->SlowMoving();
	}

	if (!m_pAgent->IsAnimReadyForShoot())
	{
		if (!m_pAgent->HasNextAnim())
		{
			m_pAgent->RequestState(SSTATE_FIRE, true);
		}
		return "";
	}

	if ((m_elapsedMs >= 120.0f && !m_pAgent->HasNextAnim()) || m_elapsedMs >= 350.0f)
	{
		SoldierObject* soldier = dynamic_cast<SoldierObject*>(m_pAgent);
		if (!m_shotConsumed && soldier)
		{
			soldier->ConsumeAmmo(1);
			m_shotConsumed = true;
		}

		SetTerminated(true);
	}

	return "";
}
