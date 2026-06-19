#include "ai/common/AIUpdateSystem.h"

#include "ai/common/AIScheduler.h"
#include "ai/perception/AgentPerceptionSystem.h"
#include "ai/perception/AgentSpatialIndexSystem.h"
#include "ai/team/TeamBlackboardService.h"
#include "components/ai/AIController.h"
#include "profiling/RuntimeProfileCounters.h"

AIUpdateSystem::FrameContext::FrameContext()
	: agents(nullptr)
	, objectManager(nullptr)
	, scheduler(nullptr)
	, spatialIndex(nullptr)
	, perceptionSystem(nullptr)
	, teamBlackboard(nullptr)
	, aiControllerCount(0)
	, perfEnabled(false)
	, timing(nullptr)
{
}

bool AIUpdateSystem::BeginFrame(int deltaMilliseconds, const FrameContext& context)
{
	RuntimeObjectUpdateTiming* timing = ResolveTiming(context);
	long long stageStartMicros = 0;
	const bool useAiScheduler = context.scheduler != nullptr && context.scheduler->IsEnabled();

	if (context.scheduler != nullptr)
	{
		if (context.perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		context.scheduler->BeginFrame(deltaMilliseconds, context.aiControllerCount);
		if (context.perfEnabled && timing != nullptr)
			timing->schedulerBeginMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
	}

	if (context.spatialIndex != nullptr)
	{
		if (context.perfEnabled)
			stageStartMicros = RuntimeStallProfiler::NowMicroseconds();
		const std::vector<AgentObject*>& agents = ResolveAgents(context);
		if (context.spatialIndex->IsEnabled())
			context.spatialIndex->Rebuild(agents);
		else
			context.spatialIndex->BeginFrameLinearFallback(static_cast<int>(agents.size()));
		if (context.perfEnabled && timing != nullptr)
			timing->spatialRebuildMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
	}

	return useAiScheduler;
}

void AIUpdateSystem::UpdatePerception(int deltaMilliseconds, const FrameContext& context)
{
	if (context.perceptionSystem == nullptr)
		return;

	RuntimeObjectUpdateTiming* timing = ResolveTiming(context);
	const long long stageStartMicros = context.perfEnabled ? RuntimeStallProfiler::NowMicroseconds() : 0;
	context.perceptionSystem->Update(ResolveAgents(context), deltaMilliseconds, context.objectManager);
	if (context.perfEnabled && timing != nullptr)
		timing->perceptionSystemMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
	context.perceptionSystem->PublishTracyCounters();
}

void AIUpdateSystem::TickAgent(AIController* ai, int deltaMilliseconds, bool useAiScheduler, AIScheduler* scheduler)
{
	if (ai == nullptr)
		return;

	if (scheduler != nullptr && useAiScheduler)
	{
		ai->SetTickInOwnerUpdateEnabled(false);
		int aiDeltaMs = 0;
		if (scheduler->ShouldTick(ai->GetAgentId(), deltaMilliseconds, &aiDeltaMs))
			ai->TickAI(aiDeltaMs);
	}
	else
	{
		ai->SetTickInOwnerUpdateEnabled(true);
	}
}

void AIUpdateSystem::RemoveAgent(unsigned int agentId, AIScheduler* scheduler)
{
	if (scheduler != nullptr)
		scheduler->RemoveAgent(agentId);
}

void AIUpdateSystem::EndFrame(int deltaMilliseconds, const FrameContext& context)
{
	RuntimeObjectUpdateTiming* timing = ResolveTiming(context);
	if (context.scheduler != nullptr)
		context.scheduler->PublishTracyCounters();

	if (context.teamBlackboard == nullptr)
		return;

	const long long stageStartMicros = context.perfEnabled ? RuntimeStallProfiler::NowMicroseconds() : 0;
	context.teamBlackboard->SyncFromAgents(ResolveAgents(context), deltaMilliseconds);
	if (context.perfEnabled && timing != nullptr)
		timing->teamBlackboardMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
	context.teamBlackboard->PublishTracyCounters();
}

void AIUpdateSystem::FillPerfStats(const FrameContext& context) const
{
	RuntimeObjectUpdateTiming* timing = ResolveTiming(context);
	if (timing == nullptr)
		return;

	if (context.spatialIndex != nullptr)
	{
		const AgentSpatialIndexSystem::Stats& stats = context.spatialIndex->GetStats();
		timing->spatialEnabled = context.spatialIndex->IsEnabled() ? 1 : 0;
		timing->spatialAgentCount = stats.agentCount;
		timing->spatialCellCount = stats.occupiedCellCount;
		timing->spatialQueryCount = stats.queryCount;
		timing->spatialCandidateCount = stats.candidateCount;
		timing->spatialFilteredCandidateCount = stats.filteredCandidateCount;
		timing->spatialResultCount = stats.resultCount;
		timing->spatialMaxCandidates = stats.maxCandidatesPerQuery;
		timing->spatialMaxFilteredCandidates = stats.maxFilteredCandidatesPerQuery;
		timing->spatialMaxResults = stats.maxResultsPerQuery;
		timing->spatialRejectedSelfCount = stats.rejectedSelfCount;
		timing->spatialRejectedTeamCount = stats.rejectedTeamCount;
		timing->spatialRejectedDeadCount = stats.rejectedDeadCount;
		timing->spatialRejectedTypeCount = stats.rejectedTypeCount;
		timing->spatialQueryCostMs = stats.queryCostMs;
		timing->spatialAvgCandidates = stats.queryCount > 0 ? static_cast<double>(stats.candidateCount) / static_cast<double>(stats.queryCount) : 0.0;
		timing->spatialAvgFilteredCandidates = stats.queryCount > 0 ? static_cast<double>(stats.filteredCandidateCount) / static_cast<double>(stats.queryCount) : 0.0;
		timing->spatialAvgResults = stats.queryCount > 0 ? static_cast<double>(stats.resultCount) / static_cast<double>(stats.queryCount) : 0.0;
	}

	if (context.perceptionSystem != nullptr)
	{
		const AgentPerceptionSystem::Stats& stats = context.perceptionSystem->GetStats();
		timing->perceptionEnabled = stats.enabled;
		timing->perceptionAgentCount = stats.agentCount;
		timing->perceptionControllerCount = stats.controllerCount;
		timing->perceptionScanCount = stats.scanCount;
		timing->perceptionVisibleCount = stats.visibleCount;
		timing->perceptionSpatialQueryCount = stats.spatialQueryCount;
		timing->perceptionSpatialCandidateCount = stats.spatialCandidateCount;
		timing->perceptionSpatialFilteredCandidateCount = stats.spatialFilteredCandidateCount;
		timing->perceptionSpatialResultCount = stats.spatialResultCount;
		timing->perceptionSpatialRejectedSelfCount = stats.spatialRejectedSelfCount;
		timing->perceptionSpatialRejectedTeamCount = stats.spatialRejectedTeamCount;
		timing->perceptionSpatialRejectedDeadCount = stats.spatialRejectedDeadCount;
		timing->perceptionSpatialRejectedTypeCount = stats.spatialRejectedTypeCount;
		timing->perceptionSpatialQueryCostMs = stats.spatialQueryCostMs;
		timing->perceptionMemoryMs = stats.memoryMs;
		timing->perceptionVisionMs = stats.visionMs;
	}

	if (context.teamBlackboard != nullptr)
	{
		const TeamBlackboardService::Stats& stats = context.teamBlackboard->GetStats();
		timing->teamCount = stats.teamCount;
		timing->teamFactCount = stats.factCount;
		timing->teamReportCount = stats.reportCount;
		timing->teamWriterCount = stats.lastWriterCount;
		timing->teamExpiredCount = stats.expiredCount;
	}
}

const std::vector<AgentObject*>& AIUpdateSystem::ResolveAgents(const FrameContext& context)
{
	static const std::vector<AgentObject*> emptyAgents;
	return context.agents != nullptr ? *context.agents : emptyAgents;
}

RuntimeObjectUpdateTiming* AIUpdateSystem::ResolveTiming(const FrameContext& context)
{
	return context.perfEnabled ? context.timing : nullptr;
}
