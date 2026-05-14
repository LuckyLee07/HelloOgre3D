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
