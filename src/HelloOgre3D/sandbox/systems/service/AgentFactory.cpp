#include "AgentFactory.h"

#include <algorithm>

#include "SandboxMacros.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btVector3.h"
#include "components/agent/AgentAttrib.h"
#include "components/agent/AgentLocomotion.h"
#include "components/ai/AIController.h"
#include "components/ComponentKeys.h"
#include "components/render/RenderComponent.h"
#include "objects/AgentObject.h"
#include "scripting/LuaPluginMgr.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/PhysicsFactory.h"
#include "systems/service/SceneFactory.h"

namespace
{
	template<typename T>
	T* AddAgentComponent(AgentObject* agent, const char* key, T* component)
	{
		if (agent != nullptr && agent->AddComponent(key, component))
		{
			return component;
		}

		SAFE_DELETE(component);
		return nullptr;
	}
}

AgentObject* AgentFactory::CreateAgent(ObjectManager* objectManager, AGENT_OBJ_TYPE agentType, const char* filepath)
{
	RenderComponent* renderComp = CreateAgentRender();
	btRigidBody* rigidBody = CreateAgentRigidBody();

	AgentObject* agent = new AgentObject(renderComp, rigidBody);
	agent->setAgentType(agentType);
	AttachAgentComponents(agent);

	if (filepath != nullptr)
	{
		LuaPluginMgr::BindLuaPluginByFile<AgentObject>(agent, filepath);
	}

	if (objectManager != nullptr)
	{
		objectManager->addNewObject(agent);
	}

	return agent;
}

RenderComponent* AgentFactory::CreateAgentRender()
{
	const Ogre::Real height = AgentLocomotion::DEFAULT_AGENT_HEIGHT;
	const Ogre::Real radius = AgentLocomotion::DEFAULT_AGENT_RADIUS;

	Ogre::SceneNode* capsuleNode = SceneFactory::CreateNodeCapsule(height, radius);
	RenderComponent* renderComp = new RenderComponent(capsuleNode);
	renderComp->SetMaterial("Ground2");
	return renderComp;
}

btRigidBody* AgentFactory::CreateAgentRigidBody()
{
	const Ogre::Real height = AgentLocomotion::DEFAULT_AGENT_HEIGHT;
	const Ogre::Real radius = AgentLocomotion::DEFAULT_AGENT_RADIUS;

	btRigidBody* rigidBody = PhysicsFactory::CreateRigidBodyCapsule(height, radius);
	rigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	return rigidBody;
}

void AgentFactory::AttachAgentComponents(AgentObject* agent)
{
	if (agent == nullptr)
	{
		return;
	}

	AgentAttrib* attrib = new AgentAttrib(agent->GetHealth(), std::max<Ogre::Real>(agent->GetHealth(), 1.0f), SOLDIER_STAND, -1);
	AddAgentComponent(agent, ComponentKeys::Attrib, attrib);
	AddAgentComponent(agent, ComponentKeys::AI, new AIController());
}
