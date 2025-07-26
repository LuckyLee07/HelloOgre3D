#include "GameObject.h"
#include "SandboxDef.h"

GameObject::GameObject() : m_pEventMgr(nullptr)
{
}

GameObject::~GameObject()
{
	SAFE_DELETE(m_pEventMgr);
}

SandboxEventDispatcherManager* GameObject::Event()
{
	if (m_pEventMgr == nullptr)
	{
		m_pEventMgr = new SandboxEventDispatcherManager();
	}
	return m_pEventMgr;
}