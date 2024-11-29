#include "BaseObject.h"

BaseObject::BaseObject(unsigned int objId, OBJTYPE objType) :
	m_objId(objId), m_objType(objType)
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