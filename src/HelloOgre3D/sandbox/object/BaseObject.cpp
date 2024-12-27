#include "BaseObject.h"

BaseObject::BaseObject() 
	: m_objId(0), m_objType(OBJ_TYPE_NONE)
{
}

BaseObject::~BaseObject()
{
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