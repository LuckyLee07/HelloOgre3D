#include "PhysicsWorld.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

PhysicsWorld::PhysicsWorld() : m_pBroadPhase(nullptr), m_pCollisionConfig(nullptr),
	m_pDispatcher(nullptr), m_pSolver(nullptr), m_pDynamicsWorld(nullptr)
{

}

PhysicsWorld::~PhysicsWorld()
{
	this->cleanup();
}

void PhysicsWorld::initilize()
{
	static const float gravity = -9.8f;

	// 1、初始化碰撞检测阶段（broadphase）
	m_pBroadPhase = new btDbvtBroadphase();

	// 2、设置碰撞配置 提供默认设置用于管理创建碰撞检测所需数据
	m_pCollisionConfig = new btDefaultCollisionConfiguration();

	// 3、创建碰撞分派器（dispatcher）
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfig);

	// 4、初始化约束求解器
	m_pSolver = new btSequentialImpulseConstraintSolver();

	// 5、创建动力学世界
	m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadPhase, m_pSolver, m_pCollisionConfig);

	// 6、设置世界的重力
	m_pDynamicsWorld->setGravity(btVector3(0.0f, gravity, 0.0f));
}

void PhysicsWorld::cleanup()
{
	m_rigidBodys.clear();

	if (m_pDynamicsWorld != nullptr)
		delete m_pDynamicsWorld;

	if (m_pSolver != nullptr)
		delete m_pSolver;

	if (m_pDispatcher != nullptr)
		delete m_pDispatcher;

	if (m_pCollisionConfig != nullptr)
		delete m_pCollisionConfig;

	if (m_pBroadPhase != nullptr)
		delete m_pBroadPhase;
}

void PhysicsWorld::stepWorld()
{
	float timeStep = 1.0f / 30.0f; // 通常情况下，时间步长设置为1/30秒，即每秒30帧
	int maxSubSteps = 1;         // 最大子步数，用于处理在长时间步长中的物理精度问题
	float fixedTimeStep = 1.0f / 30.0f; // 每个子步的固定时间长度，通常与timeStep相同

	//m_pDynamicsWorld->stepSimulation(1.0f / 30.0f, 1, 1.0f / 30.0f);
	m_pDynamicsWorld->stepSimulation(timeStep, maxSubSteps, fixedTimeStep);

	//碰撞检测相关逻辑处理
	this->checkCollision();
}

void PhysicsWorld::checkCollision()
{
	int numManifolds = m_pDispatcher->getNumManifolds();
	for (int index = 0; index < numManifolds; index++)
	{
		btPersistentManifold* pManifold = m_pDispatcher->getManifoldByIndexInternal(index);
		const btRigidBody* pRigidBodyA = static_cast<const btRigidBody*>(pManifold->getBody0());
		const btRigidBody* pRigidBodyB = static_cast<const btRigidBody*>(pManifold->getBody1());

		int numContacts = pManifold->getNumContacts();
		for (int cIndex = 0; cIndex < numContacts; cIndex++)
		{
			btManifoldPoint& pt = pManifold->getContactPoint(cIndex);
			if (pt.getDistance() < 0.0f) // 接触点距离 穿透
			{

				break;
			}
		}
	}
}

void PhysicsWorld::addRigidBody(btRigidBody* pRigidBody, int ownerId)
{
	m_rigidBodys[pRigidBody] = ownerId;
	m_pDynamicsWorld->addRigidBody(pRigidBody);
}

void PhysicsWorld::removeRigidBody(btRigidBody* pRigidBody)
{
	m_pDynamicsWorld->removeRigidBody(pRigidBody);
}