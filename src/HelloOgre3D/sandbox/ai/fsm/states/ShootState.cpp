#include "ShootState.h"
#include "objects/AgentObject.h"
#include "GameDefine.h"

ShootState::ShootState(AgentObject* pAgent) : AgentState(pAgent)
{
	m_stateId = "ShootState";
}

ShootState::~ShootState()
{

}

void ShootState::OnEnter()
{
	AgentState::OnEnter();
}

void ShootState::OnLeave()
{
	AgentState::OnLeave();
}

std::string ShootState::OnUpdate(float dt)
{
	if (m_pAgent->IsMoving())
	{
		m_pAgent->SlowMoving();
	}
	if (!m_pAgent->IsAnimReadyByFsmState(m_stateId))
	{
		if (!m_pAgent->HasNextAnim())
		{
			m_pAgent->RequestAnimByFsmState(m_stateId, true);
		}
		return ""; // 继续等待
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

	return "";
	//return "toIdle";
}
