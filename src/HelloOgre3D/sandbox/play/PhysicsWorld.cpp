#include "PhysicsWorld.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "manager/ObjectManager.h"
#include "object/BaseObject.h"
#include "play/MyRigidBody.h"

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

	// 1����ʼ����ײ���׶Σ�broadphase��
	m_pBroadPhase = new btDbvtBroadphase();

	// 2��������ײ���� �ṩĬ���������ڹ�������ײ�����������
	m_pCollisionConfig = new btDefaultCollisionConfiguration();

	// 3��������ײ��������dispatcher��
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfig);

	// 4����ʼ��Լ�������
	m_pSolver = new btSequentialImpulseConstraintSolver();

	// 5����������ѧ����
	m_pDynamicsWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadPhase, m_pSolver, m_pCollisionConfig);

	// 6���������������
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
	float timeStep = 1.0f / 30.0f; // ͨ������£�ʱ�䲽������Ϊ1/30�룬��ÿ��30֡
	int maxSubSteps = 1;         // ����Ӳ��������ڴ����ڳ�ʱ�䲽���е�����������
	float fixedTimeStep = 1.0f / 30.0f; // ÿ���Ӳ��Ĺ̶�ʱ�䳤�ȣ�ͨ����timeStep��ͬ

	//m_pDynamicsWorld->stepSimulation(1.0f / 30.0f, 1, 1.0f / 30.0f);
	m_pDynamicsWorld->stepSimulation(timeStep, maxSubSteps, fixedTimeStep);

	//��ײ�������߼�����
	this->checkCollision();
}

void PhysicsWorld::checkCollision()
{
	int numManifolds = m_pDispatcher->getNumManifolds();
	for (int index = 0; index < numManifolds; index++)
	{
		btPersistentManifold* pManifold = m_pDispatcher->getManifoldByIndexInternal(index);
		
		int numContacts = pManifold->getNumContacts();
		for (int cIndex = 0; cIndex < numContacts; cIndex++)
		{
			btManifoldPoint& pt = pManifold->getContactPoint(cIndex);
			if (pt.getDistance() < 0.0f) // �Ӵ������ ��͸
			{
				this->tiggerCollideEvent(pManifold);
				break;
			}
		}
	}
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

bool PhysicsWorld::tiggerCollideEvent(btPersistentManifold* pManifold)
{
	MyRigidBody* pRigidBody0 = static_cast<MyRigidBody*>(pManifold->getBody0()->getUserPointer());
	MyRigidBody* pRigidBody1 = static_cast<MyRigidBody*>(pManifold->getBody1()->getUserPointer());

	BaseObject* pCollideObj1 = pRigidBody0->getOwner();
	BaseObject* pCollideObj2 = pRigidBody1->getOwner();
	pCollideObj1->onCollideWith(pCollideObj2);
	pCollideObj2->onCollideWith(pCollideObj1);

	return true;
}