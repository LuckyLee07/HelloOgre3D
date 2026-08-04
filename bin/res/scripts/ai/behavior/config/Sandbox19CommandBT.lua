-- Sandbox19CommandBT.lua
-- Sandbox19 指挥切片专用拓扑：玩家指令分支优先于 AI 自主行为。
--
-- 复用方式：actionDir / actions / subtrees 直接引用 SoldierBTConfig；根节点是
-- Selector{ commandBranch, SoldierBTConfig.tree }——**整棵**原树作为兜底，
-- 因此 evadeDanger / callForBackup / combat / investigate / formation 等分支全部保留，
-- 只是在玩家有活跃指令时被压过。共享 SoldierBT.lua 一行不改。
--
-- 指令分支不使用 CachedCondition：HasCommandFocus 带副作用（覆写 blackboard.enemy），
-- 缓存会跳过求值导致副作用丢失，详见 Sandbox19CommandConditions.lua。

require("res.scripts.ai.behavior.config.SoldierBT.lua")

Sandbox19CommandBTConfig = {
    actionDir = SoldierBTConfig.actionDir,
    actions = SoldierBTConfig.actions,
    subtrees = SoldierBTConfig.subtrees,

    tree = {
        node = "Selector",
        name = "sandbox19Root",
        children = {
            {
                node = "Selector",
                name = "commandBranch",
                children = {
                    {
                        node = "Sequence",
                        name = "commandFocus",
                        children = {
                            { node = "Condition", condition = "HasCommandFocus" },
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
                    {
                        node = "Sequence",
                        name = "commandRetreat",
                        children = {
                            { node = "Condition", condition = "HasCommandRetreat" },
                            { node = "Action", action = "move" },
                        },
                    },
                    {
                        node = "Sequence",
                        name = "commandRally",
                        children = {
                            { node = "Condition", condition = "HasCommandRally" },
                            { node = "Action", action = "move" },
                        },
                    },
                },
            },

            SoldierBTConfig.tree,
        },
    },
}
