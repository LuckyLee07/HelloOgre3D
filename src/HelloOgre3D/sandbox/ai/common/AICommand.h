#ifndef __AI_COMMAND_H__
#define __AI_COMMAND_H__

#include "OgreVector3.h"

struct AICommand
{
	enum Kind
	{
		COMMAND_NONE = 0,
		COMMAND_IDLE,
		COMMAND_MOVE,
		COMMAND_ATTACK,
		COMMAND_RELOAD,
		COMMAND_DIE,
		COMMAND_FIRE_WEAPON,
		COMMAND_REQUEST_STATE,
		COMMAND_STOP,
		COMMAND_MOVE_TO,
		COMMAND_USE_SKILL,
		COMMAND_INTERACT,
	};

	AICommand()
		: kind(COMMAND_NONE)
		, stateId(0)
		, forceUpdate(false)
		, targetPosition(Ogre::Vector3::ZERO)
		, targetId(0)
		, skillId(0)
	{
	}

	static AICommand Idle()
	{
		AICommand command;
		command.kind = COMMAND_IDLE;
		return command;
	}

	static AICommand Move()
	{
		AICommand command;
		command.kind = COMMAND_MOVE;
		return command;
	}

	static AICommand Attack()
	{
		AICommand command;
		command.kind = COMMAND_ATTACK;
		return command;
	}

	static AICommand Reload()
	{
		AICommand command;
		command.kind = COMMAND_RELOAD;
		return command;
	}

	static AICommand Die()
	{
		AICommand command;
		command.kind = COMMAND_DIE;
		return command;
	}

	static AICommand FireWeapon()
	{
		AICommand command;
		command.kind = COMMAND_FIRE_WEAPON;
		return command;
	}

	static AICommand RequestState(int stateId, bool forceUpdate = false)
	{
		AICommand command;
		command.kind = COMMAND_REQUEST_STATE;
		command.stateId = stateId;
		command.forceUpdate = forceUpdate;
		return command;
	}

	static AICommand MoveTo(const Ogre::Vector3& targetPosition)
	{
		AICommand command;
		command.kind = COMMAND_MOVE_TO;
		command.targetPosition = targetPosition;
		return command;
	}

	static AICommand UseSkill(int skillId, unsigned int targetId = 0)
	{
		AICommand command;
		command.kind = COMMAND_USE_SKILL;
		command.skillId = skillId;
		command.targetId = targetId;
		return command;
	}

	static AICommand Stop()
	{
		AICommand command;
		command.kind = COMMAND_STOP;
		return command;
	}

	static AICommand Interact(unsigned int targetId)
	{
		AICommand command;
		command.kind = COMMAND_INTERACT;
		command.targetId = targetId;
		return command;
	}

	Kind kind;
	int stateId;
	bool forceUpdate;
	Ogre::Vector3 targetPosition;
	unsigned int targetId;
	int skillId;
};

#endif // __AI_COMMAND_H__
