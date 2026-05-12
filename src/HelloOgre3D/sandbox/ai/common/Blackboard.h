#ifndef __BLACKBOARD_H__
#define __BLACKBOARD_H__

#include <string>
#include <unordered_map>

#include "OgreVector3.h"
#include "script/LuaClassNameTraits.h"

class BaseObject;
class SoldierObject;
class AgentObject;

// Shared key-value container used by decision trees / behavior trees / FSM evaluators.
// Typed per value kind (agent pointer / float / vec3 / bool / int) to keep tolua binding
// straightforward and avoid std::any's debug-build overhead.
class Blackboard //tolua_exports
{ //tolua_exports
public:
	Blackboard();
	explicit Blackboard(SoldierObject* owner);
	~Blackboard();

	//tolua_begin
	SoldierObject* GetOwner() const { return m_owner; }

	void SetAgent(const std::string& key, AgentObject* value);
	AgentObject* GetAgent(const std::string& key) const;

	void SetFloat(const std::string& key, float value);
	float GetFloat(const std::string& key, float defaultValue = 0.0f) const;

	void SetInt(const std::string& key, int value);
	int GetInt(const std::string& key, int defaultValue = 0) const;

	void SetBool(const std::string& key, bool value);
	bool GetBool(const std::string& key, bool defaultValue = false) const;

	void SetVec3(const std::string& key, const Ogre::Vector3& value);
	Ogre::Vector3 GetVec3(const std::string& key) const;

	bool Has(const std::string& key) const;
	void Remove(const std::string& key);
	void Clear();
	//tolua_end

private:
	SoldierObject* m_owner;

	std::unordered_map<std::string, AgentObject*>   m_agents;
	std::unordered_map<std::string, float>          m_floats;
	std::unordered_map<std::string, int>            m_ints;
	std::unordered_map<std::string, bool>           m_bools;
	std::unordered_map<std::string, Ogre::Vector3>  m_vec3s;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(Blackboard);

#endif  // __BLACKBOARD_H__
