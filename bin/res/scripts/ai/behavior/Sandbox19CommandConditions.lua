-- Sandbox19CommandConditions.lua
-- Sandbox19 指挥切片的玩家指令条件表。
--
-- 与 SoldierConditions 平行：通过 __index 继承它的全部条件（引用的整棵 SoldierBT
-- 仍要用 HasEnemy / HasAmmo / CanShootEnemy 等），只额外提供三个 command.* 条件。
-- preset.behaviorTree.conditionsGlobal 指定本表，BehaviorSoldierAgent 据此挂载。
--
-- 指令 blackboard 约定（由 Sandbox19.lua 写入）：
--   command.kind          "focus" / "retreat" / "rally"
--   command.focusTargetId 集火目标 objectId（kind=focus 时有效）
--   command.issuedMs      下达时刻（GameManager:getTimeInMillis），用于 TTL
--   movePos               撤退/编队的目标点，直接复用 MoveAction 既有读取键

require("res.scripts.ai.behavior.SoldierConditions.lua")

Sandbox19CommandConditions = {}
setmetatable(Sandbox19CommandConditions, { __index = SoldierConditions })

local _COMMAND_TTL_MS = 8000

-- GameManager:getTimeInMillis 是"启动至今的仿真时间"，sample 刚初始化时可能为 0，
-- 因此不能拿 issuedMs>0 当有效性判据（0 是合法时刻）——有没有指令看 Has 即可。
-- elapsed>=0 用于挡住 reload 后时钟回退等异常情况。
local function _IsFresh(bb)
    if bb == nil or not bb:Has("command.issuedMs") then
        return false
    end
    local elapsed = GameManager:getTimeInMillis() - bb:GetInt("command.issuedMs", -1)
    return elapsed >= 0 and elapsed <= _COMMAND_TTL_MS
end

local function _IsKind(bb, kind)
    if not _IsFresh(bb) then
        return false
    end
    return bb:GetString("command.kind") == kind
end

-- 集火：条件带副作用——命中时把 blackboard.enemy 覆写为玩家指定目标。
--
-- 为什么要副作用：ShootAction / PursueAction 都读 bb:GetAgent("enemy")，而
-- AIController::WritePerceptionResult 每个感知 tick 都会把 enemy 改写成它自己
-- 选中的目标。在同一个 Sequence 里"条件先于动作求值"，这里的覆写紧挨着动作读取，
-- 因此能稳定压过感知结果，且不必新造一套指令专用的射击/追击动作。
--
-- 注意：本条件绝不能进 CachedCondition——缓存会跳过求值，副作用随之丢失。
function Sandbox19CommandConditions.HasCommandFocus(agent, bb)
    if not _IsKind(bb, "focus") then
        return false
    end

    local targetId = bb:GetObjectId("command.focusTargetId", 0)
    if targetId == nil or targetId <= 0 then
        return false
    end

    -- ObjectManager:getObjectById 未导出给 Lua，按 id 扫 getAllAgents 解析。
    -- 顺带天然处理了目标已被销毁的情况（扫不到即条件落空）。
    local target = nil
    local agents = ObjectManager:getAllAgents()
    for i = 0, agents:size() - 1 do
        local a = agents[i]
        if a ~= nil and a:GetObjId() == targetId then
            target = a
            break
        end
    end
    if target == nil or target:GetHealth() <= 0 then
        return false
    end

    bb:SetAgent("enemy", target)
    return true
end

-- 撤退 / 编队：目标点已由 Sandbox19.lua 写进 movePos，直接交给既有 MoveAction。
function Sandbox19CommandConditions.HasCommandRetreat(agent, bb)
    return _IsKind(bb, "retreat") and bb:Has("movePos")
end

function Sandbox19CommandConditions.HasCommandRally(agent, bb)
    return _IsKind(bb, "rally") and bb:Has("movePos")
end
