#include "DecisionBranch.h"

#include "DecisionAction.h"
#include "ScriptLuaVM.h"
#include "LogSystem.h"

extern "C" {
#include "lauxlib.h"
}

DecisionBranch::DecisionBranch()
	: m_evalLuaRef(LUA_NOREF)
{
}

DecisionBranch::~DecisionBranch()
{
	if (m_evalLuaRef != LUA_NOREF && m_evalLuaRef != LUA_REFNIL)
	{
		lua_State* L = GetScriptLuaVM()->getLuaState();
		if (L)
		{
			luaL_unref(L, LUA_REGISTRYINDEX, m_evalLuaRef);
		}
		m_evalLuaRef = LUA_NOREF;
	}
}

void DecisionBranch::AddChild(DecisionNode* child)
{
	if (!child) return;
	m_children.push_back(child);
}

void DecisionBranch::SetEvaluatorRef(int luaRef)
{
	// Replace any previously-held ref.
	if (m_evalLuaRef != LUA_NOREF && m_evalLuaRef != LUA_REFNIL)
	{
		lua_State* L = GetScriptLuaVM()->getLuaState();
		if (L)
		{
			luaL_unref(L, LUA_REGISTRYINDEX, m_evalLuaRef);
		}
	}
	m_evalLuaRef = luaRef;
}

DecisionAction* DecisionBranch::Resolve()
{
	if (m_children.empty()) return nullptr;
	if (m_evalLuaRef == LUA_NOREF || m_evalLuaRef == LUA_REFNIL)
	{
		CCLOG_INFO("DecisionBranch::Resolve: no evaluator bound, defaulting to child 1");
		DecisionNode* first = m_children.front();
		return first ? first->Resolve() : nullptr;
	}

	lua_State* L = GetScriptLuaVM()->getLuaState();
	if (!L) return nullptr;

	lua_rawgeti(L, LUA_REGISTRYINDEX, m_evalLuaRef);
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);
		CCLOG_INFO("DecisionBranch::Resolve: evaluator ref is not a function");
		return nullptr;
	}

	if (lua_pcall(L, 0, 1, 0) != LUA_OK)
	{
		const char* err = lua_tostring(L, -1);
		CCLOG_INFO("DecisionBranch evaluator error: %s", err ? err : "(nil)");
		lua_pop(L, 1);
		return nullptr;
	}

	int picked = 0;
	if (lua_isnumber(L, -1))
	{
		picked = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	// Lua convention: 1-based index. Out-of-range → fallback to last.
	if (picked < 1) picked = 1;
	if (picked > (int)m_children.size()) picked = (int)m_children.size();

	DecisionNode* choice = m_children[picked - 1];
	return choice ? choice->Resolve() : nullptr;
}
