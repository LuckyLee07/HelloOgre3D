-- Chapter7BT.lua
-- 贴近原书 Chapter 7 SoldierLogic_BehaviorTree 的拓扑。

Chapter7BTConfig = {
	actionDir = "res/scripts/ai/decision/actions/",

	actions = {
		idle = "IdleAction.lua",
		move = "MoveAction.lua",
		pursue = "PursueAction.lua",
		shoot = "ShootAction.lua",
		reload = "ReloadAction.lua",
		flee = "FleeAction.lua",
		die = "DieAction.lua",
		randomMove = "RandomMoveAction.lua",
	},

	tree = {
		node = "Selector",
		name = "chapter7Root",
		children = {
			{
				node = "Sequence",
				name = "dieBranch",
				children = {
					{ node = "Condition", condition = "IsNotAlive" },
					{ node = "Action", action = "die" },
				},
			},
			{
				node = "Sequence",
				name = "fleeBranch",
				children = {
					{ node = "Condition", condition = "IsCriticalHealth" },
					{ node = "Action", action = "flee" },
				},
			},
			{
				node = "Sequence",
				name = "combatBranch",
				children = {
					{ node = "Condition", condition = "HasEnemy" },
					{
						node = "Selector",
						children = {
							{
								node = "Sequence",
								name = "reloadBranch",
								children = {
									{ node = "Condition", condition = "HasNoAmmo" },
									{ node = "Action", action = "reload" },
								},
							},
							{
								node = "Sequence",
								name = "shootBranch",
								children = {
									{ node = "Condition", condition = "CanShootEnemy" },
									{ node = "Action", action = "shoot" },
								},
							},
							{ node = "Action", action = "pursue" },
						},
					},
				},
			},
			{
				node = "Sequence",
				name = "moveBranch",
				children = {
					{ node = "Condition", condition = "HasMovePosition" },
					{ node = "Action", action = "move" },
				},
			},
			{
				node = "Sequence",
				name = "randomBranch",
				children = {
					{ node = "Condition", condition = "RandomChance" },
					{ node = "Action", action = "randomMove" },
				},
			},
			{ node = "Action", action = "idle" },
		},
	},
}

return Chapter7BTConfig
