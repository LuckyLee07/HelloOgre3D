#include "Blackboard.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "objects/AgentObject.h"
#include "objects/SoldierObject.h"

namespace
{
	const char* EntryValueTypeToString(Blackboard::EntryValueType type)
	{
		switch (type)
		{
		case Blackboard::ENTRY_VALUE_AGENT_ID:
			return "agent-id";
		case Blackboard::ENTRY_VALUE_FLOAT:
			return "float";
		case Blackboard::ENTRY_VALUE_INT:
			return "int";
		case Blackboard::ENTRY_VALUE_BOOL:
			return "bool";
		case Blackboard::ENTRY_VALUE_STRING:
			return "string";
		case Blackboard::ENTRY_VALUE_VEC3:
			return "vec3";
		case Blackboard::ENTRY_VALUE_NONE:
		default:
			return "none";
		}
	}

	const char* EntryDecayPolicyToString(Blackboard::EntryDecayPolicy policy)
	{
		switch (policy)
		{
		case Blackboard::ENTRY_DECAY_LINEAR:
			return "linear";
		case Blackboard::ENTRY_DECAY_EXPONENTIAL:
			return "exponential";
		case Blackboard::ENTRY_DECAY_NONE:
		default:
			return "none";
		}
	}

	float ClampConfidence(float value)
	{
		if (value < 0.0f)
			return 0.0f;
		if (value > 1.0f)
			return 1.0f;
		return value;
	}

	std::string FormatEntryValue(const Blackboard::EntryValue& value)
	{
		std::ostringstream stream;
		switch (value.type)
		{
		case Blackboard::ENTRY_VALUE_AGENT_ID:
			stream << value.objectId;
			break;
		case Blackboard::ENTRY_VALUE_FLOAT:
			stream << std::fixed << std::setprecision(2) << value.floatValue;
			break;
		case Blackboard::ENTRY_VALUE_INT:
			stream << value.intValue;
			break;
		case Blackboard::ENTRY_VALUE_BOOL:
			stream << (value.boolValue ? "true" : "false");
			break;
		case Blackboard::ENTRY_VALUE_STRING:
			stream << value.stringValue;
			break;
		case Blackboard::ENTRY_VALUE_VEC3:
			stream << std::fixed << std::setprecision(2)
				<< value.vec3Value.x << "," << value.vec3Value.y << "," << value.vec3Value.z;
			break;
		case Blackboard::ENTRY_VALUE_NONE:
		default:
			stream << "-";
			break;
		}
		return stream.str();
	}
}

Blackboard::EntryValue::EntryValue()
	: type(ENTRY_VALUE_NONE)
	, objectId(-1)
	, floatValue(0.0f)
	, intValue(0)
	, boolValue(false)
	, vec3Value(Ogre::Vector3::ZERO)
{
}

Blackboard::Entry::Entry()
	: confidence(1.0f)
	, timestampMs(0)
	, ttlMs(-1)
{
}

Blackboard::EntryDecay::EntryDecay()
	: policy(ENTRY_DECAY_NONE)
	, rate(0.0f)
{
}

Blackboard::EntryDecay::EntryDecay(EntryDecayPolicy decayPolicy, float decayRate)
	: policy(decayPolicy)
	, rate(decayRate)
{
}

Blackboard::Blackboard() : m_owner(nullptr)
{
}

Blackboard::Blackboard(SoldierObject* owner) : m_owner(owner)
{
}

Blackboard::Blackboard(AgentObject* owner) : m_owner(owner)
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

void Blackboard::SetOwner(AgentObject* owner)
{
	m_owner = owner;
}

SoldierObject* Blackboard::GetOwner() const
{
	return dynamic_cast<SoldierObject*>(m_owner);
}

void Blackboard::SetAgent(const std::string& key, AgentObject* value)
{
	RemoveEntryValue(key);
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
	RemoveEntryValue(key);
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
	RemoveEntryValue(key);
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
	RemoveEntryValue(key);
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
	RemoveEntryValue(key);
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
	RemoveEntryValue(key);
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
	RemoveEntryValue(key);
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
	RemoveEntryValue(key);
	m_intArrays.erase(key);
}

void Blackboard::AddIntToArray(const std::string& key, int value)
{
	RemoveEntryValue(key);
	m_intArrays[key].push_back(value);
}

void Blackboard::SetIntArrayValue(const std::string& key, int luaIndex, int value)
{
	if (luaIndex <= 0) return;
	RemoveEntryValue(key);
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
	RemoveEntryValue(key);
	m_floatArrays.erase(key);
}

void Blackboard::AddFloatToArray(const std::string& key, float value)
{
	RemoveEntryValue(key);
	m_floatArrays[key].push_back(value);
}

void Blackboard::SetFloatArrayValue(const std::string& key, int luaIndex, float value)
{
	if (luaIndex <= 0) return;
	RemoveEntryValue(key);
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
	RemoveEntryValue(key);
	m_stringArrays.erase(key);
}

void Blackboard::AddStringToArray(const std::string& key, const std::string& value)
{
	RemoveEntryValue(key);
	m_stringArrays[key].push_back(value);
}

void Blackboard::SetStringArrayValue(const std::string& key, int luaIndex, const std::string& value)
{
	if (luaIndex <= 0) return;
	RemoveEntryValue(key);
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
	RemoveEntryValue(key);
	m_objectIdArrays.erase(key);
}

void Blackboard::AddObjectIdToArray(const std::string& key, int value)
{
	RemoveEntryValue(key);
	m_objectIdArrays[key].push_back(value);
}

void Blackboard::SetObjectIdArrayValue(const std::string& key, int luaIndex, int value)
{
	if (luaIndex <= 0) return;
	RemoveEntryValue(key);
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
		|| m_objectIdArrays.count(key) > 0
		|| m_entries.count(key) > 0;
}

void Blackboard::Remove(const std::string& key)
{
	RemoveTypedValue(key);
	RemoveEntryValue(key);
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
	m_entries.clear();
	m_entryDecays.clear();
}

void Blackboard::SetEntry(const std::string& key, const Entry& entry)
{
	RemoveTypedValue(key);
	Entry storedEntry = entry;
	storedEntry.confidence = ClampConfidence(storedEntry.confidence);
	m_entries[key] = storedEntry;
}

bool Blackboard::GetEntry(const std::string& key, Entry& outEntry) const
{
	auto iter = m_entries.find(key);
	if (iter == m_entries.end()) return false;
	outEntry = iter->second;
	return true;
}

bool Blackboard::HasEntry(const std::string& key) const
{
	return m_entries.count(key) > 0;
}

int Blackboard::GetEntryCount() const
{
	return static_cast<int>(m_entries.size());
}

void Blackboard::SetObjectIdEntry(const std::string& key, int objectId, float confidence, long long timestampMs, int ttlMs, const std::string& source)
{
	Entry entry;
	entry.value.type = ENTRY_VALUE_AGENT_ID;
	entry.value.objectId = objectId;
	entry.confidence = confidence;
	entry.timestampMs = timestampMs;
	entry.ttlMs = ttlMs;
	entry.source = source;
	SetEntry(key, entry);
}

void Blackboard::SetFloatEntry(const std::string& key, float value, float confidence, long long timestampMs, int ttlMs, const std::string& source)
{
	Entry entry;
	entry.value.type = ENTRY_VALUE_FLOAT;
	entry.value.floatValue = value;
	entry.confidence = confidence;
	entry.timestampMs = timestampMs;
	entry.ttlMs = ttlMs;
	entry.source = source;
	SetEntry(key, entry);
}

void Blackboard::SetIntEntry(const std::string& key, int value, float confidence, long long timestampMs, int ttlMs, const std::string& source)
{
	Entry entry;
	entry.value.type = ENTRY_VALUE_INT;
	entry.value.intValue = value;
	entry.confidence = confidence;
	entry.timestampMs = timestampMs;
	entry.ttlMs = ttlMs;
	entry.source = source;
	SetEntry(key, entry);
}

void Blackboard::SetBoolEntry(const std::string& key, bool value, float confidence, long long timestampMs, int ttlMs, const std::string& source)
{
	Entry entry;
	entry.value.type = ENTRY_VALUE_BOOL;
	entry.value.boolValue = value;
	entry.confidence = confidence;
	entry.timestampMs = timestampMs;
	entry.ttlMs = ttlMs;
	entry.source = source;
	SetEntry(key, entry);
}

void Blackboard::SetStringEntry(const std::string& key, const std::string& value, float confidence, long long timestampMs, int ttlMs, const std::string& source)
{
	Entry entry;
	entry.value.type = ENTRY_VALUE_STRING;
	entry.value.stringValue = value;
	entry.confidence = confidence;
	entry.timestampMs = timestampMs;
	entry.ttlMs = ttlMs;
	entry.source = source;
	SetEntry(key, entry);
}

void Blackboard::SetVec3Entry(const std::string& key, const Ogre::Vector3& value, float confidence, long long timestampMs, int ttlMs, const std::string& source)
{
	Entry entry;
	entry.value.type = ENTRY_VALUE_VEC3;
	entry.value.vec3Value = value;
	entry.confidence = confidence;
	entry.timestampMs = timestampMs;
	entry.ttlMs = ttlMs;
	entry.source = source;
	SetEntry(key, entry);
}

bool Blackboard::GetObjectIdEntry(const std::string& key, int& outObjectId, Entry* outEntry) const
{
	Entry entry;
	if (!GetEntry(key, entry) || entry.value.type != ENTRY_VALUE_AGENT_ID) return false;
	outObjectId = entry.value.objectId;
	if (outEntry != nullptr) *outEntry = entry;
	return true;
}

bool Blackboard::GetFloatEntry(const std::string& key, float& outValue, Entry* outEntry) const
{
	Entry entry;
	if (!GetEntry(key, entry) || entry.value.type != ENTRY_VALUE_FLOAT) return false;
	outValue = entry.value.floatValue;
	if (outEntry != nullptr) *outEntry = entry;
	return true;
}

bool Blackboard::GetIntEntry(const std::string& key, int& outValue, Entry* outEntry) const
{
	Entry entry;
	if (!GetEntry(key, entry) || entry.value.type != ENTRY_VALUE_INT) return false;
	outValue = entry.value.intValue;
	if (outEntry != nullptr) *outEntry = entry;
	return true;
}

bool Blackboard::GetBoolEntry(const std::string& key, bool& outValue, Entry* outEntry) const
{
	Entry entry;
	if (!GetEntry(key, entry) || entry.value.type != ENTRY_VALUE_BOOL) return false;
	outValue = entry.value.boolValue;
	if (outEntry != nullptr) *outEntry = entry;
	return true;
}

bool Blackboard::GetStringEntry(const std::string& key, std::string& outValue, Entry* outEntry) const
{
	Entry entry;
	if (!GetEntry(key, entry) || entry.value.type != ENTRY_VALUE_STRING) return false;
	outValue = entry.value.stringValue;
	if (outEntry != nullptr) *outEntry = entry;
	return true;
}

bool Blackboard::GetVec3Entry(const std::string& key, Ogre::Vector3& outValue, Entry* outEntry) const
{
	Entry entry;
	if (!GetEntry(key, entry) || entry.value.type != ENTRY_VALUE_VEC3) return false;
	outValue = entry.value.vec3Value;
	if (outEntry != nullptr) *outEntry = entry;
	return true;
}

bool Blackboard::IsEntryExpired(const Entry& entry, long long currentTimeMs) const
{
	if (currentTimeMs < 0 || entry.ttlMs < 0)
		return false;
	return currentTimeMs > entry.timestampMs + entry.ttlMs;
}

bool Blackboard::IsEntryExpired(const std::string& key, long long currentTimeMs) const
{
	Entry entry;
	if (!GetEntry(key, entry))
		return false;
	return IsEntryExpired(entry, currentTimeMs);
}

int Blackboard::PruneExpiredEntries(long long currentTimeMs)
{
	int removed = 0;
	for (std::unordered_map<std::string, Entry>::iterator iter = m_entries.begin(); iter != m_entries.end();)
	{
		if (IsEntryExpired(iter->second, currentTimeMs))
		{
			m_entryDecays.erase(iter->first);
			iter = m_entries.erase(iter);
			++removed;
		}
		else
		{
			++iter;
		}
	}
	return removed;
}

void Blackboard::SetEntryDecayPolicy(const std::string& key, EntryDecayPolicy policy, float rate)
{
	if (policy == ENTRY_DECAY_NONE || rate <= 0.0f)
	{
		m_entryDecays.erase(key);
		return;
	}
	m_entryDecays[key] = EntryDecay(policy, rate);
}

void Blackboard::ClearEntryDecayPolicy(const std::string& key)
{
	m_entryDecays.erase(key);
}

bool Blackboard::GetEntryDecayPolicy(const std::string& key, EntryDecay& outDecay) const
{
	std::unordered_map<std::string, EntryDecay>::const_iterator iter = m_entryDecays.find(key);
	if (iter == m_entryDecays.end()) return false;
	outDecay = iter->second;
	return true;
}

int Blackboard::UpdateEntries(long long currentTimeMs, int deltaMs)
{
	const int elapsedMs = std::max(0, deltaMs);
	int removed = 0;
	for (std::unordered_map<std::string, Entry>::iterator iter = m_entries.begin(); iter != m_entries.end();)
	{
		const std::string key = iter->first;
		Entry& entry = iter->second;
		if (IsEntryExpired(entry, currentTimeMs))
		{
			m_entryDecays.erase(key);
			iter = m_entries.erase(iter);
			++removed;
			continue;
		}

		std::unordered_map<std::string, EntryDecay>::const_iterator decayIter = m_entryDecays.find(key);
		if (elapsedMs > 0 && decayIter != m_entryDecays.end())
		{
			const EntryDecay& decay = decayIter->second;
			if (decay.policy == ENTRY_DECAY_LINEAR)
			{
				entry.confidence = ClampConfidence(entry.confidence - decay.rate * static_cast<float>(elapsedMs));
			}
			else if (decay.policy == ENTRY_DECAY_EXPONENTIAL)
			{
				entry.confidence = ClampConfidence(entry.confidence * static_cast<float>(std::pow(decay.rate, static_cast<float>(elapsedMs) / 1000.0f)));
			}
		}

		if (entry.confidence <= 0.0f)
		{
			m_entryDecays.erase(key);
			iter = m_entries.erase(iter);
			++removed;
			continue;
		}

		++iter;
	}
	return removed;
}

std::string Blackboard::BuildEntryDebugString(const std::string& key, long long currentTimeMs) const
{
	Entry entry;
	if (!GetEntry(key, entry))
		return key + "=<missing>";

	std::ostringstream stream;
	stream << key << "=" << FormatEntryValue(entry.value)
		<< " type=" << EntryValueTypeToString(entry.value.type)
		<< " conf=" << std::fixed << std::setprecision(2) << entry.confidence
		<< " t=" << entry.timestampMs;
	if (entry.ttlMs >= 0)
		stream << " ttl=" << entry.ttlMs;
	EntryDecay decay;
	if (GetEntryDecayPolicy(key, decay))
	{
		stream << " decay=" << EntryDecayPolicyToString(decay.policy)
			<< " rate=" << std::fixed << std::setprecision(4) << decay.rate;
	}
	if (!entry.source.empty())
		stream << " src=" << entry.source;
	if (IsEntryExpired(entry, currentTimeMs))
		stream << " expired";
	return stream.str();
}

std::string Blackboard::BuildDebugSummary(int maxEntries, long long currentTimeMs) const
{
	if (maxEntries < 0)
		maxEntries = 0;
	std::vector<std::string> keys;
	keys.reserve(m_entries.size());
	for (std::unordered_map<std::string, Entry>::const_iterator iter = m_entries.begin(); iter != m_entries.end(); ++iter)
	{
		keys.push_back(iter->first);
	}
	std::sort(keys.begin(), keys.end());

	const int showing = std::min(maxEntries, static_cast<int>(keys.size()));
	std::ostringstream stream;
	stream << "[Blackboard] metadata entries=" << m_entries.size() << " showing=" << showing;
	for (int i = 0; i < showing; ++i)
	{
		stream << "\n  " << BuildEntryDebugString(keys[i], currentTimeMs);
	}
	return stream.str();
}

void Blackboard::RemoveTypedValue(const std::string& key)
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

void Blackboard::RemoveEntryValue(const std::string& key)
{
	m_entries.erase(key);
	m_entryDecays.erase(key);
}
