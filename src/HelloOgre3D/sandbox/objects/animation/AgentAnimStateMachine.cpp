#include "AgentAnimStateMachine.h"
#include "AgentAnim.h"
#include "AgentAnimState.h"
#include "AgentAnimTransition.h"
#include "LogSystem.h"
#include "GlobalFuncs.h"
#include "object/BaseObject.h"
#include "event/SandboxContext.h"
#include "event/SandboxEventDispatcherManager.h"
#include "event/SandboxEventPayload.h"
#include "GameDefine.h"
#include "OgreMath.h"

AgentAnimStateMachine::AgentAnimStateMachine(BaseObject* owner)
	: m_owner(owner)
{
}

AgentAnimStateMachine::AgentAnimStateMachine(BaseObject* owner, bool canFireEvent)
	: m_owner(owner), m_canFireEvent(canFireEvent)
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
	m_animNotifies.clear();

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

void AgentAnimStateMachine::SetStateIdResolver(StateIdResolver resolver)
{
	m_stateIdResolver = std::move(resolver);

	// 若在 AddState 之后才注入 resolver，补刷一次已有 state 的 id。
	if (!m_stateIdResolver) return;
	for (auto& entry : m_animStates)
	{
		if (entry.second && entry.second->GetID() < 0)
		{
			entry.second->SetID(m_stateIdResolver(entry.first));
		}
	}
}

bool AgentAnimStateMachine::RequestState(int stateId)
{
	// 不再依赖全局 state 表：按实例自有 id 反查 state name。
	// 大多数 ASM 只有十几个 state，线性查足够。
	for (auto& entry : m_animStates)
	{
		if (entry.second && entry.second->GetID() == stateId)
		{
			return RequestState(entry.first);
		}
	}
	return false;
}

bool AgentAnimStateMachine::RequestState(const std::string& stateName)
{
	if (!ContainsState(stateName))
	{
		return false;
	}

	m_desiredStateName = stateName;
	if (m_pCurrState == nullptr)
	{
		this->SetCurrentState(stateName);
		return true;
	}

	if (m_pNextState == nullptr)
	{
		m_pNextState = m_animStates[stateName];
	}
	return true;
}

int AgentAnimStateMachine::GetCurrStateID()
{
	if (m_pCurrState != nullptr)
	{
		return m_pCurrState->GetID();
	}
	return -1;
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

std::string AgentAnimStateMachine::GetNextStateName()
{
	static std::string empty_str = "";

	if (m_pNextState != nullptr)
	{
		return m_pNextState->GetName();
	}
	return empty_str;
}

std::string AgentAnimStateMachine::GetDesiredStateName()
{
	return m_desiredStateName;
}

bool AgentAnimStateMachine::IsCurrentState(const std::string& stateName)
{
	return m_pCurrState != nullptr && m_pCurrState->GetName() == stateName;
}

bool AgentAnimStateMachine::IsNextState(const std::string& stateName)
{
	return m_pNextState != nullptr && m_pNextState->GetName() == stateName;
}

float AgentAnimStateMachine::GetCurrStateTime()
{
	if (m_pCurrState == nullptr || m_pCurrState->GetAnimation() == nullptr)
	{
		return 0.0f;
	}

	return m_pCurrState->GetAnimation()->GetTime();
}

float AgentAnimStateMachine::GetCurrStateLength()
{
	if (m_pCurrState == nullptr || m_pCurrState->GetAnimation() == nullptr)
	{
		return 0.0f;
	}

	return m_pCurrState->GetAnimation()->GetLength();
}

float AgentAnimStateMachine::GetCurrStateProgress()
{
	const float length = GetCurrStateLength();
	if (length <= 0.0f)
	{
		return 0.0f;
	}

	return Ogre::Math::Clamp(GetCurrStateTime() / length, 0.0f, 1.0f);
}

void AgentAnimStateMachine::AddState(AgentAnimState* animState)
{
	m_animStates[animState->GetName()] = animState;
}

void AgentAnimStateMachine::AddState(const std::string& name, AgentAnim* animation, bool looping, float rate)
{
	const int stateId = m_stateIdResolver ? m_stateIdResolver(name) : -1;
	auto pAnimState = new AgentAnimState(name, animation, looping, rate, stateId);
	this->AddState(pAnimState);
}

void AgentAnimStateMachine::AddTransition(const std::string& fromState, const std::string& toState, AgentAnimTransition* transition)
{
	m_animTransitions[fromState][toState] = transition;
}

void AgentAnimStateMachine::AddTransition(const std::string& fromState, const std::string& toState, float blendOutWindow, float duration, float blendInWindow)
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

void AgentAnimStateMachine::AddNotify(const std::string& stateName, const std::string& eventName, float normalizedTime, bool fireOnce)
{
	if (!ContainsState(stateName) || eventName.empty())
	{
		return;
	}

	AnimNotify notify;
	notify.eventName = eventName;
	notify.normalizedTime = Ogre::Math::Clamp(normalizedTime, 0.0f, 1.0f);
	notify.fireOnce = fireOnce;
	notify.fired = false;
	m_animNotifies[stateName].push_back(notify);
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

	SandboxContext context = SandboxEventPayload::Make(SandboxEventTypes::AnimStateChanged(), SandboxEventScope::Local, m_owner);
	context.Set_Number("StateId", pNextState->GetID());
	context.Set_String("StateName", pNextState->GetName());
	m_owner->Event()->Emit("ASM_STATE_CHANGE", context);
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
		ResetNotifies(stateName);
		m_pCurrState->InitAnim();
		if (m_pCurrState->GetName() == m_desiredStateName)
		{
			m_desiredStateName.clear();
		}
	}
}

void AgentAnimStateMachine::ResetNotifies(const std::string& stateName)
{
	auto notifyIt = m_animNotifies.find(stateName);
	if (notifyIt == m_animNotifies.end())
	{
		return;
	}

	for (auto& notify : notifyIt->second)
	{
		notify.fired = false;
	}
}

void AgentAnimStateMachine::FireNotifyEvent(AgentAnimState* state, const AnimNotify& notify)
{
	if (!m_canFireEvent || m_owner == nullptr || state == nullptr)
	{
		return;
	}

	SandboxContext context = SandboxEventPayload::Make(SandboxEventTypes::AnimNotify(), SandboxEventScope::Local, m_owner);
	context.Set_Number("StateId", state->GetID());
	context.Set_String("StateName", state->GetName());
	context.Set_String("EventName", notify.eventName);
	context.Set_Number("NormalizedTime", notify.normalizedTime);
	m_owner->Event()->Emit("ASM_NOTIFY", context);
}

void AgentAnimStateMachine::EvaluateNotifies(AgentAnimState* state, float previousTime, float currentTime)
{
	if (state == nullptr || state->GetAnimation() == nullptr)
	{
		return;
	}

	auto notifyIt = m_animNotifies.find(state->GetName());
	if (notifyIt == m_animNotifies.end())
	{
		return;
	}

	const float length = state->GetAnimation()->GetLength();
	if (length <= 0.0f)
	{
		return;
	}

	const bool wrapped = state->IsLooping() && currentTime < previousTime;
	const float prevProgress = Ogre::Math::Clamp(previousTime / length, 0.0f, 1.0f);
	const float currProgress = Ogre::Math::Clamp(currentTime / length, 0.0f, 1.0f);

	for (auto& notify : notifyIt->second)
	{
		if (notify.fireOnce && notify.fired)
		{
			continue;
		}

		const float threshold = Ogre::Math::Clamp(notify.normalizedTime, 0.0f, 1.0f);
		bool crossed = false;
		if (!wrapped)
		{
			crossed = prevProgress < threshold && currProgress >= threshold;
		}
		else
		{
			crossed = prevProgress < threshold || currProgress >= threshold;
		}

		if (crossed)
		{
			FireNotifyEvent(state, notify);
			if (notify.fireOnce)
			{
				notify.fired = true;
			}
		}
	}
}

void AgentAnimStateMachine::StepStateAnimation(AgentAnimState* state, float deltaTimeInMillis)
{
	if (state == nullptr || state->GetAnimation() == nullptr)
	{
		return;
	}

	const float previousTime = state->GetAnimation()->GetTime();
	state->StepAnim(deltaTimeInMillis);
	const float currentTime = state->GetAnimation()->GetTime();
	if (state->IsLooping() && currentTime < previousTime)
	{
		this->FireStateChageEvent(state);
		ResetNotifies(state->GetName());
	}
	EvaluateNotifies(state, previousTime, currentTime);
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

	if (m_pCurrState != nullptr && m_pCurrTransition == nullptr && m_pNextState == nullptr && !m_desiredStateName.empty() && m_pCurrState->GetName() != m_desiredStateName)
	{
		m_pNextState = m_animStates[m_desiredStateName];
	}

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
	ResetNotifies(m_pNextState->GetName());
	m_pNextState->InitAnim(transition->getBlendInWindow());
	this->FireStateChageEvent(m_pNextState);
}

void AgentAnimStateMachine::UpdateTransition(float deltaTimeInMillis, float currTimeInSeconds)
{
	Animation_LinearBlendTo(m_pCurrState->GetAnimation(), m_pNextState->GetAnimation(),
		m_pCurrTransition->getDuration(), m_TransitionStartTime, currTimeInSeconds);

	StepStateAnimation(m_pCurrState, deltaTimeInMillis);
	StepStateAnimation(m_pNextState, deltaTimeInMillis);

	const float epsilon = 0.0001f;
	if (m_pCurrState->GetAnimWeight() < epsilon)
	{
		m_pCurrState->ClearAnim();
		m_pCurrState = m_pNextState;
		m_pNextState = nullptr;
		m_pCurrTransition = nullptr;
		m_TransitionStartTime = 0.0f;
		if (m_pCurrState->GetName() == m_desiredStateName)
		{
			m_desiredStateName.clear();
		}
	}
}

void AgentAnimStateMachine::CompleteTransition()
{
	m_pCurrState->ClearAnim();
	ResetNotifies(m_pNextState->GetName());
	m_pNextState->InitAnim();
	m_pCurrState = m_pNextState;
	m_pNextState = nullptr;
	if (m_pCurrState->GetName() == m_desiredStateName)
	{
		m_desiredStateName.clear();
	}

	this->FireStateChageEvent(m_pCurrState);
}

void AgentAnimStateMachine::StepCurrentAnimation(float deltaTimeInMillis)
{
	StepStateAnimation(m_pCurrState, deltaTimeInMillis);
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
