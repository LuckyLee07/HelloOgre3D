#include "PhysicsWorld.h"
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4100 4127)
#endif
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btCollisionObjectWrapper.h"
#if defined(_MSC_VER)
#pragma warning(pop)
#endif
#include "systems/manager/ObjectManager.h"
#include "object/BaseObject.h"
#include "systems/physics/Collision.h"
#include "SandboxMacros.h"
#include <cmath>

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

bool PhysicsWorld::rayCastToRigidBody(const btVector3& from, const btVector3& to, btVector3& hitPoint, const btRigidBody*& rigidBody) const
{
	rigidBody = nullptr;
	if (m_pDynamicsWorld == nullptr)
	{
		return false;
	}

	btCollisionWorld::ClosestRayResultCallback rayResult(from, to);
	m_pDynamicsWorld->rayTest(from, to, rayResult);
	if (!rayResult.hasHit())
	{
		return false;
	}

	hitPoint = rayResult.m_hitPointWorld;
	rigidBody = dynamic_cast<const btRigidBody*>(rayResult.m_collisionObject);
	return rigidBody != nullptr;
}

float PhysicsWorld::sweepCamera(const btVector3& from, const btVector3& to, float radius) const
{
	if (m_pDynamicsWorld == nullptr || (to - from).length2() < SIMD_EPSILON) return 1.0f;
	struct StaticCameraSweep : btCollisionWorld::ClosestConvexResultCallback
	{
		StaticCameraSweep(const btVector3& a, const btVector3& b) : ClosestConvexResultCallback(a, b) {}
		virtual bool needsCollision(btBroadphaseProxy* proxy) const override
		{
			if (!ClosestConvexResultCallback::needsCollision(proxy)) return false;
			const btCollisionObject* object = static_cast<const btCollisionObject*>(proxy->m_clientObject);
			return object != nullptr && object->isStaticOrKinematicObject() && object->hasContactResponse();
		}
	};
	btSphereShape shape(btMax(0.05f, radius));
	btTransform start, finish;
	start.setIdentity(); finish.setIdentity();
	start.setOrigin(from); finish.setOrigin(to);
	StaticCameraSweep hit(from, to);
	m_pDynamicsWorld->convexSweepTest(&shape, start, finish, hit);
	return hit.hasHit() ? btMax(0.0f, float(hit.m_closestHitFraction) - 0.02f / float((to - from).length())) : 1.0f;
}

namespace
{
	bool QueryAllowsBody(const btBroadphaseProxy* proxy, unsigned int ignoreObjectId, bool surfaceOnly)
	{
		const btCollisionObject* body = proxy != nullptr
			? static_cast<const btCollisionObject*>(proxy->m_clientObject) : nullptr;
		if (body == nullptr || btRigidBody::upcast(body) == nullptr || !body->hasContactResponse()) return false;
		BaseObject* object = static_cast<BaseObject*>(body->getUserPointer());
		// Unknown physical bodies still obstruct the query and return -1 to callers.
		if (object == nullptr) return true;
		if (ignoreObjectId != 0 && object->GetObjId() == ignoreObjectId) return false;
		const BaseObject::ObjectType type = object->GetObjType();
		if (type == BaseObject::OBJ_TYPE_BULLET) return false;
		return !surfaceOnly || (type != BaseObject::OBJ_TYPE_AGENT && type != BaseObject::OBJ_TYPE_SOLDIER);
	}

	int QueryObjectId(const btCollisionObject* body)
	{
		const BaseObject* object = body != nullptr ? static_cast<const BaseObject*>(body->getUserPointer()) : nullptr;
		return object != nullptr && object->GetObjId() > 0 ? static_cast<int>(object->GetObjId()) : -1;
	}

	bool IsFinitePhysicsPoint(const btVector3& point)
	{
		return std::isfinite(point.x()) && std::isfinite(point.y()) && std::isfinite(point.z());
	}

	struct ProjectileSweep : btCollisionWorld::ClosestConvexResultCallback
	{
		unsigned int ignoreId;
		ProjectileSweep(const btVector3& from, const btVector3& to, unsigned int ignored)
			: ClosestConvexResultCallback(from, to), ignoreId(ignored) {}
		bool needsCollision(btBroadphaseProxy* proxy) const override
		{
			return ClosestConvexResultCallback::needsCollision(proxy) && QueryAllowsBody(proxy, ignoreId, false);
		}
	};

	struct ProjectileOverlap : btCollisionWorld::ContactResultCallback
	{
		const btCollisionObject* probe;
		const btCollisionObject* hitBody;
		unsigned int ignoreId;
		btVector3 hitPoint;
		btScalar nearestDistance2;
		ProjectileOverlap(const btCollisionObject* shape, unsigned int ignored)
			: probe(shape), hitBody(nullptr), ignoreId(ignored), hitPoint(0, 0, 0), nearestDistance2(BT_LARGE_FLOAT) {}
		bool needsCollision(btBroadphaseProxy* proxy) const override
		{
			return ContactResultCallback::needsCollision(proxy) && QueryAllowsBody(proxy, ignoreId, false);
		}
		btScalar addSingleResult(btManifoldPoint& contact, const btCollisionObjectWrapper* a, int, int,
			const btCollisionObjectWrapper* b, int, int) override
		{
			if (contact.getDistance() > 0) return 0;
			const bool probeIsA = a->getCollisionObject() == probe;
			const btCollisionObject* body = probeIsA ? b->getCollisionObject() : a->getCollisionObject();
			const btVector3 point = probeIsA ? contact.getPositionWorldOnB() : contact.getPositionWorldOnA();
			const btScalar distance2 = (point - probe->getWorldTransform().getOrigin()).length2();
			// Initial overlaps occur at the same time; use the nearest surface deterministically.
			if (hitBody == nullptr || distance2 < nearestDistance2
				|| (distance2 == nearestDistance2 && QueryObjectId(body) < QueryObjectId(hitBody)))
			{
				hitBody = body;
				hitPoint = point;
				nearestDistance2 = distance2;
			}
			return 0;
		}
	};

	struct SurfaceRay : btCollisionWorld::ClosestRayResultCallback
	{
		SurfaceRay(const btVector3& from, const btVector3& to) : ClosestRayResultCallback(from, to) {}
		bool needsCollision(btBroadphaseProxy* proxy) const override
		{
			return ClosestRayResultCallback::needsCollision(proxy) && QueryAllowsBody(proxy, 0, true);
		}
	};
}

int PhysicsWorld::traceProjectile(const btVector3& from, const btVector3& to, unsigned int ignoreObjectId,
	float height, float radius, float spawnOffset, btVector3& hitPoint) const
{
	hitPoint = btVector3(0, 0, 0);
	const btVector3 delta = to - from;
	const btScalar distance2 = delta.length2();
	if (m_pDynamicsWorld == nullptr || !IsFinitePhysicsPoint(from) || !IsFinitePhysicsPoint(to)
		|| !std::isfinite(distance2) || !std::isfinite(height) || !std::isfinite(radius) || !std::isfinite(spawnOffset)
		|| radius <= 0 || height < radius * 2 || spawnOffset < 0
		|| distance2 <= btMax(SIMD_EPSILON, btScalar(spawnOffset * spawnOffset))) return -1;
	// Lua can reposition bodies while simulation is paused. Refresh query bounds
	// without integrating physics or emitting contact events (including static bodies).
	m_pDynamicsWorld->updateAabbs();
	const btVector3 direction = delta / btSqrt(distance2);
	const btVector3 startPoint = from + direction * spawnOffset;
	btCapsuleShape shape(radius, height - radius * 2);
	btTransform start, finish;
	start.setIdentity();
	start.setRotation(shortestArcQuat(btVector3(0, 1, 0), -direction));
	start.setOrigin(startPoint);
	finish = start;
	finish.setOrigin(to);

	// A convex sweep alone can miss a projectile that starts embedded in a body.
	// The temporary probe is never inserted in the world and cannot emit collisions.
	btCollisionObject probe;
	probe.setCollisionShape(&shape);
	probe.setWorldTransform(start);
	ProjectileOverlap overlap(&probe, ignoreObjectId);
	m_pDynamicsWorld->contactTest(&probe, overlap);
	if (overlap.hitBody != nullptr)
	{
		hitPoint = overlap.hitPoint;
		return QueryObjectId(overlap.hitBody);
	}
	ProjectileSweep result(startPoint, to, ignoreObjectId);
	m_pDynamicsWorld->convexSweepTest(&shape, start, finish, result);
	if (!result.hasHit())
	{
		hitPoint = to;
		return 0;
	}
	hitPoint = result.m_hitPointWorld;
	return QueryObjectId(result.m_hitCollisionObject);
}

int PhysicsWorld::pickSurface(const btVector3& from, const btVector3& to, btVector3& hitPoint) const
{
	hitPoint = btVector3(0, 0, 0);
	const btScalar distance2 = (to - from).length2();
	if (m_pDynamicsWorld == nullptr || !IsFinitePhysicsPoint(from) || !IsFinitePhysicsPoint(to)
		|| !std::isfinite(distance2) || distance2 <= SIMD_EPSILON) return -1;
	// Picking must be current even before the first unpaused simulation tick.
	m_pDynamicsWorld->updateAabbs();
	SurfaceRay result(from, to);
	m_pDynamicsWorld->rayTest(from, to, result);
	if (!result.hasHit())
	{
		hitPoint = to;
		return 0;
	}
	hitPoint = result.m_hitPointWorld;
	return QueryObjectId(result.m_collisionObject);
}
