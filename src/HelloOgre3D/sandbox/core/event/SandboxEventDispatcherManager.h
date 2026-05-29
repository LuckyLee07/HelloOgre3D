#ifndef __SANDBOX_EVENT_DISPATCHER_MANAGER_H__  
#define __SANDBOX_EVENT_DISPATCHER_MANAGER_H__

#include <algorithm>
#include <deque>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
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
	SandboxEventDispatcher::Token Subscribe(const std::string& eventName, const SandboxEventDispatcher::Callback& callback)
	{
		auto dispatcher = GetDispatcher(eventName);
		if (dispatcher != nullptr)
		{
			return dispatcher->BindCallback(callback);
		}
		return 0;
	}

	bool Unsubscribe(const std::string& eventName, SandboxEventDispatcher::Token token)
	{
		auto dispatcher = GetDispatcher(eventName);
		if (dispatcher != nullptr)
		{
			return dispatcher->UnbindCallback(token);
		}
		return false;
	}

	// 触发事件
	void Emit(const std::string& eventName, const SandboxContext& context)
	{
		EmitNow(eventName, context);
	}

	bool QueueEmit(const std::string& eventName, const SandboxContext& context)
	{
		if (m_maxQueuedEvents <= 0 || static_cast<int>(m_eventQueue.size()) >= m_maxQueuedEvents)
		{
			++m_droppedQueuedEvents;
			return false;
		}

		QueuedEvent event;
		event.eventName = eventName;
		event.context = context;
		m_eventQueue.push_back(event);
		return true;
	}

	int FlushQueuedEvents(int maxEvents = -1)
	{
		if (m_isFlushingQueue)
			return 0;

		const int initialCount = static_cast<int>(m_eventQueue.size());
		if (initialCount <= 0)
			return 0;

		const int limit = maxEvents < 0 ? initialCount : std::min(maxEvents, initialCount);
		int dispatchedCount = 0;
		m_isFlushingQueue = true;
		while (dispatchedCount < limit && !m_eventQueue.empty())
		{
			QueuedEvent event = m_eventQueue.front();
			m_eventQueue.pop_front();
			EmitNow(event.eventName, event.context);
			++dispatchedCount;
		}
		m_isFlushingQueue = false;
		return dispatchedCount;
	}

	void SetMaxQueuedEvents(int maxQueuedEvents)
	{
		m_maxQueuedEvents = maxQueuedEvents < 0 ? 0 : maxQueuedEvents;
		while (static_cast<int>(m_eventQueue.size()) > m_maxQueuedEvents)
		{
			m_eventQueue.pop_back();
			++m_droppedQueuedEvents;
		}
	}

	int GetQueuedEventCount() const
	{
		return static_cast<int>(m_eventQueue.size());
	}

	int GetDroppedQueuedEventCount() const
	{
		return m_droppedQueuedEvents;
	}

	int GetMaxQueuedEvents() const
	{
		return m_maxQueuedEvents;
	}

private:
	void EmitNow(const std::string& eventName, const SandboxContext& context)
	{
		auto dispatcher = GetDispatcher(eventName);
		RecordEmit(eventName, context, dispatcher != nullptr ? dispatcher->GetCallbackCount() : 0);
		if (dispatcher != nullptr)
		{
			dispatcher->Invoke(context);
		}
	}

public:
	int GetEmitCount() const
	{
		return m_emitCount;
	}

	void ClearDebugStats()
	{
		m_emitCount = 0;
		m_droppedQueuedEvents = 0;
		m_eventCounts.clear();
		m_recentEvents.clear();
	}

	std::string BuildDebugSummary(const std::string& ownerName, int maxEvents) const
	{
		if (maxEvents < 0)
			maxEvents = 0;
		if (maxEvents > 32)
			maxEvents = 32;

		std::ostringstream stream;
		stream << "[AIEvent] owner=" << ownerName
			<< " dispatchers=" << m_dispatchers.size()
			<< " emits=" << m_emitCount
			<< " queued=" << m_eventQueue.size()
			<< " droppedQueued=" << m_droppedQueuedEvents;

		if (!m_eventCounts.empty())
		{
			stream << " counts={";
			bool first = true;
			for (std::map<std::string, int>::const_iterator iter = m_eventCounts.begin(); iter != m_eventCounts.end(); ++iter)
			{
				stream << (first ? "" : ",") << iter->first << ":" << iter->second;
				first = false;
			}
			stream << "}";
		}

		const int startIndex = maxEvents < static_cast<int>(m_recentEvents.size()) ? static_cast<int>(m_recentEvents.size()) - maxEvents : 0;
		for (int index = startIndex; index < static_cast<int>(m_recentEvents.size()); ++index)
		{
			const EventRecord& record = m_recentEvents[index];
			stream << "\n[AIEvent] owner=" << ownerName
				<< " event=" << record.eventName
				<< " type=" << record.eventType
				<< " scope=" << record.scope
				<< " sender=" << record.senderId
				<< " target=" << record.targetId
				<< " team=" << record.teamId
				<< " callbacks=" << record.callbackCount
				<< " timeMs=" << static_cast<int>(record.timeMs);
		}
		return stream.str();
	}

private:
	using DispatcherBase = std::shared_ptr<SandboxEventDispatcher>;
	struct QueuedEvent
	{
		std::string eventName;
		SandboxContext context;
	};

	struct EventRecord
	{
		std::string eventName;
		std::string eventType;
		std::string scope;
		int senderId;
		int targetId;
		int teamId;
		double timeMs;
		int callbackCount;
	};

	std::unordered_map<std::string, DispatcherBase> m_dispatchers;
	int m_emitCount = 0;
	std::map<std::string, int> m_eventCounts;
	std::deque<EventRecord> m_recentEvents;
	std::deque<QueuedEvent> m_eventQueue;
	int m_maxQueuedEvents = 1024;
	int m_droppedQueuedEvents = 0;
	bool m_isFlushingQueue = false;
	static const int MAX_RECENT_EVENTS = 32;

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

	void RecordEmit(const std::string& eventName, const SandboxContext& context, int callbackCount)
	{
		++m_emitCount;
		++m_eventCounts[eventName];

		EventRecord record;
		record.eventName = eventName;
		record.eventType = context.Get_String("eventType", eventName);
		record.scope = context.Get_String("scope", "local");
		record.senderId = static_cast<int>(context.Get_Number("senderId", -1.0));
		record.targetId = static_cast<int>(context.Get_Number("targetId", -1.0));
		record.teamId = static_cast<int>(context.Get_Number("teamId", -1.0));
		record.timeMs = context.Get_Number("timeMs", 0.0);
		record.callbackCount = callbackCount;
		m_recentEvents.push_back(record);
		while (static_cast<int>(m_recentEvents.size()) > MAX_RECENT_EVENTS)
		{
			m_recentEvents.pop_front();
		}
	}
};

#endif; // __SANDBOX_EVENT_DISPATCHER_MANAGER_H__
