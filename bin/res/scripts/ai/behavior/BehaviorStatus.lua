-- BehaviorStatus.lua
-- BT 节点统一返回值。OnUpdate 必须返回三选一：
--   RUNNING — 还没决出输赢，下一帧继续
--   SUCCESS — 完成（成功）
--   FAILURE — 失败（让上层 Selector 走 fallback / Sequence 立即失败）

BehaviorStatus = {
    RUNNING = 1,
    SUCCESS = 2,
    FAILURE = 3,
}

-- DT 那一套用 ActionStatus.TERMINATED=2，BT 这边 SUCCESS 也是 2。
-- 这意味着 ai/decision/actions/ 下的 lua 可以直接给 BT 复用 ——
-- "成功完成" 在两套语义里都映射到 2。
