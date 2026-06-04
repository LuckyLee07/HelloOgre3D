#include "PhysicsWorld.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "systems/manager/ObjectManager.h"
#include "object/BaseObject.h"
#include "systems/physics/Collision.h"
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
}

void PhysicsWorld::stepWorld(float deltaSeconds)
{
	// 用真实帧时间推进物理，与循环频率(60/30Hz)解耦。
	// 此前硬编码 timeStep=1/30 且每个逻辑帧调用一次：60Hz 循环下物理以 2× 真实速度流逝，
	// 导致所有 agent 实际移动是意图速度的 2 倍（脚底打滑/步子过大的真正根因）。
	if (deltaSeconds <= 0.0f)
		deltaSeconds = 1.0f / 60.0f;

	const float fixedTimeStep = 1.0f / 60.0f;
	// maxSubSteps 需 >= deltaSeconds/fixedTimeStep，否则 Bullet 会"丢时间"导致物理变慢；
	// 上限用于防止大停顿时子步爆炸（spiral of death）。
	int maxSubSteps = (int)(deltaSeconds / fixedTimeStep) + 2;
	if (maxSubSteps < 2) maxSubSteps = 2;
	if (maxSubSteps > 16) maxSubSteps = 16;

	m_pDynamicsWorld->stepSimulation(deltaSeconds, maxSubSteps, fixedTimeStep);

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

	BaseObject* pCollideObjA = static_cast<BaseObject*>(pRigidBody0->getUserPointer());
	BaseObject* pCollideObjB = static_cast<BaseObject*>(pRigidBody1->getUserPointer());
	if (pCollideObjA == nullptr || pCollideObjB == nullptr)
	{
		return false;
	}

	BaseObject::ObjectType obj1Type = pCollideObjA->GetObjType();
	BaseObject::ObjectType obj2Type = pCollideObjB->GetObjType();

	if (!(obj1Type == BaseObject::OBJ_TYPE_BULLET || obj2Type == BaseObject::OBJ_TYPE_BULLET))
	{
		return false; // 两者都没碰撞属性
	}

	Collision myCollision(pRigidBody0, pRigidBody1,
		point.m_positionWorldOnA,
		point.m_positionWorldOnB,
		point.m_normalWorldOnB);
	Collision reverseCollision(pRigidBody1, pRigidBody0,
		point.m_positionWorldOnB,
		point.m_positionWorldOnA,
		-point.m_normalWorldOnB);

	pCollideObjA->CollideWithObject(pCollideObjB, myCollision);
	pCollideObjB->CollideWithObject(pCollideObjA, reverseCollision);

	return true;
}

void PhysicsWorld::addRigidBody(btRigidBody* pRigidBody)
{
	m_pDynamicsWorld->addRigidBody(pRigidBody);
}

void PhysicsWorld::removeRigidBody(btRigidBody* pRigidBody)
{
	m_pDynamicsWorld->removeRigidBody(pRigidBody);
}
