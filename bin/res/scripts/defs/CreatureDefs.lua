local CreatureDefs = {
	schemaVersion = 1,
	triggerVolumes = {
		slice_entry = {
			id = "slice_entry",
			event = "PLAYER_ENTER?regionId=slice_entry",
			scope = "Global",
			center = { x = 0, y = 0, z = 0 },
			halfExtents = { x = 5, y = 4, z = 5 },
			targetTeamId = 0,
			drawDebug = true,
		},
	},
	defs = {
		slice_player = {
			id = "slice_player",
			displayName = "切片玩家",
			kind = "soldier",
			tags = { "creature", "slice", "player" },
			factory = {
				type = "soldier",
				script = "res/scripts/agent/IndirectSoldierAgent.lua",
				appearance = "Dark",
				weaponMesh = "models/futuristic_soldier/soldier_weapon.mesh",
			},
			teamId = 0,
			transform = {
				position = { x = 0, y = 0, z = 0 },
				rotation = { x = 0, y = 90, z = 0 },
			},
			attributes = {
				maxHealth = 120,
				health = 120,
			},
			weapon = {
				maxAmmo = 10,
				ammo = 10,
			},
			ai = {
				driver = "fsm",
				blackboard = {
					role = "player",
				},
			},
		},
		slice_guard = {
			id = "slice_guard",
			displayName = "切片守卫",
			kind = "soldier",
			tags = { "creature", "slice", "guard" },
			factory = {
				type = "soldier",
				script = "res/scripts/agent/BehaviorSoldierAgent.lua",
				appearance = "Light",
				weaponMesh = "models/futuristic_soldier/soldier_weapon.mesh",
			},
			teamId = 1,
			transform = {
				position = { x = 2.5, y = 0, z = 0 },
				rotation = { x = 0, y = -90, z = 0 },
			},
			locomotion = {
				speed = 3.0,
				target = { x = 8, y = 0, z = 0 },
				targetRadius = 1.0,
			},
			attributes = {
				maxHealth = 100,
				health = 100,
			},
			weapon = {
				maxAmmo = 10,
				ammo = 10,
			},
			ai = {
				driver = "bt",
				blackboard = {
					role = "guard",
					sliceGuardAwake = false,
					["slice.guard.shootDistanceSq"] = 9.0,
				},
			},
			behaviorTree = {
				entry = "slice_guard",
				script = "res.scripts.ai.behavior.config.SliceGuardBT",
				reload = true,
			},
			triggers = {
				{
					event = "PLAYER_ENTER?regionId=slice_entry",
					scope = "Global",
					action = "wake_behavior_tree",
					targetKey = "enemy",
					setBoolKey = "slice.guardAwake",
				},
			},
		},
		slice_guard_dark = {
			inherit = "slice_guard",
			id = "slice_guard_dark",
			displayName = "切片守卫（暗色）",
			factory = {
				appearance = "Dark",
			},
			teamId = 2,
			transform = {
				position = { x = 6, y = 0, z = 0 },
				rotation = { x = 0, y = 90, z = 0 },
			},
			locomotion = {
				target = { x = -8, y = 0, z = 0 },
			},
			ai = {
				blackboard = {
					role = "guard_dark",
				},
			},
		},
		seeking_probe = {
			id = "seeking_probe",
			displayName = "寻路探针",
			kind = "agent",
			tags = { "creature", "probe" },
			factory = {
				type = "agent",
				agentType = "seeking",
			},
			teamId = 0,
			transform = {
				position = { x = -6, y = 0, z = 0 },
				rotation = { x = 0, y = 0, z = 0 },
			},
			locomotion = {
				speed = 2.0,
				target = { x = 0, y = 0, z = 8 },
				targetRadius = 0.5,
			},
			attributes = {
				health = 50,
			},
		},
	},
}

return CreatureDefs
