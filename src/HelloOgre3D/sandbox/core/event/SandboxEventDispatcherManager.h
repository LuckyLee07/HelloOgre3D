#ifndef __SANDBOX_EVENT_DISPATCHER_MANAGER_H__  
#define __SANDBOX_EVENT_DISPATCHER_MANAGER_H__

#include <string>
#include <unordered_map>
#include <memory>
#include "SandboxEventDispatcher.h"

class SandboxEventDispatcherManager
{
public:
	SandboxEventDispatcherManager() = default;
	~SandboxEventDispatcherManager() = default;

	// 获取单例实例
	static SandboxEventDispatcherManager& GetGlobalInst()
	{
		static SandboxEventDispatcherManager instance; // C++11线程安全
		return instance;
	}

	// 创建事件
	void CreateDispatcher(const std::string& eventName)
	{
		// 已存在，避免覆盖
		if (m_dispatchers.find(eventName) != m_dispatchers.end())
		{
			return;
		}
		auto dispatcher = std::make_shared<SandboxEventDispatcher>();
		m_dispatchers[eventName] = dispatcher;
	}

	// 删除事件
	void RemoveDispatcher(const std::string& eventName)
	{
		auto iter = m_dispatchers.find(eventName);
		if (iter != m_dispatchers.end())
		{
			m_dispatchers.erase(iter);
		}
	}

	// 订阅事件
	void Subscribe(const std::string& eventName, const SandboxEventDispatcher::Callback& callback)
	{
		auto dispatcher = GetDispatcher(eventName);
		if (dispatcher != nullptr)
		{
			dispatcher->BindCallback(callback);
		}
	}

	// 触发事件
	void Emit(const std::string& eventName, const SandboxContext& context)
	{
		auto dispatcher = GetDispatcher(eventName);
		if (dispatcher != nullptr)
		{
			dispatcher->Invoke(context);
		}
	}

private:
	using DispatcherBase = std::shared_ptr<SandboxEventDispatcher>;
	std::unordered_map<std::string, DispatcherBase> m_dispatchers;

	// 获取分发器
	std::shared_ptr<SandboxEventDispatcher> GetDispatcher(const std::string& eventName)
	{
		auto iter = m_dispatchers.find(eventName);
		if (iter != m_dispatchers.end())
		{
			return iter->second;
		}
		return nullptr;
	}
};

#endif; // __SANDBOX_EVENT_DISPATCHER_MANAGER_H__