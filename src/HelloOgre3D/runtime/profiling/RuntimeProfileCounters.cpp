#include "profiling/RuntimeProfileCounters.h"
#include "profiling/Profile.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <string>

#include "OgreLogManager.h"

namespace
{
	struct RuntimeStallFrameState
	{
		RuntimeStallFrameState()
			: initialized(false)
			, enabled(false)
			, thresholdMs(50.0)
			, summaryIntervalMs(0)
			, frameIndex(0)
			, frameStarted(false)
			, frameStartMicros(0)
			, lastSummaryMicros(0)
			, frameDeltaMs(0.0)
			, drawMs(0.0)
			, aiTickCount(0)
			, aiTickMaxMs(0.0)
		{
		}

		bool initialized;
		bool enabled;
		double thresholdMs;
		int summaryIntervalMs;
		long long frameIndex;
		bool frameStarted;
		long long frameStartMicros;
		long long lastSummaryMicros;
		double frameDeltaMs;
		double drawMs;
		RuntimeSimulationTiming simulation;
		RuntimeGameUpdateTiming game;
		RuntimeObjectUpdateTiming object;
		RuntimeAiTickTiming ai;
		int aiTickCount;
		double aiTickMaxMs;
	};

	RuntimeStallFrameState s_stallFrameState;

	bool IsFalseEnvValue(const char* value)
	{
		if (value == nullptr || value[0] == '\0')
			return false;
		const std::string text(value);
		return text == "0" || text == "false" || text == "FALSE" || text == "False" || text == "off" || text == "OFF";
	}

	bool IsTrueEnvValue(const char* value)
	{
		if (value == nullptr || value[0] == '\0')
			return false;
		if (IsFalseEnvValue(value))
			return false;
		return true;
	}

	double GetEnvDouble(const char* name, double fallback)
	{
		const char* value = std::getenv(name);
		if (value == nullptr || value[0] == '\0')
			return fallback;
		const double parsed = std::atof(value);
		return parsed > 0.0 ? parsed : fallback;
	}

	int GetEnvInt(const char* name, int fallback)
	{
		const char* value = std::getenv(name);
		if (value == nullptr || value[0] == '\0')
			return fallback;
		const int parsed = std::atoi(value);
		return parsed >= 0 ? parsed : fallback;
	}

	void InitializeStallProfilerConfig()
	{
		if (s_stallFrameState.initialized)
			return;

		s_stallFrameState.enabled = IsTrueEnvValue(std::getenv("HELLO_PERF_STALL_LOG"));
		s_stallFrameState.thresholdMs = GetEnvDouble("HELLO_PERF_STALL_THRESHOLD_MS", 50.0);
		s_stallFrameState.summaryIntervalMs = GetEnvInt("HELLO_PERF_SUMMARY_INTERVAL_MS", 0);
		s_stallFrameState.initialized = true;
	}

	void PlotStallProfilerCounters(double cpuFrameMs, const RuntimeClientFrameTiming& client)
	{
		const double trackedCallbackMs = s_stallFrameState.drawMs + client.inputCaptureMs + client.updateCallMs + client.frameRenderingMs;
		const double engineGapMs = std::max(0.0, cpuFrameMs - trackedCallbackMs);
		H3D_PROFILE_PLOT("RuntimeCpuFrameMs", cpuFrameMs);
		H3D_PROFILE_PLOT("RuntimeFrameDeltaMs", s_stallFrameState.frameDeltaMs);
		H3D_PROFILE_PLOT("RuntimeEngineGapMs", engineGapMs);
		H3D_PROFILE_PLOT("RuntimeDrawMs", s_stallFrameState.drawMs);
		H3D_PROFILE_PLOT("RuntimeInputCaptureMs", client.inputCaptureMs);
		H3D_PROFILE_PLOT("RuntimeUpdateCallMs", client.updateCallMs);
		H3D_PROFILE_PLOT("RuntimeFrameRenderingMs", client.frameRenderingMs);
		H3D_PROFILE_PLOT("RuntimeSimulateCostMs", s_stallFrameState.simulation.simulateCostMs);
		H3D_PROFILE_PLOT("RuntimeGameUpdateMs", s_stallFrameState.simulation.gameUpdateMs);
		H3D_PROFILE_PLOT("RuntimeObjectManagerMs", s_stallFrameState.game.objectManagerMs);
		H3D_PROFILE_PLOT("RuntimeAiTickMs", s_stallFrameState.ai.totalMs);
		H3D_PROFILE_PLOT("RuntimeLuaTickMs", s_stallFrameState.game.luaTickMs);
		H3D_PROFILE_PLOT("RuntimeSandboxLuaMs", s_stallFrameState.game.sandboxLuaMs);
		H3D_PROFILE_PLOT("RuntimePhysicsMs", s_stallFrameState.game.physicsMs);
		H3D_PROFILE_PLOT("RuntimeDebugDrawBuildMs", s_stallFrameState.simulation.debugBuildMs);
	}

	void LogStallProfilerFrame(const char* reason, double cpuFrameMs, const RuntimeClientFrameTiming& client)
	{
		Ogre::LogManager* logManager = Ogre::LogManager::getSingletonPtr();
		if (logManager == nullptr)
			return;

		std::ostringstream stream;
		const double trackedCallbackMs = s_stallFrameState.drawMs + client.inputCaptureMs + client.updateCallMs + client.frameRenderingMs;
		const double engineGapMs = std::max(0.0, cpuFrameMs - trackedCallbackMs);
		const double frameWaitGapMs = std::max(0.0, s_stallFrameState.frameDeltaMs - cpuFrameMs);
		stream << std::fixed << std::setprecision(2);
		stream << "[FramePerf] " << reason
			<< " frame=" << s_stallFrameState.frameIndex
			<< " frameDelta=" << s_stallFrameState.frameDeltaMs
			<< " cpuFrame=" << cpuFrameMs
			<< " engineGap=" << engineGapMs
			<< " frameWait=" << frameWaitGapMs
			<< " draw=" << s_stallFrameState.drawMs
			<< " inputCapture=" << client.inputCaptureMs
			<< " updateCall=" << client.updateCallMs
			<< " frameRendering=" << client.frameRenderingMs;

		stream << "\n[FramePerf] simulate delta=" << s_stallFrameState.simulation.simulateDeltaMs
			<< " cost=" << s_stallFrameState.simulation.simulateCostMs
			<< " debugClear=" << s_stallFrameState.simulation.debugClearMs
			<< " game=" << s_stallFrameState.simulation.gameUpdateMs
			<< " debugBuild=" << s_stallFrameState.simulation.debugBuildMs
			<< " inputUpdate=" << s_stallFrameState.simulation.inputUpdateMs;

		stream << "\n[FramePerf] game luaTick=" << s_stallFrameState.game.luaTickMs
			<< " objects=" << s_stallFrameState.game.objectManagerMs
			<< " physics=" << s_stallFrameState.game.physicsMs
			<< " sandboxLua=" << s_stallFrameState.game.sandboxLuaMs;

		stream << "\n[FramePerf] objects count=" << s_stallFrameState.object.objectCount
			<< " agents=" << s_stallFrameState.object.agentCount
			<< " aiControllers=" << s_stallFrameState.object.aiControllerCount
			<< " schedulerBegin=" << s_stallFrameState.object.schedulerBeginMs
			<< " spatialRebuild=" << s_stallFrameState.object.spatialRebuildMs
			<< " eventFlush=" << s_stallFrameState.object.eventFlushMs
			<< " loop=" << s_stallFrameState.object.objectLoopMs
			<< " objectUpdate=" << s_stallFrameState.object.objectUpdateMs
			<< " objectMax=" << s_stallFrameState.object.objectUpdateMaxMs
			<< " objectEvents=" << s_stallFrameState.object.objectEventFlushMs
			<< " teamBB=" << s_stallFrameState.object.teamBlackboardMs
			<< " sceneCleanup=" << s_stallFrameState.object.sceneCleanupMs;

		stream << "\n[FramePerf] ai ticks=" << s_stallFrameState.aiTickCount
			<< " total=" << s_stallFrameState.ai.totalMs
			<< " max=" << s_stallFrameState.aiTickMaxMs
			<< " memory=" << s_stallFrameState.ai.memoryMs
			<< " vision=" << s_stallFrameState.ai.visionMs
			<< " lua=" << s_stallFrameState.ai.luaMs
			<< " driver=" << s_stallFrameState.ai.driverMs;

		stream << "\n[FramePerf] spatial enabled=" << s_stallFrameState.object.spatialEnabled
			<< " agents=" << s_stallFrameState.object.spatialAgentCount
			<< " cells=" << s_stallFrameState.object.spatialCellCount
			<< " queries=" << s_stallFrameState.object.spatialQueryCount
			<< " candidates=" << s_stallFrameState.object.spatialCandidateCount
			<< " results=" << s_stallFrameState.object.spatialResultCount
			<< " avgCandidates=" << s_stallFrameState.object.spatialAvgCandidates
			<< " avgResults=" << s_stallFrameState.object.spatialAvgResults
			<< " maxCandidates=" << s_stallFrameState.object.spatialMaxCandidates
			<< " maxResults=" << s_stallFrameState.object.spatialMaxResults;

		stream << "\n[FramePerf] teamBB teams=" << s_stallFrameState.object.teamCount
			<< " facts=" << s_stallFrameState.object.teamFactCount
			<< " reports=" << s_stallFrameState.object.teamReportCount
			<< " writers=" << s_stallFrameState.object.teamWriterCount
			<< " expired=" << s_stallFrameState.object.teamExpiredCount;

		logManager->logMessage(stream.str());
	}
}

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

RuntimeClientFrameTiming::RuntimeClientFrameTiming()
	: inputCaptureMs(0.0)
	, updateCallMs(0.0)
	, frameRenderingMs(0.0)
{
}

RuntimeSimulationTiming::RuntimeSimulationTiming()
	: simulateDeltaMs(0.0)
	, simulateCostMs(0.0)
	, debugClearMs(0.0)
	, gameUpdateMs(0.0)
	, debugBuildMs(0.0)
	, inputUpdateMs(0.0)
{
}

RuntimeGameUpdateTiming::RuntimeGameUpdateTiming()
	: luaTickMs(0.0)
	, objectManagerMs(0.0)
	, physicsMs(0.0)
	, sandboxLuaMs(0.0)
{
}

RuntimeObjectUpdateTiming::RuntimeObjectUpdateTiming()
	: schedulerBeginMs(0.0)
	, spatialRebuildMs(0.0)
	, eventFlushMs(0.0)
	, objectLoopMs(0.0)
	, objectUpdateMs(0.0)
	, objectUpdateMaxMs(0.0)
	, objectEventFlushMs(0.0)
	, teamBlackboardMs(0.0)
	, sceneCleanupMs(0.0)
	, objectCount(0)
	, agentCount(0)
	, aiControllerCount(0)
	, objectUpdateCount(0)
	, spatialEnabled(0)
	, spatialAgentCount(0)
	, spatialCellCount(0)
	, spatialQueryCount(0)
	, spatialCandidateCount(0)
	, spatialResultCount(0)
	, spatialMaxCandidates(0)
	, spatialMaxResults(0)
	, spatialAvgCandidates(0.0)
	, spatialAvgResults(0.0)
	, teamCount(0)
	, teamFactCount(0)
	, teamReportCount(0)
	, teamWriterCount(0)
	, teamExpiredCount(0)
{
}

RuntimeAiTickTiming::RuntimeAiTickTiming()
	: totalMs(0.0)
	, memoryMs(0.0)
	, visionMs(0.0)
	, luaMs(0.0)
	, driverMs(0.0)
{
}

bool RuntimeStallProfiler::IsEnabled()
{
	InitializeStallProfilerConfig();
	return s_stallFrameState.enabled;
}

long long RuntimeStallProfiler::NowMicroseconds()
{
	const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
}

double RuntimeStallProfiler::ElapsedMsSince(long long startMicros)
{
	if (startMicros <= 0)
		return 0.0;
	return static_cast<double>(NowMicroseconds() - startMicros) / 1000.0;
}

void RuntimeStallProfiler::BeginFrame(double frameDeltaMs)
{
	if (!IsEnabled())
		return;

	const bool initialized = s_stallFrameState.initialized;
	const bool enabled = s_stallFrameState.enabled;
	const double thresholdMs = s_stallFrameState.thresholdMs;
	const int summaryIntervalMs = s_stallFrameState.summaryIntervalMs;
	const long long frameIndex = s_stallFrameState.frameIndex + 1;
	const long long lastSummaryMicros = s_stallFrameState.lastSummaryMicros;

	s_stallFrameState = RuntimeStallFrameState();
	s_stallFrameState.initialized = initialized;
	s_stallFrameState.enabled = enabled;
	s_stallFrameState.thresholdMs = thresholdMs;
	s_stallFrameState.summaryIntervalMs = summaryIntervalMs;
	s_stallFrameState.frameIndex = frameIndex;
	s_stallFrameState.lastSummaryMicros = lastSummaryMicros;
	s_stallFrameState.frameStarted = true;
	s_stallFrameState.frameStartMicros = NowMicroseconds();
	s_stallFrameState.frameDeltaMs = std::max(0.0, frameDeltaMs);
}

void RuntimeStallProfiler::SetDrawTiming(double drawMs)
{
	if (!IsEnabled() || !s_stallFrameState.frameStarted)
		return;
	s_stallFrameState.drawMs = std::max(0.0, drawMs);
}

void RuntimeStallProfiler::SetSimulationTiming(const RuntimeSimulationTiming& timing)
{
	if (!IsEnabled() || !s_stallFrameState.frameStarted)
		return;
	s_stallFrameState.simulation = timing;
}

void RuntimeStallProfiler::SetGameUpdateTiming(const RuntimeGameUpdateTiming& timing)
{
	if (!IsEnabled() || !s_stallFrameState.frameStarted)
		return;
	s_stallFrameState.game = timing;
}

void RuntimeStallProfiler::SetObjectUpdateTiming(const RuntimeObjectUpdateTiming& timing)
{
	if (!IsEnabled() || !s_stallFrameState.frameStarted)
		return;
	s_stallFrameState.object = timing;
}

void RuntimeStallProfiler::AddAiTickTiming(const RuntimeAiTickTiming& timing)
{
	if (!IsEnabled() || !s_stallFrameState.frameStarted)
		return;
	++s_stallFrameState.aiTickCount;
	s_stallFrameState.ai.totalMs += std::max(0.0, timing.totalMs);
	s_stallFrameState.ai.memoryMs += std::max(0.0, timing.memoryMs);
	s_stallFrameState.ai.visionMs += std::max(0.0, timing.visionMs);
	s_stallFrameState.ai.luaMs += std::max(0.0, timing.luaMs);
	s_stallFrameState.ai.driverMs += std::max(0.0, timing.driverMs);
	s_stallFrameState.aiTickMaxMs = std::max(s_stallFrameState.aiTickMaxMs, timing.totalMs);
}

void RuntimeStallProfiler::FinishFrame(const RuntimeClientFrameTiming& timing)
{
	if (!IsEnabled() || !s_stallFrameState.frameStarted)
		return;

	const double cpuFrameMs = ElapsedMsSince(s_stallFrameState.frameStartMicros);
	PlotStallProfilerCounters(cpuFrameMs, timing);

	const bool stall = s_stallFrameState.frameDeltaMs >= s_stallFrameState.thresholdMs ||
		cpuFrameMs >= s_stallFrameState.thresholdMs ||
		s_stallFrameState.simulation.simulateCostMs >= s_stallFrameState.thresholdMs;

	bool summary = false;
	if (s_stallFrameState.summaryIntervalMs > 0)
	{
		const long long nowMicros = NowMicroseconds();
		if (s_stallFrameState.lastSummaryMicros <= 0 ||
			nowMicros - s_stallFrameState.lastSummaryMicros >= static_cast<long long>(s_stallFrameState.summaryIntervalMs) * 1000LL)
		{
			s_stallFrameState.lastSummaryMicros = nowMicros;
			summary = true;
		}
	}

	if (stall)
	{
		LogStallProfilerFrame("stall", cpuFrameMs, timing);
	}
	else if (summary)
	{
		LogStallProfilerFrame("summary", cpuFrameMs, timing);
	}

	s_stallFrameState.frameStarted = false;
}
