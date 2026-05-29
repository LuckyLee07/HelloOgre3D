#ifndef __SANDBOX_SERVICES_H__
#define __SANDBOX_SERVICES_H__

class ObjectManager;
class PhysicsWorld;
class SandboxMgr;
class ScriptLuaVM;

struct SandboxServices
{
	ObjectManager* objects = nullptr;
	PhysicsWorld* physics = nullptr;
	SandboxMgr* sandbox = nullptr;
	ScriptLuaVM* script = nullptr;
};

#endif // __SANDBOX_SERVICES_H__
