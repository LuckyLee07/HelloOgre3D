#include "IdleState.h"
#include "object/AgentObject.h"
#include "AgentTypeDef.h"

IdleState::IdleState(AgentObject* pAgent) : AgentState(pAgent)
{
	m_stateId = "IdleState";
}

IdleState::~IdleState()
{
	m_pAgent = nullptr;
}

void IdleState::OnEnter()
{
	if (m_pAgent->IsMoving())
	{
		m_pAgent->SlowMoving(2.0f);
	}
	m_pAgent->RequestState(SSTATE_IDLE_AIM);
}

void IdleState::OnLeave()
{

}

std::string IdleState::OnUpdate(float dt)
{
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
