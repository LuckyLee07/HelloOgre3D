#include "ai/common/AIScheduler.h"
#include "profiling/Profile.h"
#include "profiling/RuntimeProfileCounters.h"

#include <algorithm>
#include <sstream>

AIScheduler::AIScheduler()
	: m_enabled(false)
	, m_tickIntervalMs(50)
	, m_maxTicksPerFrame(8)
	, m_frameIndex(0)
	, m_lastDeltaMs(0)
	, m_lastAgentCount(0)
	, m_lastTickedCount(0)
	, m_lastSkippedCount(0)
	, m_lastPendingCount(0)
	, m_totalTickedCount(0)
	, m_totalSkippedCount(0)
{
}

void AIScheduler::Configure(bool enabled, int tickIntervalMs, int maxTicksPerFrame)
{
	m_enabled = enabled;
	m_tickIntervalMs = std::max(1, tickIntervalMs);
	m_maxTicksPerFrame = std::max(1, maxTicksPerFrame);
	if (!m_enabled)
	{
		m_lastTickedCount = 0;
		m_lastSkippedCount = 0;
		m_lastPendingCount = 0;
	}
}

void AIScheduler::BeginFrame(int deltaMs, int agentCount)
{
	H3D_PROFILE_SCOPE("AIScheduler::BeginFrame");
	++m_frameIndex;
	m_lastDeltaMs = std::max(0, deltaMs);
	m_lastAgentCount = std::max(0, agentCount);
	m_lastTickedCount = 0;
	m_lastSkippedCount = 0;
	m_lastPendingCount = 0;
}

bool AIScheduler::ShouldTick(unsigned int agentId, int deltaMs, int* outDeltaMs)
{
	H3D_PROFILE_SCOPE("AIScheduler::Tick");
	if (!m_enabled)
	{
		if (outDeltaMs != nullptr)
			*outDeltaMs = deltaMs;
		return true;
	}

	std::unordered_map<unsigned int, AgentEntry>::iterator found = m_entries.find(agentId);
	if (found == m_entries.end())
		found = m_entries.insert(std::make_pair(agentId, CreateEntry(agentId))).first;

	AgentEntry& entry = found->second;
	entry.accumulatedMs += std::max(0, deltaMs);
	if (entry.accumulatedMs < m_tickIntervalMs)
	{
		++m_lastPendingCount;
		return false;
	}

	if (m_lastTickedCount >= m_maxTicksPerFrame)
	{
		++entry.skippedFrames;
		++m_lastSkippedCount;
		++m_lastPendingCount;
		++m_totalSkippedCount;
		return false;
	}

	const int tickDeltaMs = entry.accumulatedMs;
	entry.accumulatedMs = 0;
	entry.skippedFrames = 0;
	++m_lastTickedCount;
	++m_totalTickedCount;
	if (outDeltaMs != nullptr)
		*outDeltaMs = tickDeltaMs;
	return true;
}

void AIScheduler::RemoveAgent(unsigned int agentId)
{
	m_entries.erase(agentId);
}

void AIScheduler::Clear()
{
	m_entries.clear();
	m_lastTickedCount = 0;
	m_lastSkippedCount = 0;
	m_lastPendingCount = 0;
	m_totalTickedCount = 0;
	m_totalSkippedCount = 0;
}

void AIScheduler::PublishTracyCounters() const
{
	RuntimeProfileCounters::PlotAiSchedulerStats(
		m_enabled ? 1 : 0,
		m_lastAgentCount,
		m_lastTickedCount,
		m_lastSkippedCount,
		m_lastPendingCount,
		m_tickIntervalMs,
		m_maxTicksPerFrame);
}

std::string AIScheduler::BuildDebugSummary() const
{
	std::ostringstream stream;
	stream << "[AIScheduler] enabled=" << (m_enabled ? "true" : "false")
		<< " intervalMs=" << m_tickIntervalMs
		<< " maxTicksPerFrame=" << m_maxTicksPerFrame
		<< " frame=" << m_frameIndex
		<< " deltaMs=" << m_lastDeltaMs
		<< " agents=" << m_lastAgentCount
		<< " tracked=" << m_entries.size()
		<< " ticked=" << m_lastTickedCount
		<< " skipped=" << m_lastSkippedCount
		<< " pending=" << m_lastPendingCount
		<< " totalTicked=" << m_totalTickedCount
		<< " totalSkipped=" << m_totalSkippedCount;
	return stream.str();
}

AIScheduler::AgentEntry AIScheduler::CreateEntry(unsigned int agentId) const
{
	AgentEntry entry;
	entry.accumulatedMs = m_tickIntervalMs > 1 ? static_cast<int>(agentId % static_cast<unsigned int>(m_tickIntervalMs)) : 0;
	entry.skippedFrames = 0;
	return entry;
}
