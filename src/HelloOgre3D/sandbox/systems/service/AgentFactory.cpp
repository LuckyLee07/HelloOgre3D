#include "AgentFactory.h"

#include <algorithm>
#include <cstring>

#include "SandboxMacros.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btVector3.h"
#include "OgreVector3.h"
#include "components/agent/AgentAttrib.h"
#include "components/agent/AgentLocomotion.h"
#include "components/ai/AIController.h"
#include "components/anim/AnimComponent.h"
#include "components/ComponentKeys.h"
#include "components/combat/WeaponComponent.h"
#include "components/render/RenderComponent.h"
#include "objects/AgentObject.h"
#include "scripting/LuaPluginMgr.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/PhysicsFactory.h"
#include "systems/service/SceneFactory.h"

namespace
{
	struct AgentAssemblyProfile
	{
		const char* name;
		bool attachAttrib;
		bool attachWeapon;
		bool attachAI;
		bool attachAnim;
		bool initBodyAnimations;
		const char* meshFile;
		bool useAgentVisualOffset;
	};

	const AgentAssemblyProfile kAgentAssemblyProfiles[] =
	{
		{ "default", true, true, true, true, true, nullptr, false },
		{ "component_probe", true, true, true, true, true, nullptr, false },
		{ "movement_only", false, false, false, false, false, nullptr, false },
		{ "animated_probe", true, true, true, true, true, "models/futuristic_soldier/futuristic_soldier_anim.mesh", true },
		{ "crossfire_drone", true, true, true, false, false, "models/crossfire/drone.mesh", false },
		{ "crossfire_sentinel", true, true, true, false, false, "models/crossfire/sentinel.mesh", false },
	};

	const AgentAssemblyProfile& ResolveAgentAssemblyProfile(const char* profileName)
	{
		if (profileName != nullptr && profileName[0] != '\0')
		{
			for (const AgentAssemblyProfile& profile : kAgentAssemblyProfiles)
			{
				if (std::strcmp(profile.name, profileName) == 0)
				{
					return profile;
				}
			}
		}

		return kAgentAssemblyProfiles[0];
	}

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

	RenderComponent* CreateAgentMeshRender(const AgentAssemblyProfile& profile)
	{
		if (profile.meshFile == nullptr || profile.meshFile[0] == '\0')
		{
			return nullptr;
		}

		RenderComponent* renderComp = new RenderComponent(profile.meshFile);
		if (profile.useAgentVisualOffset)
		{
			const Ogre::Real height = AgentLocomotion::DEFAULT_AGENT_HEIGHT;
			renderComp->SetVisualOffset(Ogre::Vector3(0.0f, -height * 0.5f, 0.0f));
		}
		return renderComp;
	}

	void AttachAgentComponents(AgentObject* agent, const AgentAssemblyProfile& profile)
	{
		if (agent == nullptr)
		{
			return;
		}

		if (profile.attachAttrib)
		{
			AgentAttrib* attrib = new AgentAttrib(agent->GetHealth(), std::max<Ogre::Real>(agent->GetHealth(), 1.0f), SOLDIER_STAND, -1);
			AddAgentComponent(agent, ComponentKeys::Attrib, attrib);
		}

		if (profile.attachWeapon)
		{
			AddAgentComponent(agent, ComponentKeys::Weapon, new WeaponComponent());
		}

		if (profile.attachAI)
		{
			AddAgentComponent(agent, ComponentKeys::AI, new AIController());
		}

		if (profile.attachAnim)
		{
			AnimComponent* anim = AddAgentComponent(agent, ComponentKeys::Anim, new AnimComponent());
			RenderComponent* render = agent->FindComponent<RenderComponent>();
			if (profile.initBodyAnimations && anim != nullptr && render != nullptr)
			{
				anim->InitBodyAnimations(render->GetEntity(), false);
			}
		}
	}
}

AgentObject* AgentFactory::CreateAgent(ObjectManager* objectManager, AGENT_OBJ_TYPE agentType, const char* filepath)
{
	return CreateAgentWithProfile(objectManager, agentType, "default", filepath);
}

AgentObject* AgentFactory::CreateAgentWithProfile(ObjectManager* objectManager, AGENT_OBJ_TYPE agentType, const char* profileName, const char* filepath)
{
	const AgentAssemblyProfile& profile = ResolveAgentAssemblyProfile(profileName);
	RenderComponent* renderComp = profile.meshFile != nullptr ? CreateAgentMeshRender(profile) : CreateAgentRender();
	const bool drone = std::strcmp(profile.name, "crossfire_drone") == 0;
	const bool sentinel = std::strcmp(profile.name, "crossfire_sentinel") == 0;
	btRigidBody* rigidBody = drone ? PhysicsFactory::CreateRigidBodyBox(1.3f, 1.4f, 1.5f)
		: sentinel ? PhysicsFactory::CreateRigidBodyBox(1.8f, 1.6f, 1.9f) : CreateAgentRigidBody();

	AgentObject* agent = new AgentObject(renderComp, rigidBody);
	agent->setAgentType(agentType);
	AttachAgentComponents(agent, profile);
	if (drone || sentinel)
	{
		rigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
		rigidBody->setFriction(0.15f);
		agent->SetMass(sentinel ? 80.0f : 25.0f);
		AgentLocomotion* locomotion = agent->GetLocomotionComponent();
		if (locomotion != nullptr)
		{
			// Configure navigation dimensions without rebuilding the box collider.
			locomotion->SetHeight(sentinel ? 1.6f : 1.4f);
			locomotion->SetRadius(sentinel ? 0.95f : 0.75f);
		}
		if (drone && renderComp != nullptr)
			renderComp->SetVisualOffset(Ogre::Vector3(0.0f, 0.25f, 0.0f));
		AIController* ai = agent->GetAIComponent();
		if (ai != nullptr)
		{
			Blackboard* blackboard = ai->GetBlackboard();
			blackboard->SetBool("crossfire.enabled", true);
			blackboard->SetBool("crossfire.sentinel", sentinel);
			blackboard->SetBool("crossfire.shield", sentinel);
			blackboard->SetFloat("crossfire.shieldCos", 0.5f);
			blackboard->SetFloat("crossfire.damage", sentinel ? 12.0f : 10.0f);
			blackboard->SetVec3("crossfire.muzzle", sentinel ? Ogre::Vector3(0.0f, 0.2f, 1.12f) : Ogre::Vector3(0.0f, -0.02f, 0.90f));
		}
	}

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
