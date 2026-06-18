#ifndef __AGENT_ACTION_CONTEXT_H__
#define __AGENT_ACTION_CONTEXT_H__

#include <string>
#include "OgreColourValue.h"
#include "OgreVector3.h"

class AgentObject;
class AgentStateController;
class AgentAttrib;
class AIController;
class IAnimController;
class WeaponComponent;

class AgentActionContext
{
public:
	explicit AgentActionContext(AgentStateController& controller);

	AgentObject* GetAgent() const;
	const std::string& GetNavMeshName() const;

	void EnterIdle();
	void EnterMove(bool forceUpdate = false);
	void EnterShoot();
	void EnterReload();
	void ExitShoot();
	void ExitReload();

	void SlowMovement(float rate = 1.0f);
	void StabilizeStationaryMovement(float damping = 0.65f, float stopSpeed = 0.9f);
	void TickMovement(float deltaTimeInMillis, bool slowMode);
	void FaceEnemy();
	void DrawPath(const Ogre::ColourValue& color, const Ogre::Vector3& offset, float radius) const;

	bool PlanPathTo(const Ogre::Vector3& target, bool updateMovePos = true);
	bool PlanPathToEnemy();
	Ogre::Vector3 RandomPoint() const;
	bool SelectRandomDestination(int maxTries = 12);
	bool SelectFleeDestination(int maxTries = 16, float minDistance = 16.0f);

	bool HasEnemy() const;
	AgentObject* GetEnemy() const;
	bool CanShootEnemy(float shootDistance = 3.0f) const;
	bool HasMovePosition(float reachDistance = 1.5f) const;
	bool HasCriticalHealth(float healthRatio = 0.2f) const;
	bool HasWeapon() const;
	bool HasAmmo() const;
	bool IsTargetReached(float threshold, bool clearMovePosition) const;
	bool IsShootAnimationReady() const;
	bool ConsumeShootExecution();
	bool IsShootPresentationFinished() const;
	bool IsReloadPresentationFinished() const;
	bool HasPendingAnimation() const;

	void ConsumeAmmo(int amount);
	void RestoreAmmo();

private:
	AIController* GetAIController() const;
	IAnimController* GetAnimController() const;
	WeaponComponent* GetWeapon() const;
	AgentAttrib* GetAttrib() const;

	AgentStateController* m_controller;
};

#endif  // __AGENT_ACTION_CONTEXT_H__
