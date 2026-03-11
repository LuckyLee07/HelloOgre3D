#include "ReloadState.h"

#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "GameDefine.h"

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

	if (m_pAgent)
	{
		m_pAgent->RequestState(SSTATE_RELOAD, true);
	}
}

void ReloadState::OnLeave()
{
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

	m_elapsedMs += dt;
	if (m_elapsedMs < 120.0f)
		return "";

	if (!m_pAgent->HasNextAnim())
	{
		SoldierObject* soldier = dynamic_cast<SoldierObject*>(m_pAgent);
		if (soldier)
		{
			soldier->RestoreAmmo();
		}
		SetTerminated(true);
	}

	return "";
}
