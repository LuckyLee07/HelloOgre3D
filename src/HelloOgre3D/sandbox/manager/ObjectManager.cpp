#include "ObjectManager.h"
#include "object/AgentObject.h"
#include "object/UIComponent.h"
#include "object/BlockObject.h"
#include "common/ScriptLuaVM.h"
#include "play/PhysicsWorld.h"
#include "ClientManager.h"
#include "service/SceneFactory.h"

ObjectManager* g_ObjectManager = nullptr;

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
}

ObjectManager* ObjectManager::GetInstance()
{
	return g_ObjectManager;
}

void ObjectManager::Update(int deltaMilliseconds)
{
	for (auto iter = m_objects.begin(); iter != m_objects.end();)
	{
		BaseObject* pObject = iter->second;
		if (pObject == nullptr || pObject->checkNeedClear())
		{
			iter = m_objects.erase(iter);
			this->realRemoveObject(pObject);
		}
		else
		{
			pObject->update(deltaMilliseconds);
			if (pObject->getObjType() == BaseObject::OBJ_TYPE_BLOCK)
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

void ObjectManager::clearAllObjects(int objType, bool forceAll)
{
	if ((objType & MGR_OBJ_UIOBJ) != 0)
	{
		auto iter = m_uicomps.begin();
		for (; iter != m_uicomps.end(); iter++)
		{
			auto pUIObj = *iter;
			m_objects.erase(pUIObj->getObjId());

			SAFE_DELETE(pUIObj);
		}
		m_uicomps.clear();
	}

	if ((objType & MGR_OBJ_BLOCK) != 0)
	{
		auto iter = m_blocks.begin();
		for (; iter != m_blocks.end(); iter++)
		{
			auto pBlock = *iter;
			if (!forceAll && pBlock->getObjType() != BaseObject::OBJ_TYPE_BLOCK)
				continue; //·ÀÖ¹É¾³ýPlane

			m_objects.erase(pBlock->getObjId());
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
			m_objects.erase(pAgent->getObjId());

			SAFE_DELETE(pAgent);
		}
		m_agents.clear();
	}
}

std::vector<VehicleObject*> ObjectManager::getAllVehicles()
{
	std::vector<VehicleObject*> vehicleVec(m_agents.begin(), m_agents.end());
	return vehicleVec;
}

void ObjectManager::addNewObject(BaseObject* pObject)
{
	unsigned int objectId = getNextObjId();
	pObject->setObjId(objectId);
	pObject->Initialize();
	m_objects[objectId] = pObject;

	auto rigidBody = pObject->getRigidBody();
	if (rigidBody != nullptr)
	{
		m_pPhysicsWorld->addRigidBody(rigidBody, pObject);
	}

	this->realAddObject(pObject);
}

void ObjectManager::realAddObject(BaseObject* pObject)
{
	int objtype = pObject->getObjType();
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
	else if (objtype >= BaseObject::OBJ_TYPE_UIOBJ)
	{
		auto newObject = dynamic_cast<UIComponent*>(pObject);
		assert(newObject != nullptr);
		m_uicomps.push_back(newObject);
	}
}

bool ObjectManager::realRemoveObject(BaseObject* pObject)
{
	if (pObject == nullptr) 
		return false;
	
	int objtype = pObject->getObjType();
	unsigned int objid = pObject->getObjId();
	if (objtype >= BaseObject::OBJ_TYPE_AGENT)
	{
		for (auto it = m_agents.begin(); it != m_agents.end(); it++)
		{
			if ((*it)->getObjId() == objid)
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
			if ((*it)->getObjId() == objid)
			{
				m_blocks.erase(it);
				SAFE_DELETE(pObject);
				return true;
			}
		}
	}
	else if (objtype >= BaseObject::OBJ_TYPE_UIOBJ)
	{
		for (auto it = m_uicomps.begin(); it != m_uicomps.end(); it++)
		{
			if ((*it)->getObjId() == objid)
			{
				m_uicomps.erase(it);
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

