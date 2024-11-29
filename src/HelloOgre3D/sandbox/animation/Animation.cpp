#include "Animation.h"
#include "OgreAnimationState.h"

namespace Fancy
{
	Animation::Animation(Ogre::AnimationState* pAnimState) : m_pAnimState(pAnimState)
	{

	}

	Animation::~Animation()
	{
		m_pAnimState = nullptr;
	}

	void Animation::Init()
	{
		SetEnabled(true);
		m_pAnimState->setWeight(1.0f);
		m_pAnimState->setTimePosition(0.0f);
	}

	void Animation::Clear()
	{
		SetEnabled(false);
	}

	void Animation::SetEnabled(bool enable)
	{
		m_pAnimState->setEnabled(enable);
	}

	void Animation::AddTime(float deltaTime)
	{
		m_pAnimState->addTime(deltaTime);
	}

	float Animation::GetTime()
	{
		return m_pAnimState->getTimePosition();
	}

	float Animation::GetLength()
	{
		return m_pAnimState->getLength();
	}

	float Animation::GetWeight()
	{
		return m_pAnimState->getWeight();
	}
}