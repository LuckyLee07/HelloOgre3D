#ifndef __OBJECT_LIFECYCLE_SYSTEM_H__
#define __OBJECT_LIFECYCLE_SYSTEM_H__

#include <unordered_map>

namespace Ogre
{
	class SceneNode;
}
class AIUpdateSystem;
class AIScheduler;
class BaseObject;
class ObjectManager;
struct RuntimeObjectUpdateTiming;

class ObjectLifecycleSystem
{
public:
	struct UpdateContext
	{
		UpdateContext();

		std::unordered_map<int, BaseObject*>* objects;
		std::unordered_map<Ogre::SceneNode*, int>* removedSceneNodes;
		ObjectManager* objectManager;
		AIUpdateSystem* aiUpdateSystem;
		AIScheduler* scheduler;
		bool useAiScheduler;
		bool perfEnabled;
		RuntimeObjectUpdateTiming* timing;
	};

	void UpdateObjects(int deltaMilliseconds, const UpdateContext& context);
	void CleanupRemovedSceneNodes(int deltaMilliseconds, const UpdateContext& context);

private:
	static RuntimeObjectUpdateTiming* ResolveTiming(const UpdateContext& context);
};

#endif // __OBJECT_LIFECYCLE_SYSTEM_H__
