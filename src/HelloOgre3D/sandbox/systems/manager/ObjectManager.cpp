#include "ObjectManager.h"
#include "objects/AgentObject.h"
#include "objects/BlockObject.h"
#include "objects/SoldierObject.h"
#include "ai/behavior/BehaviorTreeDriver.h"
#include "ai/decision/DecisionTreeDriver.h"
#include "ai/common/Blackboard.h"
#include "common/ScriptLuaVM.h"
#include "systems/physics/PhysicsWorld.h"
#include "ClientManager.h"
#include "systems/service/SceneFactory.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "objects/components/PhysicsComponent.h"
#include "ai/navigation/NavigationMesh.h"
#include "profiling/Profile.h"

#include <sstream>

ObjectManager* g_ObjectManager = nullptr;

namespace
{
	std::string TrimDebugText(const std::string& text, size_t maxLength)
	{
		if (text.size() <= maxLength)
			return text;
		if (maxLength <= 3)
			return text.substr(0, maxLength);
		return text.substr(0, maxLength - 3) + "...";
	}

	void AppendBlackboardBrief(std::ostringstream& stream, Blackboard* blackboard)
	{
		if (blackboard == nullptr)
			return;

		bool wrote = false;
		stream << " bb={";
		if (blackboard->Has("maxHealth"))
		{
			stream << "maxHealth:" << blackboard->GetFloat("maxHealth", 0.0f);
			wrote = true;
		}
		if (blackboard->Has("movePos"))
		{
			const Ogre::Vector3 movePos = blackboard->GetVec3("movePos");
			stream << (wrote ? "," : "") << "movePos:" << static_cast<int>(movePos.x) << "," << static_cast<int>(movePos.y) << "," << static_cast<int>(movePos.z);
			wrote = true;
		}
		if (blackboard->Has("__bt.traceFrame"))
		{
			stream << (wrote ? "," : "") << "btFrame:" << blackboard->GetInt("__bt.traceFrame", 0);
			wrote = true;
		}
		if (blackboard->Has("__bt.traceEventCount"))
		{
			stream << (wrote ? "," : "") << "btEvents:" << blackboard->GetInt("__bt.traceEventCount", 0);
			wrote = true;
		}
		if (!wrote)
			stream << "-";
		stream << "}";
	}
}


ObjectManager::ObjectManager(PhysicsWorld* pPhysicsWorld)
	: m_objIndex(0), m_pPhysicsWorld(pPhysicsWorld)
{
	m_pScriptVM = GetScriptLuaVM();
}

ObjectManager::~ObjectManager()
{
	m_pScriptVM = nullptr;
	m_pPhysicsWorld = nullptr;

	this->clearAllObjects(MGR_OBJ_ALLS);

	auto iter = m_navMeshes.begin();
	for ( ; iter != m_navMeshes.end(); iter++)
	{
		SAFE_DELETE(iter->second);
	}
	m_navMeshes.clear();
}

ObjectManager* ObjectManager::GetInstance()
{
	return g_ObjectManager;
}

void ObjectManager::Update(int deltaMilliseconds)
{
	H3D_PROFILE_SCOPE("ObjectManager::Update");
	for (auto iter = m_objects.begin(); iter != m_objects.end();)
	{
		BaseObject* pObject = iter->second;
		if (pObject == nullptr || pObject->CheckNeedClear())
		{
			iter = m_objects.erase(iter);
			this->realRemoveObject(pObject);
		}
		else
		{
			pObject->Update(deltaMilliseconds);
			if (pObject->GetObjType() == BaseObject::OBJ_TYPE_BLOCK)
			{
				//static_cast<BlockObject*>(pObject)->getSceneNode()->setVisible(false);
			}
			iter++;
		}
	}

	Ogre::SceneNode* pRootScene = GetClientMgr()->getRootSceneNode();
	for (auto iter = m_remSceneNodes.begin(); iter != m_remSceneNodes.end();)
	{
		int lastMilliSeconds = iter->second;
		lastMilliSeconds -= deltaMilliseconds;
		if (lastMilliSeconds <= 0)
		{
			auto pSceneNode = iter->first;
			SceneFactory::RemParticleBySceneNode(pSceneNode);
			pRootScene->removeChild(pSceneNode);
			pRootScene->getCreator()->destroySceneNode(pSceneNode);

			iter = m_remSceneNodes.erase(iter);
		}
		else
		{
			iter->second = lastMilliSeconds;
			iter++;
		}
	}
}

void ObjectManager::HandleKeyEvent(OIS::KeyCode keycode, unsigned int key)
{
	for (auto iter = m_agents.begin(); iter != m_agents.end(); iter++)
	{
		if (AgentObject* pAgent = *iter)
		{
			pAgent->HandleKeyEvent(keycode, key);
		}
	}
}

std::vector<AgentObject*> ObjectManager::getSpecifyAgents(AGENT_OBJ_TYPE agentType)
{
	if (agentType == AGENT_OBJ_NONE)
	{
		return m_agents;
	}

	std::vector<AgentObject*> specifyAgents;

	std::vector<AgentObject*>::iterator iter;
	for (iter = m_agents.begin(); iter != m_agents.end(); iter++)
	{
		AgentObject* pAgentObject = *iter;
		if (pAgentObject->getAgentType() == agentType)
		{
			specifyAgents.push_back(pAgentObject);
		}
	}

	return specifyAgents;
}

int ObjectManager::getAiAgentCount() const
{
	return static_cast<int>(m_agents.size());
}

int ObjectManager::getAiSoldierCount() const
{
	int count = 0;
	for (std::vector<AgentObject*>::const_iterator it = m_agents.begin(); it != m_agents.end(); ++it)
	{
		if (dynamic_cast<SoldierObject*>(*it) != nullptr)
			++count;
	}
	return count;
}

std::string ObjectManager::buildAiDebugSummary(int maxAgents)
{
	if (maxAgents < 0)
		maxAgents = 0;
	if (maxAgents > 32)
		maxAgents = 32;

	const int soldierCount = getAiSoldierCount();
	const int showingCount = maxAgents < static_cast<int>(m_agents.size()) ? maxAgents : static_cast<int>(m_agents.size());
	std::ostringstream stream;
	stream << "AI agents=" << m_agents.size() << " soldiers=" << soldierCount << " showing=" << showingCount;

	for (int index = 0; index < showingCount; ++index)
	{
		AgentObject* agent = m_agents[index];
		if (agent == nullptr)
			continue;

		SoldierObject* soldier = dynamic_cast<SoldierObject*>(agent);
		stream << "\n#" << index
			<< " id=" << agent->GetObjId()
			<< " team=" << agent->GetTeamId()
			<< " type=" << agent->getAgentType()
			<< " state=" << agent->GetCurStateName() << "(" << agent->GetCurStateId() << ")"
			<< " hp=" << static_cast<int>(agent->GetHealth());

		if (soldier == nullptr)
		{
			stream << " driver=Agent";
			continue;
		}

		stream << "/" << static_cast<int>(soldier->GetMaxHealth())
			<< " ammo=" << soldier->GetAmmo() << "/" << soldier->GetMaxAmmo();

		BehaviorTreeDriver* behaviorDriver = soldier->GetBehaviorTreeDriver();
		DecisionTreeDriver* decisionDriver = soldier->GetDecisionTreeDriver();
		if (behaviorDriver != nullptr)
		{
			if (!behaviorDriver->IsDebugTraceEnabled())
				behaviorDriver->SetDebugTraceEnabled(true);
			stream << " driver=BT traceFrame=" << behaviorDriver->GetDebugTraceFrame();
			AppendBlackboardBrief(stream, behaviorDriver->GetBlackboard());
			const std::string trace = behaviorDriver->GetLastDebugTrace();
			if (!trace.empty())
				stream << " trace=" << TrimDebugText(trace, 96);
			else
				stream << " trace=pending";
		}
		else if (decisionDriver != nullptr)
		{
			stream << " driver=DT";
			AppendBlackboardBrief(stream, decisionDriver->GetBlackboard());
		}
		else
		{
			stream << " driver=" << (soldier->GetFsmController() != nullptr ? "FSM" : "None");
		}
	}
	return stream.str();
}

void ObjectManager::clearAllObjects(int objType, bool forceAll)
{
	if ((objType & MGR_OBJ_BLOCK) != 0)
	{
		auto iter = m_blocks.begin();
		for (; iter != m_blocks.end(); iter++)
		{
			auto pBlock = *iter;
			if (!forceAll && pBlock->GetObjType() != BaseObject::OBJ_TYPE_BLOCK)
				continue; //防止删除Plane

			m_objects.erase(pBlock->GetObjId());
			SAFE_DELETE(pBlock);
		}
		m_blocks.clear();
	}

	if ((objType & MGR_OBJ_AGENT) != 0)
	{
		auto iter = m_agents.begin();
		for (; iter != m_agents.end(); iter++)
		{
			auto pAgent = *iter;
			m_objects.erase(pAgent->GetObjId());

			SAFE_DELETE(pAgent);
		}
		m_agents.clear();
	}
}
std::vector<BlockObject*> ObjectManager::getFixedObjects()
{
	std::size_t fixedObjects = 0;

	auto iter = m_objects.begin();
	for (; iter != m_objects.end(); iter++)
	{
		BaseObject* pObject = iter->second;
		if (pObject && pObject->GetMass() <= 0.0f)
		{
			if (pObject->GetObjType() != BaseObject::OBJ_TYPE_PLANE)
			{
				fixedObjects++;
			}
		}
	}

	std::vector<BlockObject*> objects;
	objects.reserve(fixedObjects);

	auto iter1 = m_objects.begin();
	for (; iter1 != m_objects.end(); iter1++)
	{
		BlockObject* pObject = dynamic_cast<BlockObject*>(iter1->second);
		if (pObject && pObject->GetMass() <= 0.0f)
		{
			if (pObject->GetObjType() != BaseObject::OBJ_TYPE_PLANE)
			{
				objects.push_back(pObject);
			}
		}
	}

	return objects;
}

std::vector<VehicleObject*> ObjectManager::getAllVehicles()
{
	std::vector<VehicleObject*> vehicleVec(m_agents.begin(), m_agents.end());
	return vehicleVec;
}

void ObjectManager::addNewObject(BaseObject* pObject)
{
	unsigned int objectId = getNextObjId();
	pObject->SetObjId(objectId);
	pObject->Init();
	m_objects[objectId] = pObject;

	this->realAddObject(pObject);
}

void ObjectManager::realAddObject(BaseObject* pObject)
{
	int objtype = pObject->GetObjType();
	if (objtype >= BaseObject::OBJ_TYPE_AGENT)
	{
		auto newObject = dynamic_cast<AgentObject*>(pObject);
		assert(newObject != nullptr);
		m_agents.push_back(newObject);
	}
	else if (objtype >= BaseObject::OBJ_TYPE_BLOCK)
	{
		auto newObject = dynamic_cast<BlockObject*>(pObject);
		assert(newObject != nullptr);
		m_blocks.push_back(newObject);
	}
}

bool ObjectManager::realRemoveObject(BaseObject* pObject)
{
	if (pObject == nullptr) 
		return false;
	
	int objtype = pObject->GetObjType();
	unsigned int objid = pObject->GetObjId();
	if (objtype >= BaseObject::OBJ_TYPE_AGENT)
	{
		for (auto it = m_agents.begin(); it != m_agents.end(); it++)
		{
			if ((*it)->GetObjId() == objid)
			{
				m_agents.erase(it);
				SAFE_DELETE(pObject);
				return true;
			}
		}
	}
	
	if (objtype >= BaseObject::OBJ_TYPE_BLOCK)
	{
		for (auto it = m_blocks.begin(); it != m_blocks.end(); it++)
		{
			if ((*it)->GetObjId() == objid)
			{
				m_blocks.erase(it);
				SAFE_DELETE(pObject);
				return true;
			}
		}
	}
	
	return false;
}

bool ObjectManager::removeObjectById(int objid)
{
	auto iter = m_objects.find(objid);
	if (iter != m_objects.end())
	{
		BaseObject *pObject = iter->second;
		m_objects.erase(iter);
		bool result = realRemoveObject(pObject);
		return result;
	}
	return false;
}

BaseObject* ObjectManager::getObjectById(int objid)
{
	auto iter = m_objects.find(objid);
	if (iter != m_objects.end())
	{
		return iter->second;
	}
	return nullptr;
}

void ObjectManager::markNodeRemInSeconds(Ogre::SceneNode* pSceneNode, float seconds)
{
	if (pSceneNode == nullptr)
		return;
	m_remSceneNodes[pSceneNode] = int(seconds * 1000);
}

NavigationMesh* ObjectManager::getNavigationMesh(const Ogre::String& navName)
{
	if (m_navMeshes.find(navName) == m_navMeshes.end())
	{
		return NULL;
	}
	return m_navMeshes[navName];
}

bool ObjectManager::addNavigationMesh(const Ogre::String& navName, NavigationMesh* pNavMesh)
{
	if (m_navMeshes[navName])
	{
		SAFE_DELETE(m_navMeshes[navName]);
	}
	m_navMeshes[navName] = pNavMesh;
	return true;
}

