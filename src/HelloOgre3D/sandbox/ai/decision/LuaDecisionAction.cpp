#include "LuaDecisionAction.h"

#include "components/ai/AIController.h"
#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"
#include "scripting/LuaPluginMgr.h"
#include "ai/decision/DecisionTreeDriver.h"
#include "ai/common/Blackboard.h"
#include "profiling/Profile.h"

LuaDecisionAction::LuaDecisionAction(const std::string& name, AgentObject* owner, Blackboard* blackboard)
	: DecisionAction(name), m_owner(owner), m_blackboard(blackboard)
{
}

LuaDecisionAction::LuaDecisionAction(const std::string& name, SoldierObject* owner)
	: LuaDecisionAction(name, static_cast<AgentObject*>(owner), nullptr)
{
}

LuaDecisionAction::~LuaDecisionAction()
{
}

bool LuaDecisionAction::BindToScript(const std::string& filepath)
{
	return LuaPluginMgr::BindLuaPluginByFile(this, filepath);
}

Blackboard* LuaDecisionAction::ResolveBlackboard() const
{
	if (m_blackboard != nullptr)
		return m_blackboard;
	if (!m_owner) return nullptr;
	AIController* ai = m_owner->FindComponent<AIController>();
	DecisionTreeDriver* driver = ai ? ai->GetDecisionTreeDriver() : nullptr;
	return driver ? driver->GetBlackboard() : nullptr;
}

static const char* _StatusToString(DecisionAction::Status status)
{
	switch (status)
	{
	case DecisionAction::STATUS_UNINITIALIZED:
		return "UNINITIALIZED";
	case DecisionAction::STATUS_RUNNING:
		return "RUNNING";
	case DecisionAction::STATUS_TERMINATED:
		return "TERMINATED";
	default:
		return "UNKNOWN";
	}
}

static bool _IsDecisionTraceEnabled(Blackboard* bb)
{
	return bb != nullptr && bb->GetBool("__dt.traceEnabled", false);
}

void LuaDecisionAction::OnInitialize()
{
	H3D_PROFILE_SCOPE("LuaDecisionAction::OnInitialize");
	// Lua signature: function OnInitialize(owner, blackboard) ... end
	Blackboard* bb = ResolveBlackboard();
	if (_IsDecisionTraceEnabled(bb))
	{
		bb->SetString("__dt.currentAction", GetName());
		bb->SetString("__dt.currentActionStatus", "ENTER");
	}
	callFunction("OnInitialize", "u[AgentObject]u[Blackboard]", m_owner, bb);
}

DecisionAction::Status LuaDecisionAction::OnUpdate(float deltaMs)
{
	H3D_PROFILE_SCOPE("LuaDecisionAction::OnUpdate");
	// Lua signature: function OnUpdate(deltaMs, owner, blackboard) return status end
	// Status: 1 = RUNNING, 2 = TERMINATED.
	Blackboard* bb = ResolveBlackboard();
	int statusOut = (int)STATUS_TERMINATED;
	callFunction("OnUpdate", "iu[AgentObject]u[Blackboard]>i", (int)deltaMs, m_owner, bb, &statusOut);
	Status status = statusOut == (int)STATUS_RUNNING ? STATUS_RUNNING : STATUS_TERMINATED;
	if (_IsDecisionTraceEnabled(bb))
	{
		bb->SetString("__dt.currentAction", GetName());
		bb->SetString("__dt.currentActionStatus", _StatusToString(status));
	}
	return status;
}

void LuaDecisionAction::OnCleanUp()
{
	H3D_PROFILE_SCOPE("LuaDecisionAction::OnCleanUp");
	// Lua signature: function OnCleanUp(owner, blackboard) ... end
	Blackboard* bb = ResolveBlackboard();
	if (_IsDecisionTraceEnabled(bb))
	{
		bb->SetString("__dt.currentAction", GetName());
		bb->SetString("__dt.currentActionStatus", "CLEANUP");
	}
	callFunction("OnCleanUp", "u[AgentObject]u[Blackboard]", m_owner, bb);
}
