#include "ObjectManager.h"
#include "objects/AgentObject.h"
#include "objects/BlockObject.h"
#include "objects/SoldierObject.h"
#include "ai/common/AIScheduler.h"
#include "ai/behavior/BehaviorTreeDriver.h"
#include "ai/decision/DecisionTreeDriver.h"
#include "ai/common/Blackboard.h"
#include "common/ScriptLuaVM.h"
#include "systems/physics/PhysicsWorld.h"
#include "ClientManager.h"
#include "systems/service/SceneFactory.h"
#include "OgreSceneNode.h"
#include "OgreSceneManager.h"
#include "components/ai/AIController.h"
#include "components/physics/PhysicsComponent.h"
#include "ai/navigation/NavigationMesh.h"
#include "event/SandboxEventDispatcherManager.h"
#include "profiling/Profile.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>

ObjectManager* g_ObjectManager = nullptr;

namespace
{
	const char* ObjectTypeToString(BaseObject::ObjectType type)
	{
		switch (type)
		{
		case BaseObject::OBJ_TYPE_BLOCK:
			return "Block";
		case BaseObject::OBJ_TYPE_PLANE:
			return "Plane";
		case BaseObject::OBJ_TYPE_BULLET:
			return "Bullet";
		case BaseObject::OBJ_TYPE_AGENT:
			return "Agent";
		case BaseObject::OBJ_TYPE_SOLDIER:
			return "Soldier";
		case BaseObject::OBJ_TYPE_NONE:
		default:
			return "None";
		}
	}

	std::string FormatVec3(const Ogre::Vector3& value)
	{
		std::ostringstream stream;
		stream << static_cast<int>(value.x) << "," << static_cast<int>(value.y) << "," << static_cast<int>(value.z);
		return stream.str();
	}

	std::string FormatReal(Ogre::Real value)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(2) << value;
		return stream.str();
	}

	std::string FormatVec3Precise(const Ogre::Vector3& value)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(2)
			<< value.x << "," << value.y << "," << value.z;
		return stream.str();
	}

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
		if (blackboard->Has("perception.targetId"))
		{
			stream << (wrote ? "," : "") << "senseTarget:" << blackboard->GetObjectId("perception.targetId", -1);
			if (blackboard->Has("perception.targetDistance"))
			{
				stream << "@" << static_cast<int>(blackboard->GetFloat("perception.targetDistance", 0.0f));
			}
			wrote = true;
		}
		else if (blackboard->Has("perception.hasTarget"))
		{
			stream << (wrote ? "," : "") << "senseTarget:none";
			wrote = true;
		}
		int memoryTargetId = -1;
		if (blackboard->GetObjectIdEntry("memory.lastKnownEnemyId", memoryTargetId))
		{
			stream << (wrote ? "," : "") << "memoryTarget:" << memoryTargetId;
			Blackboard::Entry memoryPosEntry;
			Ogre::Vector3 memoryPos = Ogre::Vector3::ZERO;
			if (blackboard->GetVec3Entry("memory.lastKnownEnemyPos", memoryPos, &memoryPosEntry))
			{
				stream << "@" << static_cast<int>(memoryPos.x) << "," << static_cast<int>(memoryPos.y) << "," << static_cast<int>(memoryPos.z);
				stream << " t=" << memoryPosEntry.timestampMs;
			}
			wrote = true;
		}
		if (blackboard->GetEntryCount() > 0)
		{
			stream << (wrote ? "," : "") << "meta:" << blackboard->GetEntryCount();
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
	m_aiScheduler = new AIScheduler();
	m_services.objects = this;
	m_services.physics = m_pPhysicsWorld;
	m_services.script = m_pScriptVM;
}

ObjectManager::~ObjectManager()
{
	m_pScriptVM = nullptr;
	m_pPhysicsWorld = nullptr;

	this->clearAllObjects(MGR_OBJ_ALLS);
	SAFE_DELETE(m_aiScheduler);

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
	const bool useAiScheduler = m_aiScheduler != nullptr && m_aiScheduler->IsEnabled();
	if (m_aiScheduler != nullptr)
		m_aiScheduler->BeginFrame(deltaMilliseconds, getAiSoldierCount());

	SandboxEventDispatcherManager::GetGlobalInst().FlushQueuedEvents();

	for (auto iter = m_objects.begin(); iter != m_objects.end();)
	{
		BaseObject* pObject = iter->second;
		if (pObject == nullptr || pObject->CheckNeedClear())
		{
			if (m_aiScheduler != nullptr && pObject != nullptr)
				m_aiScheduler->RemoveAgent(pObject->GetObjId());
			iter = m_objects.erase(iter);
			this->realRemoveObject(pObject);
		}
		else
		{
			AIController* ai = pObject->FindComponent<AIController>();
			if (ai != nullptr && useAiScheduler)
			{
				ai->SetTickInOwnerUpdateEnabled(false);
				int aiDeltaMs = 0;
				if (m_aiScheduler->ShouldTick(ai->GetAgentId(), deltaMilliseconds, &aiDeltaMs))
					ai->TickAI(aiDeltaMs);
			}
			else if (ai != nullptr)
			{
				ai->SetTickInOwnerUpdateEnabled(true);
			}

			pObject->Update(deltaMilliseconds);
			pObject->FlushQueuedEvents();
			if (pObject->GetObjType() == BaseObject::OBJ_TYPE_BLOCK)
			{
				//static_cast<BlockObject*>(pObject)->getSceneNode()->setVisible(false);
			}
			iter++;
		}
	}
	if (m_aiScheduler != nullptr)
		m_aiScheduler->PublishTracyCounters();

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

void ObjectManager::SetSandboxServices(const SandboxServices& services)
{
	m_services = services;
	if (m_services.objects == nullptr)
		m_services.objects = this;
	if (m_services.physics == nullptr)
		m_services.physics = m_pPhysicsWorld;
	if (m_services.script == nullptr)
		m_services.script = m_pScriptVM;

	for (std::unordered_map<int, BaseObject*>::iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter)
	{
		if (iter->second != nullptr)
			iter->second->SetSandboxServices(&m_services);
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

int ObjectManager::getObjectCount() const
{
	return static_cast<int>(m_objects.size());
}

int ObjectManager::getAiAgentCount() const
{
	return static_cast<int>(m_agents.size());
}

int ObjectManager::getAiSoldierCount() const
{
	int count = 0;
	for (std::unordered_map<int, BaseObject*>::const_iterator it = m_objects.begin(); it != m_objects.end(); ++it)
	{
		if (it->second != nullptr && it->second->FindComponent<AIController>() != nullptr)
			++count;
	}
	return count;
}

void ObjectManager::configureAiScheduler(bool enabled, int tickIntervalMs, int maxTicksPerFrame)
{
	if (m_aiScheduler == nullptr)
		return;
	m_aiScheduler->Configure(enabled, tickIntervalMs, maxTicksPerFrame);
}

std::string ObjectManager::buildAiSchedulerDebugSummary() const
{
	if (m_aiScheduler == nullptr)
		return "[AIScheduler] unavailable";
	return m_aiScheduler->BuildDebugSummary();
}

std::string ObjectManager::buildAiEventDebugSummary(int maxAgents, int maxEvents)
{
	if (maxAgents < 0)
		maxAgents = 0;
	if (maxAgents > 32)
		maxAgents = 32;
	if (maxEvents < 0)
		maxEvents = 0;
	if (maxEvents > 32)
		maxEvents = 32;

	const int showingCount = maxAgents < static_cast<int>(m_agents.size()) ? maxAgents : static_cast<int>(m_agents.size());
	std::ostringstream stream;
	stream << "[AIEvent] agents=" << m_agents.size()
		<< " showing=" << showingCount
		<< " maxEvents=" << maxEvents;
	for (int index = 0; index < showingCount; ++index)
	{
		AgentObject* agent = m_agents[index];
		if (agent == nullptr)
			continue;

		stream << "\n" << agent->Event()->BuildDebugSummary("agent#" + std::to_string(agent->GetObjId()), maxEvents);
	}
	return stream.str();
}

std::string ObjectManager::buildAiDebugSummary(int maxAgents)
{
	if (maxAgents < 0)
		maxAgents = 0;
	if (maxAgents > 32)
		maxAgents = 32;

	const int aiControllerCount = getAiSoldierCount();
	const int showingCount = maxAgents < static_cast<int>(m_agents.size()) ? maxAgents : static_cast<int>(m_agents.size());
	std::ostringstream stream;
	stream << "AI agents=" << m_agents.size() << " controllers=" << aiControllerCount << " showing=" << showingCount;

	for (int index = 0; index < showingCount; ++index)
	{
		AgentObject* agent = m_agents[index];
		if (agent == nullptr)
			continue;

		SoldierObject* soldier = dynamic_cast<SoldierObject*>(agent);
		AIController* ai = agent->FindComponent<AIController>();
		stream << "\n#" << index
			<< " id=" << agent->GetObjId()
			<< " team=" << agent->GetTeamId()
			<< " type=" << agent->getAgentType()
			<< " state=" << agent->GetCurStateName() << "(" << agent->GetCurStateId() << ")"
			<< " hp=" << static_cast<int>(agent->GetHealth())
			<< " pos=" << FormatVec3Precise(agent->GetPosition())
			<< " vel=" << FormatVec3Precise(agent->GetVelocity())
			<< " speed=" << FormatReal(agent->GetSpeed())
			<< " target=" << FormatVec3Precise(agent->GetTarget());

		if (ai == nullptr)
		{
			stream << " driver=Agent";
			continue;
		}

		if (soldier != nullptr)
		{
			stream << "/" << static_cast<int>(soldier->GetMaxHealth())
				<< " ammo=" << soldier->GetAmmo() << "/" << soldier->GetMaxAmmo();
		}

		BehaviorTreeDriver* behaviorDriver = ai->GetBehaviorTreeDriver();
		DecisionTreeDriver* decisionDriver = ai->GetDecisionTreeDriver();
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
			stream << " driver=" << (ai->GetFsmController() != nullptr ? "FSM" : "None");
		}
	}
	return stream.str();
}

std::string ObjectManager::buildObjectDebugSummary(int maxObjects)
{
	if (maxObjects < 0)
		maxObjects = 0;
	if (maxObjects > 64)
		maxObjects = 64;

	std::vector<int> objectIds;
	objectIds.reserve(m_objects.size());
	for (std::unordered_map<int, BaseObject*>::const_iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter)
	{
		objectIds.push_back(iter->first);
	}
	std::sort(objectIds.begin(), objectIds.end());

	const int showingCount = maxObjects < static_cast<int>(objectIds.size()) ? maxObjects : static_cast<int>(objectIds.size());
	std::ostringstream stream;
	stream << "[ObjectInspector] objects=" << m_objects.size()
		<< " agents=" << m_agents.size()
		<< " blocks=" << m_blocks.size()
		<< " navMeshes=" << m_navMeshes.size()
		<< " pendingSceneNodes=" << m_remSceneNodes.size()
		<< " showing=" << showingCount;

	for (int index = 0; index < showingCount; ++index)
	{
		std::unordered_map<int, BaseObject*>::const_iterator found = m_objects.find(objectIds[index]);
		if (found == m_objects.end() || found->second == nullptr)
			continue;

		BaseObject* object = found->second;
		stream << "\n[ObjectInspector] id=" << object->GetObjId()
			<< " type=" << ObjectTypeToString(object->GetObjType())
			<< " team=" << object->GetTeamId()
			<< " liveTicks=" << object->GetLiveTicks()
			<< " components=" << object->BuildComponentDebugString();

		if (AgentObject* agent = dynamic_cast<AgentObject*>(object))
		{
			stream << " pos=" << FormatVec3(agent->GetPosition())
				<< " vel=" << FormatVec3(agent->GetVelocity())
				<< " hp=" << static_cast<int>(agent->GetHealth())
				<< " speed=" << static_cast<int>(agent->GetSpeed())
				<< " target=" << FormatVec3(agent->GetTarget())
				<< " hasPath=" << (agent->HasPath() ? "true" : "false")
				<< " agentType=" << agent->getAgentType()
				<< " state=" << agent->GetCurStateName() << "(" << agent->GetCurStateId() << ")";
		}
		else if (BlockObject* block = dynamic_cast<BlockObject*>(object))
		{
			stream << " pos=" << FormatVec3(block->GetPosition())
				<< " mass=" << static_cast<int>(block->GetMass());
			if (block->GetObjType() != BaseObject::OBJ_TYPE_PLANE)
			{
				stream << " radius=" << static_cast<int>(block->GetRadius());
			}
		}

		if (SoldierObject* soldier = dynamic_cast<SoldierObject*>(object))
		{
			AIController* ai = soldier->GetAIController();
			const char* driverName = "None";
			if (ai != nullptr && ai->GetBehaviorTreeDriver() != nullptr)
				driverName = "BT";
			else if (ai != nullptr && ai->GetDecisionTreeDriver() != nullptr)
				driverName = "DT";
			else if (ai != nullptr && ai->GetFsmController() != nullptr)
				driverName = "FSM";

			stream << " soldierHpMax=" << static_cast<int>(soldier->GetMaxHealth())
				<< " ammo=" << soldier->GetAmmo() << "/" << soldier->GetMaxAmmo()
				<< " driver=" << driverName;
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
			if (m_aiScheduler != nullptr)
				m_aiScheduler->RemoveAgent(pAgent->GetObjId());

			SAFE_DELETE(pAgent);
		}
		m_agents.clear();
		if (m_aiScheduler != nullptr)
			m_aiScheduler->Clear();
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

void ObjectManager::addNewObject(BaseObject* pObject)
{
	if (pObject == nullptr)
		return;

	pObject->SetSandboxServices(&m_services);

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
				if (m_aiScheduler != nullptr)
					m_aiScheduler->RemoveAgent(objid);
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

