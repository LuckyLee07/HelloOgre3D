#include "AnimComponent.h"

#include "GameDefine.h"
#include "GameFunction.h"
#include "SandboxMacros.h"
#include "ai/fsm/AgentStateController.h"
#include "ai/fsm/states/AgentState.h"
#include "event/SandboxContext.h"
#include "objects/RenderableObject.h"
#include "objects/SoldierObject.h"
#include "objects/animation/AgentAnimStateMachine.h"
#include "objects/animation/SoldierAnimController.h"
#include "objects/animation/SoldierAnimProfile.h"
#include "profiling/Profile.h"

AnimComponent::AnimComponent(SoldierObject* owner)
	: m_owner(owner)
	, m_controller(nullptr)
	, m_asmStateChangeEventToken(0)
	, m_asmNotifyEventToken(0)
{
}

AnimComponent::~AnimComponent()
{
	SAFE_DELETE(m_controller);
}

void AnimComponent::onAttach(GameObject* owner)
{
	IComponent::onAttach(owner);
	if (m_owner == nullptr)
	{
		m_owner = dynamic_cast<SoldierObject*>(getOwner());
	}
	EnsureController();
	SubscribeAnimEvents();
}

void AnimComponent::onDetach()
{
	UnsubscribeAnimEvents();
	m_owner = nullptr;
	IComponent::onDetach();
}

void AnimComponent::update(int deltaMs)
{
	if (m_owner == nullptr || m_controller == nullptr || !m_owner->GetUseCppFSM())
	{
		return;
	}

	H3D_PROFILE_SCOPE("SoldierAnimController::Update");
	m_controller->Update((float)deltaMs);
}

SoldierAnimController* AnimComponent::GetSoldierController() const
{
	return dynamic_cast<SoldierAnimController*>(m_controller);
}

bool AnimComponent::HasNextAnim() const
{
	AgentAnimStateMachine* pAsm = GetBodyAsm();
	if (pAsm == nullptr) return false;

	return pAsm->HasNextState();
}

bool AnimComponent::IsAnimReadyForMove() const
{
	if (m_owner == nullptr) return false;

	AgentAnimStateMachine* pAsm = GetBodyAsm();
	if (pAsm == nullptr) return false;

	const std::string moveStateName = SoldierAnimProfile::GetStateNameById(ConvertAnimID(SSTATE_RUN_FORWARD, m_owner->getStanceType()));
	return pAsm->IsCurrentState(moveStateName) || pAsm->IsNextState(moveStateName);
}

bool AnimComponent::IsAnimReadyForShoot() const
{
	if (m_owner == nullptr) return false;

	AgentAnimStateMachine* pAsm = GetBodyAsm();
	if (pAsm == nullptr) return false;

	const std::string shootStateName = SoldierAnimProfile::GetStateNameById(ConvertAnimID(SSTATE_FIRE, m_owner->getStanceType()));
	return pAsm->IsCurrentState(shootStateName) || pAsm->IsNextState(shootStateName);
}

void AnimComponent::EnsureController()
{
	if (m_owner != nullptr && m_controller == nullptr)
	{
		m_controller = new SoldierAnimController(*m_owner);
	}
}

void AnimComponent::SubscribeAnimEvents()
{
	if (m_owner == nullptr)
	{
		return;
	}

	m_owner->Event()->CreateDispatcher("ASM_STATE_CHANGE");
	m_owner->Event()->CreateDispatcher("ASM_NOTIFY");
	m_asmStateChangeEventToken = m_owner->Event()->Subscribe("ASM_STATE_CHANGE", [&](const SandboxContext& context) -> void {
		if (m_owner == nullptr) return;

		int stateId = (int)context.Get_Number("StateId");
		if (!m_owner->GetUseCppFSM() && (stateId == SSTATE_FIRE || stateId == CROUCH_SSTATE_FIRE))
		{
			m_owner->ShootBullet();
		}
		if (m_controller)
		{
			m_controller->OnBodyStateChanged(stateId);
		}
		AgentStateController* fsm = m_owner->GetFsmController();
		if (!fsm) return;

		AgentState* pState = fsm->GetCurrState();
		if (!pState) return;
		pState->Event()->Emit("FSM_STATE_CHANGE", context);
	});
	m_asmNotifyEventToken = m_owner->Event()->Subscribe("ASM_NOTIFY", [&](const SandboxContext& context) -> void {
		if (m_owner == nullptr) return;

		const std::string eventName = context.Get_String("EventName");
		const int stateId = (int)context.Get_Number("StateId");
		const float normalizedTime = (float)context.Get_Number("NormalizedTime");
		if (m_owner->GetUseCppFSM() && eventName == "shoot_fire")
		{
			m_owner->ShootBullet();
		}
		if (m_controller)
		{
			m_controller->OnBodyNotify(eventName, stateId, normalizedTime);
		}
	});
}

void AnimComponent::UnsubscribeAnimEvents()
{
	if (m_owner == nullptr)
	{
		return;
	}

	m_owner->Event()->Unsubscribe("ASM_STATE_CHANGE", m_asmStateChangeEventToken);
	m_owner->Event()->Unsubscribe("ASM_NOTIFY", m_asmNotifyEventToken);
	m_asmStateChangeEventToken = 0;
	m_asmNotifyEventToken = 0;
	m_owner->Event()->RemoveDispatcher("ASM_STATE_CHANGE");
	m_owner->Event()->RemoveDispatcher("ASM_NOTIFY");
}

AgentAnimStateMachine* AnimComponent::GetBodyAsm() const
{
	RenderableObject* body = m_owner ? m_owner->getBody() : nullptr;
	return body ? body->GetObjectASM() : nullptr;
}
