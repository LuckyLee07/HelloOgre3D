#include "AgentStateEvaluators.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>

#include "AgentActionContext.h"
#include "AgentStateController.h"
#include "objects/AgentObject.h"

namespace
{
	AgentActionContext* GetActions(AgentStateController& controller)
	{
		return controller.GetActionContext();
	}

	bool EvalIsNotAlive(AgentStateController& controller)
	{
		AgentObject* agent = controller.GetAgent();
		return !agent || agent->GetHealth() <= 0.0f;
	}

	bool EvalHasCriticalHealth(AgentStateController& controller)
	{
		AgentActionContext* actions = GetActions(controller);
		return actions != nullptr && actions->HasCriticalHealth(0.2f);
	}

	bool EvalHasNoAmmo(AgentStateController& controller)
	{
		AgentActionContext* actions = GetActions(controller);
		return actions != nullptr && actions->HasWeapon() && !actions->HasAmmo();
	}

	bool EvalCanShootEnemy(AgentStateController& controller)
	{
		AgentActionContext* actions = GetActions(controller);
		return actions != nullptr && actions->CanShootEnemy(3.0f);
	}

	bool EvalHasEnemy(AgentStateController& controller)
	{
		AgentActionContext* actions = GetActions(controller);
		return actions != nullptr && actions->HasEnemy();
	}

	bool EvalHasMovePosition(AgentStateController& controller)
	{
		AgentActionContext* actions = GetActions(controller);
		return actions != nullptr && actions->HasMovePosition(1.5f);
	}

	bool EvalRandom(AgentStateController&)
	{
		static bool seeded = false;
		if (!seeded)
		{
			std::srand(static_cast<unsigned int>(std::time(nullptr)));
			seeded = true;
		}

		return (std::rand() % 1000) >= 500;
	}

	bool EvalTrue(AgentStateController&)
	{
		return true;
	}
}

void AgentStateEvaluators::ConfigureSoldierTransitions(AgentStateController& controller)
{
	auto bind = [&controller](bool (*evaluator)(AgentStateController&)) {
		return [&controller, evaluator]() {
			return evaluator(controller);
		};
	};

	controller.AddTransition("IdleState", "DeathState");
	controller.AddTransition("IdleState", "FleeState");
	controller.AddTransition("IdleState", "ReloadState");
	controller.AddTransition("IdleState", "ShootState");
	controller.AddTransition("IdleState", "PursueState");
	controller.AddTransition("IdleState", "MoveState");
	controller.AddTransition("IdleState", "RandomMoveState");
	controller.AddTransition("IdleState", "IdleState");

	controller.AddTransition("MoveState", "DeathState");
	controller.AddTransition("MoveState", "FleeState");
	controller.AddTransition("MoveState", "ReloadState");
	controller.AddTransition("MoveState", "ShootState");
	controller.AddTransition("MoveState", "PursueState");
	controller.AddTransition("MoveState", "MoveState");
	controller.AddTransition("MoveState", "RandomMoveState");
	controller.AddTransition("MoveState", "IdleState");

	controller.AddTransition("RandomMoveState", "DeathState");
	controller.AddTransition("RandomMoveState", "MoveState");

	controller.AddTransition("ShootState", "DeathState");
	controller.AddTransition("ShootState", "FleeState");
	controller.AddTransition("ShootState", "ReloadState");
	controller.AddTransition("ShootState", "ShootState");
	controller.AddTransition("ShootState", "PursueState");
	controller.AddTransition("ShootState", "MoveState");
	controller.AddTransition("ShootState", "RandomMoveState");
	controller.AddTransition("ShootState", "IdleState");

	controller.AddTransition("FleeState", "DeathState");
	controller.AddTransition("FleeState", "MoveState");

	controller.AddTransition("PursueState", "DeathState");
	controller.AddTransition("PursueState", "FleeState");
	controller.AddTransition("PursueState", "ShootState");
	controller.AddTransition("PursueState", "IdleState");

	controller.AddTransition("ReloadState", "DeathState");
	controller.AddTransition("ReloadState", "ShootState");
	controller.AddTransition("ReloadState", "PursueState");
	controller.AddTransition("ReloadState", "RandomMoveState");
	controller.AddTransition("ReloadState", "IdleState");

	controller.AddTransitionByEvaluator("IdleState", "DeathState", bind(EvalIsNotAlive));
	controller.AddTransitionByEvaluator("IdleState", "FleeState", bind(EvalHasCriticalHealth));
	controller.AddTransitionByEvaluator("IdleState", "ReloadState", bind(EvalHasNoAmmo));
	controller.AddTransitionByEvaluator("IdleState", "ShootState", bind(EvalCanShootEnemy));
	controller.AddTransitionByEvaluator("IdleState", "PursueState", bind(EvalHasEnemy));
	controller.AddTransitionByEvaluator("IdleState", "RandomMoveState", bind(EvalRandom));
	controller.AddTransitionByEvaluator("IdleState", "IdleState", bind(EvalTrue));

	controller.AddTransitionByEvaluator("MoveState", "DeathState", bind(EvalIsNotAlive));
	controller.AddTransitionByEvaluator("MoveState", "FleeState", bind(EvalHasCriticalHealth));
	controller.AddTransitionByEvaluator("MoveState", "ReloadState", bind(EvalHasNoAmmo));
	controller.AddTransitionByEvaluator("MoveState", "ShootState", bind(EvalCanShootEnemy));
	controller.AddTransitionByEvaluator("MoveState", "PursueState", bind(EvalHasEnemy));
	controller.AddTransitionByEvaluator("MoveState", "MoveState", bind(EvalHasMovePosition));
	controller.AddTransitionByEvaluator("MoveState", "RandomMoveState", bind(EvalRandom));
	controller.AddTransitionByEvaluator("MoveState", "IdleState", bind(EvalTrue));

	controller.AddTransitionByEvaluator("RandomMoveState", "DeathState", bind(EvalIsNotAlive));
	controller.AddTransitionByEvaluator("RandomMoveState", "MoveState", bind(EvalTrue));

	controller.AddTransitionByEvaluator("ShootState", "DeathState", bind(EvalIsNotAlive));
	controller.AddTransitionByEvaluator("ShootState", "FleeState", bind(EvalHasCriticalHealth));
	controller.AddTransitionByEvaluator("ShootState", "ReloadState", bind(EvalHasNoAmmo));
	controller.AddTransitionByEvaluator("ShootState", "ShootState", bind(EvalCanShootEnemy));
	controller.AddTransitionByEvaluator("ShootState", "PursueState", bind(EvalHasEnemy));
	controller.AddTransitionByEvaluator("ShootState", "RandomMoveState", bind(EvalRandom));
	controller.AddTransitionByEvaluator("ShootState", "IdleState", bind(EvalTrue));

	controller.AddTransitionByEvaluator("FleeState", "DeathState", bind(EvalIsNotAlive));
	controller.AddTransitionByEvaluator("FleeState", "MoveState", bind(EvalTrue));

	controller.AddTransitionByEvaluator("PursueState", "DeathState", bind(EvalIsNotAlive));
	controller.AddTransitionByEvaluator("PursueState", "FleeState", bind(EvalHasCriticalHealth));
	controller.AddTransitionByEvaluator("PursueState", "ShootState", bind(EvalCanShootEnemy));
	controller.AddTransitionByEvaluator("PursueState", "IdleState", bind(EvalTrue));

	controller.AddTransitionByEvaluator("ReloadState", "DeathState", bind(EvalIsNotAlive));
	controller.AddTransitionByEvaluator("ReloadState", "ShootState", bind(EvalCanShootEnemy));
	controller.AddTransitionByEvaluator("ReloadState", "PursueState", bind(EvalHasEnemy));
	controller.AddTransitionByEvaluator("ReloadState", "RandomMoveState", bind(EvalRandom));
	controller.AddTransitionByEvaluator("ReloadState", "IdleState", bind(EvalTrue));
}
