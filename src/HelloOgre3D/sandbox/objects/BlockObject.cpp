#include "BlockObject.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "btBulletDynamicsCommon.h"
#include "GameDefine.h"
#include "GameFunction.h"
#include "systems/manager/SandboxMgr.h"
#include "systems/physics/PhysicsWorld.h"
#include "game/GameManager.h"
#include "systems/manager/ObjectManager.h"
#include "OgreParticleSystemManager.h"
#include "OgreParticleEmitter.h"
#include "components/PhysicsComponent.h"
#include "systems/service/SceneFactory.h"
#include "systems/service/PhysicsFactory.h"
#include "RenderableObject.h"

using namespace Ogre;

BlockObject::BlockObject(const Ogre::String& meshFile, btRigidBody* pRigidBody)
{
	SetObjType(BaseObject::OBJ_TYPE_BLOCK);

	m_pEntity = new RenderableObject(meshFile);

	if (pRigidBody == nullptr)
	{
		Ogre::Entity* pEntity = m_pEntity->GetEntity();
		if (!pEntity) return;
		
		Ogre::Mesh* meshPtr = pEntity->getMesh().getPointer();
		pRigidBody = PhysicsFactory::CreateRigidBodyBox(meshPtr, 1.0f);
	}

	m_physicsComp = new PhysicsComponent(pRigidBody);
	this->AddComponent("physics", m_physicsComp);
}

BlockObject::BlockObject(const Ogre::MeshPtr& meshPtr, btRigidBody* pRigidBody)
{
	m_pEntity = new RenderableObject(meshPtr);

	if (pRigidBody == nullptr)
		pRigidBody = PhysicsFactory::CreateRigidBodyBox(meshPtr.get(), 1.0f);

	m_physicsComp = new PhysicsComponent(pRigidBody);
	this->AddComponent("physics", m_physicsComp);
}

BlockObject::BlockObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody)
{
	m_pEntity = new RenderableObject(pSceneNode);
	
	m_physicsComp = new PhysicsComponent(pRigidBody);
	this->AddComponent("physics", m_physicsComp);
}

BlockObject::~BlockObject()
{
	// 先清理绑定的粒子
	auto iter = m_particleNodes.begin();
	for (; iter != m_particleNodes.end(); iter++)
	{
		Ogre::SceneNode* particleNode = *iter;
		SceneFactory::RemParticleBySceneNode(particleNode);
	}
	m_particleNodes.clear();

	SAFE_DELETE(m_pEntity);
}

void BlockObject::Init()
{

}

void BlockObject::Update(int deltaMsec)
{
	this->updateWorldTransform();
}

void BlockObject::updateWorldTransform()
{
	btRigidBody* pRigidBody = m_physicsComp->GetRigidBody();
	assert(pRigidBody != nullptr);
	if (!pRigidBody) return;

	const btVector3& rigidBodyPos = pRigidBody->getWorldTransform().getOrigin();
	m_pEntity->SetPosition(BtVector3ToVector3(rigidBodyPos));

	const btQuaternion& rigidBodyRotation = pRigidBody->getWorldTransform().getRotation();
	m_pEntity->SetOrientation(BtQuaternionToQuaternion(rigidBodyRotation));;
}

void BlockObject::SetMass(const Ogre::Real mass)
{
	m_physicsComp->SetMass(mass);
}

Ogre::Real BlockObject::GetMass() const
{
	return m_physicsComp->GetMass();
}

void BlockObject::setPosition(const Ogre::Vector3& position)
{
	m_physicsComp->SetPosition(position);

	this->updateWorldTransform();
}

Ogre::Vector3 BlockObject::GetPosition() const
{
	return m_physicsComp->GetPosition();
}

Ogre::Real BlockObject::GetRadius() const
{
	btRigidBody* pRigidBody = m_physicsComp->GetRigidBody();
	assert(pRigidBody != nullptr);

	btVector3 aabbMin;
	btVector3 aabbMax;

	pRigidBody->getAabb(aabbMin, aabbMax);
	return aabbMax.distance(aabbMin) / 2.0f;
}

void BlockObject::setRotation(const Ogre::Vector3& rotation)
{
	Ogre::Quaternion qRotation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	this->setOrientation(qRotation);
}

void BlockObject::setOrientation(const Ogre::Quaternion& quaternion)
{
	m_physicsComp->SetOrientation(quaternion);

	this->updateWorldTransform();
}

void BlockObject::setMaterial(const Ogre::String& materialName)
{
	m_pEntity->SetMaterial(materialName);
}

void BlockObject::applyImpulse(const Ogre::Vector3& impulse)
{
	m_physicsComp->ApplyForce(impulse);
}

void BlockObject::applyAngularImpulse(const Ogre::Vector3& aImpulse)
{
	m_physicsComp->ApplyAngularForce(aImpulse);
}

void BlockObject::CollideWithObject(BaseObject* pCollideObj, const Collision& collision)
{
	if (pCollideObj == nullptr) return;

	int objType = pCollideObj->GetObjType();
	if (objType == OBJ_TYPE_BULLET) // 子弹类型
	{
		pCollideObj->SetNeedClear(); // 标记为清理
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
	Ogre::SceneNode* pRootScene = GetGameManager()->getRootSceneNode();
	Ogre::SceneNode* particleImpact = SceneFactory::CreateParticle(pRootScene, "BulletImpact");

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

Ogre::Entity* BlockObject::GetEntity()
{
	return m_pEntity->GetEntity();
}

Ogre::SceneNode* BlockObject::GetSceneNode()
{
	return m_pEntity->GetSceneNode();
}