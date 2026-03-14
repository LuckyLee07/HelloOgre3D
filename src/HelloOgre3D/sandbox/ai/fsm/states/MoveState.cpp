#include "MoveState.h"
#include "objects/AgentObject.h"
#include "GameDefine.h"
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
	AgentState::OnEnter();
}

void MoveState::OnLeave()
{
	AgentState::OnLeave();
}

std::string MoveState::OnUpdate(float dt)
{
	// Keep movement authority aligned with command execution and locomotion pose.
	// Only apply steering when MOVE is executing and body animation is in
	// locomotion states; this avoids "moving force first, run pose later".
	const AgentCommandType commandType = m_pAgent->GetCurrentCommandType();
	const int bodyStateId = m_pAgent->GetCurStateId();
	const bool isLocomotionPose =
		(bodyStateId == SSTATE_RUN_FORWARD ||
		bodyStateId == SSTATE_RUN_BACKWARD ||
		bodyStateId == CROUCH_SSTATE_FORWARD);
	// Feed-forward: during fire->run blending, allow steering once a next state
	// is queued so movement can ramp up before current state id flips to run.
	const bool isShootPose = (bodyStateId == SSTATE_FIRE || bodyStateId == CROUCH_SSTATE_FIRE);
	const bool hasPendingAnimTransition = m_pAgent->HasNextAnim();
	const bool canFeedForwardMove = isShootPose && hasPendingAnimTransition;
	if (commandType == AGENT_COMMAND_MOVE && (isLocomotionPose || canFeedForwardMove))
	{
		GetScriptLuaVM()->callFunction("Agent_MovingState", "u[AgentObject]i", m_pAgent, (int)dt);
	}

	auto pInput = m_pAgent->GetInput();
	if (pInput->isKeyDown(OIS::KC_2))
	{
		return "toShoot";
	}
	else if (pInput->isKeyDown(OIS::KC_3))
	{
		return ""; // 继续Walk
	}
	else if (pInput->isKeyDown(OIS::KC_4))
	{
		return "toDeath";
	}

	return "";
	//return "toIdle";
}
