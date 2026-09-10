-- TeamSharingExperimentBT.lua
-- M3 controlled scene: agents only consume explicit move positions or idle.
-- Perception still runs in C++; combat and random roaming are excluded so the
-- receiver's displacement can be attributed to the team-sharing path.

TeamSharingExperimentBTConfig = {
	actionDir = "res/scripts/ai/decision/actions/",
	actions = {
		idle = "IdleAction.lua",
		move = "MoveAction.lua",
		die = "DieAction.lua",
	},
	tree = {
		node = "Selector",
		children = {
			{
				node = "Sequence",
				children = {
					{ node = "Condition", condition = "IsAlive" },
					{
						node = "Selector",
						name = "teamExperimentAction",
						reevaluateMs = 100,
						children = {
							{
								node = "Sequence",
								children = {
									{ node = "Condition", condition = "HasMovePosition" },
									{ node = "Action", action = "move" },
								},
							},
							{ node = "Action", action = "idle" },
						},
					},
				},
			},
			{ node = "Action", action = "die" },
		},
	},
}

return TeamSharingExperimentBTConfig
