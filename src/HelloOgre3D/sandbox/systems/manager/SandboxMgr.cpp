#include "SandboxMgr.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"
#include "ScriptLuaVM.h"
#include "GameFunction.h"
#include "objects/BlockObject.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "recast/include/Recast.h"
#include "ai/navigation/NavigationMesh.h"
#include "components/agent/AgentLocomotion.h"
#include "core/object/BaseObject.h"
#include "ObjectManager.h"
#include "systems/physics/PhysicsWorld.h"
#include "systems/service/CameraService.h"
#include "btBulletDynamicsCommon.h"

namespace
{
	const float DEFAULT_AGENT_WALKABLE_CLIMB = AgentLocomotion::DEFAULT_AGENT_RADIUS / 2.0f;
	const float DEFAULT_AGENT_WALKABLE_SLOPE = 45.0f;
}

SandboxMgr::SandboxMgr(CameraService* cameraService,
	ObjectManager* objectManager,
	Ogre::SceneManager* sceneManager)
	: m_cameraService(cameraService),
	m_objectManager(objectManager),
	m_pRootSceneNode(nullptr)
{
	m_pRootSceneNode = sceneManager->getRootSceneNode();
}

SandboxMgr::~SandboxMgr()
{
    m_pRootSceneNode = nullptr;
}

Ogre::SceneManager* SandboxMgr::GetSceneCreator()
{
	return m_pRootSceneNode->getCreator();
}

Ogre::Camera* SandboxMgr::GetSceneGraphCamera()
{
	return m_cameraService != nullptr ? m_cameraService->GetCamera() : nullptr;
}

void SandboxMgr::CallFile(const Ogre::String& filepath)
{
    GetScriptLuaVM()->callFile(filepath.c_str());
}

void SandboxMgr::SetUseCppFsmFlag(bool value)
{
    m_useCppFsmFlag = value;
}

void SandboxMgr::SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation)
{
    const Ogre::Quaternion& newOrientation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	GetSceneCreator()->setSkyBox(true, materialName, 5000.0f, true, newOrientation);
}

void SandboxMgr::SetAmbientLight(const Ogre::Vector3& colourValue)
{
    Ogre::ColourValue tempColor(colourValue.x, colourValue.y, colourValue.z);
	GetSceneCreator()->setAmbientLight(tempColor);
}

Ogre::Light* SandboxMgr::CreateDirectionalLight(const Ogre::Vector3& direction)
{
    Ogre::SceneNode* const lightNode = m_pRootSceneNode->createChildSceneNode();

    Ogre::Light* const lightEntity = GetSceneCreator()->createLight();

    lightEntity->setCastShadows(true);
    lightEntity->setType(Ogre::Light::LT_DIRECTIONAL);

    lightEntity->setDiffuseColour(1.0f, 1.0f, 1.0f);
    lightEntity->setSpecularColour(0, 0, 0);
    lightEntity->setDirection(direction);

    lightNode->attachObject(lightEntity);

    return lightEntity;
}

void SandboxMgr::setMaterial(BlockObject* pObject, const Ogre::String& materialName)
{
    assert(pObject != nullptr);
    setMaterial(pObject->GetSceneNode(), materialName);
}

void SandboxMgr::setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName)
{
    Ogre::SceneNode::ObjectIterator it = pNode->getAttachedObjectIterator();

    while (it.hasMoreElements())
    {
        const Ogre::String movableType =
            it.current()->second->getMovableType();

        if (movableType == Ogre::EntityFactory::FACTORY_TYPE_NAME)
        {
            Ogre::Entity* const entity =
                static_cast<Ogre::Entity*>(it.current()->second);
            entity->setMaterialName(materialName);
        }
        else if (movableType ==
            Ogre::ManualObjectFactory::FACTORY_TYPE_NAME)
        {
            Ogre::ManualObject* const entity =
                static_cast<Ogre::ManualObject*>(it.current()->second);
            unsigned int sections = entity->getNumSections();

            for (unsigned int id = 0; id < sections; ++id)
            {
                entity->setMaterialName(id, materialName);
            }
        }

        it.getNext();
    }
}

void SandboxMgr::UpdateSceneGraph()
{
    Ogre::SceneManager* sceneMgr = GetSceneCreator();
    if (sceneMgr) sceneMgr->_updateSceneGraph(GetSceneGraphCamera());
}

NavigationMesh* SandboxMgr::CreateNavigationMesh(const rcConfig& config, const Ogre::String& navMeshName)
{
	ObjectManager* objectManager = m_objectManager;
	if (objectManager == nullptr) return NULL;

    const std::vector<BlockObject*> objects = objectManager->getFixedObjects();
    NavigationMesh* pNavMesh = new NavigationMesh(config, objects);

    if (!pNavMesh->IsValid())
    {
        SAFE_DELETE(pNavMesh);
        return NULL;
    }

    bool result = objectManager->addNavigationMesh(navMeshName, pNavMesh);
    if (!result)
    {
        SAFE_DELETE(pNavMesh);
        return NULL;
    }

    return pNavMesh;
}

void SandboxMgr::DefaultConfig(rcConfig& config)
{
    config.cs = 0.1f;
    config.ch = 0.1f;
    config.walkableSlopeAngle = DEFAULT_AGENT_WALKABLE_SLOPE;
    config.walkableHeight = static_cast<int>(
        ceilf(AgentLocomotion::DEFAULT_AGENT_HEIGHT / config.ch));
    config.walkableClimb = static_cast<int>(
        floorf(DEFAULT_AGENT_WALKABLE_CLIMB / config.ch));
    config.walkableRadius = static_cast<int>(
        ceilf(AgentLocomotion::DEFAULT_AGENT_RADIUS * 1.25f / config.cs));
    config.maxEdgeLen = static_cast<int>(20.0f / config.cs);
    config.maxSimplificationError = 1.0f;
    config.minRegionArea = static_cast<int>(pow(50.0f, 2));
    config.mergeRegionArea = static_cast<int>(pow(100.0f, 2));
    config.maxVertsPerPoly = 3;
    config.detailSampleDist = 5.0f * config.cs;
    config.detailSampleMaxError = 1.0f * config.ch;

    config.bmin[0] = -100.05f;
    config.bmin[1] = -100.05f;
    config.bmin[2] = -100.05f;

    config.bmax[0] = 100.05f;
    config.bmax[1] = 100.05f;
    config.bmax[2] = 100.05f;

    rcCalcGridSize(config.bmin, config.bmax, config.cs, &config.width, &config.height);
}

void SandboxMgr::ApplySettingConfig(rcConfig& config, float height, float radius, float climb)
{
    if (height > 0.0f)
        config.walkableHeight = static_cast<int>(ceilf(height / config.ch));
    if (radius > 0.0f)
        config.walkableRadius = static_cast<int>(ceilf(radius / config.cs));
    if (climb > 0.0f)
        config.walkableClimb = static_cast<int>(floorf(climb / config.ch));
    
}

Ogre::Vector3 SandboxMgr::RandomPoint(const Ogre::String& navMeshName) const
{
	ObjectManager* objectManager = m_objectManager;
	if (objectManager == nullptr) return Ogre::Vector3::ZERO;

    NavigationMesh* pNavmesh = objectManager->getNavigationMesh(navMeshName);
    if (pNavmesh != NULL)
    {
        return pNavmesh->RandomPoint();
    }
    return Ogre::Vector3::ZERO;
}

Ogre::Vector3 SandboxMgr::FindClosestPoint(const Ogre::String& navMeshName, const Ogre::Vector3& point) const
{
	ObjectManager* objectManager = m_objectManager;
	if (objectManager == nullptr) return Ogre::Vector3::ZERO;

    NavigationMesh* pNavmesh = objectManager->getNavigationMesh(navMeshName);
    if (pNavmesh != NULL)
    {
        return pNavmesh->FindClosestPoint(point);
    }
    return Ogre::Vector3::ZERO;
}

bool SandboxMgr::FindPath(const Ogre::String& navMeshName, const Ogre::Vector3& start, const Ogre::Vector3& end, std::vector<Ogre::Vector3>& outPath) const
{
	ObjectManager* objectManager = m_objectManager;
	if (objectManager == nullptr) return false;

    NavigationMesh* pNavmesh = objectManager->getNavigationMesh(navMeshName);
    if (pNavmesh != NULL)
    {
        return pNavmesh->FindPath(start, end, outPath);
    }
    return false;
}

int SandboxMgr::RayCastObjectId(const Ogre::Vector3& from, const Ogre::Vector3& to) const
{
	ObjectManager* objectManager = m_objectManager;
	PhysicsWorld* physicsWorld = objectManager != nullptr ? objectManager->GetSandboxServices().physics : nullptr;
	if (physicsWorld == nullptr)
	{
		return -1;
	}

	btVector3 hitPoint(0.0f, 0.0f, 0.0f);
	const btRigidBody* rigidBody = nullptr;
	const bool result = physicsWorld->rayCastToRigidBody(
		btVector3(from.x, from.y, from.z),
		btVector3(to.x, to.y, to.z),
		hitPoint,
		rigidBody);
	if (!result)
	{
		return 0;
	}

	const BaseObject* object = rigidBody != nullptr ? static_cast<const BaseObject*>(rigidBody->getUserPointer()) : nullptr;
	return object != nullptr ? static_cast<int>(object->GetObjId()) : -1;
}
