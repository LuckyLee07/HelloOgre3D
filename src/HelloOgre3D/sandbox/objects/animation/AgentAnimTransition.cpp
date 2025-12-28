#include "AgentAnimTransition.h"

AgentAnimTransition::AgentAnimTransition(float blendOutWindow/* = 0.0f*/, float duration /*= 0.2f */ , float blendInWindow/* = 0.0f*/)
	: m_duration(duration), m_blendCurve("linear"), m_blendInWindow(blendInWindow), m_blendOutWindow(blendOutWindow)
{
		
}

AgentAnimTransition::~AgentAnimTransition()
{

}

std::string AgentAnimTransition::getBlendCurve()
{
	return m_blendCurve;
}

float AgentAnimTransition::getDuration()
{
	return m_duration;
}

float AgentAnimTransition::getBlendInWindow()
{
	return m_blendInWindow;
}

float AgentAnimTransition::getBlendOutWindow()
{
	return m_blendOutWindow;
}
