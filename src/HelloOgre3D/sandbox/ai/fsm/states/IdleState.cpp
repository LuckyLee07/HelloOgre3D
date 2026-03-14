#include "IdleState.h"
#include "GameDefine.h"
#include "objects/AgentObject.h"

IdleState::IdleState(AgentObject* pAgent) : AgentState(pAgent)
{
	m_stateId = "IdleState";
}

IdleState::~IdleState()
{
}

void IdleState::OnEnter()
{
	AgentState::OnEnter();
}

void IdleState::OnLeave()
{
	AgentState::OnLeave();
}

std::string IdleState::OnUpdate(float dt)
{
	(void)dt;

	auto pInput = m_pAgent->GetInput();
	if (pInput->isKeyDown(OIS::KC_2))
	{
		return "toShoot";
	}
	else if (pInput->isKeyDown(OIS::KC_3))
	{
		return "toMove";
	}
	else if (pInput->isKeyDown(OIS::KC_4))
	{
		return "toDeath";
	}

	return "";
}
