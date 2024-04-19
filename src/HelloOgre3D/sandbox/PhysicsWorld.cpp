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
}

void PhysicsWorld::addRigidBody(btRigidBody* pRigidBody)
{
	m_pDynamicsWorld->addRigidBody(pRigidBody);
}