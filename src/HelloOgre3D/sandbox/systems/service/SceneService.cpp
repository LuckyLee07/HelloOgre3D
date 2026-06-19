#include "SceneService.h"
#include "GameFunction.h"
#include "objects/BlockObject.h"
#include "systems/service/CameraService.h"
#include "OgreCamera.h"
#include "OgreColourValue.h"
#include "OgreEntity.h"
#include "OgreLight.h"
#include "OgreManualObject.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include <cassert>

SceneService::SceneService(Ogre::SceneManager* sceneManager, CameraService* cameraService)
	: m_sceneManager(sceneManager),
	m_cameraService(cameraService)
{
}

SceneService::~SceneService()
{
	m_sceneManager = nullptr;
	m_cameraService = nullptr;
}

void SceneService::SetSceneManager(Ogre::SceneManager* sceneManager)
{
	m_sceneManager = sceneManager;
}

void SceneService::SetCameraService(CameraService* cameraService)
{
	m_cameraService = cameraService;
}

Ogre::SceneManager* SceneService::GetSceneManager() const
{
	return m_sceneManager;
}

Ogre::SceneNode* SceneService::GetRootSceneNode() const
{
	return m_sceneManager != nullptr ? m_sceneManager->getRootSceneNode() : nullptr;
}

Ogre::Camera* SceneService::GetSceneGraphCamera() const
{
	return m_cameraService != nullptr ? m_cameraService->GetCamera() : nullptr;
}

void SceneService::SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation)
{
	Ogre::SceneManager* sceneMgr = GetSceneManager();
	if (sceneMgr == nullptr)
		return;

	const Ogre::Quaternion& newOrientation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	sceneMgr->setSkyBox(true, materialName, 5000.0f, true, newOrientation);
}

void SceneService::SetAmbientLight(const Ogre::Vector3& colourValue)
{
	Ogre::SceneManager* sceneMgr = GetSceneManager();
	if (sceneMgr == nullptr)
		return;

	Ogre::ColourValue tempColor(colourValue.x, colourValue.y, colourValue.z);
	sceneMgr->setAmbientLight(tempColor);
}

Ogre::Light* SceneService::CreateDirectionalLight(const Ogre::Vector3& direction)
{
	Ogre::SceneManager* sceneMgr = GetSceneManager();
	Ogre::SceneNode* rootNode = GetRootSceneNode();
	if (sceneMgr == nullptr || rootNode == nullptr)
		return nullptr;

	Ogre::SceneNode* const lightNode = rootNode->createChildSceneNode();
	Ogre::Light* const lightEntity = sceneMgr->createLight();

	lightEntity->setCastShadows(true);
	lightEntity->setType(Ogre::Light::LT_DIRECTIONAL);
	lightEntity->setDiffuseColour(1.0f, 1.0f, 1.0f);
	lightEntity->setSpecularColour(0, 0, 0);
	lightEntity->setDirection(direction);

	lightNode->attachObject(lightEntity);

	return lightEntity;
}

void SceneService::setMaterial(BlockObject* pObject, const Ogre::String& materialName)
{
	assert(pObject != nullptr);
	setMaterial(pObject->GetSceneNode(), materialName);
}

void SceneService::setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName)
{
	if (pNode == nullptr)
		return;

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

void SceneService::UpdateSceneGraph()
{
	Ogre::SceneManager* sceneMgr = GetSceneManager();
	if (sceneMgr != nullptr)
		sceneMgr->_updateSceneGraph(GetSceneGraphCamera());
}
