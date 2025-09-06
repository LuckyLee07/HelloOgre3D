#include "MoveState.h"
#include "object/AgentObject.h"
#include "AgentTypeDef.h"
#include "ScriptLuaVM.h"

MoveState::MoveState(AgentObject* pAgent) : AgentState(pAgent)
{
	m_stateId = "MoveState";
}

MoveState::~MoveState()
{

}

void MoveState::OnEnter()
{
	
}

void MoveState::OnLeave()
{

}

std::string MoveState::OnUpdate(float dt)
{
	GetScriptLuaVM()->callFunction("Agent_MovingState", "u[AgentObject]i", m_pAgent, (int)dt);
	
	if (!m_pAgent->IsAnimReadyForMove())
	{
		return "";
	}

	auto pInput = m_pAgent->GetInput();
	if (pInput->isKeyDown(OIS::KC_2))
	{
		return "toShoot";
	}
	else if (pInput->isKeyDown(OIS::KC_3))
	{
		return ""; // ¼ÌÐøWalk
	}
	else if (pInput->isKeyDown(OIS::KC_4))
	{
		return "toDeath";
	}

	return "toIdle";
}
