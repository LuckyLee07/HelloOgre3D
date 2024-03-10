
#include "chapter_0_helloworld/include/DemoHelloWorld.h"
#include "demo_framework/include/LuaScriptUtilities.h"
#include "demo_framework/include/Sandbox.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"

Ogre::AnimationState* state = NULL;
Ogre::AnimationState* state2 = NULL;
Ogre::Entity* meshEntity = NULL;
Ogre::Entity* weaponEntity = NULL;
Ogre::SceneNode* meshNode = NULL;
Ogre::SceneNode* weaponNode = NULL;

DemoHelloWorld::DemoHelloWorld()
    : SandboxApplication("Learning Game AI Programming with Lua - Chapter 0 HelloWorld")
{
}

DemoHelloWorld::~DemoHelloWorld()
{
}

void DemoHelloWorld::Initialize()
{
    SandboxApplication::Initialize();

    //AddResourceLocation("../../../src/chapter_0_helloworld/script");
    //CreateSandbox("Sandbox.lua");

    Ogre::SceneNode* const sandboxNode = GetSceneManager()->getRootSceneNode()->createChildSceneNode();
    Sandbox* pSandbox = new Sandbox(GenerateSandboxId(), sandboxNode, GetCamera());
    pSandbox->Initialize();

    Ogre::Camera* const camera = pSandbox->GetCamera();

    camera->setPosition(Ogre::Vector3(7, 5, -18));

    const Ogre::Quaternion rotation = LuaScriptUtilities::QuaternionFromRotationDegrees(-160, 0, -180);
    camera->setOrientation(rotation);

    Ogre::SceneNode* pRootNode = pSandbox->GetRootNode();

    const Ogre::Quaternion orientation = LuaScriptUtilities::QuaternionFromRotationDegrees(0, 180, 0);
    pRootNode->getCreator()->setSkyBox(true, "ThickCloudsWaterSkyBox", 5000.0f, true, orientation);
    
    pRootNode->getCreator()->setAmbientLight(Ogre::ColourValue(0.3, 0.3, 0.3));
    Ogre::SceneNode* const light = pRootNode->createChildSceneNode();
    Ogre::Light* const lightEntity = pRootNode->getCreator()->createLight();

    lightEntity->setCastShadows(true);
    lightEntity->setType(Ogre::Light::LT_DIRECTIONAL);

    lightEntity->setDiffuseColour(1.0f, 1.0f, 1.0f);
    lightEntity->setSpecularColour(0, 0, 0);
    lightEntity->setDirection(Ogre::Vector3(1, -1, 1));

    light->attachObject(lightEntity);

    lightEntity->setDiffuseColour(1.8, 1.4, 0.9);
    lightEntity->setSpecularColour(1.8, 1.4, 0.9);

    Ogre::SceneNode* const plane = LuaScriptUtilities::CreatePlane(pRootNode, 200, 200);
    const Ogre::Quaternion rotation1 = LuaScriptUtilities::QuaternionFromRotationDegrees(0, 0, 0);
    plane->setOrientation(rotation1);
    plane->setPosition(Ogre::Vector3(0, 0, 0));
    Ogre::MovableObject* pMovable = plane->getAttachedObject(0);
    Ogre::Entity* const planeEntity = dynamic_cast<Ogre::Entity*>(pMovable);
    if (planeEntity != nullptr)
    {
        planeEntity->setMaterialName("Ground2");
    }
    //plane->setMaterialName("");

    //Core.SetMaterial(plane, );
    /*
    Ogre::SceneNode::ObjectIterator it = pRootNode->getAttachedObjectIterator();
    while (it.hasMoreElements())
    {
        const Ogre::String movableType =
            it.current()->second->getMovableType();

        if (movableType == Ogre::EntityFactory::FACTORY_TYPE_NAME)
        {
            Ogre::Entity* const entity =
                static_cast<Ogre::Entity*>(it.current()->second);
            entity->setMaterialName("Ground2");
        }
        else if (movableType ==
            Ogre::ManualObjectFactory::FACTORY_TYPE_NAME)
        {
            Ogre::ManualObject* const entity =
                static_cast<Ogre::ManualObject*>(it.current()->second);
            unsigned int sections = entity->getNumSections();

            for (unsigned int id = 0; id < sections; ++id)
            {
                entity->setMaterialName(id, "Ground2");
            }
        }

        it.getNext();
    }
    */
}

void DemoHelloWorld::Update()
{
    SandboxApplication::Update();
}