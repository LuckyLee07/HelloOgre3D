#include "AgentStateController.h"
#include "object/AgentObject.h"
#include "AgentStateFactory.h"
#include "ScriptLuaVM.h"
#include "AgentFSM.h"
#include "AgentLuaState.h"

AgentStateController::AgentStateController(AgentObject* soldier) : m_agent(soldier)
{
	m_fsm = new AgentFSM();
	//Init(); // 放到Lua初始化
}

AgentStateController::~AgentStateController()
{
	SAFE_DELETE(m_fsm);
}

void AgentStateController::Init()
{
	AgentStateFactory::Init();

	AddState("IdleState");
	AddState("ShootState");
	AddState("DeathState");
	AddState("MoveState");
	//AddStateExByLua("MoveState", "res/scripts/states/MoveState.lua");

	AddTransition("IdleState", "MoveState");
	AddTransition("IdleState", "ShootState");
	AddTransition("IdleState", "DeathState");

	AddTransition("MoveState", "IdleState");
	AddTransition("MoveState", "ShootState");
	AddTransition("MoveState", "DeathState");

	AddTransition("ShootState", "IdleState");
	AddTransition("ShootState", "MoveState");
	AddTransition("ShootState", "DeathState");

	m_fsm->SetCurrentState("IdleState");

	//GetScriptLuaVM()->callFunction("FSM_Controller_OnInit", "u[AgentStateController]", this);
}

void AgentStateController::Update(float dtime)
{
	if (!m_agent) return;
	
	if (m_fsm) 
		m_fsm->Update(dtime);
}

void AgentStateController::ChangeState(const std::string& stateName)
{
	if (m_fsm) 
		m_fsm->RequestState(stateName);
}

AgentState* AgentStateController::GetCurrState() const
{
	if (!m_fsm) return nullptr;
	
	return m_fsm->GetCurrState();
}

bool AgentStateController::AddState(const std::string& name)
{
	AgentState* state = AgentStateFactory::Create(name.c_str(), m_agent);
	if (state == nullptr)
	{
		CCLOG_INFO("AgentStateController: failed to create state: %s\n", name.c_str());
		return false;
	}
	m_fsm->AddState(state);
	return true;
}

bool AgentStateController::AddStateExByLua(const std::string& name, const std::string& filepath)
{
	AgentState* state = AgentStateFactory::Create("AgentLuaState", m_agent);
	if (state == nullptr)
	{
		CCLOG_INFO("AgentStateController: failed to create state: %s\n", name.c_str());
		return false;
	}

	AgentLuaState* pState = static_cast<AgentLuaState*>(state);
	pState->SetStateId(name);
	pState->BindToScript(filepath);
	
	m_fsm->AddState(pState);
	return true;
}

void AgentStateController::AddTransition(const std::string& from, const std::string& to)
{
	m_fsm->AddTransition(from, to);
}