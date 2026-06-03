#ifndef __SANDBOX_EVENT_PAYLOAD_H__
#define __SANDBOX_EVENT_PAYLOAD_H__

#include <string>
#include "SandboxContext.h"

class BaseObject;

namespace Ogre
{
	class Vector3;
}

enum class SandboxEventScope
{
	Local,
	Team,
	Global,
};

namespace SandboxEventFields
{
	const char* EventType();
	const char* Scope();
	const char* SenderId();
	const char* TargetId();
	const char* TeamId();
	const char* TimeMs();
	const char* PositionX();
	const char* PositionY();
	const char* PositionZ();
}

namespace SandboxEventTypes
{
	const char* HealthChanged();
	const char* AnimStateChanged();
	const char* AnimNotify();
	const char* EnemySighted();
	const char* BulletShot();
	const char* BulletImpact();
	const char* DeadFriendlySighted();
	const char* SupportRequested();
	const char* SupportResponded();
}

class SandboxEventPayload
{
public:
	static const char* ToString(SandboxEventScope scope);
	static SandboxContext Make(const char* eventType, SandboxEventScope scope, const BaseObject* sender, const BaseObject* target = nullptr, double timeMs = 0.0);
	static SandboxContext& SetPosition(SandboxContext& context, const Ogre::Vector3& position);
};

#endif // __SANDBOX_EVENT_PAYLOAD_H__
