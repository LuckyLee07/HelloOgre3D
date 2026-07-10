#include "SoldierFactory.h"

#include <algorithm>
#include <cstring>

#include "SandboxMacros.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btVector3.h"
#include "components/agent/AgentAttrib.h"
#include "components/agent/AgentLocomotion.h"
#include "components/ai/AIController.h"
#include "components/anim/AnimComponent.h"
#include "components/combat/WeaponComponent.h"
#include "components/ComponentKeys.h"
#include "components/control/PlayerController.h"
#include "components/render/RenderComponent.h"
#include "objects/SoldierObject.h"
#include "scripting/LuaPluginMgr.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/PhysicsFactory.h"

namespace
{
	struct SoldierAssemblyProfile
	{
		const char* name;
		bool attachAI;
		bool attachPlayer;
	};

	const SoldierAssemblyProfile kSoldierAssemblyProfiles[] =
	{
		{ "default", true, false },
		{ "ai_soldier", true, false },
		{ "player_soldier", false, true },
	};

	const SoldierAssemblyProfile& ResolveSoldierAssemblyProfile(const char* profileName)
	{
		if (profileName != nullptr && profileName[0] != '\0')
		{
			for (const SoldierAssemblyProfile& profile : kSoldierAssemblyProfiles)
			{
				if (std::strcmp(profile.name, profileName) == 0)
					return profile;
			}
		}
		return kSoldierAssemblyProfiles[0];
	}

	template<typename T>
	T* AddSoldierComponent(SoldierObject* soldier, const char* key, T* component)
	{
		if (soldier != nullptr && soldier->AddComponent(key, component))
		{
			return component;
		}

		SAFE_DELETE(component);
		return nullptr;
	}

	void AttachSoldierComponents(SoldierObject* soldier, const SoldierAssemblyProfile& profile)
	{
		if (soldier == nullptr)
			return;

		AgentAttrib* attrib = new AgentAttrib(soldier->GetHealth(), std::max<Ogre::Real>(soldier->GetHealth(), 1.0f), SOLDIER_STAND, -1);
		AddSoldierComponent(soldier, ComponentKeys::Attrib, attrib);
		AddSoldierComponent(soldier, ComponentKeys::Weapon, new WeaponComponent());

		if (profile.attachAI)
			AddSoldierComponent(soldier, ComponentKeys::AI, new AIController());
		else if (profile.attachPlayer)
			AddSoldierComponent(soldier, ComponentKeys::Player, new PlayerController());

		AnimComponent* anim = AddSoldierComponent(soldier, ComponentKeys::Anim, new AnimComponent());
		RenderComponent* render = soldier->FindComponent<RenderComponent>();
		if (anim != nullptr && render != nullptr)
			anim->InitBodyAnimations(render->GetEntity(), true);
	}
}

SoldierObject* SoldierFactory::CreateSoldier(ObjectManager* objectManager, const Ogre::String& meshFile, const char* filepath)
{
	return CreateSoldierWithProfile(objectManager, meshFile, "default", filepath);
}

SoldierObject* SoldierFactory::CreateSoldierWithProfile(ObjectManager* objectManager, const Ogre::String& meshFile, const char* profileName, const char* filepath)
{
	const SoldierAssemblyProfile& profile = ResolveSoldierAssemblyProfile(profileName);
	RenderComponent* renderComp = CreateSoldierRender(meshFile);
	btRigidBody* capsuleRigidBody = CreateSoldierRigidBody();

	SoldierObject* soldier = new SoldierObject(renderComp, capsuleRigidBody);
	AttachSoldierComponents(soldier, profile);

	if (filepath != nullptr)
	{
		LuaPluginMgr::BindLuaPluginByFile<SoldierObject>(soldier, filepath);
	}

	if (objectManager != nullptr)
	{
		objectManager->addNewObject(soldier);
	}

	return soldier;
}

RenderComponent* SoldierFactory::CreateSoldierRender(const Ogre::String& meshFile)
{
	const Ogre::Real height = AgentLocomotion::DEFAULT_AGENT_HEIGHT;

	RenderComponent* renderComp = new RenderComponent(meshFile);
	renderComp->SetVisualOffset(Ogre::Vector3(0.0f, -height * 0.5f, 0.0f));
	return renderComp;
}

btRigidBody* SoldierFactory::CreateSoldierRigidBody()
{
	const Ogre::Real height = AgentLocomotion::DEFAULT_AGENT_HEIGHT;
	const Ogre::Real radius = AgentLocomotion::DEFAULT_AGENT_RADIUS;

	btRigidBody* capsuleRigidBody = PhysicsFactory::CreateRigidBodyCapsule(height, radius);
	capsuleRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	return capsuleRigidBody;
}
