#ifndef __SANDBOX_EVENT_DISPATCHER_H__  
#define __SANDBOX_EVENT_DISPATCHER_H__

#include <vector>
#include <functional>
#include "SandboxContext.h"

class SandboxEventDispatcher
{
public:
	using Callback = std::function<void(const SandboxContext&)>;

	// �󶨻ص�
	void BindCallback(const Callback& callback)
	{
		m_callbacks.push_back(callback);
	}

	// ִ�лص�
	void Invoke(const SandboxContext& context)
	{
		for (const auto& callback: m_callbacks)
		{
			callback(context);
		}
	}

private:
	std::vector<Callback> m_callbacks;
};

#endif; // __SANDBOX_EVENT_DISPATCHER_H__