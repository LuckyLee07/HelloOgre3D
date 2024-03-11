
#include "chapter_0_helloworld/include/DemoHelloWorld.h"
#include "demo_framework/include/LuaScriptUtilities.h"
#include "ogre3d/include/Ogre.h"

Ogre::AnimationState* state = NULL;
Ogre::AnimationState* state2 = NULL;
Ogre::Entity* meshEntity = NULL;
Ogre::Entity* weaponEntity = NULL;
Ogre::SceneNode* meshNode = NULL;
Ogre::SceneNode* weaponNode = NULL;

DemoHelloWorld::DemoHelloWorld() : Application("Learning Game AI Programming")
{
}

DemoHelloWorld::~DemoHelloWorld()
{
}

void DemoHelloWorld::customInit()
{
	const Ogre::ColourValue ambient(0.0f, 0.0f, 0.0f);

	GetRenderWindow()->getViewport(0)->setBackgroundColour(ambient);
	GetSceneManager()->setAmbientLight(ambient);
	GetSceneManager()->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

	GetCamera()->setFarClipDistance(1000.0f);
	GetCamera()->setNearClipDistance(0.1f);
	GetCamera()->setAutoAspectRatio(true);

	GetRenderWindow()->setDeactivateOnFocusChange(false);

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(4);
}

void DemoHelloWorld::Initialize()
{
    //SandboxApplication::Initialize();
	this->customInit();

	//Gorilla::Silverback* mSilverback = new Gorilla::Silverback();
	//AddResourceLocation("../../../src/demo_framework/script");
    //AddResourceLocation("../../../src/chapter_1_introduction/script");

    //CreateSandbox("Sandbox.lua");

	GetCamera()->setPosition(Ogre::Vector3(7, 5, -18));
	const Ogre::Quaternion rotation = LuaScriptUtilities::QuaternionFromRotationDegrees(-160, 0, -180);
	GetCamera()->setOrientation(rotation);

	const Ogre::Quaternion orientation = LuaScriptUtilities::QuaternionFromRotationDegrees(-160, 0, -180);

	Ogre::SceneNode* const pSandboxRootNode = GetSceneManager()->getRootSceneNode()->createChildSceneNode();
	pSandboxRootNode->getCreator()->setSkyBox(true, "ThickCloudsWaterSkyBox", 5000.0f, true, orientation);

	pSandboxRootNode->getCreator()->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
	Ogre::SceneNode* const light = pSandboxRootNode->createChildSceneNode();
	Ogre::Light* const lightEntity = pSandboxRootNode->getCreator()->createLight();

	lightEntity->setCastShadows(true);
	lightEntity->setType(Ogre::Light::LT_DIRECTIONAL);

	lightEntity->setDiffuseColour(1.0f, 1.0f, 1.0f);
	lightEntity->setSpecularColour(0, 0, 0);
	lightEntity->setDirection(Ogre::Vector3(1, -1, 1));

	light->attachObject(lightEntity);

	lightEntity->setDiffuseColour(1.8, 1.4, 0.9);
	lightEntity->setSpecularColour(1.8, 1.4, 0.9);

	Ogre::SceneNode* const plane = LuaScriptUtilities::CreatePlane(pSandboxRootNode, 200, 200);
	const Ogre::Quaternion rotation1 = LuaScriptUtilities::QuaternionFromRotationDegrees(0, 0, 0);
	plane->setOrientation(rotation1);
	plane->setPosition(Ogre::Vector3(0, 0, 0));
	Ogre::MovableObject* pMovable = plane->getAttachedObject(0);
	Ogre::Entity* const planeEntity = dynamic_cast<Ogre::Entity*>(pMovable);
	if (planeEntity != nullptr)
	{
		planeEntity->setMaterialName("Ground2");
	}
}

void DemoHelloWorld::Update()
{
    //SandboxApplication::Update();
}