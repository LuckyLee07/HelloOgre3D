#include "MyRigidBody.h"
#include "object/BaseObject.h"

MyRigidBody::MyRigidBody(BaseObject* pObject) : m_pOwnerObj(pObject)
{

}

MyRigidBody::~MyRigidBody()
{
	m_pOwnerObj = nullptr;
}

BaseObject* MyRigidBody::getOwner()
{
	return m_pOwnerObj;
}

btRigidBody* MyRigidBody::getRigidBody()
{
	return m_pOwnerObj->getRigidBody();
}