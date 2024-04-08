#include "GameManager.h"
#include "Ogre.h"
#include "Procedural.h"
#include "SandboxDef.h"
#include <algorithm>
#include "ClientManager.h"

using namespace Ogre;

GameManager* g_GameManager = nullptr;

GameManager::GameManager(SceneManager* sceneManager)
	: m_pSceneManager(sceneManager)
{
	m_pRootSceneNode = m_pSceneManager->getRootSceneNode();
}

GameManager::~GameManager()
{
	m_pSceneManager = nullptr;
	m_pRootSceneNode = nullptr;
}

GameManager* GameManager::GetInstance()
{
	return g_GameManager;
}

Camera* GameManager::GetCamera()
{
	return GetClientMgr()->getCamera();
}

SceneManager* GameManager::GetSceneManager()
{
	return m_pRootSceneNode->getCreator();
}

SceneNode* GameManager::CreatePlane(float length, float width)
{
	const Ogre::Real clampedLength = Ogre::Real(std::max(0.0f, length));
	const Ogre::Real clampedWidth = Ogre::Real(std::max(0.0f, width));

	Procedural::PlaneGenerator planeGenerator;
	planeGenerator.setSizeX(clampedLength);
	planeGenerator.setSizeY(clampedWidth);
	
	// TODO : Accept specifiers for UV tiling.
	planeGenerator.setUTile(clampedLength / 2);
	planeGenerator.setVTile(clampedWidth / 2);

	const Ogre::MeshPtr mesh = planeGenerator.realizeMesh();

	Ogre::SceneNode* const plane = m_pRootSceneNode->createChildSceneNode();

	Ogre::Entity* const planeEntity = plane->getCreator()->createEntity(mesh);

	planeEntity->setMaterialName(DEFAULT_MATERIAL);

	plane->attachObject(planeEntity);

	return plane;
}

void GameManager::SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation)
{
	const Ogre::Quaternion& newOrientation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	GetSceneManager()->setSkyBox(true, materialName, 5000.0f, true, newOrientation);
}

void GameManager::SetAmbientLight(const Ogre::Vector3& colourValue)
{
	Ogre::ColourValue tempColor(colourValue.x, colourValue.y, colourValue.z);
	GetSceneManager()->setAmbientLight(tempColor);
}

Ogre::Light* GameManager::CreateDirectionalLight(const Ogre::Vector3& direction)
{
	Ogre::SceneNode* const lightNode = m_pRootSceneNode->createChildSceneNode();

	Ogre::Light* const lightEntity = GetSceneManager()->createLight();

	lightEntity->setCastShadows(true);
	lightEntity->setType(Ogre::Light::LT_DIRECTIONAL);

	lightEntity->setDiffuseColour(1.0f, 1.0f, 1.0f);
	lightEntity->setSpecularColour(0, 0, 0);
	lightEntity->setDirection(direction);

	lightNode->attachObject(lightEntity);

	return lightEntity;
}

void GameManager::setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName)
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
