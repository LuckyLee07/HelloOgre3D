#include "BaseObject.h"

BaseObject::BaseObject(unsigned int objectId, OBJTYPE objType) :
	m_objectId(objectId), m_objType(objType)
{
}

BaseObject::~BaseObject()
{
}

unsigned int BaseObject::getObjId()
{
	return m_objectId;
}
BaseObject::OBJTYPE BaseObject::getObjType()
{
	return m_objType;
}