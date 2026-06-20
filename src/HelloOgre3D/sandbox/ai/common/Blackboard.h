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
	enum EntryValueType
	{
		ENTRY_VALUE_NONE = 0,
		ENTRY_VALUE_AGENT_ID,
		ENTRY_VALUE_FLOAT,
		ENTRY_VALUE_INT,
		ENTRY_VALUE_BOOL,
		ENTRY_VALUE_STRING,
		ENTRY_VALUE_VEC3
	};

	enum EntryDecayPolicy
	{
		ENTRY_DECAY_NONE = 0,
		ENTRY_DECAY_LINEAR,
		ENTRY_DECAY_EXPONENTIAL
	};

	struct EntryValue
	{
		EntryValueType type;
		int objectId;
		float floatValue;
		int intValue;
		bool boolValue;
		std::string stringValue;
		Ogre::Vector3 vec3Value;

		EntryValue();
	};

	struct Entry
	{
		EntryValue value;
		float confidence;
		long long timestampMs;
		int ttlMs;
		std::string source;

		Entry();
	};

	struct EntryDecay
	{
		EntryDecayPolicy policy;
		float rate;

		EntryDecay();
		EntryDecay(EntryDecayPolicy decayPolicy, float decayRate);
	};

	Blackboard();
	explicit Blackboard(SoldierObject* owner);
	explicit Blackboard(AgentObject* owner);
	~Blackboard();

	void SetOwner(SoldierObject* owner);
	void SetOwner(AgentObject* owner);

	//tolua_begin
	AgentObject* GetAgentOwner() const { return m_owner; }
	SoldierObject* GetOwner() const;

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
	int GetRevision() const { return m_revision; }
	int GetKeyRevision(const std::string& key) const;
	//tolua_end

	void SetEntry(const std::string& key, const Entry& entry);
	bool GetEntry(const std::string& key, Entry& outEntry) const;
	bool HasEntry(const std::string& key) const;
	int GetEntryCount() const;

	void SetObjectIdEntry(const std::string& key, int objectId, float confidence = 1.0f, long long timestampMs = 0, int ttlMs = -1, const std::string& source = "");
	void SetFloatEntry(const std::string& key, float value, float confidence = 1.0f, long long timestampMs = 0, int ttlMs = -1, const std::string& source = "");
	void SetIntEntry(const std::string& key, int value, float confidence = 1.0f, long long timestampMs = 0, int ttlMs = -1, const std::string& source = "");
	void SetBoolEntry(const std::string& key, bool value, float confidence = 1.0f, long long timestampMs = 0, int ttlMs = -1, const std::string& source = "");
	void SetStringEntry(const std::string& key, const std::string& value, float confidence = 1.0f, long long timestampMs = 0, int ttlMs = -1, const std::string& source = "");
	void SetVec3Entry(const std::string& key, const Ogre::Vector3& value, float confidence = 1.0f, long long timestampMs = 0, int ttlMs = -1, const std::string& source = "");

	bool GetObjectIdEntry(const std::string& key, int& outObjectId, Entry* outEntry = nullptr) const;
	bool GetFloatEntry(const std::string& key, float& outValue, Entry* outEntry = nullptr) const;
	bool GetIntEntry(const std::string& key, int& outValue, Entry* outEntry = nullptr) const;
	bool GetBoolEntry(const std::string& key, bool& outValue, Entry* outEntry = nullptr) const;
	bool GetStringEntry(const std::string& key, std::string& outValue, Entry* outEntry = nullptr) const;
	bool GetVec3Entry(const std::string& key, Ogre::Vector3& outValue, Entry* outEntry = nullptr) const;

	bool IsEntryExpired(const Entry& entry, long long currentTimeMs) const;
	bool IsEntryExpired(const std::string& key, long long currentTimeMs) const;
	int PruneExpiredEntries(long long currentTimeMs);
	void SetEntryDecayPolicy(const std::string& key, EntryDecayPolicy policy, float rate);
	void ClearEntryDecayPolicy(const std::string& key);
	bool GetEntryDecayPolicy(const std::string& key, EntryDecay& outDecay) const;
	int UpdateEntries(long long currentTimeMs, int deltaMs);
	std::string BuildEntryDebugString(const std::string& key, long long currentTimeMs = -1) const;
	std::string BuildDebugSummary(int maxEntries = 8, long long currentTimeMs = -1) const;

private:
	void MarkChanged(const std::string& key);
	static bool ShouldTrackRevision(const std::string& key);
	void RemoveTypedValue(const std::string& key);
	void RemoveEntryValue(const std::string& key);

	AgentObject* m_owner;

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
	std::unordered_map<std::string, Entry>                   m_entries;
	std::unordered_map<std::string, EntryDecay>              m_entryDecays;
	std::unordered_map<std::string, int>                     m_keyRevisions;
	int m_revision;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(Blackboard);

#endif  // __BLACKBOARD_H__
