#include "DeathState.h"
#include "objects/AgentObject.h"
#include "GameDefine.h"

DeathState::DeathState(AgentObject* pAgent) : AgentState(pAgent)
{
	m_stateId = "DeathState";
}

DeathState::~DeathState()
{

}

void DeathState::OnEnter()
{
	AgentState::OnEnter();
}

void DeathState::OnLeave()
{
	AgentState::OnLeave();
}

std::string DeathState::OnUpdate(float dt)
{
	(void)dt;

	return "";
}
