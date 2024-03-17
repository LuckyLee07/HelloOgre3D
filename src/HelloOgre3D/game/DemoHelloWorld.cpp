
#include "DemoHelloWorld.h"
#include "Ogre.h"
#include "Procedural.h"
#include "ScriptLuaVM.h"
#include "LuaInterface.h"
#include "CCLogSystem.h"

#define DEFAULT_MATERIAL "White"

extern int tolua_open_all(lua_State* tolua_S);

static Ogre::Quaternion QuaternionFromRotationDegrees(
	Ogre::Real xRotation, Ogre::Real yRotation, Ogre::Real zRotation)
{
	Ogre::Matrix3 matrix;
	matrix.FromEulerAnglesXYZ(
		Ogre::Degree(xRotation), Ogre::Degree(yRotation), Ogre::Degree(zRotation));
	return Ogre::Quaternion(matrix);
}

Ogre::SceneNode* CreatePlane(
	Ogre::SceneNode* const parentNode,
	const Ogre::Real length,
	const Ogre::Real width)
{
	assert(parentNode);

	const Ogre::Real clampedLength = std::max(Ogre::Real(0), length);
	const Ogre::Real clampedWidth = std::max(Ogre::Real(0), width);

	Procedural::PlaneGenerator planeGenerator;
	planeGenerator.setSizeX(clampedLength);
	planeGenerator.setSizeY(clampedWidth);
	// TODO(David Young): Accept specifiers for UV tiling.
	planeGenerator.setUTile(clampedLength / 2);
	planeGenerator.setVTile(clampedWidth / 2);

	const Ogre::MeshPtr mesh = planeGenerator.realizeMesh();

	Ogre::SceneNode* const plane = parentNode->createChildSceneNode();

	Ogre::Entity* const planeEntity = plane->getCreator()->createEntity(mesh);

	planeEntity->setMaterialName(DEFAULT_MATERIAL);

	plane->attachObject(planeEntity);

	return plane;
}

DemoHelloWorld::DemoHelloWorld() : Application("Learning Game AI Programming")
{
}

DemoHelloWorld::~DemoHelloWorld()
{
}

void DemoHelloWorld::Initialize()
{
    Application::Initialize();

	// 设置ToLua对象 
	ScriptLuaVM* pScriptVM = ScriptLuaVM::GetInstance();
	tolua_open_all(pScriptVM->getLuaState());

	// 设置lua可用的c++对象 
	pScriptVM->setUserTypePointer("LuaInterface", "LuaInterface", LuaInterface::GetInstance());

	pScriptVM->callFile("res/scripts/script_init.lua");
	pScriptVM->callString("sayhello('Hello', 11, 99)");
	int result = 0;
	pScriptVM->callFunction("sayhello", "sis>i", "HelloWorld", 11, "hh", &result);
	pScriptVM->callFunction("sayhello", "sii>i", "HelloWorld", 11, 99, &result);
	CCLOG_INFO("Fxkk======>>> %d", result);

	GetCamera()->setPosition(Ogre::Vector3(7, 5, -18));
	const Ogre::Quaternion rotation = QuaternionFromRotationDegrees(-160, 0, -180);
	GetCamera()->setOrientation(rotation);

	const Ogre::Quaternion orientation = QuaternionFromRotationDegrees(-160, 0, -180);

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
	
	Ogre::SceneNode* const plane = CreatePlane(pSandboxRootNode, 200, 200);
	const Ogre::Quaternion rotation1 = QuaternionFromRotationDegrees(0, 0, 0);
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