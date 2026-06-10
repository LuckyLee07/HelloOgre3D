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
class btVector3;
class BaseObject;

class PhysicsWorld
{
public:
	PhysicsWorld();
	~PhysicsWorld();

	void initilize();
	void cleanup();

	void stepWorld(float deltaSeconds);
	void checkCollision();

	void addRigidBody(btRigidBody* pRigidBody);
	void removeRigidBody(btRigidBody* pRigidBody);
	bool rayCastToRigidBody(const btVector3& from, const btVector3& to, btVector3& hitPoint, const btRigidBody*& rigidBody) const;

	bool tiggerCollideEvent(btPersistentManifold* pManifold, btManifoldPoint& point);

private:
	btBroadphaseInterface* m_pBroadPhase;
	btDefaultCollisionConfiguration* m_pCollisionConfig;
	btCollisionDispatcher* m_pDispatcher;
	btSequentialImpulseConstraintSolver* m_pSolver;
	btDiscreteDynamicsWorld* m_pDynamicsWorld;
};

#endif; // __PHYSICS_WORLD_H__
