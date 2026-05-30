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
				name = "AliveGuard",
				reactive = true,
				children = {
					{ node = "Condition", condition = "IsAlive" },
					{
						node = "Selector",
						name = "AwakeOrIdle",
						reactive = true,
						children = {
							{
								node = "Sequence",
								name = "WakeByTriggerThenAttack",
								children = {
									{
										node = "Event",
										event = "PLAYER_ENTER",
										regionId = "slice_entry",
										targetKey = "enemy",
										targetIdKey = "slice.guard.primaryTargetId",
										setBoolKey = "slice.guardAwake",
										consume = false,
									},
									{
										node = "Condition",
										condition = "HasObjectId",
										params = { "slice.guard.primaryTargetId" },
									},
									{
										node = "Condition",
										condition = "ArrayCountAtLeast",
										params = {
											{
												blackboard = "slice.guard.recentTargetIds",
												type = "object-id-array",
												default = {},
											},
											1,
										},
									},
									{ node = "Condition", condition = "HasEnemy" },
									{
										node = "Parallel",
										name = "AttackWithSenseTick",
										successPolicy = "all",
										failurePolicy = "one",
										children = {
											{
												node = "Selector",
												reactive = true,
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
																			{
																				node = "Condition",
																				condition = "CanShootEnemy",
																				params = {
																					{
																						blackboard = "slice.guard.shootDistanceSq",
																						type = "float",
																						default = 9.0,
																					},
																				},
																			},
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
											{
												node = "ForceSuccess",
												child = { node = "Wait", waitMs = 50 },
											},
										},
									},
								},
							},
							{
								node = "Random",
								name = "IdleFallback",
								children = {
									{ node = "Action", action = "idle" },
								},
							},
						},
					},
				},
			},
			{ node = "Action", action = "die" },
		},
	},
}

return SliceGuardBTConfig
