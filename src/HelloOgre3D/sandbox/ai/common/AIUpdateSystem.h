#ifndef __AI_UPDATE_SYSTEM_H__
#define __AI_UPDATE_SYSTEM_H__

#include <vector>

class AgentObject;
class AIController;
class AIScheduler;
class AgentPerceptionSystem;
class AgentSpatialIndexSystem;
class ObjectManager;
class TeamBlackboardService;
struct RuntimeObjectUpdateTiming;

class AIUpdateSystem
{
public:
	struct FrameContext
	{
		FrameContext();

		const std::vector<AgentObject*>* agents;
		ObjectManager* objectManager;
		AIScheduler* scheduler;
		AgentSpatialIndexSystem* spatialIndex;
		AgentPerceptionSystem* perceptionSystem;
		TeamBlackboardService* teamBlackboard;
		int aiControllerCount;
		bool perfEnabled;
		RuntimeObjectUpdateTiming* timing;
	};

	bool BeginFrame(int deltaMilliseconds, const FrameContext& context);
	void UpdatePerception(int deltaMilliseconds, const FrameContext& context);
	void TickAgent(AIController* ai, int deltaMilliseconds, bool useAiScheduler, AIScheduler* scheduler);
	void RemoveAgent(unsigned int agentId, AIScheduler* scheduler);
	void EndFrame(int deltaMilliseconds, const FrameContext& context);
	void FillPerfStats(const FrameContext& context) const;

private:
	static const std::vector<AgentObject*>& ResolveAgents(const FrameContext& context);
	static RuntimeObjectUpdateTiming* ResolveTiming(const FrameContext& context);
};

#endif // __AI_UPDATE_SYSTEM_H__
