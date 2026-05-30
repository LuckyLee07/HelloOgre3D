#include "SoldierFactory.h"

#include <algorithm>

#include "SandboxMacros.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "LinearMath/btVector3.h"
#include "components/agent/AgentAttrib.h"
#include "components/agent/AgentLocomotion.h"
#include "components/ai/AIController.h"
#include "components/anim/AnimComponent.h"
#include "components/combat/WeaponComponent.h"
#include "components/ComponentKeys.h"
#include "components/render/RenderComponent.h"
#include "objects/SoldierObject.h"
#include "scripting/LuaPluginMgr.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/PhysicsFactory.h"

namespace
{
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
}

SoldierObject* SoldierFactory::CreateSoldier(ObjectManager* objectManager, const Ogre::String& meshFile, const char* filepath)
{
	RenderComponent* renderComp = CreateSoldierRender(meshFile);
	btRigidBody* capsuleRigidBody = CreateSoldierRigidBody();

	SoldierObject* soldier = new SoldierObject(renderComp, capsuleRigidBody);
	AttachSoldierComponents(soldier);

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

void SoldierFactory::AttachSoldierComponents(SoldierObject* soldier)
{
	if (soldier == nullptr)
	{
		return;
	}

	AgentAttrib* attrib = new AgentAttrib(soldier->GetHealth(), std::max<Ogre::Real>(soldier->GetHealth(), 1.0f), SOLDIER_STAND, -1);
	AddSoldierComponent(soldier, ComponentKeys::Attrib, attrib);

	AddSoldierComponent(soldier, ComponentKeys::Weapon, new WeaponComponent());
	AddSoldierComponent(soldier, ComponentKeys::AI, new AIController());

	AnimComponent* anim = AddSoldierComponent(soldier, ComponentKeys::Anim, new AnimComponent());
	RenderComponent* render = soldier->FindComponent<RenderComponent>();
	if (anim != nullptr && render != nullptr)
	{
		anim->InitBodyAnimations(render->GetEntity(), true);
	}
}
