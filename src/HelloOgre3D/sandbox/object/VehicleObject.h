#ifndef __VEHICLE_OBJECT__
#define __VEHICLE_OBJECT__

#include "OgreMath.h"
#include "OgreVector3.h"
#include "opensteer/include/Vec3.h"
#include "opensteer/include/SteerLibrary.h"
#include "play/AgentPath.h"
#include "SandboxDef.h"
#include "BaseObject.h"

class btRigidBody;
class BlockObject;
class AgentObject;

class VehicleObject : public BaseObject //tolua_exports
	, private OpenSteer::SteerLibraryMixin<OpenSteer::AbstractVehicle>
{ //tolua_exports
public:
	VehicleObject(btRigidBody* pRigidBody);
	virtual ~VehicleObject();

	void Initialize();
	void DeleteRighdBody();
	void ResetRigidBody(btRigidBody* pRigidBody);

	btRigidBody* getRigidBody() { return m_pRigidBody; }

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
	void SetMaxForce(Ogre::Real maxForce);
	void SetMaxSpeed(Ogre::Real maxSpeed);

	Ogre::Real GetMass() const;
	Ogre::Real GetSpeed() const;
	Ogre::Real GetHeight() const { return m_height; }
	Ogre::Real GetRadius() const { return m_radius; }
	Ogre::Real GetHealth() const { return m_health; }
	Ogre::Real GetMaxForce() const { return m_maxForce; }
	Ogre::Real GetMaxSpeed() const { return m_maxSpeed; }
	Ogre::Real GetTargetRadius() const { return m_targetRadius; }
	
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

	bool HasPath() const { return m_hasPath; }
	void SetPath(const std::vector<Ogre::Vector3>& points, bool cyclic);
	const std::vector<Ogre::Vector3>& GetPath() { return m_points; }
	//tolua_end
	void SetPath(const AgentPath& agentPath);
	
	Ogre::Vector3 ForceToStayOnPath(AgentPath& path, Ogre::Real predictionTime);
	Ogre::Vector3 ForceToFollowPath(AgentPath& path, Ogre::Real predictionTime);
	
	Ogre::Vector3 ForceToAvoidAgents(const std::vector<AgentObject*>& agents, Ogre::Real predictionTime);
	Ogre::Vector3 ForceToAvoidObjects(const std::vector<BlockObject*>& objects, Ogre::Real predictionTime);

public:
	static const float DEFAULT_AGENT_MASS;
	static const float DEFAULT_AGENT_HEIGHT;
	static const float DEFAULT_AGENT_RADIUS;
	static const float DEFAULT_AGENT_SPEED;
	static const float DEFAULT_AGENT_HEALTH;
	static const float DEFAULT_AGENT_MAX_FORCE;
	static const float DEFAULT_AGENT_MAX_SPEED;
	static const float DEFAULT_AGENT_TARGET_RADIUS;
	
protected:
	btRigidBody* m_pRigidBody;

private:
	Ogre::Real m_mass;
	Ogre::Real m_height;
	Ogre::Real m_radius;
	Ogre::Real m_speed;
	Ogre::Real m_health;
	Ogre::Real m_maxForce;
	Ogre::Real m_maxSpeed;

	Ogre::Real m_targetRadius;
	Ogre::Vector3 m_targetPos;
	VehicleObject* m_pTargetAgent;

	bool m_hasPath;
	AgentPath m_path;
	std::vector<Ogre::Vector3> m_points;

private:
	/**
	 * implementation of OpenSteer AbstractVehicle.
	 * @see opensteer/include/AbstractVehicle.h
	 */
	virtual float mass(void) const override;
	virtual float setMass(float) override;

	virtual float radius(void) const override;
	virtual float setRadius(float) override;

	virtual OpenSteer::Vec3 velocity(void) const override;

	virtual float speed(void) const override;
	virtual float setSpeed(float) override;

	virtual OpenSteer::Vec3 predictFuturePosition(const float predictionTime) const override;

	virtual float maxForce(void) const override;
	virtual float setMaxForce(float) override;

	virtual float maxSpeed(void) const override;
	virtual float setMaxSpeed(float) override;

	/**
	 * implementation of OpenSteer AbstractLocalSpace.
	 * @see opensteer/include/LocalSpace.h
	 */
	virtual OpenSteer::Vec3 side(void) const;
	virtual OpenSteer::Vec3 setSide(OpenSteer::Vec3 s);
	virtual OpenSteer::Vec3 up(void) const;
	virtual OpenSteer::Vec3 setUp(OpenSteer::Vec3 u);
	virtual OpenSteer::Vec3 forward(void) const;
	virtual OpenSteer::Vec3 setForward(OpenSteer::Vec3 f);
	virtual OpenSteer::Vec3 position(void) const;
	virtual OpenSteer::Vec3 setPosition(OpenSteer::Vec3 p);

	virtual bool rightHanded(void) const;

	virtual void resetLocalSpace(void);
	
	virtual void setUnitSideFromForwardAndUp(void);

	virtual OpenSteer::Vec3 localizeDirection(const OpenSteer::Vec3& globalDirection) const;
	virtual OpenSteer::Vec3 localizePosition(const OpenSteer::Vec3& globalPosition) const;
	virtual OpenSteer::Vec3 globalizePosition(const OpenSteer::Vec3& localPosition) const;
	virtual OpenSteer::Vec3 globalizeDirection(const OpenSteer::Vec3& localDirection) const;

	virtual void regenerateOrthonormalBasisUF(const OpenSteer::Vec3& newUnitForward);
	virtual void regenerateOrthonormalBasis(const OpenSteer::Vec3& newForward);
	virtual void regenerateOrthonormalBasis(const OpenSteer::Vec3& newForward, const OpenSteer::Vec3& newUp);

	virtual OpenSteer::Vec3 localRotateForwardToSide(const OpenSteer::Vec3& v) const;
	virtual OpenSteer::Vec3 globalRotateForwardToSide(const OpenSteer::Vec3& globalForward) const;

}; //tolua_exports

#endif  // __VEHICLE_OBJECT__
