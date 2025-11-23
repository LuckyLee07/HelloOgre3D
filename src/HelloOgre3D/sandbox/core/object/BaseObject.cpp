#include "BaseObject.h"
#include "GameObject.h"
#include "SandboxMacros.h"

BaseObject::BaseObject()
	: m_objId(0), m_liveTicks(0), m_needClearTicks(-1)
{
	m_objType = OBJ_TYPE_NONE;
	m_pGameObjet = new GameObject();
	m_pGameObjet->setUserData(this);
}

BaseObject::~BaseObject()
{
	SAFE_DELETE(m_pGameObjet);
}

unsigned int BaseObject::GetObjId()
{
	return m_objId;
}

BaseObject::ObjectType BaseObject::GetObjType()
{
	return m_objType;
}

void BaseObject::SetObjId(unsigned int objId)
{
	m_objId = objId;
}

void BaseObject::SetObjType(ObjectType objType)
{
	m_objType = objType;
}

void BaseObject::Update(int deltaMs)
{
	(void)deltaMs;
	//if (m_liveTick > 0) m_liveTick--;
}

void BaseObject::OnDestroy()
{

}

bool BaseObject::CheckNeedClear()
{
	if (m_needClearTicks > 0)
		m_needClearTicks--;

	if (m_needClearTicks == 0)
	{
		m_needClearTicks = -1;
		return true;
	}
	return false;
}

// 在特殊场景下强制重置销毁状态，确保对象能被立即回收，而无需等待之前设置的 liveTick
void BaseObject::SetNeedClear(int delay_ticks, bool force)
{
	if (!force && m_needClearTicks >= 0) return;
	
	m_needClearTicks = delay_ticks;
}

void BaseObject::CollideWithObject(BaseObject* pCollideObj, const Collision& collision)
{
	// CollideWithObject
}

bool BaseObject::AddComponent(const std::string& key, IComponent* comp)
{
	return m_pGameObjet->addComponent(key, comp);
}

bool BaseObject::RemoveComponent(IComponent* comp)
{
	return m_pGameObjet->removeComponent(comp);
}

IComponent* BaseObject::GetComponent(const std::string& key)
{
	return m_pGameObjet->getComponent(key);
}