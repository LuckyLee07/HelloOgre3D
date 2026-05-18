#ifndef HELLO_FAIRYGUI_AUDIO_ENGINE_H
#define HELLO_FAIRYGUI_AUDIO_ENGINE_H

#include "cocos2d.h"

namespace cocos2d
{
	namespace experimental
	{
		class AudioEngine
		{
		public:
			static int play2d(const std::string&, bool = false, float = 1.0f)
			{
				return 0;
			}
		};
	}

	using AudioEngine = experimental::AudioEngine;
}

#endif
