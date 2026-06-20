#include "LuaCondition.h"

#include <algorithm>

#include "ai/common/Blackboard.h"
#include "ScriptLuaVM.h"

extern "C" {
#include "lauxlib.h"
}

LuaCondition::LuaCondition(Blackboard* blackboard)
	: m_evalLuaRef(LUA_NOREF)
	, m_blackboard(blackboard)
	, m_resultCacheEnabled(false)
	, m_hasCachedResult(false)
	, m_resultCacheTtlMs(0.0f)
	, m_resultCacheAgeMs(0.0f)
	, m_cachedRevision(-1)
	, m_cachedStatus(STATUS_INVALID)
	, m_resultCacheHitCount(0)
	, m_resultCacheInvalidatedCount(0)
{
}

LuaCondition::~LuaCondition()
{
	if (m_evalLuaRef != LUA_NOREF)
	{
		lua_State* L = GetScriptLuaVM()->getLuaState();
		if (L) luaL_unref(L, LUA_REGISTRYINDEX, m_evalLuaRef);
	}
}

void LuaCondition::SetEvaluatorRef(int luaRef)
{
	if (m_evalLuaRef != LUA_NOREF)
	{
		lua_State* L = GetScriptLuaVM()->getLuaState();
		if (L) luaL_unref(L, LUA_REGISTRYINDEX, m_evalLuaRef);
	}
	m_evalLuaRef = luaRef;
	InvalidateResultCache();
}

void LuaCondition::SetResultCacheEnabled(bool enabled)
{
	m_resultCacheEnabled = enabled;
	InvalidateResultCache();
}

void LuaCondition::SetResultCacheTtlMs(float ttlMs)
{
	m_resultCacheTtlMs = std::max(0.0f, ttlMs);
	InvalidateResultCache();
}

void LuaCondition::AddResultCacheDependencyKey(const std::string& key)
{
	if (key.empty())
		return;
	if (std::find(m_resultCacheDependencyKeys.begin(), m_resultCacheDependencyKeys.end(), key) == m_resultCacheDependencyKeys.end())
	{
		m_resultCacheDependencyKeys.push_back(key);
		InvalidateResultCache();
	}
}

void LuaCondition::ClearResultCacheDependencyKeys()
{
	if (!m_resultCacheDependencyKeys.empty())
	{
		m_resultCacheDependencyKeys.clear();
		InvalidateResultCache();
	}
}

int LuaCondition::GetResultCacheDependencyCount() const
{
	return static_cast<int>(m_resultCacheDependencyKeys.size());
}

BehaviorNode::Status LuaCondition::Tick(float deltaMs)
{
	if (m_evalLuaRef == LUA_NOREF) return TraceStatus(STATUS_FAILURE);

	const int dependencyRevision = ComputeDependencyRevision();
	if (m_resultCacheEnabled && m_blackboard != nullptr && m_hasCachedResult && m_cachedStatus != STATUS_INVALID)
	{
		const bool revisionMatched = dependencyRevision == m_cachedRevision;
		const bool ttlMatched = m_resultCacheTtlMs <= 0.0f || (m_resultCacheAgeMs + std::max(0.0f, deltaMs)) <= m_resultCacheTtlMs;
		if (revisionMatched && ttlMatched)
		{
			m_resultCacheAgeMs += std::max(0.0f, deltaMs);
			++m_resultCacheHitCount;
			return TraceStatus(m_cachedStatus);
		}
		++m_resultCacheInvalidatedCount;
	}

	lua_State* L = GetScriptLuaVM()->getLuaState();
	if (!L) return TraceStatus(STATUS_FAILURE);

	lua_rawgeti(L, LUA_REGISTRYINDEX, m_evalLuaRef);
	if (lua_pcall(L, 0, 1, 0) != 0)
	{
		// pcall 出错：错误消息已在栈顶，pop 掉
		lua_pop(L, 1);
		return TraceStatus(STATUS_FAILURE);
	}

	const bool result = lua_toboolean(L, -1) != 0;
	lua_pop(L, 1);
	const Status status = result ? STATUS_SUCCESS : STATUS_FAILURE;
	if (m_resultCacheEnabled && m_blackboard != nullptr)
	{
		m_hasCachedResult = true;
		m_cachedStatus = status;
		m_cachedRevision = ComputeDependencyRevision();
		m_resultCacheAgeMs = 0.0f;
	}
	return TraceStatus(status);
}

int LuaCondition::ComputeDependencyRevision() const
{
	if (m_blackboard == nullptr)
		return -1;

	if (m_resultCacheDependencyKeys.empty())
		return m_blackboard->GetRevision();

	int revision = 17;
	for (std::vector<std::string>::const_iterator iter = m_resultCacheDependencyKeys.begin(); iter != m_resultCacheDependencyKeys.end(); ++iter)
	{
		revision = revision * 131 + m_blackboard->GetKeyRevision(*iter);
	}
	return revision;
}

void LuaCondition::InvalidateResultCache()
{
	m_hasCachedResult = false;
	m_cachedRevision = -1;
	m_cachedStatus = STATUS_INVALID;
	m_resultCacheAgeMs = 0.0f;
}
