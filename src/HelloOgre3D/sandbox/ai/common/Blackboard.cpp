#include "Blackboard.h"

#include <algorithm>

Blackboard::Blackboard() : m_owner(nullptr)
{
}

Blackboard::Blackboard(SoldierObject* owner) : m_owner(owner)
{
}

Blackboard::~Blackboard()
{
	Clear();
}

void Blackboard::SetOwner(SoldierObject* owner)
{
	m_owner = owner;
}

void Blackboard::SetAgent(const std::string& key, AgentObject* value)
{
	m_agents[key] = value;
}

AgentObject* Blackboard::GetAgent(const std::string& key) const
{
	auto iter = m_agents.find(key);
	if (iter == m_agents.end()) return nullptr;
	return iter->second;
}

void Blackboard::SetFloat(const std::string& key, float value)
{
	m_floats[key] = value;
}

float Blackboard::GetFloat(const std::string& key, float defaultValue) const
{
	auto iter = m_floats.find(key);
	if (iter == m_floats.end()) return defaultValue;
	return iter->second;
}

void Blackboard::SetInt(const std::string& key, int value)
{
	m_ints[key] = value;
}

int Blackboard::GetInt(const std::string& key, int defaultValue) const
{
	auto iter = m_ints.find(key);
	if (iter == m_ints.end()) return defaultValue;
	return iter->second;
}

void Blackboard::SetObjectId(const std::string& key, int value)
{
	m_objectIds[key] = value;
}

int Blackboard::GetObjectId(const std::string& key, int defaultValue) const
{
	auto iter = m_objectIds.find(key);
	if (iter == m_objectIds.end()) return defaultValue;
	return iter->second;
}

void Blackboard::SetBool(const std::string& key, bool value)
{
	m_bools[key] = value;
}

bool Blackboard::GetBool(const std::string& key, bool defaultValue) const
{
	auto iter = m_bools.find(key);
	if (iter == m_bools.end()) return defaultValue;
	return iter->second;
}

void Blackboard::SetString(const std::string& key, const std::string& value)
{
	m_strings[key] = value;
}

std::string Blackboard::GetString(const std::string& key) const
{
	auto iter = m_strings.find(key);
	if (iter == m_strings.end()) return "";
	return iter->second;
}

void Blackboard::SetVec3(const std::string& key, const Ogre::Vector3& value)
{
	m_vec3s[key] = value;
}

Ogre::Vector3 Blackboard::GetVec3(const std::string& key) const
{
	auto iter = m_vec3s.find(key);
	if (iter == m_vec3s.end()) return Ogre::Vector3::ZERO;
	return iter->second;
}

void Blackboard::ClearIntArray(const std::string& key)
{
	m_intArrays.erase(key);
}

void Blackboard::AddIntToArray(const std::string& key, int value)
{
	m_intArrays[key].push_back(value);
}

void Blackboard::SetIntArrayValue(const std::string& key, int luaIndex, int value)
{
	if (luaIndex <= 0) return;
	std::vector<int>& values = m_intArrays[key];
	if ((int)values.size() < luaIndex) values.resize(luaIndex, 0);
	values[luaIndex - 1] = value;
}

int Blackboard::GetIntArrayValue(const std::string& key, int luaIndex, int defaultValue) const
{
	auto iter = m_intArrays.find(key);
	if (iter == m_intArrays.end() || luaIndex <= 0 || luaIndex > (int)iter->second.size()) return defaultValue;
	return iter->second[luaIndex - 1];
}

int Blackboard::GetIntArrayCount(const std::string& key) const
{
	auto iter = m_intArrays.find(key);
	if (iter == m_intArrays.end()) return 0;
	return (int)iter->second.size();
}

bool Blackboard::ContainsIntInArray(const std::string& key, int value) const
{
	auto iter = m_intArrays.find(key);
	if (iter == m_intArrays.end()) return false;
	return std::find(iter->second.begin(), iter->second.end(), value) != iter->second.end();
}

void Blackboard::ClearFloatArray(const std::string& key)
{
	m_floatArrays.erase(key);
}

void Blackboard::AddFloatToArray(const std::string& key, float value)
{
	m_floatArrays[key].push_back(value);
}

void Blackboard::SetFloatArrayValue(const std::string& key, int luaIndex, float value)
{
	if (luaIndex <= 0) return;
	std::vector<float>& values = m_floatArrays[key];
	if ((int)values.size() < luaIndex) values.resize(luaIndex, 0.0f);
	values[luaIndex - 1] = value;
}

float Blackboard::GetFloatArrayValue(const std::string& key, int luaIndex, float defaultValue) const
{
	auto iter = m_floatArrays.find(key);
	if (iter == m_floatArrays.end() || luaIndex <= 0 || luaIndex > (int)iter->second.size()) return defaultValue;
	return iter->second[luaIndex - 1];
}

int Blackboard::GetFloatArrayCount(const std::string& key) const
{
	auto iter = m_floatArrays.find(key);
	if (iter == m_floatArrays.end()) return 0;
	return (int)iter->second.size();
}

void Blackboard::ClearStringArray(const std::string& key)
{
	m_stringArrays.erase(key);
}

void Blackboard::AddStringToArray(const std::string& key, const std::string& value)
{
	m_stringArrays[key].push_back(value);
}

void Blackboard::SetStringArrayValue(const std::string& key, int luaIndex, const std::string& value)
{
	if (luaIndex <= 0) return;
	std::vector<std::string>& values = m_stringArrays[key];
	if ((int)values.size() < luaIndex) values.resize(luaIndex);
	values[luaIndex - 1] = value;
}

std::string Blackboard::GetStringArrayValue(const std::string& key, int luaIndex) const
{
	auto iter = m_stringArrays.find(key);
	if (iter == m_stringArrays.end() || luaIndex <= 0 || luaIndex > (int)iter->second.size()) return "";
	return iter->second[luaIndex - 1];
}

int Blackboard::GetStringArrayCount(const std::string& key) const
{
	auto iter = m_stringArrays.find(key);
	if (iter == m_stringArrays.end()) return 0;
	return (int)iter->second.size();
}

bool Blackboard::ContainsStringInArray(const std::string& key, const std::string& value) const
{
	auto iter = m_stringArrays.find(key);
	if (iter == m_stringArrays.end()) return false;
	return std::find(iter->second.begin(), iter->second.end(), value) != iter->second.end();
}

void Blackboard::ClearObjectIdArray(const std::string& key)
{
	m_objectIdArrays.erase(key);
}

void Blackboard::AddObjectIdToArray(const std::string& key, int value)
{
	m_objectIdArrays[key].push_back(value);
}

void Blackboard::SetObjectIdArrayValue(const std::string& key, int luaIndex, int value)
{
	if (luaIndex <= 0) return;
	std::vector<int>& values = m_objectIdArrays[key];
	if ((int)values.size() < luaIndex) values.resize(luaIndex, -1);
	values[luaIndex - 1] = value;
}

int Blackboard::GetObjectIdArrayValue(const std::string& key, int luaIndex, int defaultValue) const
{
	auto iter = m_objectIdArrays.find(key);
	if (iter == m_objectIdArrays.end() || luaIndex <= 0 || luaIndex > (int)iter->second.size()) return defaultValue;
	return iter->second[luaIndex - 1];
}

int Blackboard::GetObjectIdArrayCount(const std::string& key) const
{
	auto iter = m_objectIdArrays.find(key);
	if (iter == m_objectIdArrays.end()) return 0;
	return (int)iter->second.size();
}

bool Blackboard::ContainsObjectIdInArray(const std::string& key, int value) const
{
	auto iter = m_objectIdArrays.find(key);
	if (iter == m_objectIdArrays.end()) return false;
	return std::find(iter->second.begin(), iter->second.end(), value) != iter->second.end();
}

bool Blackboard::Has(const std::string& key) const
{
	return m_agents.count(key) > 0
		|| m_floats.count(key) > 0
		|| m_ints.count(key) > 0
		|| m_objectIds.count(key) > 0
		|| m_bools.count(key) > 0
		|| m_strings.count(key) > 0
		|| m_vec3s.count(key) > 0
		|| m_intArrays.count(key) > 0
		|| m_floatArrays.count(key) > 0
		|| m_stringArrays.count(key) > 0
		|| m_objectIdArrays.count(key) > 0;
}

void Blackboard::Remove(const std::string& key)
{
	m_agents.erase(key);
	m_floats.erase(key);
	m_ints.erase(key);
	m_objectIds.erase(key);
	m_bools.erase(key);
	m_strings.erase(key);
	m_vec3s.erase(key);
	m_intArrays.erase(key);
	m_floatArrays.erase(key);
	m_stringArrays.erase(key);
	m_objectIdArrays.erase(key);
}

void Blackboard::Clear()
{
	m_agents.clear();
	m_floats.clear();
	m_ints.clear();
	m_objectIds.clear();
	m_bools.clear();
	m_strings.clear();
	m_vec3s.clear();
	m_intArrays.clear();
	m_floatArrays.clear();
	m_stringArrays.clear();
	m_objectIdArrays.clear();
}
