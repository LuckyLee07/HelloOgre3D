#include "AgentActionContext.h"

#include "AgentStateController.h"
#include "GameDefine.h"
#include "debug/DebugDrawer.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "objects/animation/SoldierAnimController.h"

namespace
{
	SoldierObject* AsSoldier(AgentObject* agent)
	{
		return dynamic_cast<SoldierObject*>(agent);
	}
}

AgentActionContext::AgentActionContext(AgentStateController& controller)
	: m_controller(&controller)
{
}

AgentObject* AgentActionContext::GetAgent() const
{
	return m_controller ? m_controller->GetAgent() : nullptr;
}

SoldierObject* AgentActionContext::GetSoldier() const
{
	return AsSoldier(GetAgent());
}

const std::string& AgentActionContext::GetNavMeshName() const
{
	return m_controller->GetNavMeshName();
}

void AgentActionContext::EnterIdle()
{
	SoldierObject* soldier = GetSoldier();
	if (soldier && soldier->GetAnimController())
	{
		soldier->GetAnimController()->ClearAllActions();
		soldier->GetAnimController()->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
		return;
	}

	AgentObject* agent = GetAgent();
	if (agent)
	{
		agent->RequestState(SSTATE_IDLE_AIM);
	}
}

void AgentActionContext::EnterMove(bool forceUpdate)
{
	SoldierObject* soldier = GetSoldier();
	if (soldier && soldier->GetAnimController())
	{
		(void)forceUpdate;
		soldier->GetAnimController()->ClearAllActions();
		soldier->GetAnimController()->SetLocomotionIntent(SoldierLocomotionIntent::Move);
		return;
	}

	AgentObject* agent = GetAgent();
	if (agent)
	{
		agent->RequestState(SSTATE_RUN_FORWARD, forceUpdate);
	}
}

void AgentActionContext::EnterShoot()
{
	SoldierObject* soldier = GetSoldier();
	if (soldier && soldier->GetAnimController())
	{
		soldier->GetAnimController()->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
		soldier->GetAnimController()->RequestAction(SoldierActionIntent::Shoot, true);
		return;
	}

	AgentObject* agent = GetAgent();
	if (agent)
	{
		agent->RequestState(SSTATE_FIRE, true);
	}
}

void AgentActionContext::EnterReload()
{
	SoldierObject* soldier = GetSoldier();
	if (soldier && soldier->GetAnimController())
	{
		soldier->GetAnimController()->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
		soldier->GetAnimController()->RequestAction(SoldierActionIntent::Reload, true);
		return;
	}

	AgentObject* agent = GetAgent();
	if (agent)
	{
		agent->RequestState(SSTATE_RELOAD, true);
	}
}

void AgentActionContext::ExitShoot()
{
	SoldierObject* soldier = GetSoldier();
	if (soldier && soldier->GetAnimController())
	{
		soldier->GetAnimController()->ClearAction(SoldierActionIntent::Shoot);
	}
}

void AgentActionContext::ExitReload()
{
	SoldierObject* soldier = GetSoldier();
	if (soldier && soldier->GetAnimController())
	{
		soldier->GetAnimController()->ClearAction(SoldierActionIntent::Reload);
	}
}
void AgentActionContext::SlowMovement(float rate)
{
	AgentObject* agent = GetAgent();
	if (agent && agent->IsMoving())
	{
		agent->SlowMoving(rate);
	}
}

void AgentActionContext::StabilizeStationaryMovement(float damping, float stopSpeed)
{
	AgentObject* agent = GetAgent();
	if (!agent || !agent->OnGround())
	{
		return;
	}

	Ogre::Vector3 velocity = agent->GetVelocity();
	const Ogre::Real yMovement = velocity.y;
	velocity.y = 0.0f;

	if (velocity.isZeroLength())
	{
		return;
	}

	velocity *= damping;
	if (velocity.squaredLength() < (stopSpeed * stopSpeed))
	{
		velocity = Ogre::Vector3::ZERO;
	}

	velocity.y = yMovement;
	agent->SetVelocity(velocity);
}

void AgentActionContext::TickMovement(float deltaTimeInMillis, bool slowMode)
{
	AgentObject* agent = GetAgent();
	if (!m_controller || !agent)
	{
		return;
	}

	SoldierObject* soldier = GetSoldier();
	if (soldier && soldier->GetAnimController())
	{
		if (soldier->GetAnimController()->IsMovePresentationReady())
		{
			m_controller->ApplySteering(deltaTimeInMillis * 0.001f, slowMode);
		}
		return;
	}

	if (agent->IsAnimReadyForMove())
	{
		m_controller->ApplySteering(deltaTimeInMillis * 0.001f, slowMode);
	}
}

void AgentActionContext::FaceEnemy()
{
	SoldierObject* soldier = GetSoldier();
	if (!soldier)
	{
		return;
	}

	AgentObject* enemy = soldier->GetEnemy();
	if (!enemy)
	{
		return;
	}

	const Ogre::Vector3 forwardToEnemy = enemy->GetPosition() - soldier->GetPosition();
	if (!forwardToEnemy.isZeroLength())
	{
		soldier->SetForward(forwardToEnemy);
	}
}

void AgentActionContext::DrawPath(const Ogre::ColourValue& color, const Ogre::Vector3& offset, float radius) const
{
	AgentObject* agent = GetAgent();
	DebugDrawer* debugDrawer = DebugDrawer::GetInstance();
	if (!agent || !debugDrawer)
	{
		return;
	}

	const std::vector<Ogre::Vector3>& path = agent->GetPath();
	if (path.empty())
	{
		return;
	}

	debugDrawer->drawPath(path, color, false, offset);
	debugDrawer->drawCircle(path.back() + offset, radius, 20, color, false);
}

bool AgentActionContext::PlanPathTo(const Ogre::Vector3& target, bool updateMovePos)
{
	return m_controller ? m_controller->PlanPathTo(target, updateMovePos) : false;
}

bool AgentActionContext::PlanPathToEnemy()
{
	return m_controller ? m_controller->PlanPathToEnemy() : false;
}

Ogre::Vector3 AgentActionContext::RandomPoint() const
{
	return m_controller ? m_controller->RandomPoint() : Ogre::Vector3::ZERO;
}

bool AgentActionContext::SelectRandomDestination(int maxTries)
{
	for (int i = 0; i < maxTries; ++i)
	{
		const Ogre::Vector3 target = RandomPoint();
		if (target == Ogre::Vector3::ZERO)
		{
			continue;
		}

		if (PlanPathTo(target, true))
		{
			return true;
		}
	}

	return false;
}

bool AgentActionContext::SelectFleeDestination(int maxTries, float minDistance)
{
	SoldierObject* soldier = GetSoldier();
	if (!m_controller || !soldier)
	{
		return false;
	}

	AgentObject* enemy = soldier->GetEnemy();
	const bool hasEnemy = (enemy != nullptr);
	const float minDistanceSq = minDistance * minDistance;

	for (int i = 0; i < maxTries; ++i)
	{
		const Ogre::Vector3 target = RandomPoint();
		if (target == Ogre::Vector3::ZERO)
		{
			continue;
		}

		if (hasEnemy && target.squaredDistance(enemy->GetPosition()) < minDistanceSq)
		{
			continue;
		}

		if (PlanPathTo(target, true))
		{
			return true;
		}
	}

	return false;
}

bool AgentActionContext::HasEnemy() const
{
	SoldierObject* soldier = GetSoldier();
	return soldier ? soldier->HasEnemy(GetNavMeshName()) : false;
}

bool AgentActionContext::IsTargetReached(float threshold, bool clearMovePosition) const
{
	SoldierObject* soldier = GetSoldier();
	if (!soldier)
	{
		return false;
	}

	const bool reached = soldier->IsTargetReached(threshold);
	if (reached && clearMovePosition)
	{
		soldier->ClearMovePosition();
	}

	return reached;
}

bool AgentActionContext::IsShootAnimationReady() const
{
	SoldierObject* soldier = GetSoldier();
	if (soldier && soldier->GetAnimController())
	{
		return soldier->GetAnimController()->IsShootPresentationReady();
	}

	AgentObject* agent = GetAgent();
	return agent ? agent->IsAnimReadyForShoot() : false;
}

bool AgentActionContext::ConsumeShootExecution()
{
	SoldierObject* soldier = GetSoldier();
	return soldier && soldier->GetAnimController() ? soldier->GetAnimController()->ConsumeShootExecution() : false;
}

bool AgentActionContext::IsShootPresentationFinished() const
{
	SoldierObject* soldier = GetSoldier();
	return soldier && soldier->GetAnimController() ? soldier->GetAnimController()->IsShootPresentationFinished() : false;
}

bool AgentActionContext::IsReloadPresentationFinished() const
{
	SoldierObject* soldier = GetSoldier();
	return soldier && soldier->GetAnimController() ? soldier->GetAnimController()->IsReloadPresentationFinished() : false;
}

bool AgentActionContext::HasPendingAnimation() const
{
	SoldierObject* soldier = GetSoldier();
	if (soldier && soldier->GetAnimController())
	{
		return soldier->GetAnimController()->HasPendingPresentation();
	}

	AgentObject* agent = GetAgent();
	return agent ? agent->HasNextAnim() : false;
}

void AgentActionContext::ConsumeAmmo(int amount)
{
	SoldierObject* soldier = GetSoldier();
	if (soldier)
	{
		soldier->ConsumeAmmo(amount);
	}
}

void AgentActionContext::RestoreAmmo()
{
	SoldierObject* soldier = GetSoldier();
	if (soldier)
	{
		soldier->RestoreAmmo();
	}
}
