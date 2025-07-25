#include "AgentAnim.h"
#include "OgreAnimationState.h"

AgentAnim::AgentAnim(Ogre::AnimationState* pAnimState) : m_pAnimState(pAnimState)
{

}

AgentAnim::~AgentAnim()
{
	m_pAnimState = nullptr;
}

void AgentAnim::Init(float startTime)
{
	m_pAnimState->setTimePosition(0.0f);
	SetEnabled(true);
	m_pAnimState->setWeight(1.0f);
	m_pAnimState->setTimePosition(startTime);
}

void AgentAnim::Clear()
{
	SetEnabled(false);
}

void AgentAnim::SetEnabled(bool enable)
{
	m_pAnimState->setEnabled(enable);
}

void AgentAnim::SetLooping(bool looping)
{
	m_pAnimState->setLoop(looping);
}

void AgentAnim::AddTime(float deltaTime)
{
	m_pAnimState->addTime(deltaTime);
}

float AgentAnim::GetTime()
{
	return m_pAnimState->getTimePosition();
}

float AgentAnim::GetLength()
{
	return m_pAnimState->getLength();
}

float AgentAnim::GetWeight()
{
	return m_pAnimState->getWeight();
}

void AgentAnim::SetWeight(float weight)
{
	m_pAnimState->setWeight(weight);
}