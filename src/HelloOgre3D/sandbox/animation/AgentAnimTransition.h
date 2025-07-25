#ifndef __AGENT_ANIM_TRANSITION_H__
#define __AGENT_ANIM_TRANSITION_H__

#include <string>

class AgentAnimTransition
{
public:
	AgentAnimTransition(float blendOutWindow = 0.0f, float duration = 0.2f, float blendInWindow = 0.0f);
	~AgentAnimTransition();

	std::string getBlendCurve();

	float getDuration();
	float getBlendInWindow();
	float getBlendOutWindow();

private:
	float m_duration;

	std::string m_blendCurve;
	float m_blendInWindow;
	float m_blendOutWindow;
};

#endif  // __AGENT_ANIM_TRANSITION_H__
