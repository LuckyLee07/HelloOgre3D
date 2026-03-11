#include "RandomMoveState.h"
#include "ai/fsm/AgentStateController.h"

RandomMoveState::RandomMoveState(AgentObject* pAgent)
	: AgentState(pAgent)
{
	m_stateId = "RandomMoveState";
}

RandomMoveState::~RandomMoveState()
{
}

void RandomMoveState::OnEnter()
{
	SetTerminated(false);

	bool planned = false;
	if (m_controller)
	{
		for (int i = 0; i < 12 && !planned; ++i)
		{
			const Ogre::Vector3 target = m_controller->RandomPoint();
			if (target == Ogre::Vector3::ZERO)
				continue;

			planned = m_controller->PlanPathTo(target, true);
		}
	}

	(void)planned;
	SetTerminated(true);
}

void RandomMoveState::OnLeave()
{
}

std::string RandomMoveState::OnUpdate(float dt)
{
	(void)dt;
	return "";
}
