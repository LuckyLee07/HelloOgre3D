#include "RaycastService.h"

#include "core/object/BaseObject.h"
#include "components/combat/WeaponComponent.h"
#include <cmath>
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

Ogre::Vector3 RaycastService::SweepCamera(const Ogre::Vector3& from, const Ogre::Vector3& to, float radius) const
{
	if (m_physicsWorld == nullptr || from.isNaN() || to.isNaN()) return to;
	const float fraction = m_physicsWorld->sweepCamera(
		btVector3(from.x, from.y, from.z), btVector3(to.x, to.y, to.z), radius);
	return from + (to - from) * fraction;
}

namespace
{
	bool IsFiniteQueryPoint(const Ogre::Vector3& point)
	{
		return std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z);
	}
}

int RaycastService::TraceProjectile(const Ogre::Vector3& from, const Ogre::Vector3& to,
	int ignoreObjectId, Ogre::Vector3& hitPoint) const
{
	// Copy inputs before touching the output: Lua may reuse the same Vector3 userdata.
	const Ogre::Vector3 start = from;
	const Ogre::Vector3 finish = to;
	hitPoint = Ogre::Vector3::ZERO;
	if (m_physicsWorld == nullptr || ignoreObjectId < 0
		|| !IsFiniteQueryPoint(start) || !IsFiniteQueryPoint(finish)) return -1;
	btVector3 hit(0, 0, 0);
	const int result = m_physicsWorld->traceProjectile(
		btVector3(start.x, start.y, start.z), btVector3(finish.x, finish.y, finish.z),
		static_cast<unsigned int>(ignoreObjectId), WeaponProjectileGeometry::Height,
		WeaponProjectileGeometry::Radius, WeaponProjectileGeometry::SpawnOffset, hit);
	hitPoint = Ogre::Vector3(hit.x(), hit.y(), hit.z());
	return result;
}

int RaycastService::PickSurface(const Ogre::Vector3& from, const Ogre::Vector3& to, Ogre::Vector3& hitPoint) const
{
	const Ogre::Vector3 start = from;
	const Ogre::Vector3 finish = to;
	hitPoint = Ogre::Vector3::ZERO;
	if (m_physicsWorld == nullptr || !IsFiniteQueryPoint(start) || !IsFiniteQueryPoint(finish)) return -1;
	btVector3 hit(0, 0, 0);
	const int result = m_physicsWorld->pickSurface(
		btVector3(start.x, start.y, start.z), btVector3(finish.x, finish.y, finish.z), hit);
	hitPoint = Ogre::Vector3(hit.x(), hit.y(), hit.z());
	return result;
}
