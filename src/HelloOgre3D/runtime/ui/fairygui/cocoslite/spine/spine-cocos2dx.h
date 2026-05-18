#ifndef HELLO_FAIRYGUI_SPINE_COCOS2DX_H
#define HELLO_FAIRYGUI_SPINE_COCOS2DX_H

#include "cocos2d.h"

namespace spine
{
	class Animation
	{
	public:
		struct Name
		{
			const char* buffer() const { return ""; }
		};

		Name getName() const { return Name(); }
	};

	class TrackEntry
	{
	public:
		Animation* getAnimation() { return &_animation; }
		bool isComplete() const { return true; }
		bool getLoop() const { return false; }
		void setLoop(bool) {}
		void setTimeScale(float) {}
		void setTrackTime(float) {}
		float getAnimationEnd() const { return 0; }
		float getAnimationStart() const { return 0; }

	private:
		Animation _animation;
	};

	class AnimationState
	{
	public:
		TrackEntry* getCurrent(int) { return nullptr; }
		TrackEntry* setAnimation(int, Animation*, bool) { return &_entry; }
		void clearTrack(int) {}

	private:
		TrackEntry _entry;
	};

	class SkeletonAnimation : public cocos2d::Node
	{
	public:
		CREATE_FUNC(SkeletonAnimation);

		static SkeletonAnimation* createWithBinaryFile(const std::string&, const std::string&)
		{
			return SkeletonAnimation::create();
		}

		static SkeletonAnimation* createWithJsonFile(const std::string&, const std::string&)
		{
			return SkeletonAnimation::create();
		}

		AnimationState* getState() { return &_state; }
		Animation* findAnimation(const std::string&) { return nullptr; }
		void setSkin(const std::string&) {}

	private:
		AnimationState _state;
	};
}

#endif
