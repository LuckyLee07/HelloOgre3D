local SliceGuardBTConfig = {
	actionDir = "res/scripts/ai/decision/actions/",
	debugTrace = true,

	actions = {
		idle = "IdleAction.lua",
		pursue = "PursueAction.lua",
		shoot = "ShootAction.lua",
		reload = "ReloadAction.lua",
		die = "DieAction.lua",
	},

	tree = {
		node = "Selector",
		children = {
			{
				node = "Sequence",
				name = "WakeByTriggerThenAttack",
				children = {
					{ node = "Condition", condition = "IsAlive" },
					{
						node = "Event",
						event = "PLAYER_ENTER",
						regionId = "slice_entry",
						targetKey = "enemy",
						setBoolKey = "slice.guardAwake",
						consume = false,
					},
					{ node = "Condition", condition = "HasEnemy" },
					{
						node = "Selector",
						children = {
							{
								node = "Sequence",
								children = {
									{ node = "Condition", condition = "HasAmmo" },
									{
										node = "Selector",
										children = {
											{
												node = "Sequence",
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
							{ node = "Action", action = "reload" },
						},
					},
				},
			},
			{ node = "Action", action = "idle" },
			{ node = "Action", action = "die" },
		},
	},
}

return SliceGuardBTConfig
