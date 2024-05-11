#ifndef __PHYSICS_WORLD_H__  
#define __PHYSICS_WORLD_H__

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

	void addRigidBody(btRigidBody* pRigidBody);
	void removeRigidBody(btRigidBody* pRigidBody);

private:
	btBroadphaseInterface* m_pBroadPhase;
	btDefaultCollisionConfiguration* m_pCollisionConfig;
	btCollisionDispatcher* m_pDispatcher;
	btSequentialImpulseConstraintSolver* m_pSolver;
	btDiscreteDynamicsWorld* m_pDynamicsWorld;
};

#endif; // __PHYSICS_WORLD_H__