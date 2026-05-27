#ifndef __AGENT_FACTORY_H__
#define __AGENT_FACTORY_H__

#include "GameDefine.h"

class AgentObject;
class ObjectManager;
class RenderComponent;
class btRigidBody;

class AgentFactory
{
public:
	static AgentObject* CreateAgent(ObjectManager* objectManager, AGENT_OBJ_TYPE agentType, const char* filepath = nullptr);

private:
	static RenderComponent* CreateAgentRender();
	static btRigidBody* CreateAgentRigidBody();
	static void AttachAgentComponents(AgentObject* agent);
};

#endif // __AGENT_FACTORY_H__
