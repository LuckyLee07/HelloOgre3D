#ifndef __OBJECT_REGISTRY_H__
#define __OBJECT_REGISTRY_H__

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <vector>
#include "objects/AgentObject.h"
#include "objects/BlockObject.h"

class ObjectRegistry
{
public:
	typedef std::unordered_map<int, BaseObject*> ObjectMap;

	ObjectRegistry()
		: m_nextObjectId(0)
	{
	}

	unsigned int AllocateObjectId()
	{
		return ++m_nextObjectId;
	}

	void RegisterObject(unsigned int objectId, BaseObject* object)
	{
		if (object == nullptr)
			return;
		m_objects[objectId] = object;
		RegisterTypeIndex(object);
	}

	bool EraseObjectById(int objectId, BaseObject** outObject = nullptr)
	{
		ObjectMap::iterator iter = m_objects.find(objectId);
		if (iter == m_objects.end())
		{
			if (outObject != nullptr)
				*outObject = nullptr;
			return false;
		}

		if (outObject != nullptr)
			*outObject = iter->second;
		m_objects.erase(iter);
		return true;
	}

	bool UnregisterTypeIndex(BaseObject* object)
	{
		if (object == nullptr)
			return false;

		const int objectType = object->GetObjType();
		const unsigned int objectId = object->GetObjId();
		if (objectType >= BaseObject::OBJ_TYPE_AGENT)
			return RemoveByObjectId(m_agents, objectId);
		if (objectType >= BaseObject::OBJ_TYPE_BLOCK)
			return RemoveByObjectId(m_blocks, objectId);
		return false;
	}

	BaseObject* FindObject(int objectId) const
	{
		ObjectMap::const_iterator iter = m_objects.find(objectId);
		return iter != m_objects.end() ? iter->second : nullptr;
	}

	ObjectMap& Objects() { return m_objects; }
	const ObjectMap& Objects() const { return m_objects; }

	std::vector<AgentObject*>& Agents() { return m_agents; }
	const std::vector<AgentObject*>& Agents() const { return m_agents; }

	std::vector<BlockObject*>& Blocks() { return m_blocks; }
	const std::vector<BlockObject*>& Blocks() const { return m_blocks; }

	void ClearAgents() { m_agents.clear(); }
	void ClearBlocks() { m_blocks.clear(); }

private:
	void RegisterTypeIndex(BaseObject* object)
	{
		const int objectType = object->GetObjType();
		if (objectType >= BaseObject::OBJ_TYPE_AGENT)
		{
			AgentObject* agent = dynamic_cast<AgentObject*>(object);
			assert(agent != nullptr);
			m_agents.push_back(agent);
		}
		else if (objectType >= BaseObject::OBJ_TYPE_BLOCK)
		{
			BlockObject* block = dynamic_cast<BlockObject*>(object);
			assert(block != nullptr);
			m_blocks.push_back(block);
		}
	}

	template <typename TObject>
	bool RemoveByObjectId(std::vector<TObject*>& objects, unsigned int objectId)
	{
		typename std::vector<TObject*>::iterator iter = std::remove_if(objects.begin(), objects.end(), [objectId](TObject* object) {
			return object != nullptr && object->GetObjId() == objectId;
		});
		if (iter == objects.end())
			return false;
		objects.erase(iter, objects.end());
		return true;
	}

	unsigned int m_nextObjectId;
	ObjectMap m_objects;
	std::vector<AgentObject*> m_agents;
	std::vector<BlockObject*> m_blocks;
};

#endif; // __OBJECT_REGISTRY_H__
