#include "MyRigidBody.h"
#include "object/BaseObject.h"
#include "components/PhysicsComponent.h"

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
	IComponent* pComponent = m_pOwnerObj->GetComponent("physics");
	PhysicsComponent* pPhysicsComp = dynamic_cast<PhysicsComponent*>(pComponent);
	return pPhysicsComp ? pPhysicsComp->GetRigidBody() : nullptr;
}