#include "AnimationState.h"
#include "Animation.h"
#include "OgreAnimationState.h"

namespace Fancy
{
	AnimationState::AnimationState(const std::string& name, Animation* animation, bool looping, float rate) 
		: m_stateName(name), m_pAnimation(animation), m_looping(looping), m_rate(rate)
	{
		m_pAnimation->SetEnabled(true);
		m_pAnimation->SetLooping(m_looping);
	}

	AnimationState::~AnimationState()
	{
		m_pAnimation = nullptr;
	}

	const std::string& AnimationState::GetName() const
	{
		return m_stateName;
	}

	float AnimationState::getRate() const
	{
		return m_rate;
	}
}