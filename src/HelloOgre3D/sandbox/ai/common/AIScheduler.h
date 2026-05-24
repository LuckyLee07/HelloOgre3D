#ifndef __AI_SCHEDULER_H__
#define __AI_SCHEDULER_H__

#include <string>
#include <unordered_map>

class AIScheduler
{
public:
	AIScheduler();

	void Configure(bool enabled, int tickIntervalMs, int maxTicksPerFrame);
	bool IsEnabled() const { return m_enabled; }

	void BeginFrame(int deltaMs, int agentCount);
	bool ShouldTick(unsigned int agentId, int deltaMs, int* outDeltaMs);
	void RemoveAgent(unsigned int agentId);
	void Clear();
	void PublishTracyCounters() const;
	std::string BuildDebugSummary() const;

private:
	struct AgentEntry
	{
		int accumulatedMs;
		int skippedFrames;
	};

	AgentEntry CreateEntry(unsigned int agentId) const;

	bool m_enabled;
	int m_tickIntervalMs;
	int m_maxTicksPerFrame;
	unsigned int m_frameIndex;
	int m_lastDeltaMs;
	int m_lastAgentCount;
	int m_lastTickedCount;
	int m_lastSkippedCount;
	int m_lastPendingCount;
	int m_totalTickedCount;
	int m_totalSkippedCount;
	std::unordered_map<unsigned int, AgentEntry> m_entries;
};

#endif // __AI_SCHEDULER_H__
