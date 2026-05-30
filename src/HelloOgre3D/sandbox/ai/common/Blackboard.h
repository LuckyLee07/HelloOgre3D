#ifndef __BLACKBOARD_H__
#define __BLACKBOARD_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "OgreVector3.h"
#include "script/LuaClassNameTraits.h"

class BaseObject;
class SoldierObject;
class AgentObject;

// Shared key-value container used by decision trees / behavior trees / FSM evaluators.
// Typed per value kind (agent pointer / object id / scalar / vec3 / array) to keep tolua binding
// straightforward and avoid std::any's debug-build overhead.
class Blackboard //tolua_exports
{ //tolua_exports
public:
	Blackboard();
	explicit Blackboard(SoldierObject* owner);
	~Blackboard();

	void SetOwner(SoldierObject* owner);

	//tolua_begin
	SoldierObject* GetOwner() const { return m_owner; }

	void SetAgent(const std::string& key, AgentObject* value);
	AgentObject* GetAgent(const std::string& key) const;

	void SetFloat(const std::string& key, float value);
	float GetFloat(const std::string& key, float defaultValue = 0.0f) const;

	void SetInt(const std::string& key, int value);
	int GetInt(const std::string& key, int defaultValue = 0) const;

	void SetObjectId(const std::string& key, int value);
	int GetObjectId(const std::string& key, int defaultValue = -1) const;

	void SetBool(const std::string& key, bool value);
	bool GetBool(const std::string& key, bool defaultValue = false) const;

	void SetString(const std::string& key, const std::string& value);
	std::string GetString(const std::string& key) const;

	void SetVec3(const std::string& key, const Ogre::Vector3& value);
	Ogre::Vector3 GetVec3(const std::string& key) const;

	void ClearIntArray(const std::string& key);
	void AddIntToArray(const std::string& key, int value);
	void SetIntArrayValue(const std::string& key, int luaIndex, int value);
	int GetIntArrayValue(const std::string& key, int luaIndex, int defaultValue = 0) const;
	int GetIntArrayCount(const std::string& key) const;
	bool ContainsIntInArray(const std::string& key, int value) const;

	void ClearFloatArray(const std::string& key);
	void AddFloatToArray(const std::string& key, float value);
	void SetFloatArrayValue(const std::string& key, int luaIndex, float value);
	float GetFloatArrayValue(const std::string& key, int luaIndex, float defaultValue = 0.0f) const;
	int GetFloatArrayCount(const std::string& key) const;

	void ClearStringArray(const std::string& key);
	void AddStringToArray(const std::string& key, const std::string& value);
	void SetStringArrayValue(const std::string& key, int luaIndex, const std::string& value);
	std::string GetStringArrayValue(const std::string& key, int luaIndex) const;
	int GetStringArrayCount(const std::string& key) const;
	bool ContainsStringInArray(const std::string& key, const std::string& value) const;

	void ClearObjectIdArray(const std::string& key);
	void AddObjectIdToArray(const std::string& key, int value);
	void SetObjectIdArrayValue(const std::string& key, int luaIndex, int value);
	int GetObjectIdArrayValue(const std::string& key, int luaIndex, int defaultValue = -1) const;
	int GetObjectIdArrayCount(const std::string& key) const;
	bool ContainsObjectIdInArray(const std::string& key, int value) const;

	bool Has(const std::string& key) const;
	void Remove(const std::string& key);
	void Clear();
	//tolua_end

private:
	SoldierObject* m_owner;

	std::unordered_map<std::string, AgentObject*>   m_agents;
	std::unordered_map<std::string, float>          m_floats;
	std::unordered_map<std::string, int>            m_ints;
	std::unordered_map<std::string, int>            m_objectIds;
	std::unordered_map<std::string, bool>           m_bools;
	std::unordered_map<std::string, std::string>    m_strings;
	std::unordered_map<std::string, Ogre::Vector3>  m_vec3s;
	std::unordered_map<std::string, std::vector<int>>         m_intArrays;
	std::unordered_map<std::string, std::vector<float>>       m_floatArrays;
	std::unordered_map<std::string, std::vector<std::string>> m_stringArrays;
	std::unordered_map<std::string, std::vector<int>>         m_objectIdArrays;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(Blackboard);

#endif  // __BLACKBOARD_H__
