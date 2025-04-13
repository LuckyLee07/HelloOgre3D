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

	// ��ȡ����ʵ��
	static SandboxEventDispatcherManager& GetGlobalInst()
	{
		static SandboxEventDispatcherManager instance; // C++11�̰߳�ȫ
		return instance;
	}

	// �����¼�
	void CreateDispatcher(const std::string& eventName)
	{
		// �Ѵ��ڣ����⸲��
		if (m_dispatchers.find(eventName) != m_dispatchers.end())
		{
			return;
		}
		auto dispatcher = std::make_shared<SandboxEventDispatcher>();
		m_dispatchers[eventName] = dispatcher;
	}

	// ɾ���¼�
	void RemoveDispatcher(const std::string& eventName)
	{
		auto iter = m_dispatchers.find(eventName);
		if (iter != m_dispatchers.end())
		{
			m_dispatchers.erase(iter);
		}
	}

	// �����¼�
	void Subscribe(const std::string& eventName, const SandboxEventDispatcher::Callback& callback)
	{
		auto dispatcher = GetDispatcher(eventName);
		if (dispatcher != nullptr)
		{
			dispatcher->BindCallback(callback);
		}
	}

	// �����¼�
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

	// ��ȡ�ַ���
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