#include "AnimationState.h"
#include "Animation.h"
#include "OgreAnimationState.h"

namespace Fancy
{
	AnimationState::AnimationState(const std::string& name, Animation* animation, bool looping, float rate) 
		: m_stateName(name), m_pAnimation(animation), m_looping(looping), m_rate(rate)
	{
		m_pAnimation->SetEnabled(false);
		m_pAnimation->SetLooping(m_looping);
	}

	AnimationState::~AnimationState()
	{
		m_pAnimation = nullptr;
	}

	void AnimationState::InitAnimation(float startTime)
	{
		m_pAnimation->Init(startTime);
	}

	void AnimationState::ClearAnimation()
	{
		m_pAnimation->Clear();
	}

	float AnimationState::GetAnimationWeight()
	{
		return m_pAnimation->GetWeight();
	}

	void AnimationState::StepAnimation(float deltaTimeInMillis, float rate)
	{
		float realRate = rate > 0.0f ? rate : m_rate;
		float deltaTimeInSeconds = deltaTimeInMillis / 1000;
		m_pAnimation->AddTime(deltaTimeInSeconds * realRate);
	}

	const std::string& AnimationState::GetName() const
	{
		return m_stateName;
	}

	float AnimationState::GetRate() const
	{
		return m_rate;
	}
}