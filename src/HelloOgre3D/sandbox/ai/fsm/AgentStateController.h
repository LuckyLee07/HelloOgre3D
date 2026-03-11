#ifndef __AGENT_STATE_CONTROLLER_H__
#define __AGENT_STATE_CONTROLLER_H__

#include <functional>
#include <string>
#include "OgreVector3.h"

class AgentActionContext;
class AgentState;
class AgentFSM;
class AgentObject;

class AgentStateController //tolua_exports
{ //tolua_exports
public:
	AgentStateController(AgentObject* soldier);
	~AgentStateController();

	void Init();
	void Update(float dtime);
	void ChangeState(const std::string& stateName);

	AgentState* GetCurrState() const;
	AgentActionContext* GetActionContext() const { return m_actionContext; }

	//tolua_begin
	bool AddState(const std::string& name);
	bool AddStateExByLua(const std::string& name, const std::string& filepath);

	void AddTransition(const std::string& from, const std::string& to);
	//tolua_end

	void AddTransitionByEvaluator(const std::string& from, const std::string& to, std::function<bool()> evaluator);

	AgentObject* GetAgent() const { return m_agent; }
	const std::string& GetNavMeshName() const { return m_navMeshName; }

	bool PlanPathTo(const Ogre::Vector3& target, bool updateMovePos = true);
	bool PlanPathToEnemy();
	Ogre::Vector3 RandomPoint() const;

	void ApplySteering(float deltaTimeInSeconds, bool slowMode);

private:
	AgentFSM* m_fsm;
	AgentObject* m_agent;
	AgentActionContext* m_actionContext;

	std::string m_navMeshName;

}; //tolua_exports

#endif  // __AGENT_STATE_CONTROLLER_H__