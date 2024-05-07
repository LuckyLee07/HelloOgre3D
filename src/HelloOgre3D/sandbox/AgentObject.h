#ifndef __AGENT_OBJECT__
#define __AGENT_OBJECT__

#include "BaseObject.h"
#include "OgreMath.h"
#include "OgreVector3.h"

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
class AgentObject : public BaseObject //tolua_exports
{ //tolua_exports
public:
	AgentObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody);
	virtual ~AgentObject();

	void Initialize();

	//tolua_begin

	btRigidBody* getRigidBody() { return m_pRigidBody; }
	Ogre::SceneNode* getSceneNode() { return m_pSceneNode; }
	AGENT_OBJ_TYPE getAgentType() { return m_agentType; }
	void setAgentType(AGENT_OBJ_TYPE agentType) { m_agentType = agentType; }

	void setMass(const Ogre::Real mass);
	void setPosition(const Ogre::Vector3& position);
	void setRotation(const Ogre::Vector3& position);
	void setOrientation(const Ogre::Quaternion& quaternion);

	void setForward(const Ogre::Vector3& forward);

	void setTarget(const Ogre::Vector3& targetPos);
	void setTargetRadius(Ogre::Real radius);

	Ogre::Vector3 getPosition();
	Ogre::Quaternion getOrientation();

	Ogre::Real getMass() { return m_mass; }
	Ogre::Real getHeight() { return m_height; }
	Ogre::Real getRadius() { return m_radius; }
	//tolua_end

	void update(int deltaMilisec) override;

	void updateWorldTransform();

public:
	static const float DEFAULT_AGENT_MASS;
	static const float DEFAULT_AGENT_HEIGHT;
	static const float DEFAULT_AGENT_RADIUS;
	
private:
	btRigidBody* m_pRigidBody;
	Ogre::SceneNode* m_pSceneNode;

	Ogre::Real m_mass;
	Ogre::Real m_height;
	Ogre::Real m_radius;

	Ogre::Real m_targetRadius;
	Ogre::Vector3 m_targetPos;

	AGENT_OBJ_TYPE m_agentType;
	
}; //tolua_exports

#endif  // __AGENT_OBJECT__
