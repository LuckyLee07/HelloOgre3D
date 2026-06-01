#include "profiling/RuntimeProfileCounters.h"
#include "profiling/Profile.h"

void RuntimeProfileCounters::PlotFairyGuiServiceStats(int serviceOpenTotal, int serviceKindCount, int toastQueueCount, int loadingRefTotal, int serviceCreatedTotal, int serviceClosedTotal, int serviceFailedTotal, int servicePeakOpen)
{
	(void)serviceOpenTotal;
	(void)serviceKindCount;
	(void)toastQueueCount;
	(void)loadingRefTotal;
	(void)serviceCreatedTotal;
	(void)serviceClosedTotal;
	(void)serviceFailedTotal;
	(void)servicePeakOpen;

	H3D_PROFILE_PLOT("FGUIServiceOpen", static_cast<double>(serviceOpenTotal));
	H3D_PROFILE_PLOT("FGUIServiceKinds", static_cast<double>(serviceKindCount));
	H3D_PROFILE_PLOT("FGUIToastQueue", static_cast<double>(toastQueueCount));
	H3D_PROFILE_PLOT("FGUILoadingRefs", static_cast<double>(loadingRefTotal));
	H3D_PROFILE_PLOT("FGUIServiceCreated", static_cast<double>(serviceCreatedTotal));
	H3D_PROFILE_PLOT("FGUIServiceClosed", static_cast<double>(serviceClosedTotal));
	H3D_PROFILE_PLOT("FGUIServiceFailed", static_cast<double>(serviceFailedTotal));
	H3D_PROFILE_PLOT("FGUIServicePeakOpen", static_cast<double>(servicePeakOpen));
}

void RuntimeProfileCounters::PlotAiSchedulerStats(int enabled, int agentCount, int tickedCount, int skippedCount, int pendingCount, int tickIntervalMs, int maxTicksPerFrame)
{
	(void)enabled;
	(void)agentCount;
	(void)tickedCount;
	(void)skippedCount;
	(void)pendingCount;
	(void)tickIntervalMs;
	(void)maxTicksPerFrame;

	H3D_PROFILE_PLOT("AISchedulerEnabled", static_cast<double>(enabled));
	H3D_PROFILE_PLOT("AISchedulerAgents", static_cast<double>(agentCount));
	H3D_PROFILE_PLOT("AISchedulerTicked", static_cast<double>(tickedCount));
	H3D_PROFILE_PLOT("AISchedulerSkipped", static_cast<double>(skippedCount));
	H3D_PROFILE_PLOT("AISchedulerPending", static_cast<double>(pendingCount));
	H3D_PROFILE_PLOT("AISchedulerTickIntervalMs", static_cast<double>(tickIntervalMs));
	H3D_PROFILE_PLOT("AISchedulerMaxTicksPerFrame", static_cast<double>(maxTicksPerFrame));
}

void RuntimeProfileCounters::PlotTeamBlackboardStats(int teamCount, int factCount, int reportCount, int writerCount, int expiredCount)
{
	(void)teamCount;
	(void)factCount;
	(void)reportCount;
	(void)writerCount;
	(void)expiredCount;

	H3D_PROFILE_PLOT("TeamBlackboardTeams", static_cast<double>(teamCount));
	H3D_PROFILE_PLOT("TeamBlackboardFacts", static_cast<double>(factCount));
	H3D_PROFILE_PLOT("TeamBlackboardReports", static_cast<double>(reportCount));
	H3D_PROFILE_PLOT("TeamBlackboardWriters", static_cast<double>(writerCount));
	H3D_PROFILE_PLOT("TeamBlackboardExpired", static_cast<double>(expiredCount));
}
