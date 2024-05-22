#ifndef __AGENT_OBJECT__
#define __AGENT_OBJECT__

#include "BaseObject.h"
#include "OgreMath.h"
#include "OgreVector3.h"
#include "opensteer/include/Vec3.h"
#include "opensteer/include/SteerLibrary.h"
#include "AgentPath.h"

namespace Ogre {
	class SceneNode;
}

//tolua_begin
enum AGENT_OBJ_TYPE
{
	AGENT_OBJ_NONE = 0,
	AGENT_OBJ_SEEKING = 1,
	AGENT_OBJ_FOLLOWER = 2,
	AGENT_OBJ_PATHING = 3,
	AGENT_OBJ_PURSUING = 4,
};
//tolua_end

class btRigidBody;
class SandboxObject;
class AgentObject : public BaseObject //tolua_exports
	, private OpenSteer::SteerLibraryMixin<OpenSteer::AbstractVehicle>
{ //tolua_exports
public:
	AgentObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody);
	virtual ~AgentObject();

	void Initialize();
	void ResetRigidBody(btRigidBody* pRigidBody);

	btRigidBody* getRigidBody() { return m_pRigidBody; }
	Ogre::SceneNode* getSceneNode() { return m_pSceneNode; }

	//tolua_begin
	AGENT_OBJ_TYPE getAgentType() { return m_agentType; }
	void setAgentType(AGENT_OBJ_TYPE agentType) { m_agentType = agentType; }

	void SetPosition(const Ogre::Vector3& position);
	void SetRotation(const Ogre::Vector3& rotation);
	void SetOrientation(const Ogre::Quaternion& quaternion);

	void SetForward(const Ogre::Vector3& forward);
	void SetVelocity(const Ogre::Vector3& velocity);

	void SetTarget(const Ogre::Vector3& targetPos);
	void SetTargetRadius(Ogre::Real radius);
	
	Ogre::Vector3 GetUp() const;
	Ogre::Vector3 GetLeft() const;
	Ogre::Vector3 GetTarget() const;

	Ogre::Vector3 GetForward() const;
	Ogre::Vector3 GetVelocity() const;

	Ogre::Vector3 GetPosition() const ;
	Ogre::Quaternion GetOrientation() const;


	void SetMass(Ogre::Real objMass);
	void SetHeight(Ogre::Real objMass);
	void SetRadius(Ogre::Real objMass);
	void SetSpeed(Ogre::Real objMass);
	void SetHealth(Ogre::Real objMass);
	void SetMaxForce(Ogre::Real objMass);
	void SetMaxSpeed(Ogre::Real objMass);

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
	Ogre::Vector3 ForceToAvoidObjects(const std::vector<SandboxObject*>& objects, Ogre::Real predictionTime);

	void update(int deltaMilisec) override;

	void updateWorldTransform();

public:
	static const float DEFAULT_AGENT_MASS;
	static const float DEFAULT_AGENT_HEIGHT;
	static const float DEFAULT_AGENT_RADIUS;
	static const float DEFAULT_AGENT_SPEED;
	static const float DEFAULT_AGENT_HEALTH;
	static const float DEFAULT_AGENT_MAX_FORCE;
	static const float DEFAULT_AGENT_MAX_SPEED;
	static const float DEFAULT_AGENT_TARGET_RADIUS;
	
private:
	AGENT_OBJ_TYPE m_agentType;

	btRigidBody* m_pRigidBody;
	Ogre::SceneNode* m_pSceneNode;

	Ogre::Real m_mass;
	Ogre::Real m_height;
	Ogre::Real m_radius;
	Ogre::Real m_speed;
	Ogre::Real m_health;
	Ogre::Real m_maxForce;
	Ogre::Real m_maxSpeed;

	Ogre::Real m_targetRadius;
	Ogre::Vector3 m_targetPos;
	AgentObject* m_pTargetAgent;

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

#endif  // __AGENT_OBJECT__
