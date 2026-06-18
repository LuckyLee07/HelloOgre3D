#ifndef __SANDBOX_SERVICES_H__
#define __SANDBOX_SERVICES_H__

class ObjectManager;
class PhysicsWorld;
class InputManager;
class SandboxMgr;
class ScriptLuaVM;
class ObjectFactory;

struct SandboxServices
{
	ObjectManager* objects = nullptr;
	PhysicsWorld* physics = nullptr;
	InputManager* input = nullptr;
	SandboxMgr* sandbox = nullptr;
	ScriptLuaVM* script = nullptr;
	ObjectFactory* objectFactory = nullptr;
};

#endif // __SANDBOX_SERVICES_H__
