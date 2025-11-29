#ifndef __PHYSICS_WORLD_H__  
#define __PHYSICS_WORLD_H__

#include <map>

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btManifoldPoint;
class btPersistentManifold;
class BaseObject;
class MyRigidBody;

class PhysicsWorld
{
public:
	PhysicsWorld();
	~PhysicsWorld();

	void initilize();
	void cleanup();

	void stepWorld();
	void checkCollision();

	void addRigidBody(btRigidBody* pRigidBody);
	void removeRigidBody(btRigidBody* pRigidBody);

	bool tiggerCollideEvent(btPersistentManifold* pManifold, btManifoldPoint& point);

private:
	btBroadphaseInterface* m_pBroadPhase;
	btDefaultCollisionConfiguration* m_pCollisionConfig;
	btCollisionDispatcher* m_pDispatcher;
	btSequentialImpulseConstraintSolver* m_pSolver;
	btDiscreteDynamicsWorld* m_pDynamicsWorld;
};

#endif; // __PHYSICS_WORLD_H__