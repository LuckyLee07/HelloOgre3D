#include "AgentAnimState.h"
#include "AgentAnim.h"
#include "OgreAnimationState.h"

AgentAnimState::AgentAnimState(const std::string& name, AgentAnim* animation, bool looping, float rate)
	: m_stateName(name), m_pAnim(animation), m_looping(looping), m_rate(rate)
{
	m_pAnim->SetEnabled(false);
	m_pAnim->SetLooping(m_looping);
}

AgentAnimState::~AgentAnimState()
{
	m_pAnim = nullptr;
}

void AgentAnimState::InitAnim(float startTime)
{
	m_pAnim->Init(startTime);
}

void AgentAnimState::ClearAnim()
{
	m_pAnim->Clear();
}

float AgentAnimState::GetAnimWeight() const
{
	return m_pAnim->GetWeight();
}

float AgentAnimState::GetRate() const
{
	return m_rate;
}

void AgentAnimState::StepAnim(float deltaTimeInMillis, float rate)
{
	float realRate = rate > 0.0f ? rate : m_rate;
	float deltaTimeInSeconds = deltaTimeInMillis / 1000;
	m_pAnim->AddTime(deltaTimeInSeconds * realRate);
}

const std::string& AgentAnimState::GetName() const
{
	return m_stateName;
}