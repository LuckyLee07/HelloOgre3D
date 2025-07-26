#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include "SandboxEventDispatcherManager.h"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();
	
	SandboxEventDispatcherManager* Event();

private:
	SandboxEventDispatcherManager* m_pEventMgr;
};

#endif // __GAME_OBJECT__