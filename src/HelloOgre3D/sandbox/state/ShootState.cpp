#include "ShootState.h"
#include "object/AgentObject.h"
#include "AgentTypeDef.h"

ShootState::ShootState(AgentObject* pAgent) : AgentState(pAgent)
{
	m_stateId = "ShootState";
}

ShootState::~ShootState()
{

}

void ShootState::OnEnter()
{
	m_pAgent->RequestState(SSTATE_FIRE);
}

void ShootState::OnLeave()
{

}

std::string ShootState::OnUpdate(float dt)
{
	if (m_pAgent->IsMoving())
	{
		m_pAgent->SlowMoving();
	}
	if (!m_pAgent->IsAnimReadyForShoot())
	{
		if (!m_pAgent->IsHasNextAnim())
		{
			m_pAgent->RequestState(SSTATE_FIRE);
		}
		return ""; // �����ȴ�
	}

	auto pInput = m_pAgent->GetInput();
	if (pInput->isKeyDown(OIS::KC_2))
	{
		return "";
	}
	else if (pInput->isKeyDown(OIS::KC_3))
	{
		return "toMove";
	}
	else if (pInput->isKeyDown(OIS::KC_4))
	{
		return "toDeath";
	}

	return "toIdle";
}
