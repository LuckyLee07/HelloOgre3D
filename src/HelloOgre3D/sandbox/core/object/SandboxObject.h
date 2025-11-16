#ifndef __SANDBOX_OBJECT__
#define __SANDBOX_OBJECT__

#include "event/SandboxEventDispatcherManager.h"

class SandboxObject
{
public:
	SandboxObject();
	virtual ~SandboxObject();
	
	SandboxEventDispatcherManager* Event();

private:
	SandboxEventDispatcherManager* m_pEventMgr;
};

#endif // __SANDBOX_OBJECT__