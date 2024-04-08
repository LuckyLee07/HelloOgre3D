
#include "DemoHelloWorld.h"
#include "Ogre.h"
#include "Procedural.h"

#include "ScriptLuaVM.h"
#include "LuaInterface.h"
#include "LogSystem.h"
#include "SandboxDef.h"
#include "GameManager.h"
#include "ClientManager.h"

extern int tolua_open_all(lua_State* tolua_S);


DemoHelloWorld::DemoHelloWorld()
	: Application("Learning Game AI Programming")
{
}

DemoHelloWorld::~DemoHelloWorld()
{
}

void DemoHelloWorld::Update()
{
    Application::Update();
}

void DemoHelloWorld::Run()
{
	if (!Application::Setup()) 
		return;
	
	this->InitLuaEnv();

	this->InitGameScene();

	Application::Draw();
}

void DemoHelloWorld::InitLuaEnv()
{
	// 设置ToLua对象 
	ScriptLuaVM* pScriptVM = GetScriptLuaVM();
	tolua_open_all(pScriptVM->getLuaState());

	// 设置lua可用的c++对象 
	pScriptVM->setUserTypePointer("Sandbox", "GameManager", GameManager::GetInstance());
	pScriptVM->setUserTypePointer("LuaInterface", "LuaInterface", LuaInterface::GetInstance());

	pScriptVM->callFile("res/scripts/script_init.lua");
}

void DemoHelloWorld::InitGameScene()
{
	GetScriptLuaVM()->callFunction("Sandbox_Initialize", ">");
	/*
	GetSandboxMgr()->getCamera()->setPosition(Ogre::Vector3(7, 5, -18));
	const Ogre::Quaternion rotation = QuaternionFromRotationDegrees(-160, 0, -180);
	GetSandboxMgr()->getCamera()->setOrientation(rotation);

	const Ogre::Quaternion orientation = QuaternionFromRotationDegrees(-160, 0, -180);

	Ogre::SceneNode* const pSandboxRootNode = GetSandboxMgr()->getRootSceneNode();
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

	Ogre::SceneNode* const plane = g_OgreWrapper->CreatePlane(200, 200);
	const Ogre::Quaternion rotation1 = QuaternionFromRotationDegrees(0, 0, 0);
	plane->setOrientation(rotation1);
	plane->setPosition(Ogre::Vector3(0, 0, 0));
	Ogre::MovableObject* pMovable = plane->getAttachedObject(0);
	Ogre::Entity* const planeEntity = dynamic_cast<Ogre::Entity*>(pMovable);
	if (planeEntity != nullptr)
	{
		planeEntity->setMaterialName("Ground2");
	}
	*/
}