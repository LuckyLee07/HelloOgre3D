#include "BaseObject.h"
#include "GameObject.h"
#include "SandboxMacros.h"

BaseObject::BaseObject() : m_objId(0)
{
	m_objType = OBJ_TYPE_NONE;
	m_pGameObjet = new GameObject();
	m_pGameObjet->setUserData(this);
}

BaseObject::~BaseObject()
{
	SAFE_DELETE(m_pGameObjet);
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
	//if (m_liveTick > 0) m_liveTick--;
}

btRigidBody* BaseObject::getRigidBody() const
{
	return nullptr;
}

bool BaseObject::checkNeedClear()
{
	if (m_liveTick > 0) 
		m_liveTick--;

	return m_liveTick == 0;
}

// 在特殊场景下强制重置销毁状态，确保对象能被立即回收，而无需等待之前设置的 liveTick
void BaseObject::setNeedClear(int liveTick, bool force)
{
	if (!force && m_liveTick >= 0) return;
	
	m_liveTick = liveTick;
}

void BaseObject::onCollideWith(BaseObject*, const Collision& )
{

}