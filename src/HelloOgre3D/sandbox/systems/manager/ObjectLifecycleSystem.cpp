#include "ObjectLifecycleSystem.h"

#include "ObjectManager.h"
#include "ai/common/AIUpdateSystem.h"
#include "ai/common/AIScheduler.h"
#include "components/ai/AIController.h"
#include "core/object/BaseObject.h"
#include "profiling/RuntimeProfileCounters.h"
#include "systems/service/SceneFactory.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"

#include <algorithm>

ObjectLifecycleSystem::UpdateContext::UpdateContext()
	: objects(nullptr)
	, removedSceneNodes(nullptr)
	, objectManager(nullptr)
	, aiUpdateSystem(nullptr)
	, scheduler(nullptr)
	, useAiScheduler(false)
	, perfEnabled(false)
	, timing(nullptr)
{
}

void ObjectLifecycleSystem::UpdateObjects(int deltaMilliseconds, const UpdateContext& context)
{
	if (context.objects == nullptr)
		return;

	RuntimeObjectUpdateTiming* timing = ResolveTiming(context);
	const long long objectLoopStartMicros = context.perfEnabled ? RuntimeStallProfiler::NowMicroseconds() : 0;
	for (auto iter = context.objects->begin(); iter != context.objects->end();)
	{
		BaseObject* pObject = iter->second;
		if (pObject == nullptr || pObject->CheckNeedClear())
		{
			if (context.aiUpdateSystem != nullptr && context.scheduler != nullptr && pObject != nullptr)
				context.aiUpdateSystem->RemoveAgent(pObject->GetObjId(), context.scheduler);
			iter = context.objects->erase(iter);
			if (context.objectManager != nullptr)
				context.objectManager->realRemoveObject(pObject);
		}
		else
		{
			AIController* ai = pObject->FindComponent<AIController>();
			if (context.aiUpdateSystem != nullptr)
				context.aiUpdateSystem->TickAgent(ai, deltaMilliseconds, context.useAiScheduler, context.scheduler);

			const long long objectUpdateStartMicros = context.perfEnabled ? RuntimeStallProfiler::NowMicroseconds() : 0;
			pObject->Update(deltaMilliseconds);
			if (context.perfEnabled && timing != nullptr)
			{
				const double objectUpdateMs = RuntimeStallProfiler::ElapsedMsSince(objectUpdateStartMicros);
				timing->objectUpdateMs += objectUpdateMs;
				timing->objectUpdateMaxMs = std::max(timing->objectUpdateMaxMs, objectUpdateMs);
				++timing->objectUpdateCount;
			}
			const long long objectEventStartMicros = context.perfEnabled ? RuntimeStallProfiler::NowMicroseconds() : 0;
			pObject->FlushQueuedEvents();
			if (context.perfEnabled && timing != nullptr)
				timing->objectEventFlushMs += RuntimeStallProfiler::ElapsedMsSince(objectEventStartMicros);
			if (pObject->GetObjType() == BaseObject::OBJ_TYPE_BLOCK)
			{
				//static_cast<BlockObject*>(pObject)->getSceneNode()->setVisible(false);
			}
			iter++;
		}
	}
	if (context.perfEnabled && timing != nullptr)
		timing->objectLoopMs = RuntimeStallProfiler::ElapsedMsSince(objectLoopStartMicros);
}

void ObjectLifecycleSystem::CleanupRemovedSceneNodes(int deltaMilliseconds, const UpdateContext& context)
{
	if (context.removedSceneNodes == nullptr)
		return;

	Ogre::SceneNode* pRootScene = SceneFactory::GetRootSceneNode();
	RuntimeObjectUpdateTiming* timing = ResolveTiming(context);
	const long long stageStartMicros = context.perfEnabled ? RuntimeStallProfiler::NowMicroseconds() : 0;
	for (auto iter = context.removedSceneNodes->begin(); pRootScene != nullptr && iter != context.removedSceneNodes->end();)
	{
		int lastMilliSeconds = iter->second;
		lastMilliSeconds -= deltaMilliseconds;
		if (lastMilliSeconds <= 0)
		{
			auto pSceneNode = iter->first;
			SceneFactory::RemParticleBySceneNode(pSceneNode);
			pRootScene->removeChild(pSceneNode);
			pRootScene->getCreator()->destroySceneNode(pSceneNode);

			iter = context.removedSceneNodes->erase(iter);
		}
		else
		{
			iter->second = lastMilliSeconds;
			iter++;
		}
	}
	if (context.perfEnabled && timing != nullptr)
		timing->sceneCleanupMs = RuntimeStallProfiler::ElapsedMsSince(stageStartMicros);
}

RuntimeObjectUpdateTiming* ObjectLifecycleSystem::ResolveTiming(const UpdateContext& context)
{
	return context.timing;
}
