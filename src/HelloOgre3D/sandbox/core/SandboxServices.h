#ifndef __SANDBOX_SERVICES_H__
#define __SANDBOX_SERVICES_H__

class ObjectManager;
class PhysicsWorld;
class InputManager;
class OgreCameraController;
class ScriptLuaVM;
class ObjectFactory;
class AgentConfigService;
class NavigationService;
class RaycastService;
class SceneService;
class ScriptService;
class CameraService;

struct SandboxServices
{
	ObjectManager* objects = nullptr;
	PhysicsWorld* physics = nullptr;
	InputManager* input = nullptr;
	OgreCameraController* cameraController = nullptr;
	CameraService* camera = nullptr;
	ScriptLuaVM* script = nullptr;
	ObjectFactory* objectFactory = nullptr;
	AgentConfigService* agentConfig = nullptr;
	NavigationService* navigation = nullptr;
	RaycastService* raycast = nullptr;
	SceneService* scene = nullptr;
	ScriptService* scriptService = nullptr;
};

#endif // __SANDBOX_SERVICES_H__
