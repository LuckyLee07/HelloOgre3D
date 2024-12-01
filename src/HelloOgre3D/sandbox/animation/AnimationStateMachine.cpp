#include "AnimationStateMachine.h"
#include "Animation.h"
#include "AnimationState.h"
#include "AnimationTransition.h"
#include "LogSystem.h"
#include "GlobalFuncs.h"

namespace Fancy
{
	AnimationStateMachine::AnimationStateMachine() : m_pCurrentState(nullptr), m_pNextState(nullptr), 
		m_pCurrentTransition(nullptr), m_TransitionStartTime(0.0f)
	{

	}

	AnimationStateMachine::~AnimationStateMachine()
	{
		auto stateIt = m_animStates.begin();
		for (; stateIt != m_animStates.end(); stateIt++)
		{
			delete stateIt->second;
		}
		m_animStates.clear();

		auto transIt = m_animTransitions.begin();
		for (; transIt != m_animTransitions.end(); transIt++)
		{
			auto transIter = transIt->second.begin();
			for (; transIter != transIt->second.end(); transIter++)
			{
				delete transIter->second;
			}
			transIt->second.clear();
		}
		m_animTransitions.clear();
	}


	bool AnimationStateMachine::RequestState(const std::string& stateName)
	{
		if (m_pNextState == nullptr && ContainsState(stateName))
		{
			if (m_pCurrentState == nullptr)
			{
				this->SetCurrentState(stateName);
			}
			else
			{
				m_pNextState = m_animStates[stateName];
			}
			return true;
		}
		return false;
	}

	std::string AnimationStateMachine::GetCurrStateName()
	{
		static std::string empty_str = "";

		if (m_pCurrentState != nullptr)
		{
			return m_pCurrentState->GetName();
		}
		return empty_str;
	}

	void AnimationStateMachine::AddState(AnimationState* animState)
	{
		m_animStates[animState->GetName()] = animState;
	}

	void AnimationStateMachine::AddState(const std::string& name, Animation* animation, bool looping/* = false*/, float rate/* = 1.0f*/)
	{
		auto pAnimState = new AnimationState(name, animation, looping, rate);
		this->AddState(pAnimState);
	}

	void AnimationStateMachine::AddTransition(const std::string& fromState, const std::string& toState, AnimationTransition* transition)
	{
		m_animTransitions[fromState][toState] = transition;
	}

	void AnimationStateMachine::AddTransition(const std::string& fromState, const std::string& toState, float blendOutWindow/* = 0.0f*/, float duration/* = 0.2f*/, float blendInWindow/* = 0.0f*/)
	{
		if (ContainsState(fromState) && ContainsState(toState))
		{
			auto transition = new AnimationTransition(blendOutWindow, duration, blendInWindow);
			this->AddTransition(fromState, toState, transition);
		}
		else
		{
			if (!ContainsState(fromState))
			{
				CCLOG_INFO("Error===>>Can't Find State: %s", fromState.c_str());
			}
			else if (!ContainsState(toState))
			{
				CCLOG_INFO("Error===>>Can't Find State: %s", toState.c_str());
			}
		}
	}

	void AnimationStateMachine::SetCurrentState(const std::string& stateName)
	{
		if (m_animStates.find(stateName) != m_animStates.end())
		{
			if (m_pCurrentState != nullptr)
				m_pCurrentState->ClearAnimation();
			if (m_pNextState != nullptr)
				m_pNextState->ClearAnimation();

			m_pNextState = nullptr;
			m_pCurrentTransition = nullptr;
			m_TransitionStartTime = 0.0f;
			m_pCurrentState = m_animStates[stateName];
			m_pCurrentState->InitAnimation();
		}
	}

	bool AnimationStateMachine::ContainsState(const std::string& stateName)
	{
		return m_animStates.find(stateName) != m_animStates.end();
	}

	bool AnimationStateMachine::ContainsTransition(const std::string& fromState, const std::string& toState)
	{
		auto& fromStateIt = m_animTransitions.find(fromState);
		if (fromStateIt != m_animTransitions.end())
		{
			return fromStateIt->second.find(toState) != fromStateIt->second.end();
		}
		return false;
	}

	void AnimationStateMachine::Update(float deltaTimeInMillis, long long currTimeInMillis)
	{
		float deltaTimeInSeconds = deltaTimeInMillis / 1000.0f;
		float currTimeInSeconds = currTimeInMillis / 1000.0f;

		if (m_pCurrentTransition == nullptr && m_pNextState != nullptr)
		{
			float currAnimTime = m_pCurrentState->m_pAnimation->GetTime();
			float currAnimLength = m_pCurrentState->m_pAnimation->GetLength();
			float deltaTime = deltaTimeInSeconds * m_pCurrentState->GetRate();

			if (ContainsTransition(m_pCurrentState->GetName(), m_pNextState->GetName()))
			{
				AnimationTransition* transition = m_animTransitions[m_pCurrentState->GetName()][m_pNextState->GetName()];
				if ((currAnimTime + deltaTime) >= (currAnimLength - transition->getBlendOutWindow()))
				{
					StartTransition(transition, currTimeInSeconds);
				}
			}
			else
			{
				if ((currAnimTime + deltaTime) >= currAnimLength)
				{
					CompleteTransition();
				}
			}
		}

		if (m_pCurrentTransition != nullptr)
		{
			UpdateTransition(deltaTimeInMillis, currTimeInSeconds);
		}
		else if (m_pCurrentState)
		{
			StepCurrentAnimation(deltaTimeInMillis);
		}
	}

	void AnimationStateMachine::StartTransition(AnimationTransition* transition, float currTimeInSeconds)
	{
		m_pCurrentTransition = transition;
		m_TransitionStartTime = currTimeInSeconds;
		m_pNextState->InitAnimation(transition->getBlendInWindow());
	}

	void AnimationStateMachine::UpdateTransition(float deltaTimeInMillis, float currTimeInSeconds)
	{
		Animation_LinearBlendTo(m_pCurrentState->GetAnimation(), m_pNextState->GetAnimation(), 
			m_pCurrentTransition->getDuration(), m_TransitionStartTime, currTimeInSeconds);

		m_pCurrentState->StepAnimation(deltaTimeInMillis);
		m_pNextState->StepAnimation(deltaTimeInMillis);

		const float epsilon = 0.0001f;  // 设定个较小的容差值
		if (m_pCurrentState->GetAnimationWeight() < epsilon)
		{
			m_pCurrentState->ClearAnimation();
			m_pCurrentState = m_pNextState;
			m_pNextState = nullptr;
			m_pCurrentTransition = nullptr;
			m_TransitionStartTime = 0.0f;
		}
	}

	void AnimationStateMachine::CompleteTransition()
	{
		m_pCurrentState->ClearAnimation();
		m_pNextState->InitAnimation();
		m_pCurrentState = m_pNextState;
		m_pNextState = nullptr;
	}

	void AnimationStateMachine::StepCurrentAnimation(float deltaTimeInMillis)
	{
		Animation* pAnimation = m_pCurrentState->m_pAnimation;

		float currAnimTime = pAnimation->GetTime();
		float currAnimLength = pAnimation->GetLength();
		float stepRate = m_pCurrentState->GetRate();

		float deltaTime = (deltaTimeInMillis / 1000.0f) * stepRate;

		float timeStepped = currAnimTime + deltaTime;
		while (timeStepped >= currAnimLength)
		{
			timeStepped -= currAnimLength; // Looping
		}
		pAnimation->AddTime(deltaTime);
	}

	void AnimationStateMachine::Animation_LinearBlendIn(Animation* animation, float blendTime, float startTime, float currTime)
	{
		float realBlendTime = clamp(0.1f, blendTime, blendTime);
		float percent = (currTime - startTime) / realBlendTime;

		float realPercent = clamp(0.0f, 1.0f, percent);
		animation->SetWeight(realPercent);
	}

	void AnimationStateMachine::Animation_LinearBlendOut(Animation* animation, float blendTime, float startTime, float currTime)
	{
		float realBlendTime = clamp(0.1f, blendTime, blendTime);
		float percent = (currTime - startTime) / realBlendTime;

		float realPercent = clamp(0.0f, 1.0f, percent);
		animation->SetWeight(1.0f - realPercent);
	}

	void AnimationStateMachine::Animation_LinearBlendTo(Animation* startAnim, Animation* endAnim, float blendTime, float startTime, float currTime)
	{
		Animation_LinearBlendIn(endAnim, blendTime, startTime, currTime);
		Animation_LinearBlendOut(startAnim, blendTime, startTime, currTime);
	}
}