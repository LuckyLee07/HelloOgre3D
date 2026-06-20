#ifndef __RUNTIME_PROFILE_COUNTERS_H__
#define __RUNTIME_PROFILE_COUNTERS_H__

class RuntimeProfileCounters
{
public:
	static void PlotFairyGuiServiceStats(int serviceOpenTotal, int serviceKindCount, int toastQueueCount, int loadingRefTotal, int serviceCreatedTotal, int serviceClosedTotal, int serviceFailedTotal, int servicePeakOpen);
	static void PlotAiSchedulerStats(int enabled, int agentCount, int tickedCount, int skippedCount, int pendingCount, int tickIntervalMs, int maxTicksPerFrame);
	static void PlotAgentPerceptionStats(int enabled, int controllerCount, int scanCount, int visibleCount, int spatialQueryCount, int spatialCandidateCount, int spatialFilteredCandidateCount, int spatialResultCount);
	static void PlotHearingDangerSenseStats(int enabled, int eventCount, int publishedEventCount, int prunedEventCount, int runCount, int skipCount, int agentCheckCount, int heardResponseCount, int dangerResponseCount, int investigationCount, int retreatFactApplyCount);
	static void PlotTeamBlackboardStats(int teamCount, int factCount, int reportCount, int writerCount, int expiredCount);
};

struct RuntimeClientFrameTiming
{
	RuntimeClientFrameTiming();

	double inputCaptureMs;
	double updateCallMs;
	double frameRenderingMs;
};

struct RuntimeSimulationTiming
{
	RuntimeSimulationTiming();

	double simulateDeltaMs;
	double simulateCostMs;
	double debugClearMs;
	double gameUpdateMs;
	double debugBuildMs;
	double inputUpdateMs;
};

struct RuntimeGameUpdateTiming
{
	RuntimeGameUpdateTiming();

	double luaTickMs;
	double objectManagerMs;
	double physicsMs;
	double sandboxLuaMs;
};

struct RuntimeObjectUpdateTiming
{
	RuntimeObjectUpdateTiming();

	double schedulerBeginMs;
	double spatialRebuildMs;
	double perceptionSystemMs;
	double eventFlushMs;
	double objectLoopMs;
	double objectUpdateMs;
	double objectUpdateMaxMs;
	double objectEventFlushMs;
	double teamBlackboardMs;
	double sceneCleanupMs;
	int objectCount;
	int agentCount;
	int aiControllerCount;
	int objectUpdateCount;
	int perceptionEnabled;
	int perceptionAgentCount;
	int perceptionControllerCount;
	int perceptionScanCount;
	int perceptionVisibleCount;
	int perceptionSpatialQueryCount;
	int perceptionSpatialCandidateCount;
	int perceptionSpatialFilteredCandidateCount;
	int perceptionSpatialResultCount;
	int perceptionSpatialRejectedSelfCount;
	int perceptionSpatialRejectedTeamCount;
	int perceptionSpatialRejectedDeadCount;
	int perceptionSpatialRejectedTypeCount;
	double perceptionSpatialQueryCostMs;
	double perceptionMemoryMs;
	double perceptionVisionMs;
	int spatialEnabled;
	int spatialAgentCount;
	int spatialCellCount;
	int spatialQueryCount;
	int spatialCandidateCount;
	int spatialFilteredCandidateCount;
	int spatialResultCount;
	int spatialMaxCandidates;
	int spatialMaxFilteredCandidates;
	int spatialMaxResults;
	int spatialRejectedSelfCount;
	int spatialRejectedTeamCount;
	int spatialRejectedDeadCount;
	int spatialRejectedTypeCount;
	double spatialQueryCostMs;
	double spatialAvgCandidates;
	double spatialAvgFilteredCandidates;
	double spatialAvgResults;
	int teamCount;
	int teamFactCount;
	int teamReportCount;
	int teamWriterCount;
	int teamExpiredCount;
};

struct RuntimeAiTickTiming
{
	RuntimeAiTickTiming();

	double totalMs;
	double memoryMs;
	double visionMs;
	double luaMs;
	double driverMs;
};

class RuntimeStallProfiler
{
public:
	static bool IsEnabled();
	static long long NowMicroseconds();
	static double ElapsedMsSince(long long startMicros);
	static void BeginFrame(double frameDeltaMs);
	static void SetDrawTiming(double drawMs);
	static void SetSimulationTiming(const RuntimeSimulationTiming& timing);
	static void SetGameUpdateTiming(const RuntimeGameUpdateTiming& timing);
	static void SetObjectUpdateTiming(const RuntimeObjectUpdateTiming& timing);
	static void AddAiTickTiming(const RuntimeAiTickTiming& timing);
	static void FinishFrame(const RuntimeClientFrameTiming& timing);
};

#endif // __RUNTIME_PROFILE_COUNTERS_H__
