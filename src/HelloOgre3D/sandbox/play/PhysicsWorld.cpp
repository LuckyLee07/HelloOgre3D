#include "PhysicsWorld.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "manager/ObjectManager.h"
#include "object/BaseObject.h"
#include "play/MyRigidBody.h"
#include "SandboxMacros.h"

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

	auto iter = m_rigidBodys.begin();
	for (; iter != m_rigidBodys.end(); iter++)
	{
		SAFE_DELETE(iter->second);
	}
	m_rigidBodys.clear();
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

		if (pManifold->getBody0() == nullptr || pManifold->getBody1() == nullptr)
			continue;
		
		int numContacts = pManifold->getNumContacts();
		for (int cIndex = 0; cIndex < numContacts; cIndex++)
		{
			btManifoldPoint& point = pManifold->getContactPoint(cIndex);
			if (point.getDistance() < 0.0f) // 接触点距离 穿透
			{
				this->tiggerCollideEvent(pManifold, point);
				break;
			}
		}
	}
}

bool PhysicsWorld::tiggerCollideEvent(btPersistentManifold* pManifold, btManifoldPoint& point)
{
	const btRigidBody* pRigidBody0 = static_cast<const btRigidBody*>(pManifold->getBody0());
	const btRigidBody* pRigidBody1 = static_cast<const btRigidBody*>(pManifold->getBody1());

	MyRigidBody* myRigidBodyA = static_cast<MyRigidBody*>(pRigidBody0->getUserPointer());
	MyRigidBody* myRigidBodyB = static_cast<MyRigidBody*>(pRigidBody1->getUserPointer());

	BaseObject* pCollideObjA = myRigidBodyA->getOwner();
	BaseObject* pCollideObjB = myRigidBodyB->getOwner();

	if (!pCollideObjA->canCollide() && !pCollideObjB->canCollide())
	{
		return false; // 两者都没碰撞属性
	}

	Collision myCollision(pRigidBody0, pRigidBody1,
		point.m_positionWorldOnA,
		point.m_positionWorldOnB,
		point.m_normalWorldOnB);

	pCollideObjA->onCollideWith(pCollideObjB, myCollision);

	return true;
}

void PhysicsWorld::addRigidBody(btRigidBody* pRigidBody, BaseObject* pObject)
{
	MyRigidBody *pTempRigid = new MyRigidBody(pObject);
	m_rigidBodys[pRigidBody] = pTempRigid;
	pRigidBody->setUserPointer(pTempRigid);

	m_pDynamicsWorld->addRigidBody(pRigidBody);
}

void PhysicsWorld::removeRigidBody(btRigidBody* pRigidBody)
{
	m_pDynamicsWorld->removeRigidBody(pRigidBody);
	auto iter = m_rigidBodys.find(pRigidBody);
	if (iter != m_rigidBodys.end())
	{
		SAFE_DELETE(iter->second);
		m_rigidBodys.erase(iter);
	}
}
