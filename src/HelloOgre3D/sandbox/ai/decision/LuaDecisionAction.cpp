#include "LuaDecisionAction.h"

#include "components/ai/AIController.h"
#include "objects/SoldierObject.h"
#include "scripting/LuaPluginMgr.h"
#include "ai/decision/DecisionTreeDriver.h"
#include "ai/common/Blackboard.h"
#include "profiling/Profile.h"

LuaDecisionAction::LuaDecisionAction(const std::string& name, SoldierObject* owner)
	: DecisionAction(name), m_owner(owner)
{
}

LuaDecisionAction::~LuaDecisionAction()
{
}

bool LuaDecisionAction::BindToScript(const std::string& filepath)
{
	return LuaPluginMgr::BindLuaPluginByFile(this, filepath);
}

static Blackboard* _GetBlackboardFromOwner(SoldierObject* owner)
{
	if (!owner) return nullptr;
	AIController* ai = owner->GetAIController();
	DecisionTreeDriver* driver = ai ? ai->GetDecisionTreeDriver() : nullptr;
	return driver ? driver->GetBlackboard() : nullptr;
}

void LuaDecisionAction::OnInitialize()
{
	H3D_PROFILE_SCOPE("LuaDecisionAction::OnInitialize");
	// Lua signature: function OnInitialize(owner, blackboard) ... end
	Blackboard* bb = _GetBlackboardFromOwner(m_owner);
	callFunction("OnInitialize", "u[SoldierObject]u[Blackboard]", m_owner, bb);
}

DecisionAction::Status LuaDecisionAction::OnUpdate(float deltaMs)
{
	H3D_PROFILE_SCOPE("LuaDecisionAction::OnUpdate");
	// Lua signature: function OnUpdate(deltaMs, owner, blackboard) return status end
	// Status: 1 = RUNNING, 2 = TERMINATED.
	Blackboard* bb = _GetBlackboardFromOwner(m_owner);
	int statusOut = (int)STATUS_TERMINATED;
	callFunction("OnUpdate", "iu[SoldierObject]u[Blackboard]>i", (int)deltaMs, m_owner, bb, &statusOut);
	if (statusOut == (int)STATUS_RUNNING) return STATUS_RUNNING;
	return STATUS_TERMINATED;
}

void LuaDecisionAction::OnCleanUp()
{
	H3D_PROFILE_SCOPE("LuaDecisionAction::OnCleanUp");
	// Lua signature: function OnCleanUp(owner, blackboard) ... end
	Blackboard* bb = _GetBlackboardFromOwner(m_owner);
	callFunction("OnCleanUp", "u[SoldierObject]u[Blackboard]", m_owner, bb);
}
