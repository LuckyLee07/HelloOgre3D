#include "OgreWrapper.h"
#include "Ogre.h"
#include "Procedural.h"
#include "SandboxDef.h"
#include <algorithm>

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

SceneNode* OgreWrapper::CreatePlane(int length, int width)
{
	const Ogre::Real clampedLength = Ogre::Real(std::max(0, length));
	const Ogre::Real clampedWidth = Ogre::Real(std::max(0, width));

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
