local CreatureDefs = {
	schemaVersion = 1,
	defs = {
		slice_guard = {
			id = "slice_guard",
			displayName = "切片守卫",
			kind = "soldier",
			tags = { "creature", "slice", "guard" },
			factory = {
				type = "soldier",
				script = "res/scripts/agent/IndirectSoldierAgent.lua",
				appearance = "Light",
				weaponMesh = "models/futuristic_soldier/soldier_weapon.mesh",
			},
			teamId = 1,
			transform = {
				position = { x = 0, y = 0, z = 0 },
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
				driver = "fsm",
				blackboard = {
					role = "guard",
				},
			},
			behaviorTree = {
				entry = "slice_guard",
				script = "res.scripts.ai.behavior.config.SoldierBT",
			},
			triggers = {
				{
					event = "PLAYER_ENTER?regionId=slice_entry",
					scope = "Global",
					action = "wake_behavior_tree",
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
