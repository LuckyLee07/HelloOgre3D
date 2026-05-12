#include "LuaBehaviorAction.h"

#include "objects/SoldierObject.h"
#include "scripting/LuaPluginMgr.h"
#include "ai/behavior/BehaviorTreeDriver.h"
#include "ai/common/Blackboard.h"

LuaBehaviorAction::LuaBehaviorAction(const std::string& name, SoldierObject* owner)
	: BehaviorAction(name), m_owner(owner)
{
}

LuaBehaviorAction::~LuaBehaviorAction()
{
}

bool LuaBehaviorAction::BindToScript(const std::string& filepath)
{
	return LuaPluginMgr::BindLuaPluginByFile(this, filepath);
}

static Blackboard* _GetBlackboardFromOwner(SoldierObject* owner)
{
	if (!owner) return nullptr;
	BehaviorTreeDriver* driver = owner->GetBehaviorTreeDriver();
	return driver ? driver->GetBlackboard() : nullptr;
}

void LuaBehaviorAction::OnInitialize()
{
	// Lua: function OnInitialize(owner, blackboard) ... end
	Blackboard* bb = _GetBlackboardFromOwner(m_owner);
	callFunction("OnInitialize", "u[SoldierObject]u[Blackboard]", m_owner, bb);
}

BehaviorAction::Status LuaBehaviorAction::OnUpdate(float deltaMs)
{
	// Lua: function OnUpdate(deltaMs, owner, blackboard) return status end
	// 返回值: 1=RUNNING, 2=SUCCESS, 3=FAILURE
	Blackboard* bb = _GetBlackboardFromOwner(m_owner);
	int statusOut = (int)STATUS_FAILURE;
	callFunction("OnUpdate", "iu[SoldierObject]u[Blackboard]>i", (int)deltaMs, m_owner, bb, &statusOut);

	if (statusOut == (int)STATUS_RUNNING) return STATUS_RUNNING;
	if (statusOut == (int)STATUS_SUCCESS) return STATUS_SUCCESS;
	return STATUS_FAILURE;
}

void LuaBehaviorAction::OnCleanUp()
{
	Blackboard* bb = _GetBlackboardFromOwner(m_owner);
	callFunction("OnCleanUp", "u[SoldierObject]u[Blackboard]", m_owner, bb);
}
