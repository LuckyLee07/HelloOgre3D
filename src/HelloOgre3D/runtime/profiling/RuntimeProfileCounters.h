#ifndef __RUNTIME_PROFILE_COUNTERS_H__
#define __RUNTIME_PROFILE_COUNTERS_H__

class RuntimeProfileCounters
{
public:
	static void PlotFairyGuiServiceStats(int serviceOpenTotal, int serviceKindCount, int toastQueueCount, int loadingRefTotal, int serviceCreatedTotal, int serviceClosedTotal, int serviceFailedTotal, int servicePeakOpen);
	static void PlotAiSchedulerStats(int enabled, int agentCount, int tickedCount, int skippedCount, int pendingCount, int tickIntervalMs, int maxTicksPerFrame);
	static void PlotTeamBlackboardStats(int teamCount, int factCount, int reportCount, int writerCount, int expiredCount);
};

#endif // __RUNTIME_PROFILE_COUNTERS_H__
