#include "SandboxMgr.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"
#include "ScriptLuaVM.h"
#include "GameFunction.h"
#include "GameManager.h"
#include "objects/BlockObject.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "recast/include/Recast.h"
#include "ai/navigation/NavigationMesh.h"
#include "components/agent/AgentLocomotion.h"
#include "ObjectManager.h"

SandboxMgr* g_SandboxMgr = nullptr;

namespace
{
	const float DEFAULT_AGENT_WALKABLE_CLIMB = AgentLocomotion::DEFAULT_AGENT_RADIUS / 2.0f;
	const float DEFAULT_AGENT_WALKABLE_SLOPE = 45.0f;
}

SandboxMgr::SandboxMgr(UIService& uiService,
	CameraService& cameraService,
	ObjectFactory& objectFactory,
	Ogre::SceneManager* sceneManager)
	: m_uiService(uiService), 
	m_cameraService(cameraService),
	m_objectFactory(objectFactory),
	m_pRootSceneNode(nullptr)
{
	m_pRootSceneNode = sceneManager->getRootSceneNode();
}

SandboxMgr::~SandboxMgr()
{
    m_pRootSceneNode = nullptr;
}

SandboxMgr* SandboxMgr::GetInstance()
{
	return g_SandboxMgr;
}

Ogre::SceneManager* SandboxMgr::GetSceneCreator()
{
	return m_pRootSceneNode->getCreator();
}

Ogre::Camera* SandboxMgr::GetCamera()
{
	return m_cameraService.GetCamera();
}

Ogre::Vector3 SandboxMgr::GetCameraUp()
{
	return m_cameraService.GetCameraUp();
}

Ogre::Vector3 SandboxMgr::GetCameraLeft()
{
	return m_cameraService.GetCameraLeft();
}

Ogre::Vector3 SandboxMgr::GetCameraForward()
{
	return m_cameraService.GetCameraForward();
}

Ogre::Vector3 SandboxMgr::GetCameraPosition()
{
	return m_cameraService.GetCameraPosition();
}

Ogre::Vector3 SandboxMgr::GetCameraRotation()
{
	return m_cameraService.GetCameraRotation();
}

Ogre::Quaternion SandboxMgr::GetCameraOrientation()
{
	return m_cameraService.GetCameraOrientation();
}

long long SandboxMgr::GetRenderTime()
{
	return m_cameraService.GetRenderTime();
}

long long SandboxMgr::GetSimulateTime()
{
	return m_cameraService.GetSimulateTime();
}

long long SandboxMgr::GetTotalSimulateTime()
{
	return m_cameraService.GetTotalSimulateTime();
}

void SandboxMgr::CallFile(const Ogre::String& filepath)
{
    GetScriptLuaVM()->callFile(filepath.c_str());
}

BlockObject* SandboxMgr::CreatePlane(float length, float width)
{
	return m_objectFactory.CreatePlane(length, width);
}

BlockObject* SandboxMgr::CreateBlockObject(const Ogre::String& meshfilePath)
{
	return m_objectFactory.CreateBlockObject(meshfilePath);
}

BlockObject* SandboxMgr::CreateBlockBox(float width, float height, float length, float uTile, float vTile)
{
	return m_objectFactory.CreateBlockBox(width, height, length, uTile, vTile);
}

BlockObject* SandboxMgr::CreateBullet(Ogre::Real height, Ogre::Real radius)
{
	return m_objectFactory.CreateBullet(height, radius);
}

AgentObject* SandboxMgr::CreateAgent(AGENT_OBJ_TYPE agentType, const char* filepath/* = nullptr */)
{
	return m_objectFactory.CreateAgent(agentType, filepath);
}

SoldierObject* SandboxMgr::CreateSoldier(const Ogre::String& meshFile, const char* filepath/* = nullptr */)
{
	return m_objectFactory.CreateSoldier(meshFile, filepath);
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

UIFrame* SandboxMgr::CreateUIFrame(unsigned int index/* = 1 */)
{
	return m_uiService.CreateUIFrame(index);
}

void SandboxMgr::SetMarkupColor(unsigned int index, const Ogre::ColourValue& color)
{
    return m_uiService.SetMarkupColor(index, color);
}

void SandboxMgr::UpdateSceneGraph()
{
    Ogre::SceneManager* sceneMgr = GetSceneCreator();
    if (sceneMgr) sceneMgr->_updateSceneGraph(GetCamera());
}

NavigationMesh* SandboxMgr::CreateNavigationMesh(const rcConfig& config, const Ogre::String& navMeshName)
{
	ObjectManager* objectManager = m_objectFactory.GetObjectManager();
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
	ObjectManager* objectManager = m_objectFactory.GetObjectManager();
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
	ObjectManager* objectManager = m_objectFactory.GetObjectManager();
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
	ObjectManager* objectManager = m_objectFactory.GetObjectManager();
	if (objectManager == nullptr) return false;

    NavigationMesh* pNavmesh = objectManager->getNavigationMesh(navMeshName);
    if (pNavmesh != NULL)
    {
        return pNavmesh->FindPath(start, end, outPath);
    }
    return false;
}
