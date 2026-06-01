-- SoldierBT.lua
-- Data-only topology for the Sandbox8 soldier behavior tree.
-- Runtime construction is handled by BehaviorTreeLoader.lua.

SoldierBTConfig = {
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
        moveToLastKnownEnemy = "MoveToLastKnownEnemyAction.lua",
        investigateSound = "InvestigateSoundAction.lua",
        evadeDanger = "EvadeDangerAction.lua",
        moveToFormationSlot = "MoveToFormationSlotAction.lua",
        callForBackup = "CallForBackupAction.lua",
        waitForSquadMate = "WaitForSquadMateAction.lua",
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
                        name = "action",
                        reevaluateMs = 200,
                        children = {
                            {
                                node = "Sequence",
                                children = {
                                    { node = "Condition", condition = "IsCriticalHealth" },
                                    {
                                        node = "Selector",
                                        children = {
                                            {
                                                node = "Sequence",
                                                children = {
                                                    { node = "Condition", condition = "HasMovePosition" },
                                                    { node = "Action", action = "move" },
                                                },
                                            },
                                            { node = "Action", action = "flee" },
                                        },
                                    },
                                },
                            },
                            {
                                node = "Sequence",
                                name = "evadeDanger",
                                children = {
                                    { node = "Condition", condition = "HasDangerThreat", minDanger = 0.1 },
                                    { node = "Action", action = "evadeDanger", reuse = false },
                                },
                            },
                            {
                                node = "Sequence",
                                name = "callForBackup",
                                children = {
                                    { node = "Condition", condition = "ShouldCallForBackup" },
                                    { node = "Action", action = "callForBackup", reuse = false },
                                },
                            },
                            {
                                node = "Sequence",
                                children = {
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
                            {
                                node = "Sequence",
                                name = "investigateSound",
                                children = {
                                    { node = "Condition", condition = "HasHeardSound", minConfidence = 0.05 },
                                    { node = "Action", action = "investigateSound", reuse = false },
                                },
                            },
                            {
                                node = "Sequence",
                                name = "investigateMemory",
                                children = {
                                    { node = "Condition", condition = "HasLastKnownEnemyMemory", minConfidence = 0.05 },
                                    { node = "Action", action = "moveToLastKnownEnemy", reuse = false },
                                },
                            },
                            {
                                node = "Sequence",
                                name = "waitForSquadMate",
                                children = {
                                    { node = "Condition", condition = "ShouldWaitForSquadMate" },
                                    { node = "Action", action = "waitForSquadMate", reuse = false },
                                },
                            },
                            {
                                node = "Sequence",
                                name = "moveToFormationSlot",
                                children = {
                                    { node = "Condition", condition = "HasFormationSlot" },
                                    { node = "Action", action = "moveToFormationSlot", reuse = false },
                                },
                            },
                            {
                                node = "Selector",
                                children = {
                                    {
                                        node = "Sequence",
                                        children = {
                                            { node = "Condition", condition = "HasMovePosition" },
                                            { node = "Action", action = "move" },
                                        },
                                    },
                                    {
                                        node = "Selector",
                                        children = {
                                            {
                                                node = "Sequence",
                                                children = {
                                                    { node = "Condition", condition = "RandomChance", params = { 0.5 } },
                                                    { node = "Action", action = "randomMove" },
                                                },
                                            },
                                            { node = "Action", action = "idle" },
                                        },
                                    },
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

return SoldierBTConfig
