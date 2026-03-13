#include "ReloadState.h"

#include "GameDefine.h"
#include "ai/fsm/AgentActionContext.h"
#include "ai/fsm/AgentStateController.h"
#include "objects/AgentObject.h"

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

	m_elapsedMs += dt;
	if (m_elapsedMs < 120.0f)
		return "";

	AgentActionContext* actions = m_controller ? m_controller->GetActionContext() : nullptr;
	const bool reloadFinished = actions ? actions->IsReloadPresentationFinished() : !m_pAgent->HasNextAnim();
	if (reloadFinished)
	{
		if (actions)
		{
			actions->RestoreAmmo();
		}
		SetTerminated(true);
	}

	return "";
}