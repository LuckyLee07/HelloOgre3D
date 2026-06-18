#include "LuaBehaviorAction.h"

#include "components/ai/AIController.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "scripting/LuaPluginMgr.h"
#include "ai/behavior/BehaviorTreeDriver.h"
#include "ai/common/Blackboard.h"
#include "profiling/Profile.h"

LuaBehaviorAction::LuaBehaviorAction(const std::string& name, AgentObject* owner, Blackboard* blackboard)
	: BehaviorAction(name), m_owner(owner), m_blackboard(blackboard)
{
}

LuaBehaviorAction::LuaBehaviorAction(const std::string& name, SoldierObject* owner)
	: LuaBehaviorAction(name, static_cast<AgentObject*>(owner), nullptr)
{
}

LuaBehaviorAction::~LuaBehaviorAction()
{
}

bool LuaBehaviorAction::BindToScript(const std::string& filepath)
{
	return LuaPluginMgr::BindLuaPluginByFile(this, filepath);
}

SoldierObject* LuaBehaviorAction::GetOwner() const
{
	return dynamic_cast<SoldierObject*>(m_owner);
}

Blackboard* LuaBehaviorAction::ResolveBlackboard() const
{
	if (m_blackboard != nullptr)
		return m_blackboard;
	if (!m_owner) return nullptr;
	AIController* ai = m_owner->FindComponent<AIController>();
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
	Blackboard* bb = ResolveBlackboard();
	if (bb != nullptr)
	{
		bb->SetString("__bt.currentAction", GetName());
		bb->SetString("__bt.currentActionStatus", "ENTER");
	}
	SoldierObject* soldier = GetOwner();
	if (soldier != nullptr)
		callFunction("OnInitialize", "u[SoldierObject]u[Blackboard]", soldier, bb);
	else
		callFunction("OnInitialize", "u[AgentObject]u[Blackboard]", m_owner, bb);
}

BehaviorAction::Status LuaBehaviorAction::OnUpdate(float deltaMs)
{
	H3D_PROFILE_SCOPE("LuaBehaviorAction::OnUpdate");
	// Lua: function OnUpdate(deltaMs, owner, blackboard) return status end
	// 返回值: 1=RUNNING, 2=SUCCESS, 3=FAILURE
	Blackboard* bb = ResolveBlackboard();
	int statusOut = (int)STATUS_FAILURE;
	SoldierObject* soldier = GetOwner();
	if (soldier != nullptr)
		callFunction("OnUpdate", "iu[SoldierObject]u[Blackboard]>i", (int)deltaMs, soldier, bb, &statusOut);
	else
		callFunction("OnUpdate", "iu[AgentObject]u[Blackboard]>i", (int)deltaMs, m_owner, bb, &statusOut);

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
	Blackboard* bb = ResolveBlackboard();
	if (bb != nullptr)
	{
		bb->SetString("__bt.currentAction", GetName());
		bb->SetString("__bt.currentActionStatus", "CLEANUP");
	}
	SoldierObject* soldier = GetOwner();
	if (soldier != nullptr)
		callFunction("OnCleanUp", "u[SoldierObject]u[Blackboard]", soldier, bb);
	else
		callFunction("OnCleanUp", "u[AgentObject]u[Blackboard]", m_owner, bb);
}
