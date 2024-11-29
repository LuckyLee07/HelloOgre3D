#include "AnimationState.h"

namespace Fancy
{
	AnimationState::AnimationState(const std::string& name, Animation* animation, bool looping, float rate) 
		: m_stateName(name), m_pAnimation(animation), m_looping(looping), m_rate(rate)
	{

	}

	AnimationState::~AnimationState()
	{
		m_pAnimation = nullptr;
	}

	const std::string& AnimationState::GetName()
	{
		return m_stateName;
	}
}