#include "BlockObject.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "btBulletDynamicsCommon.h"
#include "GameDefine.h"
#include "GameFunction.h"
#include "manager/SandboxMgr.h"
#include "manager/ClientManager.h"
#include "play/PhysicsWorld.h"
#include "game/GameManager.h"
#include "manager/ObjectManager.h"
#include "play/MyRigidBody.h"
#include "OgreParticleSystemManager.h"
#include "OgreParticleEmitter.h"
#include "components/RenderComponent.h"

using namespace Ogre;

BlockObject::BlockObject(const Ogre::String& meshFile, btRigidBody* pRigidBody)
	: EntityObject(meshFile), m_pRigidBody(pRigidBody)
{
	setObjType(BaseObject::OBJ_TYPE_BLOCK);

	if (pRigidBody == nullptr)
	{
		Ogre::Entity* pEntity = getEntity();
		if (!pEntity) return;
		
		Ogre::Mesh* meshPtr = pEntity->getMesh().getPointer();
		m_pRigidBody = SandboxMgr::CreateRigidBodyBox(meshPtr, 1.0f);
	}
	m_pRigidBody->setUserPointer(this);
}

BlockObject::BlockObject(const Ogre::MeshPtr& meshPtr, btRigidBody* pRigidBody)
	: EntityObject(meshPtr), m_pRigidBody(pRigidBody)
{
	if (pRigidBody == nullptr)
		m_pRigidBody = SandboxMgr::CreateRigidBodyBox(meshPtr.get(), 1.0f);

	m_pRigidBody->setUserPointer(this);
}

BlockObject::BlockObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody)
	: EntityObject(pSceneNode), m_pRigidBody(pRigidBody)
{	
	if (m_pRigidBody)
		m_pRigidBody->setUserPointer(this);
}

BlockObject::~BlockObject()
{
	// 先清理绑定的粒子
	auto iter = m_particleNodes.begin();
	for (; iter != m_particleNodes.end(); iter++)
	{
		Ogre::SceneNode* particleNode = *iter;
		SandboxMgr::RemParticleBySceneNode(particleNode);
	}
	m_particleNodes.clear();

	this->DeleteRighdBody();
}

void BlockObject::DeleteRighdBody()
{
	if (m_pRigidBody != nullptr)
	{
		PhysicsWorld* pPhysicsWorld = g_GameManager->getPhysicsWorld();
		pPhysicsWorld->removeRigidBody(m_pRigidBody);

		delete m_pRigidBody->getMotionState();
		delete m_pRigidBody->getCollisionShape();
		SAFE_DELETE(m_pRigidBody);
	}
}

void BlockObject::update(int deltaMsec)
{
	this->updateWorldTransform();
}

void BlockObject::updateWorldTransform()
{
	const btVector3& rigidBodyPos = m_pRigidBody->getWorldTransform().getOrigin();
	m_renderComp->SetPosition(BtVector3ToVector3(rigidBodyPos));

	const btQuaternion& rigidBodyRotation = m_pRigidBody->getWorldTransform().getRotation();
	m_renderComp->SetOrientation(BtQuaternionToQuaternion(rigidBodyRotation));;
}

void BlockObject::SetMass(const Ogre::Real mass)
{
	btVector3 localInertia(0, 0, 0);
	m_pRigidBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
	m_pRigidBody->setMassProps(mass, localInertia);
	m_pRigidBody->updateInertiaTensor();
	m_pRigidBody->activate(true);
}

Ogre::Real BlockObject::GetMass() const
{
	btScalar inverseMass = m_pRigidBody->getInvMass();

	if (inverseMass <= 0)
		return 0.0f;

	return 1.0f / inverseMass;
}

void BlockObject::setPosition(const Ogre::Vector3& position)
{
	btVector3 btPosition(position.x, position.y, position.z);
	btTransform transform = m_pRigidBody->getWorldTransform();
	transform.setOrigin(btPosition);

	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);

	this->updateWorldTransform();
}

Ogre::Vector3 BlockObject::GetPosition() const
{
	const btVector3& position = m_pRigidBody->getCenterOfMassPosition();
	return BtVector3ToVector3(position);
}

Ogre::Real BlockObject::GetRadius() const
{
	btVector3 aabbMin;
	btVector3 aabbMax;

	m_pRigidBody->getAabb(aabbMin, aabbMax);
	return aabbMax.distance(aabbMin) / 2.0f;
}

void BlockObject::setRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	this->setOrientation(qRotation);
}

void BlockObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	btQuaternion btRotation(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	btTransform transform = m_pRigidBody->getWorldTransform();
	transform.setRotation(btRotation);

	m_pRigidBody->setWorldTransform(transform);
	m_pRigidBody->activate(true);

	this->updateWorldTransform();
}

void BlockObject::applyImpulse(const Ogre::Vector3& impulse)
{
	btVector3 torque(impulse.x, impulse.y, impulse.z);
	m_pRigidBody->applyCentralForce(torque);
	m_pRigidBody->activate(true);
}

void BlockObject::applyAngularImpulse(const Ogre::Vector3& aImpulse)
{
	btVector3 torqueImpulse(aImpulse.x, aImpulse.y, aImpulse.z);
	m_pRigidBody->applyTorque(torqueImpulse);
	m_pRigidBody->activate(true);
}

bool BlockObject::canCollide() 
{ 
	if (m_objType == OBJ_TYPE_BULLET)
	{
		return true;
	}
	return false;
}

void BlockObject::onCollideWith(BaseObject* pCollideObj, const Collision& collision)
{
	if (pCollideObj == nullptr) return;

	int objType = pCollideObj->getObjType();
	if (objType == OBJ_TYPE_BULLET) // 子弹类型
	{
		pCollideObj->setNeedClear(); // 标记为清理
		this->setBulletCollideImpact(collision);
	}
}

void BlockObject::addParticleNode(Ogre::SceneNode* particleNode)
{
	m_particleNodes.push_back(particleNode);
}

void BlockObject::setBulletCollideImpact(const Collision& collision)
{
	// 创建射击碰撞效果
	Ogre::SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();
	Ogre::SceneNode* particleImpact = SandboxMgr::CreateParticle(pRootScene, "BulletImpact");

	// 2秒后清掉该粒子效果
	g_ObjectManager->markNodeRemInSeconds(particleImpact, 2.0f);

	// 获取父节点的世界位置和旋转
	particleImpact->setPosition(collision.pointA_);

	const unsigned short numAttachedObjects = particleImpact->numAttachedObjects();
	for (unsigned short index = 0; index < numAttachedObjects; ++index)
	{
		Ogre::MovableObject* pObject = particleImpact->getAttachedObject(index);
		if (pObject->getMovableType() == Ogre::ParticleSystemFactory::FACTORY_TYPE_NAME)
		{
			Ogre::ParticleSystem* particle = static_cast<Ogre::ParticleSystem*>(pObject);
			particle->setEmitting(true);

			for (unsigned short i = 0; i < particle->getNumEmitters(); i++)
			{
				Ogre::ParticleEmitter* pEmitter = particle->getEmitter(i);
				pEmitter->setDirection(-collision.normalOnB_);
			}
		}
	}
}

OpenSteer::Vec3 BlockObject::getPosition() const
{
	return Vector3ToVec3(GetPosition());
}

float BlockObject::getRadius() const
{
	return GetRadius();
}

OpenSteer::Vec3 BlockObject::steerToAvoid(const OpenSteer::AbstractVehicle& vehicle,
	const float minTimeToCollision) const
{
	// minimum distance to obstacle before avoidance is required
	const float minDistanceToCollision = minTimeToCollision * vehicle.speed();
	const float minDistanceToCenter = minDistanceToCollision + getRadius();

	// contact distance: sum of radii of obstacle and vehicle
	const float totalRadius = getRadius() + vehicle.radius();

	// obstacle center relative to vehicle position
	const OpenSteer::Vec3 localOffset = getPosition() - vehicle.position();

	// distance along vehicle's forward axis to obstacle's center
	const float forwardComponent = localOffset.dot(vehicle.forward());
	const OpenSteer::Vec3 forwardOffset = forwardComponent * vehicle.forward();

	// offset from forward axis to obstacle's center
	const OpenSteer::Vec3 offForwardOffset = localOffset - forwardOffset;

	// test to see if sphere overlaps with obstacle-free corridor
	const bool inCylinder = offForwardOffset.length() < totalRadius;
	const bool nearby = forwardComponent < minDistanceToCenter;
	const bool inFront = forwardComponent > 0;

	// if all three conditions are met, steer away from sphere center
	if (inCylinder && nearby && inFront)
	{
		return offForwardOffset * -1;
	}
	else
	{
		return OpenSteer::Vec3::zero;
	}
}