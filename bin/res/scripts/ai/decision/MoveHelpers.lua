-- MoveHelpers.lua
-- DT 的 MoveAction / PursueAction 共用的路径构建 + steering 应用工具。
-- 把路径走 + OpenSteer 加速度积分这套从 chapter_6 / IndirectSoldierAgent 抽出来，
-- 让 action 文件自身只负责终止条件与状态机请求，不管细节。

require("res.scripts.agent.AgentUtils.lua")

MoveHelpers = {}

-- 在 navmesh "default" 上找一条 from→to 的路径，并设置到 agent。
-- 失败（FindPath 找不到 / path 空）返回 false；成功返回 true。
-- agent 内部会做朝向预校正（见 AgentUtils.Agent_SetPath）。
function MoveHelpers.BuildAndSetPath(agent, fromPos, toPos)
    local path = std.vector_Ogre__Vector3_()
    local ok = Sandbox:FindPath("default", fromPos, toPos, path)
    if not ok or path:size() == 0 then
        return false
    end
    Agent_SetPath(agent, path, false)
    agent:SetTarget(toPos)
    agent:SetTargetRadius(1.0)
    return true
end

-- 调用 OpenSteer 转向力 + 通过 accumulator 平滑施加到 agent。
-- accumulator: 一个 Vector3（per-action 的积分器，在 OnInitialize 里 new 一个就好），
-- ApplySteeringForce2 会把当前帧 acceleration 写回去做下一帧插值。
function MoveHelpers.ApplySteering(agent, accumulator, deltaMs)
    local dtSec = deltaMs / 1000.0
    local force = Soldier_CalculateSteering(agent, dtSec)
    AgentUtilities_ApplySteeringForce2(agent, force, accumulator, dtSec)
    AgentUtilities_ClampHorizontalSpeed(agent)
end

-- 仿 Sandbox6 C++ MoveState/PursueState 的路径可视化：折线 + 末端圆圈，
-- 略高于地面避免 z-fighting；调用方传自己 state 的颜色/偏移/半径。
-- 仅在 action 处于 RUNNING 时调用，agent idle/shoot/reload 期间不画。
-- 仿 Sandbox6 C++ MoveState/PursueState 的路径可视化：折线 + 末端圆圈。
-- 圆心由 caller 传入：要确保 Y 在地面（navmesh 上），不然圆飘在空中很怪。
-- - MoveAction 直接用 agent:GetTarget()（来自 Sandbox:RandomPoint，已贴地）
-- - PursueAction 必须把敌人 GetPosition() 投影到 navmesh，再传进来
function MoveHelpers.DrawPath(agent, circleCenter, color, offset, radius)
    if _G.HELLO_SUPPRESS_AI_PATH_DRAW == true then return end
    if not agent then return end
    local path = agent:GetPath()
    if path == nil or path:size() == 0 then return end
    DebugDrawer:drawPath(path, color, false, offset)
    if circleCenter then
        DebugDrawer:drawCircle(circleCenter + offset, radius, 20, color, false)
    end
end
