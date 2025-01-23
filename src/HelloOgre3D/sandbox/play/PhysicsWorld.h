#ifndef __PHYSICS_WORLD_H__  
#define __PHYSICS_WORLD_H__

#include <map>

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;

class PhysicsWorld
{
public:
	PhysicsWorld();
	~PhysicsWorld();

	void initilize();
	void cleanup();

	void stepWorld();
	void checkCollision();

	void addRigidBody(btRigidBody* pRigidBody, int ownerId = 0);
	void removeRigidBody(btRigidBody* pRigidBody);

private:
	btBroadphaseInterface* m_pBroadPhase;
	btDefaultCollisionConfiguration* m_pCollisionConfig;
	btCollisionDispatcher* m_pDispatcher;
	btSequentialImpulseConstraintSolver* m_pSolver;
	btDiscreteDynamicsWorld* m_pDynamicsWorld;

	std::map<btRigidBody*, int> m_rigidBodys;
};

#endif; // __PHYSICS_WORLD_H__