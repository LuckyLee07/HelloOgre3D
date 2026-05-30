#ifndef __MEMORY_STORE_H__
#define __MEMORY_STORE_H__

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

#include "ai/common/Blackboard.h"
#include "ai/perception/AgentPerceptionQuery.h"

namespace AIMemoryKeys
{
	static const char* const kPerceptionLastKnownTargetId = "perception.lastKnownTargetId";
	static const char* const kPerceptionLastKnownTargetPos = "perception.lastKnownTargetPos";
	static const char* const kMemoryLastKnownEnemyId = "memory.lastKnownEnemyId";
	static const char* const kMemoryLastKnownEnemyPos = "memory.lastKnownEnemyPos";
	static const char* const kMemoryLastKnownEnemyDistance = "memory.lastKnownEnemyDistance";
	static const char* const kMemoryLastKnownEnemyObservedAtMs = "memory.lastKnownEnemyObservedAtMs";
	static const char* const kMemorySnapshotHasLastKnownEnemy = "memory.snapshot.hasLastKnownEnemy";
	static const char* const kMemorySnapshotLastKnownEnemyId = "memory.snapshot.lastKnownEnemyId";
	static const char* const kMemorySnapshotLastKnownEnemyPos = "memory.snapshot.lastKnownEnemyPos";
	static const char* const kMemorySnapshotLastKnownEnemyConfidence = "memory.snapshot.lastKnownEnemyConfidence";
	static const char* const kMemorySnapshotLastKnownEnemyObservedAtMs = "memory.snapshot.lastKnownEnemyObservedAtMs";
	static const char* const kMemorySnapshotLastKnownEnemyAgeMs = "memory.snapshot.lastKnownEnemyAgeMs";
}

struct EnemyMemory
{
	EnemyMemory()
		: enemyId(-1)
		, lastKnownPosition(Ogre::Vector3::ZERO)
		, distance(0.0f)
		, confidence(0.0f)
		, observedTimeMs(0)
		, ttlMs(-1)
	{
	}

	bool IsValid(long long currentTimeMs = -1) const
	{
		if (enemyId < 0)
			return false;
		if (currentTimeMs < 0 || ttlMs < 0)
			return true;
		return currentTimeMs <= observedTimeMs + ttlMs;
	}

	int AgeMs(long long currentTimeMs) const
	{
		if (currentTimeMs < observedTimeMs)
			return 0;
		return static_cast<int>(currentTimeMs - observedTimeMs);
	}

	int enemyId;
	Ogre::Vector3 lastKnownPosition;
	float distance;
	float confidence;
	long long observedTimeMs;
	int ttlMs;
	std::string source;
};

struct MemoryStoreConfig
{
	MemoryStoreConfig()
		: ttlMs(8000)
		, decayPolicy(Blackboard::ENTRY_DECAY_LINEAR)
		, decayRate(1.0f / 8000.0f)
		, source("MemoryStore")
	{
	}

	int ttlMs;
	Blackboard::EntryDecayPolicy decayPolicy;
	float decayRate;
	std::string source;
};

class MemoryStore
{
public:
	explicit MemoryStore(Blackboard* blackboard = nullptr)
		: m_blackboard(blackboard)
	{
	}

	void SetBlackboard(Blackboard* blackboard)
	{
		m_blackboard = blackboard;
	}

	void Clear()
	{
		if (m_blackboard == nullptr)
			return;

		m_blackboard->Remove(AIMemoryKeys::kPerceptionLastKnownTargetId);
		m_blackboard->Remove(AIMemoryKeys::kPerceptionLastKnownTargetPos);
		m_blackboard->Remove(AIMemoryKeys::kMemoryLastKnownEnemyId);
		m_blackboard->Remove(AIMemoryKeys::kMemoryLastKnownEnemyPos);
		m_blackboard->Remove(AIMemoryKeys::kMemoryLastKnownEnemyDistance);
		m_blackboard->Remove(AIMemoryKeys::kMemoryLastKnownEnemyObservedAtMs);
		ClearSnapshot();
	}

	void RememberVisibleEnemy(const AgentPerceptionResult& result, long long currentTimeMs, const MemoryStoreConfig& config)
	{
		if (m_blackboard == nullptr || result.targetId < 0)
			return;

		const float confidence = Clamp01(result.confidence);
		const int ttlMs = config.ttlMs;
		const float distance = std::sqrt(std::max(0.0f, result.distanceSquared));
		const std::string source = config.source.empty() ? std::string("MemoryStore") : config.source;

		m_blackboard->SetObjectId(AIMemoryKeys::kPerceptionLastKnownTargetId, result.targetId);
		m_blackboard->SetVec3(AIMemoryKeys::kPerceptionLastKnownTargetPos, result.targetPosition);
		m_blackboard->SetInt(AIMemoryKeys::kMemoryLastKnownEnemyObservedAtMs, static_cast<int>(currentTimeMs));

		m_blackboard->SetObjectIdEntry(AIMemoryKeys::kMemoryLastKnownEnemyId, result.targetId, confidence, currentTimeMs, ttlMs, source);
		m_blackboard->SetVec3Entry(AIMemoryKeys::kMemoryLastKnownEnemyPos, result.targetPosition, confidence, currentTimeMs, ttlMs, source);
		m_blackboard->SetFloatEntry(AIMemoryKeys::kMemoryLastKnownEnemyDistance, distance, confidence, currentTimeMs, ttlMs, source);
		ApplyDecay(AIMemoryKeys::kMemoryLastKnownEnemyId, config);
		ApplyDecay(AIMemoryKeys::kMemoryLastKnownEnemyPos, config);
		ApplyDecay(AIMemoryKeys::kMemoryLastKnownEnemyDistance, config);
		SyncSnapshot(currentTimeMs);
	}

	bool HasLastKnownEnemy(long long currentTimeMs = -1) const
	{
		EnemyMemory memory;
		return GetLastKnownEnemy(currentTimeMs, memory);
	}

	bool GetLastKnownEnemy(long long currentTimeMs, EnemyMemory& outMemory) const
	{
		outMemory = EnemyMemory();
		if (m_blackboard == nullptr)
			return false;

		int enemyId = -1;
		Blackboard::Entry idEntry;
		if (!m_blackboard->GetObjectIdEntry(AIMemoryKeys::kMemoryLastKnownEnemyId, enemyId, &idEntry))
			return false;
		if (m_blackboard->IsEntryExpired(idEntry, currentTimeMs))
			return false;

		Ogre::Vector3 position = Ogre::Vector3::ZERO;
		Blackboard::Entry positionEntry;
		if (!m_blackboard->GetVec3Entry(AIMemoryKeys::kMemoryLastKnownEnemyPos, position, &positionEntry))
			return false;
		if (m_blackboard->IsEntryExpired(positionEntry, currentTimeMs))
			return false;

		float distance = 0.0f;
		if (!m_blackboard->GetFloatEntry(AIMemoryKeys::kMemoryLastKnownEnemyDistance, distance))
		{
			distance = 0.0f;
		}

		outMemory.enemyId = enemyId;
		outMemory.lastKnownPosition = position;
		outMemory.distance = distance;
		outMemory.confidence = std::min(idEntry.confidence, positionEntry.confidence);
		outMemory.observedTimeMs = positionEntry.timestampMs;
		outMemory.ttlMs = positionEntry.ttlMs;
		outMemory.source = !positionEntry.source.empty() ? positionEntry.source : idEntry.source;
		return outMemory.IsValid(currentTimeMs);
	}

	void SyncSnapshot(long long currentTimeMs)
	{
		if (m_blackboard == nullptr)
			return;

		EnemyMemory memory;
		if (!GetLastKnownEnemy(currentTimeMs, memory))
		{
			ClearSnapshot();
			return;
		}

		m_blackboard->SetBool(AIMemoryKeys::kMemorySnapshotHasLastKnownEnemy, true);
		m_blackboard->SetObjectId(AIMemoryKeys::kMemorySnapshotLastKnownEnemyId, memory.enemyId);
		m_blackboard->SetVec3(AIMemoryKeys::kMemorySnapshotLastKnownEnemyPos, memory.lastKnownPosition);
		m_blackboard->SetFloat(AIMemoryKeys::kMemorySnapshotLastKnownEnemyConfidence, memory.confidence);
		m_blackboard->SetInt(AIMemoryKeys::kMemorySnapshotLastKnownEnemyObservedAtMs, static_cast<int>(memory.observedTimeMs));
		m_blackboard->SetInt(AIMemoryKeys::kMemorySnapshotLastKnownEnemyAgeMs, memory.AgeMs(currentTimeMs));
	}

	std::string BuildDebugString(long long currentTimeMs) const
	{
		EnemyMemory memory;
		if (!GetLastKnownEnemy(currentTimeMs, memory))
			return "memory={target:none}";

		std::ostringstream stream;
		stream << "memory={target:" << memory.enemyId
			<< ",pos:" << static_cast<int>(memory.lastKnownPosition.x)
			<< "," << static_cast<int>(memory.lastKnownPosition.y)
			<< "," << static_cast<int>(memory.lastKnownPosition.z)
			<< ",age:" << memory.AgeMs(currentTimeMs)
			<< ",conf:" << std::fixed << std::setprecision(2) << memory.confidence;
		if (memory.ttlMs >= 0)
			stream << ",ttl:" << memory.ttlMs;
		if (!memory.source.empty())
			stream << ",src:" << memory.source;
		stream << "}";
		return stream.str();
	}

private:
	void ClearSnapshot()
	{
		if (m_blackboard == nullptr)
			return;

		m_blackboard->SetBool(AIMemoryKeys::kMemorySnapshotHasLastKnownEnemy, false);
		m_blackboard->Remove(AIMemoryKeys::kMemorySnapshotLastKnownEnemyId);
		m_blackboard->Remove(AIMemoryKeys::kMemorySnapshotLastKnownEnemyPos);
		m_blackboard->Remove(AIMemoryKeys::kMemorySnapshotLastKnownEnemyConfidence);
		m_blackboard->Remove(AIMemoryKeys::kMemorySnapshotLastKnownEnemyObservedAtMs);
		m_blackboard->Remove(AIMemoryKeys::kMemorySnapshotLastKnownEnemyAgeMs);
		m_blackboard->Remove(AIMemoryKeys::kPerceptionLastKnownTargetId);
		m_blackboard->Remove(AIMemoryKeys::kPerceptionLastKnownTargetPos);
	}

	static float Clamp01(float value)
	{
		if (value < 0.0f)
			return 0.0f;
		if (value > 1.0f)
			return 1.0f;
		return value;
	}

	void ApplyDecay(const char* key, const MemoryStoreConfig& config)
	{
		if (m_blackboard == nullptr || key == nullptr)
			return;

		m_blackboard->SetEntryDecayPolicy(key, config.decayPolicy, config.decayRate);
	}

	Blackboard* m_blackboard;
};

#endif // __MEMORY_STORE_H__
