#include "Blackboard.h"

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

bool Blackboard::Has(const std::string& key) const
{
	return m_agents.count(key) > 0
		|| m_floats.count(key) > 0
		|| m_ints.count(key) > 0
		|| m_bools.count(key) > 0
		|| m_strings.count(key) > 0
		|| m_vec3s.count(key) > 0;
}

void Blackboard::Remove(const std::string& key)
{
	m_agents.erase(key);
	m_floats.erase(key);
	m_ints.erase(key);
	m_bools.erase(key);
	m_strings.erase(key);
	m_vec3s.erase(key);
}

void Blackboard::Clear()
{
	m_agents.clear();
	m_floats.clear();
	m_ints.clear();
	m_bools.clear();
	m_strings.clear();
	m_vec3s.clear();
}
