#include "AgentAnimStateMachine.h"
#include "AgentAnim.h"
#include "AgentAnimState.h"
#include "AgentAnimTransition.h"
#include "LogSystem.h"
#include "GlobalFuncs.h"
#include "BaseObject.h"
#include "SandboxEventDispatcherManager.h"

AgentAnimStateMachine::AgentAnimStateMachine(BaseObject* owner) : m_owner(owner),
	m_pCurrState(nullptr), m_pNextState(nullptr),
	m_pCurrTransition(nullptr), m_TransitionStartTime(0.0f), m_canFireEvent(true)
{

}

AgentAnimStateMachine::~AgentAnimStateMachine()
{
	m_owner = nullptr;
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


bool AgentAnimStateMachine::RequestState(const std::string& stateName)
{
	if (m_pNextState == nullptr && ContainsState(stateName))
	{
		if (m_pCurrState == nullptr)
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

std::string AgentAnimStateMachine::GetCurrStateName()
{
	static std::string empty_str = "";

	if (m_pCurrState != nullptr)
	{
		return m_pCurrState->GetName();
	}
	return empty_str;
}

void AgentAnimStateMachine::AddState(AgentAnimState* animState)
{
	m_animStates[animState->GetName()] = animState;
}

void AgentAnimStateMachine::AddState(const std::string& name, AgentAnim* animation, bool looping/* = false*/, float rate/* = 1.0f*/)
{
	auto pAnimState = new AgentAnimState(name, animation, looping, rate);
	this->AddState(pAnimState);
}

void AgentAnimStateMachine::AddTransition(const std::string& fromState, const std::string& toState, AgentAnimTransition* transition)
{
	m_animTransitions[fromState][toState] = transition;
}

void AgentAnimStateMachine::AddTransition(const std::string& fromState, const std::string& toState, float blendOutWindow/* = 0.0f*/, float duration/* = 0.2f*/, float blendInWindow/* = 0.0f*/)
{
	if (ContainsState(fromState) && ContainsState(toState))
	{
		auto transition = new AgentAnimTransition(blendOutWindow, duration, blendInWindow);
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

void AgentAnimStateMachine::SetCanFireEvent(bool canFireEvent)
{
	m_canFireEvent = canFireEvent;
}

void AgentAnimStateMachine::FireStateChageEvent(AgentAnimState* pNextState)
{
	if (!m_canFireEvent) return;

	if (m_owner == nullptr || pNextState == nullptr) 
		return;
		
	SandboxContext context;
	context.Set_String("StateId", pNextState->GetName());
	m_owner->Event()->Emit("FSM_STATE_CHANGE", context);
}

void AgentAnimStateMachine::SetCurrentState(const std::string& stateName)
{
	if (m_animStates.find(stateName) != m_animStates.end())
	{
		if (m_pCurrState != nullptr)
			m_pCurrState->ClearAnim();
		if (m_pNextState != nullptr)
			m_pNextState->ClearAnim();

		m_pNextState = nullptr;
		m_pCurrTransition = nullptr;
		m_TransitionStartTime = 0.0f;
		m_pCurrState = m_animStates[stateName];
		m_pCurrState->InitAnim();
	}
}

bool AgentAnimStateMachine::ContainsState(const std::string& stateName)
{
	return m_animStates.find(stateName) != m_animStates.end();
}

bool AgentAnimStateMachine::ContainsTransition(const std::string& fromState, const std::string& toState)
{
	auto fromStateIt = m_animTransitions.find(fromState);
	if (fromStateIt != m_animTransitions.end())
	{
		return fromStateIt->second.find(toState) != fromStateIt->second.end();
	}
	return false;
}

void AgentAnimStateMachine::Update(float deltaTimeInMillis, long long currTimeInMillis)
{
	float deltaTimeInSeconds = deltaTimeInMillis / 1000.0f;
	float currTimeInSeconds = currTimeInMillis / 1000.0f;

	if (m_pCurrTransition == nullptr && m_pNextState != nullptr)
	{
		float currAnimTime = m_pCurrState->m_pAnim->GetTime();
		float currAnimLength = m_pCurrState->m_pAnim->GetLength();
		float deltaTime = deltaTimeInSeconds * m_pCurrState->GetRate();

		if (ContainsTransition(m_pCurrState->GetName(), m_pNextState->GetName()))
		{
			AgentAnimTransition* transition = m_animTransitions[m_pCurrState->GetName()][m_pNextState->GetName()];
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

	if (m_pCurrTransition != nullptr)
	{
		UpdateTransition(deltaTimeInMillis, currTimeInSeconds);
	}
	else if (m_pCurrState)
	{
		StepCurrentAnimation(deltaTimeInMillis);
	}
}

void AgentAnimStateMachine::StartTransition(AgentAnimTransition* transition, float currTimeInSeconds)
{
	m_pCurrTransition = transition;
	m_TransitionStartTime = currTimeInSeconds;
	m_pNextState->InitAnim(transition->getBlendInWindow());

	this->FireStateChageEvent(m_pNextState);
}

void AgentAnimStateMachine::UpdateTransition(float deltaTimeInMillis, float currTimeInSeconds)
{
	Animation_LinearBlendTo(m_pCurrState->GetAnimation(), m_pNextState->GetAnimation(),
		m_pCurrTransition->getDuration(), m_TransitionStartTime, currTimeInSeconds);

	m_pCurrState->StepAnim(deltaTimeInMillis);
	m_pNextState->StepAnim(deltaTimeInMillis);

	const float epsilon = 0.0001f;  // 设定个较小的容差值
	if (m_pCurrState->GetAnimWeight() < epsilon)
	{
		m_pCurrState->ClearAnim();
		m_pCurrState = m_pNextState;
		m_pNextState = nullptr;
		m_pCurrTransition = nullptr;
		m_TransitionStartTime = 0.0f;
	}
}

void AgentAnimStateMachine::CompleteTransition()
{
	m_pCurrState->ClearAnim();
	m_pNextState->InitAnim();
	m_pCurrState = m_pNextState;
	m_pNextState = nullptr;

	this->FireStateChageEvent(m_pNextState);
}

void AgentAnimStateMachine::StepCurrentAnimation(float deltaTimeInMillis)
{
	AgentAnim* pAnimation = m_pCurrState->m_pAnim;

	float currAnimTime = pAnimation->GetTime();
	float currAnimLength = pAnimation->GetLength();
	float stepRate = m_pCurrState->GetRate();

	float deltaTime = (deltaTimeInMillis / 1000.0f) * stepRate;

	float timeStepped = currAnimTime + deltaTime;
	while (timeStepped >= currAnimLength)
	{
		timeStepped -= currAnimLength; // Looping
		this->FireStateChageEvent(m_pCurrState);
	}
	pAnimation->AddTime(deltaTime);
}

void AgentAnimStateMachine::Animation_LinearBlendIn(AgentAnim* animation, float blendTime, float startTime, float currTime)
{
	float realBlendTime = clamp(0.1f, blendTime, blendTime);
	float percent = (currTime - startTime) / realBlendTime;

	float realPercent = clamp(0.0f, 1.0f, percent);
	animation->SetWeight(realPercent);
}

void AgentAnimStateMachine::Animation_LinearBlendOut(AgentAnim* animation, float blendTime, float startTime, float currTime)
{
	float realBlendTime = clamp(0.1f, blendTime, blendTime);
	float percent = (currTime - startTime) / realBlendTime;

	float realPercent = clamp(0.0f, 1.0f, percent);
	animation->SetWeight(1.0f - realPercent);
}

void AgentAnimStateMachine::Animation_LinearBlendTo(AgentAnim* startAnim, AgentAnim* endAnim, float blendTime, float startTime, float currTime)
{
	Animation_LinearBlendIn(endAnim, blendTime, startTime, currTime);
	Animation_LinearBlendOut(startAnim, blendTime, startTime, currTime);
}
