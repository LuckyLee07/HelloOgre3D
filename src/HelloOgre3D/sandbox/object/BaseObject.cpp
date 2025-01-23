#include "BaseObject.h"
#include "SandboxDef.h"

BaseObject::BaseObject() : m_objId(0), 
	m_objType(OBJ_TYPE_NONE), m_eventManager(nullptr)
{
}

BaseObject::~BaseObject()
{
	SAFE_DELETE(m_eventManager);
}

SandboxEventDispatcherManager* BaseObject::Event()
{
	if (!m_eventManager)
	{
		m_eventManager = new SandboxEventDispatcherManager;
	}
	return m_eventManager;
}

unsigned int BaseObject::getObjId()
{
	return m_objId;
}

BaseObject::OBJTYPE BaseObject::getObjType()
{
	return m_objType;
}

void BaseObject::setObjId(unsigned int objId)
{
	m_objId = objId;
}

void BaseObject::setObjType(OBJTYPE objType)
{
	m_objType = objType;
}

void BaseObject::update(int deltaMsec)
{
	(void)deltaMsec;
}

btRigidBody* BaseObject::getRigidBody()
{
	return nullptr;
}
