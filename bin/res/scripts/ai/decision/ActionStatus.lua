-- ActionStatus.lua
-- DecisionAction Lua 回调的返回值常量。
-- 必须与 C++ DecisionAction::Status 枚举数值一致：
--   STATUS_UNINITIALIZED = 0
--   STATUS_RUNNING       = 1
--   STATUS_TERMINATED    = 2

ActionStatus = {
    UNINITIALIZED = 0,
    RUNNING       = 1,
    TERMINATED    = 2,
}
