#include "AnimationTransition.h"

namespace Fancy
{
	AnimationTransition::AnimationTransition(float blendOutWindow/* = 0.0f*/, float duration /*= 0.2f */ , float blendInWindow/* = 0.0f*/)
		: m_duration(duration), m_blendCurve("linear"), m_blendInWindow(blendInWindow), m_blendOutWindow(blendOutWindow)
	{
		
	}

	AnimationTransition::~AnimationTransition()
	{

	}

	std::string AnimationTransition::getBlendCurve()
	{
		return m_blendCurve;
	}

	float AnimationTransition::getDuration()
	{
		return m_duration;
	}

	float AnimationTransition::getBlendInWindow()
	{
		return m_blendInWindow;
	}

	float AnimationTransition::getBlendOutWindow()
	{
		return m_blendOutWindow;
	}
}