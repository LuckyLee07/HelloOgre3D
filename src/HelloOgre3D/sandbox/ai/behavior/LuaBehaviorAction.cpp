#include "LuaBehaviorAction.h"

#include "components/ai/AIController.h"
#include "objects/SoldierObject.h"
#include "scripting/LuaPluginMgr.h"
#include "ai/behavior/BehaviorTreeDriver.h"
#include "ai/common/Blackboard.h"
#include "profiling/Profile.h"

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
	AIController* ai = owner->GetAIController();
	BehaviorTreeDriver* driver = ai ? ai->GetBehaviorTreeDriver() : nullptr;
	return driver ? driver->GetBlackboard() : nullptr;
}

static const char* _StatusToString(BehaviorAction::Status status)
{
	switch (status)
	{
	case BehaviorAction::STATUS_RUNNING:
		return "RUNNING";
	case BehaviorAction::STATUS_SUCCESS:
		return "SUCCESS";
	case BehaviorAction::STATUS_FAILURE:
		return "FAILURE";
	case BehaviorAction::STATUS_INVALID:
	default:
		return "INVALID";
	}
}

void LuaBehaviorAction::OnInitialize()
{
	H3D_PROFILE_SCOPE("LuaBehaviorAction::OnInitialize");
	// Lua: function OnInitialize(owner, blackboard) ... end
	Blackboard* bb = _GetBlackboardFromOwner(m_owner);
	if (bb != nullptr)
	{
		bb->SetString("__bt.currentAction", GetName());
		bb->SetString("__bt.currentActionStatus", "ENTER");
	}
	callFunction("OnInitialize", "u[SoldierObject]u[Blackboard]", m_owner, bb);
}

BehaviorAction::Status LuaBehaviorAction::OnUpdate(float deltaMs)
{
	H3D_PROFILE_SCOPE("LuaBehaviorAction::OnUpdate");
	// Lua: function OnUpdate(deltaMs, owner, blackboard) return status end
	// 返回值: 1=RUNNING, 2=SUCCESS, 3=FAILURE
	Blackboard* bb = _GetBlackboardFromOwner(m_owner);
	int statusOut = (int)STATUS_FAILURE;
	callFunction("OnUpdate", "iu[SoldierObject]u[Blackboard]>i", (int)deltaMs, m_owner, bb, &statusOut);

	Status status = STATUS_FAILURE;
	if (statusOut == (int)STATUS_RUNNING)
		status = STATUS_RUNNING;
	else if (statusOut == (int)STATUS_SUCCESS)
		status = STATUS_SUCCESS;
	if (bb != nullptr)
	{
		bb->SetString("__bt.currentAction", GetName());
		bb->SetString("__bt.currentActionStatus", _StatusToString(status));
	}
	return status;
}

void LuaBehaviorAction::OnCleanUp()
{
	H3D_PROFILE_SCOPE("LuaBehaviorAction::OnCleanUp");
	Blackboard* bb = _GetBlackboardFromOwner(m_owner);
	if (bb != nullptr)
	{
		bb->SetString("__bt.currentAction", GetName());
		bb->SetString("__bt.currentActionStatus", "CLEANUP");
	}
	callFunction("OnCleanUp", "u[SoldierObject]u[Blackboard]", m_owner, bb);
}
