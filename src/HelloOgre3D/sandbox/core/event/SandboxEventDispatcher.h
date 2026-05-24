#ifndef __SANDBOX_EVENT_DISPATCHER_H__  
#define __SANDBOX_EVENT_DISPATCHER_H__

#include <algorithm>
#include <functional>
#include <vector>
#include "SandboxContext.h"

class SandboxEventDispatcher
{
public:
	using Callback = std::function<void(const SandboxContext&)>;
	using Token = int;
	struct CallbackEntry
	{
		Token token;
		Callback callback;
	};

	// 绑定回调
	Token BindCallback(const Callback& callback)
	{
		CallbackEntry entry;
		entry.token = ++m_nextToken;
		entry.callback = callback;
		m_callbacks.push_back(entry);
		return entry.token;
	}

	bool UnbindCallback(Token token)
	{
		if (token <= 0)
			return false;

		std::vector<CallbackEntry>::iterator iter = std::remove_if(m_callbacks.begin(), m_callbacks.end(), [token](const CallbackEntry& entry) {
			return entry.token == token;
		});
		if (iter == m_callbacks.end())
			return false;

		m_callbacks.erase(iter, m_callbacks.end());
		return true;
	}

	int GetCallbackCount() const
	{
		return static_cast<int>(m_callbacks.size());
	}

	// 执行回调
	void Invoke(const SandboxContext& context)
	{
		const std::vector<CallbackEntry> callbacks = m_callbacks;
		for (const auto& entry: callbacks)
		{
			if (HasCallback(entry.token) && entry.callback)
				entry.callback(context);
		}
	}

private:
	bool HasCallback(Token token) const
	{
		for (const auto& entry: m_callbacks)
		{
			if (entry.token == token)
				return true;
		}
		return false;
	}

	std::vector<CallbackEntry> m_callbacks;
	Token m_nextToken = 0;
};

#endif; // __SANDBOX_EVENT_DISPATCHER_H__
