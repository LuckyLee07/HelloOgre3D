#include "SandboxObject.h"
#include "SandboxMacros.h"

SandboxObject::SandboxObject() : m_pEventMgr(nullptr)
{
}

SandboxObject::~SandboxObject()
{
	SAFE_DELETE(m_pEventMgr);
}

SandboxEventDispatcherManager* SandboxObject::Event()
{
	if (m_pEventMgr == nullptr)
	{
		m_pEventMgr = new SandboxEventDispatcherManager();
	}
	return m_pEventMgr;
}