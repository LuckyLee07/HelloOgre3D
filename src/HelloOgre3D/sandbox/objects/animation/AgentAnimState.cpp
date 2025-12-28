#include "AgentAnimState.h"
#include "AgentAnim.h"
#include "OgreAnimationState.h"

// Óë SOLDIER_STATE¶ÔÓ¦
char* AgentAnims[] =
{
	"dead",
	"fire",
	"idle_aim",
	"run_forward",
	"run_backward",
	"dead_headshot",
	"fall_dead",
	"fall_idle",
	"jump_land",
	"jump_up",
	"melee",
	"reload",
	"smg_transform",
	"sniper_transform",

	"crouch_dead",
	"crouch_fire",
	"crouch_idle_aim",
	"crouch_forward",
};
std::string AgentAnimState::GetNameByID(int stateId)
{
	int numStates = sizeof(AgentAnims)/sizeof(AgentAnims[0]);
	if (stateId >= 0 && stateId < numStates)
	{
		return AgentAnims[stateId];
	}
	return "";
}

int AgentAnimState::GetIDByName(const std::string& stateName)
{
	int numStates = sizeof(AgentAnims)/sizeof(AgentAnims[0]);
	for (int index = 0; index < numStates; index++)
	{
		if (std::strcmp(AgentAnims[index], stateName.c_str()) == 0)
			return index;
	}
	return -1;
}

AgentAnimState::AgentAnimState(const std::string& name, AgentAnim* animation, bool looping, float rate)
	: m_stateName(name), m_pAnim(animation), m_looping(looping), m_rate(rate)
{
	m_pAnim->SetEnabled(false);
	m_pAnim->SetLooping(m_looping);

	m_stateId = GetIDByName(name);
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