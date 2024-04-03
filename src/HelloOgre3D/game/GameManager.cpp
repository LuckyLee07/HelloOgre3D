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

void GameManager::CreateSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation)
{
	const Ogre::Quaternion& newOrientation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	GetSceneManager()->setSkyBox(true, materialName, 5000.0f, true, newOrientation);
}

void GameManager::SetAmbientLight(const Ogre::Vector3& colourValue)
{
	Ogre::ColourValue tempColor(colourValue.x, colourValue.y, colourValue.z);
	GetSceneManager()->setAmbientLight(tempColor);
}

