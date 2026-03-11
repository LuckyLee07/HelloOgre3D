#include "FleeState.h"

#include <cstdlib>

#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "GameDefine.h"
#include "ai/fsm/AgentStateController.h"

FleeState::FleeState(AgentObject* pAgent)
	: AgentState(pAgent)
{
	m_stateId = "FleeState";
}

FleeState::~FleeState()
{
}

void FleeState::OnEnter()
{
	SetTerminated(false);
	SelectFleeDestination();

	if (m_pAgent)
	{
		m_pAgent->RequestState(SSTATE_RUN_FORWARD, true);
	}
}

void FleeState::OnLeave()
{
}

std::string FleeState::OnUpdate(float dt)
{
	if (!m_pAgent)
		return "";

	if (m_pAgent->GetHealth() <= 0.0f)
	{
		SetTerminated(true);
		return "";
	}

	if (m_controller && m_pAgent->IsAnimReadyForMove())
	{
		m_controller->ApplySteering(dt * 0.001f, false);
	}

	SoldierObject* soldier = dynamic_cast<SoldierObject*>(m_pAgent);
	if (soldier && soldier->IsTargetReached(1.5f))
	{
		soldier->ClearMovePosition();
		SetTerminated(true);
	}

	return "";
}

void FleeState::SelectFleeDestination()
{
	if (!m_controller || !m_pAgent)
	{
		SetTerminated(true);
		return;
	}

	SoldierObject* soldier = dynamic_cast<SoldierObject*>(m_pAgent);
	AgentObject* enemy = soldier ? soldier->GetEnemy() : nullptr;
	const bool hasEnemy = (enemy != nullptr);

	bool planned = false;
	for (int i = 0; i < 16 && !planned; ++i)
	{
		Ogre::Vector3 target = m_controller->RandomPoint();
		if (target == Ogre::Vector3::ZERO)
			continue;

		if (hasEnemy)
		{
			const float minFleeDistanceSq = 16.0f * 16.0f;
			const float distSq = target.squaredDistance(enemy->GetPosition());
			if (distSq < minFleeDistanceSq)
				continue;
		}

		planned = m_controller->PlanPathTo(target, true);
	}

	if (!planned)
	{
		SetTerminated(true);
	}
}
