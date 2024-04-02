#include "OgreWrapper.h"
#include "Ogre.h"
#include "Procedural.h"
#include "SandboxDef.h"
#include <algorithm>
#include "SandboxManager.h"

using namespace Ogre;

OgreWrapper* g_OgreWrapper = nullptr;

OgreWrapper::OgreWrapper(SceneManager* sceneManager)
	: m_pSceneManager(sceneManager)
{
	m_pRootSceneNode = m_pSceneManager->getRootSceneNode();
}

OgreWrapper::~OgreWrapper()
{
	m_pSceneManager = nullptr;
	m_pRootSceneNode = nullptr;
}

OgreWrapper* OgreWrapper::GetInstance()
{
	return g_OgreWrapper;
}

SceneNode* OgreWrapper::CreatePlane(float length, float width)
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

void OgreWrapper::CreateSkyBox(const Ogre::String materialName, Ogre::Vector3& rotation)
{
	const Ogre::Quaternion& newOrientation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
	m_pRootSceneNode->getCreator()->setSkyBox(true, materialName, 5000.0f, true, newOrientation);
}

Camera* OgreWrapper::GetCamera()
{
	return GetSandboxMgr()->getCamera();
}