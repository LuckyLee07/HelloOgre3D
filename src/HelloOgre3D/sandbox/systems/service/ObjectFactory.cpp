#include "ObjectFactory.h"
#include "SceneFactory.h"
#include "PhysicsFactory.h"
#include "objects/BlockObject.h"
#include "objects/AgentObject.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/AgentFactory.h"
#include "systems/service/SoldierFactory.h"
#include "Ogre.h"
#include "Procedural.h"
#include "btBulletDynamicsCommon.h"
#include "AppConfig.h"

#include <algorithm>
#include <cstdlib>

namespace
{
	class VisualPlaneBlock final : public BlockObject
	{
	public:
		VisualPlaneBlock(Ogre::SceneNode* node, const Ogre::MeshPtr& mesh)
			: BlockObject(node, nullptr), m_ownedMesh(mesh)
		{
		}

		~VisualPlaneBlock() override
		{
			// This factory creates a unique procedural mesh for this object.
			// Remove only its registration. The entity keeps the MeshPtr alive
			// until the base destructor releases the render component.
			Ogre::MeshManager* manager = Ogre::MeshManager::getSingletonPtr();
			if (manager != nullptr) manager->remove(m_ownedMesh->getHandle());
		}

	private:
		Ogre::MeshPtr m_ownedMesh;
	};
}

ObjectFactory::ObjectFactory(ObjectManager* pMananger)
	: m_objectManager(pMananger)
{
}

BlockObject* ObjectFactory::CreatePlane(float length, float width)
{
	Ogre::SceneNode* planeNode = SceneFactory::CreateNodePlane(length, width);

	btRigidBody* planeRigidBody = PhysicsFactory::CreateRigidBodyPlane(btVector3(0, 1.0f, 0), 0);

	BlockObject* pObject = new BlockObject(planeNode, planeRigidBody);
	pObject->SetObjType(BaseObject::OBJ_TYPE_PLANE);

	m_objectManager->addNewObject(pObject);

	return pObject;
}

BlockObject* ObjectFactory::CreateVisualPlane(float width, float height)
{
	const Ogre::Real clampedWidth = std::max(Ogre::Real(0.01f), Ogre::Real(width));
	const Ogre::Real clampedHeight = std::max(Ogre::Real(0.01f), Ogre::Real(height));

	Procedural::PlaneGenerator planeGenerator;
	planeGenerator.setSizeX(clampedWidth);
	planeGenerator.setSizeY(clampedHeight);
	planeGenerator.setUTile(1.0f);
	planeGenerator.setVTile(1.0f);

	const Ogre::MeshPtr mesh = planeGenerator.realizeMesh();
	Ogre::SceneNode* planeNode = SceneFactory::CreateChildSceneNode();
	Ogre::Entity* planeEntity = planeNode->getCreator()->createEntity(mesh);
	planeEntity->setMaterialName(DEFAULT_MATERIAL);
	planeEntity->setCastShadows(false);
	planeNode->attachObject(planeEntity);

	BlockObject* pObject = new VisualPlaneBlock(planeNode, mesh);
	// Visual planes participate in normal object ownership and cleanup. The plane
	// type excludes them from navmesh; no rigid body keeps them out of sight rays.
	pObject->SetObjType(BaseObject::OBJ_TYPE_PLANE);
	m_objectManager->addNewObject(pObject);

	return pObject;
}

BlockObject* ObjectFactory::CreateBlockObject(const Ogre::String& meshFilePath)
{
	BlockObject* pObject = new BlockObject(meshFilePath);
	pObject->SetObjType(BaseObject::OBJ_TYPE_BLOCK);

	m_objectManager->addNewObject(pObject);

	return pObject;
}

BlockObject* ObjectFactory::CreateBlockBox(float width, float height, float length, float uTile, float vTile)
{
	assert(width > 0 && height > 0 && length > 0);
	const Ogre::Vector3 size(width, height, length);
	const Ogre::Vector3 normals[] = {
		Ogre::Vector3::NEGATIVE_UNIT_Z, Ogre::Vector3::UNIT_Z,
		Ogre::Vector3::NEGATIVE_UNIT_Y, Ogre::Vector3::UNIT_Y,
		Ogre::Vector3::NEGATIVE_UNIT_X, Ogre::Vector3::UNIT_X,
	};
	Procedural::TriangleBuffer buffer;
	for (const Ogre::Vector3& normal : normals)
	{
		// Match PlaneGenerator's current Ogre basis. The legacy BoxGenerator
		// assumes a different perpendicular axis on X faces, swapping their
		// height and depth and making non-cubic visuals disagree with Bullet.
		const Ogre::Vector3 axisU = normal.perpendicular();
		const Ogre::Vector3 axisV = normal.crossProduct(axisU);
		const Ogre::Vector3 extentU(Ogre::Math::Abs(axisU.x), Ogre::Math::Abs(axisU.y), Ogre::Math::Abs(axisU.z));
		const Ogre::Vector3 extentV(Ogre::Math::Abs(axisV.x), Ogre::Math::Abs(axisV.y), Ogre::Math::Abs(axisV.z));
		const Ogre::Real faceWidth = extentU.dotProduct(size);
		const Ogre::Real faceHeight = extentV.dotProduct(size);

		Procedural::PlaneGenerator planeGenerator;
		planeGenerator.setNormal(normal);
		planeGenerator.setSizeX(faceWidth);
		planeGenerator.setSizeY(faceHeight);
		planeGenerator.setPosition(normal * size * 0.5f);
		// Interpret tiling as density along the supplied width/depth. Cubes
		// retain their original UVs; wall faces no longer stretch a floor UV.
		planeGenerator.setUTile((faceWidth / width) * uTile);
		planeGenerator.setVTile((faceHeight / length) * vTile);
		planeGenerator.addToTriangleBuffer(buffer);
	}
	const Ogre::MeshPtr meshPtr = buffer.transformToMesh(Procedural::Utils::getName());

	// Check the realised render mesh, including Ogre's culling padding, against
	// the physical box contract. Long thin boxes exposed a regression cubes hid.
	const char* smoke = std::getenv("HELLO_SANDBOX_SMOKE_TEST");
	if (smoke != nullptr && Ogre::String(smoke) == "1")
	{
		const Ogre::AxisAlignedBox& bounds = meshPtr->getBounds();
		const Ogre::Real padding = 1.0f + 2.0f * Ogre::MeshManager::getSingleton().getBoundsPaddingFactor();
		const Ogre::Vector3 actual = bounds.getSize() / padding;
		const bool valid = actual.positionEquals(size, 0.0001f) && bounds.getCenter().isZeroLength();
		Ogre::LogManager::getSingleton().logMessage(
			Ogre::String("[BlockBoxGeometrySelfTest] ") + (valid ? "PASS" : "FAIL")
			+ " nonCubic=" + ((width != height || height != length) ? "1" : "0")
			+ " expected=" + Ogre::StringConverter::toString(size)
			+ " actual=" + Ogre::StringConverter::toString(actual));
	}

	btRigidBody* planeRigidBody = PhysicsFactory::CreateRigidBodyBox(width, height, length);

	BlockObject* pObject = new BlockObject(meshPtr, planeRigidBody);
	pObject->SetObjType(BaseObject::OBJ_TYPE_BLOCK);
	pObject->GetEntity()->setMaterialName(DEFAULT_MATERIAL);

	m_objectManager->addNewObject(pObject);

	return pObject;
}

BlockObject* ObjectFactory::CreateBullet(Ogre::Real height, Ogre::Real radius)
{
	Ogre::SceneNode* pSceneNode = SceneFactory::CreateChildSceneNode();

	btRigidBody* capsuleRigidBody = PhysicsFactory::CreateRigidBodyCapsule(height, radius);

	BlockObject* pBullet = new BlockObject(pSceneNode, capsuleRigidBody);
	pBullet->SetObjType(BaseObject::OBJ_TYPE_BULLET);

	m_objectManager->addNewObject(pBullet);

	return pBullet;
}

AgentObject* ObjectFactory::CreateAgent(AGENT_OBJ_TYPE agentType, const char* filepath)
{
	return AgentFactory::CreateAgent(m_objectManager, agentType, filepath);
}

AgentObject* ObjectFactory::CreateAgentWithProfile(AGENT_OBJ_TYPE agentType, const char* profileName, const char* filepath)
{
	return AgentFactory::CreateAgentWithProfile(m_objectManager, agentType, profileName, filepath);
}

SoldierObject* ObjectFactory::CreateSoldier(const Ogre::String& meshFile, const char* filepath)
{
	return SoldierFactory::CreateSoldier(m_objectManager, meshFile, filepath);
}

SoldierObject* ObjectFactory::CreateSoldierWithProfile(const Ogre::String& meshFile, const char* profileName, const char* filepath)
{
	return SoldierFactory::CreateSoldierWithProfile(m_objectManager, meshFile, profileName, filepath);
}

// Defer deletion to ObjectLifecycleSystem so Lua callbacks never delete their
// own host or invalidate an active object-update iteration.
bool ObjectFactory::RequestDestroyAgent(int objId)
{
	if (m_objectManager == nullptr || objId <= 0) return false;
	AgentObject* agent = dynamic_cast<AgentObject*>(m_objectManager->getObjectById(objId));
	if (agent == nullptr) return false;
	// Bullets may outlive the host; detach their non-owning owner reference.
	for (BlockObject* block : m_objectManager->getAllBlocks())
	{
		if (block != nullptr && block->GetOwner() == agent) block->SetOwner(nullptr);
	}
	agent->SetNeedClear(0, true);
	return true;
}

int ObjectFactory::ClearProjectiles()
{
	return m_objectManager != nullptr ? m_objectManager->ClearProjectiles() : 0;
}
