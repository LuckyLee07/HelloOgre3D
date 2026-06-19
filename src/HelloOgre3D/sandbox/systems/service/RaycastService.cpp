#include "RaycastService.h"

#include "core/object/BaseObject.h"
#include "systems/physics/PhysicsWorld.h"
#include "btBulletDynamicsCommon.h"

RaycastService::RaycastService(PhysicsWorld* physicsWorld)
	: m_physicsWorld(physicsWorld)
{
}

RaycastService::~RaycastService()
{
	m_physicsWorld = nullptr;
}

void RaycastService::SetPhysicsWorld(PhysicsWorld* physicsWorld)
{
	m_physicsWorld = physicsWorld;
}

int RaycastService::RayCastObjectId(const Ogre::Vector3& from, const Ogre::Vector3& to) const
{
	PhysicsWorld* physicsWorld = m_physicsWorld;
	if (physicsWorld == nullptr)
		return -1;

	btVector3 hitPoint(0.0f, 0.0f, 0.0f);
	const btRigidBody* rigidBody = nullptr;
	const bool result = physicsWorld->rayCastToRigidBody(
		btVector3(from.x, from.y, from.z),
		btVector3(to.x, to.y, to.z),
		hitPoint,
		rigidBody);
	if (!result)
		return 0;

	const BaseObject* object = rigidBody != nullptr ? static_cast<const BaseObject*>(rigidBody->getUserPointer()) : nullptr;
	return object != nullptr ? static_cast<int>(object->GetObjId()) : -1;
}
