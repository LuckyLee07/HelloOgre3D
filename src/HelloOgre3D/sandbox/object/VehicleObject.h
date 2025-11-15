#ifndef __VEHICLE_OBJECT__
#define __VEHICLE_OBJECT__

#include "OgreMath.h"
#include "OgreVector3.h"
#include "steer/AgentPath.h"
#include "BaseObject.h"

class btRigidBody;
class BlockObject;
class AgentObject;
class AgentLocomotion;
class OpenSteerAdapter;
class PhysicsComponent;

class VehicleObject : public BaseObject //tolua_exports
{ //tolua_exports
public:
	VehicleObject(btRigidBody* pRigidBody);
	virtual ~VehicleObject();

	btRigidBody* getRigidBody() const override;
	void ResetRigidBody(btRigidBody* pRigidBody);

	virtual void Initialize();
	virtual void update(int deltaMilisec);
	virtual void updateWorldTransform();

	//tolua_begin
	virtual void setPosition(const Ogre::Vector3& position);
	virtual void setRotation(const Ogre::Vector3& rotation);
	virtual void setOrientation(const Ogre::Quaternion& quaternion);

	virtual Ogre::Vector3 GetPosition() const;
	virtual Ogre::Quaternion GetOrientation() const;

	void SetForward(const Ogre::Vector3& forward);
	void SetVelocity(const Ogre::Vector3& velocity);

	void SetTarget(const Ogre::Vector3& targetPos);
	void SetTargetRadius(Ogre::Real radius);
	
	Ogre::Vector3 GetTarget() const;
	Ogre::Vector3 GetVelocity() const;

	virtual Ogre::Vector3 GetUp() const;
	virtual Ogre::Vector3 GetLeft() const;
	virtual Ogre::Vector3 GetForward() const;
	
	void SetMass(Ogre::Real mass);
	void SetHeight(Ogre::Real height);
	void SetRadius(Ogre::Real radius);
	void SetSpeed(Ogre::Real speed);
	void SetHealth(Ogre::Real health);

	Ogre::Real GetMass() const;
	Ogre::Real GetSpeed() const;
	Ogre::Real GetHeight() const { return m_height; }
	Ogre::Real GetRadius() const { return m_radius; }
	Ogre::Real GetHealth() const { return m_health; }

	void SetMaxForce(Ogre::Real maxForce);
	void SetMaxSpeed(Ogre::Real maxSpeed);
	
	Ogre::Real GetMaxForce() const;
	Ogre::Real GetMaxSpeed() const;

	Ogre::Real GetTargetRadius() const;
	Ogre::Vector3 PredictFuturePosition(Ogre::Real predictionTime) const;
	
	Ogre::Vector3 ForceToPosition(const Ogre::Vector3& position);
	Ogre::Vector3 ForceToFollowPath(Ogre::Real predictionTime);
	Ogre::Vector3 ForceToStayOnPath(Ogre::Real predictionTime);
	Ogre::Vector3 ForceToWander(Ogre::Real deltaMilliSeconds);
	Ogre::Vector3 ForceToTargetSpeed(Ogre::Real targetSpeed);

	// 规避之力 智能体或其他障碍物
	Ogre::Vector3 ForceToAvoidAgents(Ogre::Real predictionTime);
	Ogre::Vector3 ForceToAvoidObjects(Ogre::Real predictionTime);

	Ogre::Vector3 ForceToCombine(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle);
	Ogre::Vector3 ForceToSeparate(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle);

	void ApplyForce(const Ogre::Vector3& force);

	void SetPath(const std::vector<Ogre::Vector3>& points, bool cyclic);
	const std::vector<Ogre::Vector3>& GetPath();
	bool HasPath() const;

	Ogre::Real GetDistanceAlongPath(const Ogre::Vector3& position) const;
	Ogre::Vector3 GetNearestPointOnPath(const Ogre::Vector3& position) const;
	Ogre::Vector3 GetPointOnPath(const Ogre::Real distance) const;

	//tolua_end
	void SetPath(const AgentPath& agentPath);
	
	// 新增：Locomotion 门面
	AgentLocomotion* GetLocomotion() { return m_locomotion; }
	const AgentLocomotion* GetLocomotion() const { return m_locomotion; }

	OpenSteerAdapter* GetAdapter() const;

public:
	static const float DEFAULT_AGENT_MASS;
	static const float DEFAULT_AGENT_HEIGHT;
	static const float DEFAULT_AGENT_RADIUS;
	static const float DEFAULT_AGENT_SPEED;
	static const float DEFAULT_AGENT_HEALTH;

private:
	Ogre::Real m_mass;
	Ogre::Real m_height;
	Ogre::Real m_radius;
	Ogre::Real m_speed;
	Ogre::Real m_health;

	AgentLocomotion* m_locomotion = nullptr;

	PhysicsComponent* m_physicsComp = nullptr;

}; //tolua_exports

#endif  // __VEHICLE_OBJECT__
