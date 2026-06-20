#include "AgentActionContext.h"

#include <algorithm>
#include <vector>

#include "AgentStateController.h"
#include "GameDefine.h"
#include "components/agent/AgentLocomotion.h"
#include "components/agent/AgentAttrib.h"
#include "components/ai/AIController.h"
#include "components/anim/AnimComponent.h"
#include "components/anim/IAnimController.h"
#include "components/combat/WeaponComponent.h"
#include "debug/DebugDrawer.h"
#include "objects/AgentObject.h"

AgentActionContext::AgentActionContext(AgentStateController& controller)
	: m_controller(&controller)
{
}

AgentObject* AgentActionContext::GetAgent() const
{
	return m_controller ? m_controller->GetAgent() : nullptr;
}

AIController* AgentActionContext::GetAIController() const
{
	AgentObject* agent = GetAgent();
	return agent != nullptr ? agent->FindComponent<AIController>() : nullptr;
}

IAnimController* AgentActionContext::GetAnimController() const
{
	AgentObject* agent = GetAgent();
	AnimComponent* anim = agent != nullptr ? agent->FindComponent<AnimComponent>() : nullptr;
	return anim != nullptr ? anim->GetController() : nullptr;
}

WeaponComponent* AgentActionContext::GetWeapon() const
{
	AgentObject* agent = GetAgent();
	return agent != nullptr ? agent->FindComponent<WeaponComponent>() : nullptr;
}

AgentAttrib* AgentActionContext::GetAttrib() const
{
	AgentObject* agent = GetAgent();
	return agent != nullptr ? agent->FindComponent<AgentAttrib>() : nullptr;
}

const std::string& AgentActionContext::GetNavMeshName() const
{
	return m_controller->GetNavMeshName();
}

void AgentActionContext::EnterIdle()
{
	IAnimController* animController = GetAnimController();
	if (animController != nullptr)
	{
		animController->ClearAllActions();
		animController->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
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
	IAnimController* animController = GetAnimController();
	if (animController != nullptr)
	{
		(void)forceUpdate;
		animController->ClearAllActions();
		animController->SetLocomotionIntent(SoldierLocomotionIntent::Move);
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
	IAnimController* animController = GetAnimController();
	if (animController != nullptr)
	{
		animController->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
		animController->RequestAction(SoldierActionIntent::Shoot, true);
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
	IAnimController* animController = GetAnimController();
	if (animController != nullptr)
	{
		animController->SetLocomotionIntent(SoldierLocomotionIntent::Idle);
		animController->RequestAction(SoldierActionIntent::Reload, true);
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
	IAnimController* animController = GetAnimController();
	if (animController != nullptr)
	{
		animController->ClearAction(SoldierActionIntent::Shoot);
	}
}

void AgentActionContext::ExitReload()
{
	IAnimController* animController = GetAnimController();
	if (animController != nullptr)
	{
		animController->ClearAction(SoldierActionIntent::Reload);
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

	IAnimController* animController = GetAnimController();
	if (animController != nullptr)
	{
		if (animController->IsMovePresentationReady())
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
	AgentObject* agent = GetAgent();
	if (!agent)
	{
		return;
	}

	AgentObject* enemy = GetEnemy();
	if (!enemy)
	{
		return;
	}

	const Ogre::Vector3 forwardToEnemy = enemy->GetPosition() - agent->GetPosition();
	if (!forwardToEnemy.isZeroLength())
	{
		agent->SetForward(forwardToEnemy);
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

	static const std::vector<Ogre::Vector3> kEmptyPath;
	AgentLocomotion* locomotion = agent->FindComponent<AgentLocomotion>();
	const std::vector<Ogre::Vector3>& path = locomotion != nullptr ? locomotion->GetPath() : kEmptyPath;
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
	if (!m_controller || !GetAgent())
	{
		return false;
	}

	AgentObject* enemy = GetEnemy();
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
	AIController* ai = GetAIController();
	return ai ? ai->HasEnemy(GetNavMeshName()) : false;
}

AgentObject* AgentActionContext::GetEnemy() const
{
	AIController* ai = GetAIController();
	return ai ? ai->GetEnemy() : nullptr;
}

bool AgentActionContext::CanShootEnemy(float shootDistance) const
{
	AIController* ai = GetAIController();
	return ai ? ai->CanShootEnemy(GetNavMeshName(), shootDistance) : false;
}

bool AgentActionContext::HasMovePosition(float reachDistance) const
{
	AIController* ai = GetAIController();
	return ai ? ai->HasMovePosition(reachDistance) : false;
}

bool AgentActionContext::HasCriticalHealth(float healthRatio) const
{
	AgentObject* agent = GetAgent();
	if (agent == nullptr)
	{
		return false;
	}

	const AgentAttrib* attrib = GetAttrib();
	const float maxHealth = attrib != nullptr
		? std::max(1.0f, static_cast<float>(attrib->GetMaxHealth()))
		: std::max(1.0f, static_cast<float>(agent->GetHealth()));
	return agent->GetHealth() < (maxHealth * healthRatio);
}

bool AgentActionContext::HasWeapon() const
{
	return GetWeapon() != nullptr;
}

bool AgentActionContext::HasAmmo() const
{
	WeaponComponent* weapon = GetWeapon();
	return weapon == nullptr || weapon->HasAmmo();
}

bool AgentActionContext::IsTargetReached(float threshold, bool clearMovePosition) const
{
	AIController* ai = GetAIController();
	if (ai == nullptr)
	{
		return false;
	}

	const bool reached = ai->IsTargetReached(threshold);
	if (reached && clearMovePosition)
	{
		ai->ClearMovePosition();
	}

	return reached;
}

bool AgentActionContext::IsShootAnimationReady() const
{
	IAnimController* animController = GetAnimController();
	if (animController != nullptr)
	{
		return animController->IsShootPresentationReady();
	}

	AgentObject* agent = GetAgent();
	return agent ? agent->IsAnimReadyForShoot() : false;
}

bool AgentActionContext::ConsumeShootExecution()
{
	IAnimController* animController = GetAnimController();
	return animController != nullptr ? animController->ConsumeShootExecution() : false;
}

bool AgentActionContext::IsShootPresentationFinished() const
{
	IAnimController* animController = GetAnimController();
	return animController != nullptr ? animController->IsShootPresentationFinished() : false;
}

bool AgentActionContext::IsReloadPresentationFinished() const
{
	IAnimController* animController = GetAnimController();
	return animController != nullptr ? animController->IsReloadPresentationFinished() : false;
}

bool AgentActionContext::HasPendingAnimation() const
{
	IAnimController* animController = GetAnimController();
	if (animController != nullptr)
	{
		return animController->HasPendingPresentation();
	}

	AgentObject* agent = GetAgent();
	return agent ? agent->HasNextAnim() : false;
}

void AgentActionContext::ConsumeAmmo(int amount)
{
	WeaponComponent* weapon = GetWeapon();
	if (weapon != nullptr)
	{
		weapon->ConsumeAmmo(amount);
	}
}

void AgentActionContext::RestoreAmmo()
{
	WeaponComponent* weapon = GetWeapon();
	if (weapon != nullptr)
	{
		weapon->RestoreAmmo();
	}
}
