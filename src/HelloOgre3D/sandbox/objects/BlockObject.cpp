#include "BlockObject.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OgreSceneManager.h"
#include "btBulletDynamicsCommon.h"
#include "GameDefine.h"
#include "GameFunction.h"
#include "ai/tactics/TacticalService.h"
#include "core/SandboxServices.h"
#include "systems/physics/PhysicsWorld.h"
#include "systems/manager/ObjectManager.h"
#include "OgreParticleSystemManager.h"
#include "OgreParticleSystem.h"
#include "OgreParticleEmitter.h"
#include "components/ComponentKeys.h"
#include "components/physics/PhysicsComponent.h"
#include "components/render/RenderComponent.h"
#include "event/SandboxEventPayload.h"
#include "systems/service/SceneFactory.h"
#include "systems/service/PhysicsFactory.h"

using namespace Ogre;

namespace
{
	void SpawnBulletImpactWithServices(const Collision& collision, const SandboxServices* services);
}

BlockObject::BlockObject(const Ogre::String& meshFile, btRigidBody* pRigidBody)
{
	SetObjType(BaseObject::OBJ_TYPE_BLOCK);

	m_renderComp = new RenderComponent(meshFile);
	this->AddComponent(ComponentKeys::Render, m_renderComp);

	if (pRigidBody == nullptr)
	{
		Ogre::Entity* pEntity = m_renderComp->GetEntity();
		if (!pEntity) return;
		
		Ogre::Mesh* meshPtr = pEntity->getMesh().getPointer();
		pRigidBody = PhysicsFactory::CreateRigidBodyBox(meshPtr, 1.0f);
	}

	m_physicsComp = new PhysicsComponent(pRigidBody);
	this->AddComponent(ComponentKeys::Physics, m_physicsComp);
}

BlockObject::BlockObject(const Ogre::MeshPtr& meshPtr, btRigidBody* pRigidBody)
{
	SetObjType(BaseObject::OBJ_TYPE_BLOCK);

	m_renderComp = new RenderComponent(meshPtr);
	this->AddComponent(ComponentKeys::Render, m_renderComp);

	if (pRigidBody == nullptr)
		pRigidBody = PhysicsFactory::CreateRigidBodyBox(meshPtr.get(), 1.0f);

	m_physicsComp = new PhysicsComponent(pRigidBody);
	this->AddComponent(ComponentKeys::Physics, m_physicsComp);
}

BlockObject::BlockObject(Ogre::SceneNode* pSceneNode, btRigidBody* pRigidBody)
{
	SetObjType(BaseObject::OBJ_TYPE_BLOCK);

	m_renderComp = new RenderComponent(pSceneNode);
	this->AddComponent(ComponentKeys::Render, m_renderComp);
	
	m_physicsComp = new PhysicsComponent(pRigidBody);
	this->AddComponent(ComponentKeys::Physics, m_physicsComp);
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

	m_renderComp = nullptr;
	m_physicsComp = nullptr;
	m_ownerObject = nullptr;
}

void BlockObject::Init()
{

}

void BlockObject::Update(int deltaMsec)
{
	(void)deltaMsec;
	this->updateWorldTransform();
}

void BlockObject::updateWorldTransform()
{
	if (m_renderComp != nullptr)
	{
		m_renderComp->SyncFromOwnerTransform();
	}
}

void BlockObject::SetMass(const Ogre::Real mass)
{
	if (m_physicsComp != nullptr)
		m_physicsComp->SetMass(mass);
}

Ogre::Real BlockObject::GetMass() const
{
	return m_physicsComp != nullptr ? m_physicsComp->GetMass() : Ogre::Real(0.0f);
}

void BlockObject::setPosition(const Ogre::Vector3& position)
{
	if (m_physicsComp != nullptr && m_physicsComp->GetRigidBody() != nullptr)
	{
		m_physicsComp->SetPosition(position);
	}
	else if (m_renderComp != nullptr)
	{
		m_renderComp->SetPosition(position);
	}

	this->updateWorldTransform();
}

Ogre::Vector3 BlockObject::GetPosition() const
{
	if (m_physicsComp != nullptr && m_physicsComp->GetRigidBody() != nullptr)
		return m_physicsComp->GetPosition();
	return m_renderComp != nullptr ? m_renderComp->GetDerivedPosition() : Ogre::Vector3::ZERO;
}

Ogre::Real BlockObject::GetRadius() const
{
	if (m_physicsComp == nullptr || m_physicsComp->GetRigidBody() == nullptr)
		return Ogre::Real(0.0f);

	btRigidBody* pRigidBody = m_physicsComp->GetRigidBody();

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
	if (m_physicsComp != nullptr && m_physicsComp->GetRigidBody() != nullptr)
	{
		m_physicsComp->SetOrientation(quaternion);
	}
	else if (m_renderComp != nullptr)
	{
		m_renderComp->SetOrientation(quaternion);
	}

	this->updateWorldTransform();
}

void BlockObject::setMaterial(const Ogre::String& materialName)
{
	if (m_renderComp != nullptr)
		m_renderComp->SetMaterial(materialName);
}

void BlockObject::applyImpulse(const Ogre::Vector3& impulse)
{
	if (m_physicsComp != nullptr)
		m_physicsComp->ApplyForce(impulse);
}

void BlockObject::applyAngularImpulse(const Ogre::Vector3& aImpulse)
{
	if (m_physicsComp != nullptr)
		m_physicsComp->ApplyAngularForce(aImpulse);
}

void BlockObject::CollideWithObject(BaseObject* pCollideObj, const Collision& collision)
{
	if (pCollideObj == nullptr) return;

	int objType = pCollideObj->GetObjType();
	if (objType == OBJ_TYPE_BULLET) // 子弹类型
	{
		pCollideObj->SetNeedClear(); // 标记为清理
		SpawnBulletImpactWithServices(collision, GetSandboxServices());
		BlockObject* bullet = dynamic_cast<BlockObject*>(pCollideObj);
		const SandboxServices* services = GetSandboxServices();
		ObjectManager* objectManager = services != nullptr ? services->objects : nullptr;
		TacticalService* tactics = objectManager != nullptr ? objectManager->GetTacticalService() : nullptr;
		if (bullet != nullptr && tactics != nullptr)
		{
			BaseObject* bulletOwner = bullet->GetOwner();
			tactics->publishTacticalEvent(
				SandboxEventTypes::BulletImpact(),
				bulletOwner != nullptr ? static_cast<int>(bulletOwner->GetObjId()) : -1,
				static_cast<int>(GetObjId()),
				bulletOwner != nullptr ? bulletOwner->GetTeamId() : -1,
				GetTeamId(),
				collision.pointA_,
				0,
				"global",
				false);
		}
	}
}

void BlockObject::addParticleNode(Ogre::SceneNode* particleNode)
{
	m_particleNodes.push_back(particleNode);
}

void BlockObject::SpawnBulletImpact(const Collision& collision)
{
	SpawnBulletImpactWithServices(collision, nullptr);
}

void BlockObject::SpawnBulletImpact(const Collision& collision, const SandboxServices* services)
{
	SpawnBulletImpactWithServices(collision, services);
}

namespace
{
void SpawnBulletImpactWithServices(const Collision& collision, const SandboxServices* services)
{
	// 创建射击碰撞效果
	Ogre::SceneNode* particleImpact = SceneFactory::CreateParticle("BulletImpact");

	// 2秒后清掉该粒子效果
	ObjectManager* objectManager = services != nullptr ? services->objects : nullptr;
	if (objectManager != nullptr)
		objectManager->markNodeRemInSeconds(particleImpact, 2.0f);

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

Ogre::Entity* BlockObject::GetEntity() const
{
	return m_renderComp != nullptr ? m_renderComp->GetEntity() : nullptr;
}

Ogre::SceneNode* BlockObject::GetSceneNode()
{
	return m_renderComp != nullptr ? m_renderComp->GetSceneNode() : nullptr;
}
