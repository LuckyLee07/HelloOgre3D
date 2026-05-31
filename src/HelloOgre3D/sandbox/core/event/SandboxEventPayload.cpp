#include "event/SandboxEventPayload.h"
#include "object/BaseObject.h"
#include "OgreVector3.h"

namespace SandboxEventFields
{
	const char* EventType() { return "eventType"; }
	const char* Scope() { return "scope"; }
	const char* SenderId() { return "senderId"; }
	const char* TargetId() { return "targetId"; }
	const char* TeamId() { return "teamId"; }
	const char* TimeMs() { return "timeMs"; }
	const char* PositionX() { return "x"; }
	const char* PositionY() { return "y"; }
	const char* PositionZ() { return "z"; }
}

namespace SandboxEventTypes
{
	const char* HealthChanged() { return "HEALTH_CHANGE"; }
	const char* AnimStateChanged() { return "ASM_STATE_CHANGE"; }
	const char* AnimNotify() { return "ASM_NOTIFY"; }
	const char* EnemySighted() { return "EnemySighted"; }
	const char* BulletShot() { return "BulletShot"; }
	const char* BulletImpact() { return "BulletImpact"; }
	const char* SupportRequested() { return "SupportRequested"; }
	const char* SupportResponded() { return "SupportResponded"; }
}

const char* SandboxEventPayload::ToString(SandboxEventScope scope)
{
	switch (scope)
	{
	case SandboxEventScope::Team:
		return "team";
	case SandboxEventScope::Global:
		return "global";
	case SandboxEventScope::Local:
	default:
		return "local";
	}
}

SandboxContext SandboxEventPayload::Make(const char* eventType, SandboxEventScope scope, const BaseObject* sender, const BaseObject* target, double timeMs)
{
	SandboxContext context;
	context.Set_String(SandboxEventFields::EventType(), eventType != nullptr ? eventType : "");
	context.Set_String(SandboxEventFields::Scope(), ToString(scope));
	context.Set_Number(SandboxEventFields::TimeMs(), timeMs);
	if (sender != nullptr)
	{
		context.Set_Number(SandboxEventFields::SenderId(), static_cast<double>(sender->GetObjId()));
		context.Set_Number(SandboxEventFields::TeamId(), static_cast<double>(sender->GetTeamId()));
	}
	if (target != nullptr)
	{
		context.Set_Number(SandboxEventFields::TargetId(), static_cast<double>(target->GetObjId()));
	}
	return context;
}

SandboxContext& SandboxEventPayload::SetPosition(SandboxContext& context, const Ogre::Vector3& position)
{
	context.Set_Number(SandboxEventFields::PositionX(), position.x);
	context.Set_Number(SandboxEventFields::PositionY(), position.y);
	context.Set_Number(SandboxEventFields::PositionZ(), position.z);
	return context;
}
