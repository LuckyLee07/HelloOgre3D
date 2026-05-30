#ifndef __SANDBOX_EVENT_DISPATCHER_MANAGER_H__  
#define __SANDBOX_EVENT_DISPATCHER_MANAGER_H__

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <deque>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include "SandboxEventDispatcher.h"
#include "SandboxEventPayload.h"

class SandboxEventDispatcherManager
{
public:
	using Callback = SandboxEventDispatcher::Callback;
	using Filter = SandboxEventDispatcher::Filter;
	using Token = SandboxEventDispatcher::Token;

	SandboxEventDispatcherManager() = default;
	~SandboxEventDispatcherManager() = default;

	static SandboxEventDispatcherManager& GetGlobalInst()
	{
		static SandboxEventDispatcherManager instance;
		return instance;
	}

	void CreateDispatcher(const std::string& eventName)
	{
		CreateDispatcher(eventName, SandboxEventScope::Local);
	}

	void CreateDispatcher(const std::string& eventName, SandboxEventScope scope)
	{
		const EventNameParts parts = ParseEventName(eventName);
		GetOrCreateDispatcher(BuildDispatcherKey(parts.baseName, scope));
	}

	void RemoveDispatcher(const std::string& eventName)
	{
		RemoveDispatcher(eventName, SandboxEventScope::Local);
	}

	void RemoveDispatcher(const std::string& eventName, SandboxEventScope scope)
	{
		const EventNameParts parts = ParseEventName(eventName);
		const std::string dispatcherKey = BuildDispatcherKey(parts.baseName, scope);
		std::unordered_map<std::string, DispatcherBase>::iterator iter = m_dispatchers.find(dispatcherKey);
		if (iter != m_dispatchers.end())
			m_dispatchers.erase(iter);
	}

	Token Subscribe(const std::string& eventName, const Callback& callback)
	{
		return Subscribe(eventName, callback, Filter());
	}

	Token Subscribe(const std::string& eventName, const Callback& callback, const Filter& filter)
	{
		return SubscribeScoped(eventName, SandboxEventScope::Local, callback, filter);
	}

	Token SubscribeFiltered(const std::string& eventName, const Filter& filter, const Callback& callback)
	{
		return Subscribe(eventName, callback, filter);
	}

	Token SubscribeScoped(const std::string& eventName, SandboxEventScope scope, const Callback& callback)
	{
		return SubscribeScoped(eventName, scope, callback, Filter());
	}

	Token SubscribeScoped(const std::string& eventName, SandboxEventScope scope, const Callback& callback, const Filter& filter)
	{
		if (!callback)
			return 0;

		const EventNameParts parts = ParseEventName(eventName);
		std::shared_ptr<SandboxEventDispatcher> dispatcher = GetOrCreateDispatcher(BuildDispatcherKey(parts.baseName, scope));
		return dispatcher != nullptr ? dispatcher->BindCallback(callback, BuildSubscriptionFilter(parts.params, filter)) : 0;
	}

	Token SubscribeScopedFiltered(const std::string& eventName, SandboxEventScope scope, const Filter& filter, const Callback& callback)
	{
		return SubscribeScoped(eventName, scope, callback, filter);
	}

	bool Unsubscribe(const std::string& eventName, Token token)
	{
		return UnsubscribeScoped(eventName, SandboxEventScope::Local, token);
	}

	bool UnsubscribeScoped(const std::string& eventName, SandboxEventScope scope, Token token)
	{
		std::shared_ptr<SandboxEventDispatcher> dispatcher = GetDispatcher(BuildDispatcherKey(ParseEventName(eventName).baseName, scope));
		return dispatcher != nullptr ? dispatcher->UnbindCallback(token) : false;
	}

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
		{
			++m_recursiveFlushSkipCount;
			return 0;
		}

		const int initialCount = static_cast<int>(m_eventQueue.size());
		if (initialCount <= 0)
			return 0;

		const int limit = ResolveFlushLimit(initialCount, maxEvents);
		if (limit < initialCount)
			++m_throttledFlushCount;
		if (limit <= 0)
			return 0;

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

	void SetMaxEventsPerFlush(int maxEventsPerFlush)
	{
		m_maxEventsPerFlush = maxEventsPerFlush;
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

	int GetMaxEventsPerFlush() const
	{
		return m_maxEventsPerFlush;
	}

	int GetThrottledFlushCount() const
	{
		return m_throttledFlushCount;
	}

	int GetRecursiveFlushSkipCount() const
	{
		return m_recursiveFlushSkipCount;
	}

	int GetEmitCount() const
	{
		return m_emitCount;
	}

	void ClearDebugStats()
	{
		m_emitCount = 0;
		m_droppedQueuedEvents = 0;
		m_throttledFlushCount = 0;
		m_recursiveFlushSkipCount = 0;
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
			<< " maxQueued=" << m_maxQueuedEvents
			<< " maxPerFlush=" << m_maxEventsPerFlush
			<< " droppedQueued=" << m_droppedQueuedEvents
			<< " throttledFlushes=" << m_throttledFlushCount
			<< " recursiveFlushSkips=" << m_recursiveFlushSkipCount;

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
				<< " base=" << record.baseName
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
	using EventParams = std::map<std::string, std::string>;

	struct EventNameParts
	{
		std::string baseName;
		EventParams params;
	};

	struct QueuedEvent
	{
		std::string eventName;
		SandboxContext context;
	};

	struct EventRecord
	{
		std::string eventName;
		std::string baseName;
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
	int m_maxEventsPerFlush = 256;
	int m_droppedQueuedEvents = 0;
	int m_throttledFlushCount = 0;
	int m_recursiveFlushSkipCount = 0;
	bool m_isFlushingQueue = false;
	static const int MAX_RECENT_EVENTS = 32;

	void EmitNow(const std::string& eventName, const SandboxContext& context)
	{
		const EventNameParts parts = ParseEventName(eventName);
		SandboxContext routedContext = context;
		ApplyEventParams(routedContext, parts.params);
		SandboxEventScope scope = GetScope(routedContext);
		if (scope == SandboxEventScope::Global && !IsGlobalManager())
		{
			GetGlobalInst().EmitNow(eventName, routedContext);
			return;
		}

		DispatchToScope(parts.baseName, eventName, scope, routedContext);
	}

	void DispatchToScope(const std::string& baseName, const std::string& eventName, SandboxEventScope scope, const SandboxContext& context)
	{
		std::shared_ptr<SandboxEventDispatcher> dispatcher = GetDispatcher(BuildDispatcherKey(baseName, scope));
		const int callbackCount = dispatcher != nullptr ? dispatcher->GetMatchingCallbackCount(context) : 0;
		RecordEmit(eventName, baseName, context, callbackCount, scope);
		if (dispatcher != nullptr)
			dispatcher->Invoke(context);
	}

	std::shared_ptr<SandboxEventDispatcher> GetDispatcher(const std::string& dispatcherKey)
	{
		std::unordered_map<std::string, DispatcherBase>::iterator iter = m_dispatchers.find(dispatcherKey);
		if (iter != m_dispatchers.end())
			return iter->second;
		return nullptr;
	}

	std::shared_ptr<SandboxEventDispatcher> GetOrCreateDispatcher(const std::string& dispatcherKey)
	{
		std::shared_ptr<SandboxEventDispatcher> dispatcher = GetDispatcher(dispatcherKey);
		if (dispatcher != nullptr)
			return dispatcher;

		dispatcher = std::make_shared<SandboxEventDispatcher>();
		m_dispatchers[dispatcherKey] = dispatcher;
		return dispatcher;
	}

	int ResolveFlushLimit(int queuedCount, int maxEvents) const
	{
		if (maxEvents >= 0)
			return std::min(maxEvents, queuedCount);
		if (m_maxEventsPerFlush < 0)
			return queuedCount;
		return std::min(m_maxEventsPerFlush, queuedCount);
	}

	bool IsGlobalManager() const
	{
		return this == &GetGlobalInst();
	}

	static std::string BuildDispatcherKey(const std::string& baseName, SandboxEventScope scope)
	{
		return std::string(SandboxEventPayload::ToString(scope)) + ":" + baseName;
	}

	static SandboxEventScope GetScope(const SandboxContext& context)
	{
		const std::string scope = context.Get_String(SandboxEventFields::Scope(), SandboxEventPayload::ToString(SandboxEventScope::Local));
		if (scope == "team" || scope == "Team" || scope == "TEAM")
			return SandboxEventScope::Team;
		if (scope == "global" || scope == "Global" || scope == "GLOBAL")
			return SandboxEventScope::Global;
		return SandboxEventScope::Local;
	}

	static EventNameParts ParseEventName(const std::string& eventName)
	{
		EventNameParts parts;
		const std::string::size_type queryStart = eventName.find('?');
		parts.baseName = queryStart == std::string::npos ? eventName : eventName.substr(0, queryStart);
		if (queryStart == std::string::npos)
			return parts;

		std::string::size_type paramStart = queryStart + 1;
		while (paramStart < eventName.size())
		{
			const std::string::size_type next = eventName.find('&', paramStart);
			const std::string param = eventName.substr(paramStart, next == std::string::npos ? std::string::npos : next - paramStart);
			const std::string::size_type equals = param.find('=');
			if (equals != std::string::npos && equals > 0)
				parts.params[param.substr(0, equals)] = param.substr(equals + 1);
			else if (!param.empty())
				parts.params[param] = "true";

			if (next == std::string::npos)
				break;
			paramStart = next + 1;
		}
		return parts;
	}

	static Filter BuildSubscriptionFilter(const EventParams& eventParams, const Filter& filter)
	{
		if (eventParams.empty())
			return filter;
		if (!filter)
		{
			return [eventParams](const SandboxContext& context) -> bool {
				return MatchesEventParams(context, eventParams);
			};
		}

		return [eventParams, filter](const SandboxContext& context) -> bool {
			return MatchesEventParams(context, eventParams) && filter(context);
		};
	}

	static bool MatchesEventParams(const SandboxContext& context, const EventParams& eventParams)
	{
		for (EventParams::const_iterator iter = eventParams.begin(); iter != eventParams.end(); ++iter)
		{
			if (!MatchesParam(context, iter->first, iter->second))
				return false;
		}
		return true;
	}

	static bool MatchesParam(const SandboxContext& context, const std::string& key, const std::string& value)
	{
		if (!context.Has(key))
			return false;

		double expectedNumber = 0.0;
		if (ParseDouble(value, expectedNumber))
		{
			const double sentinel = std::numeric_limits<double>::max();
			const double actualNumber = context.Get_Number(key, sentinel);
			if (actualNumber != sentinel && std::fabs(actualNumber - expectedNumber) < 0.0001)
				return true;
		}

		return context.Get_String(key, "") == value;
	}

	static void ApplyEventParams(SandboxContext& context, const EventParams& eventParams)
	{
		for (EventParams::const_iterator iter = eventParams.begin(); iter != eventParams.end(); ++iter)
		{
			double numberValue = 0.0;
			if (ParseDouble(iter->second, numberValue))
				context.Set_Number(iter->first, numberValue);
			else
				context.Set_String(iter->first, iter->second);
		}
	}

	static bool ParseDouble(const std::string& text, double& outValue)
	{
		if (text.empty())
			return false;

		char* endPtr = nullptr;
		outValue = std::strtod(text.c_str(), &endPtr);
		return endPtr != text.c_str() && endPtr != nullptr && *endPtr == '\0';
	}

	void RecordEmit(const std::string& eventName, const std::string& baseName, const SandboxContext& context, int callbackCount, SandboxEventScope routedScope)
	{
		++m_emitCount;
		++m_eventCounts[baseName];

		EventRecord record;
		record.eventName = eventName;
		record.baseName = baseName;
		record.eventType = context.Get_String(SandboxEventFields::EventType(), baseName);
		record.scope = SandboxEventPayload::ToString(routedScope);
		record.senderId = static_cast<int>(context.Get_Number(SandboxEventFields::SenderId(), -1.0));
		record.targetId = static_cast<int>(context.Get_Number(SandboxEventFields::TargetId(), -1.0));
		record.teamId = static_cast<int>(context.Get_Number(SandboxEventFields::TeamId(), -1.0));
		record.timeMs = context.Get_Number(SandboxEventFields::TimeMs(), 0.0);
		record.callbackCount = callbackCount;
		m_recentEvents.push_back(record);
		while (static_cast<int>(m_recentEvents.size()) > MAX_RECENT_EVENTS)
		{
			m_recentEvents.pop_front();
		}
	}
};

#endif; // __SANDBOX_EVENT_DISPATCHER_MANAGER_H__
