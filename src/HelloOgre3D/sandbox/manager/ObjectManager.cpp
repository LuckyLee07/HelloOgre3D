#include "ObjectManager.h"
#include "object/AgentObject.h"
#include "object/UIComponent.h"
#include "object/EntityObject.h"
#include "object/BlockObject.h"
#include "base/ScriptLuaVM.h"
#include "play/PhysicsWorld.h"
#include "SandboxDef.h"

ObjectManager* g_ObjectManager = nullptr;

ObjectManager::ObjectManager(PhysicsWorld* pPhysicsWorld)
	: m_objectIndex(0), m_pPhysicsWorld(pPhysicsWorld)
{
	m_pScriptVM = GetScriptLuaVM();
}

ObjectManager::~ObjectManager()
{
	m_pScriptVM = nullptr;
	m_pPhysicsWorld = nullptr;

	this->clearAllAgents();
	this->clearAllBlocks();
	this->clearAllEntitys();
	this->clearAllUIObjects();
}

ObjectManager* ObjectManager::GetInstance()
{
	return g_ObjectManager;
}

void ObjectManager::Update(int deltaMilliseconds)
{
	for (auto iter = m_entitys.begin(); iter != m_entitys.end(); iter++)
	{
		if (EntityObject* pObject = *iter)
			pObject->update(deltaMilliseconds);
	}
	for (auto iter = m_blocks.begin(); iter != m_blocks.end(); iter++)
	{
		if (BlockObject* pObject = *iter)
			pObject->update(deltaMilliseconds);
	}
	for (auto iter = m_agents.begin(); iter != m_agents.end(); iter++)
	{
		if (AgentObject* pAgent = *iter) 
			pAgent->update(deltaMilliseconds);
	}
	for (auto iter = m_uicomps.begin(); iter != m_uicomps.end(); iter++)
	{
		if (UIComponent* pComponent = *iter) 
			pComponent->update(deltaMilliseconds);
	}
}

void ObjectManager::HandleKeyEvent(OIS::KeyCode keycode, unsigned int key)
{
	for (auto iter = m_agents.begin(); iter != m_agents.end(); iter++)
	{
		if (AgentObject* pAgent = *iter)
			m_pScriptVM->callFunction("Agent_EventHandle", "u[AgentObject]i", pAgent, keycode);
	}
}

void ObjectManager::addUIObject(UIComponent* pUIObject)
{
	unsigned int objectId = getNextObjectId();

	pUIObject->setObjId(objectId);
	pUIObject->Initialize();

	m_uicomps.push_back(pUIObject);
}

void ObjectManager::addAgentObject(AgentObject* pAgentObject)
{
	unsigned int objectId = getNextObjectId();

	pAgentObject->setObjId(objectId);
	pAgentObject->Initialize();

	m_agents.push_back(pAgentObject);

	auto rigidBody = pAgentObject->getRigidBody();
	if (rigidBody != nullptr)
	{
		m_pPhysicsWorld->addRigidBody(rigidBody);
	}
}

void ObjectManager::addBlockObject(BlockObject* pBlockObject)
{
	unsigned int objectId = getNextObjectId();

	pBlockObject->setObjId(objectId);
	pBlockObject->Initialize();

	m_blocks.push_back(pBlockObject);

	auto rigidBody = pBlockObject->getRigidBody();
	if (rigidBody != nullptr)
	{
		m_pPhysicsWorld->addRigidBody(rigidBody);
	}
}

void ObjectManager::addEntityObject(EntityObject* pEntityObject)
{
	unsigned int objectId = getNextObjectId();

	pEntityObject->setObjId(objectId);
	pEntityObject->Initialize();

	m_entitys.push_back(pEntityObject);
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

void ObjectManager::clearAllAgents()
{
	auto iter = m_agents.begin();
	for (; iter != m_agents.end(); iter++)
	{
		auto pAgent = *iter;
		SAFE_DELETE(pAgent);
	}
	m_agents.clear();
}

void ObjectManager::clearAllEntitys()
{
	auto iter = m_entitys.begin();
	for (; iter != m_entitys.end(); iter++)
	{
		auto pEntity = *iter;
		SAFE_DELETE(pEntity);
	}
	m_entitys.clear();
}

void ObjectManager::clearAllBlocks(bool forceAll)
{
	auto iter = m_blocks.begin();
	for (; iter != m_blocks.end(); iter++)
	{
		auto pBlock = *iter;
		if (!forceAll && pBlock->getObjType() != BaseObject::OBJ_TYPE_BLOCK)
			continue; //·ÀÖ¹É¾³ýPlane

		SAFE_DELETE(pBlock);
	}
	m_blocks.clear();
}

void ObjectManager::clearAllUIObjects()
{
	auto iter = m_uicomps.begin();
	for (; iter != m_uicomps.end(); iter++)
	{
		auto pUIObj = *iter;
		SAFE_DELETE(pUIObj);
	}
	m_uicomps.clear();
}