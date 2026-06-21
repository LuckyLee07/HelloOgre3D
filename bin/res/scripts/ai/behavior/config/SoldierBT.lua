-- SoldierBT.lua
-- Data-only topology for the Sandbox8 soldier behavior tree.
-- Runtime construction is handled by BehaviorTreeLoader.lua.

local function CachedCondition(condition, cacheKeys, extra)
    local node = {
        node = "Condition",
        condition = condition,
        cacheMs = 500,
        cacheKeys = cacheKeys,
    }
    if extra ~= nil then
        for key, value in pairs(extra) do
            node[key] = value
        end
    end
    return node
end

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

    subtrees = {
        combat = {
            node = "Sequence",
            name = "combatBranch",
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
                                    CachedCondition("HasDangerThreat", { "sense.dangerEscapePos", "sense.dangerLevel" }, { minDanger = 0.1 }),
                                    { node = "Action", action = "evadeDanger", reuse = false },
                                },
                            },
                            {
                                node = "Sequence",
                                name = "callForBackup",
                                children = {
                                    CachedCondition("ShouldCallForBackup", { "team.shouldCallForBackup" }),
                                    { node = "Action", action = "callForBackup", reuse = false },
                                },
                            },
                            { node = "Subtree", subtree = "combat" },
                            {
                                node = "Sequence",
                                name = "investigateSound",
                                children = {
                                    CachedCondition("HasHeardSound", { "sense.heardSoundPos", "sense.heardSoundConfidence", "perception.hasTarget" }, { minConfidence = 0.05 }),
                                    { node = "Action", action = "investigateSound", reuse = false },
                                },
                            },
                            {
                                node = "Sequence",
                                name = "investigateMemory",
                                children = {
                                    CachedCondition("HasLastKnownEnemyMemory", {
                                        "memory.snapshot.hasLastKnownEnemy",
                                        "memory.snapshot.lastKnownEnemyId",
                                        "memory.snapshot.lastKnownEnemyObservedAtMs",
                                        "memory.snapshot.lastKnownEnemyConfidence",
                                        "memory.search.completedEnemyId",
                                        "memory.search.completedObservedAtMs",
                                    }, { minConfidence = 0.05 }),
                                    { node = "Action", action = "moveToLastKnownEnemy", reuse = false },
                                },
                            },
                            {
                                node = "Sequence",
                                name = "waitForSquadMate",
                                children = {
                                    CachedCondition("ShouldWaitForSquadMate", { "formation.waitForSquadMate", "formation.readyCount", "formation.minReadyCount" }),
                                    { node = "Action", action = "waitForSquadMate", reuse = false },
                                },
                            },
                            {
                                node = "Sequence",
                                name = "moveToFormationSlot",
                                children = {
                                    CachedCondition("HasFormationSlot", { "formation.enabled", "formation.slotPos" }),
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
