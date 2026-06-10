-- PursueAction.lua
-- 经 navmesh + OpenSteer 转向追击 blackboard.enemy。
-- 进入 3 单位射击距离后 TERMINATED；敌人移动超过阈值就重新规划路径。
-- 同 MoveAction：切片重入不刹车、不重置 accumulator，避免视觉卡顿。

require("res.scripts.ai.decision.ActionStatus.lua")
require("res.scripts.ai.decision.MoveHelpers.lua")

local _PURSUE_REACH    = 2.0    -- 接近到 2m 内停止追击；与 Sandbox6 PursueState.cpp kPursueReachDistance 对齐
local _REPATH_INTERVAL = 500    -- ms（越小越准但越闪）
local _REPATH_DELTA_SQ = 1.0    -- 敌人位置移动平方距离 > 1m² 重新规划

local _pursueReach = _PURSUE_REACH
local _repathInterval = _REPATH_INTERVAL
local _repathDeltaSq = _REPATH_DELTA_SQ
local _acc = nil
local _msSinceRepath = 0
local _lastEnemyPos = nil

local function _ConfigureFromBlackboard(bb)
    _pursueReach = bb and bb:GetFloat("pursue.reach", _PURSUE_REACH) or _PURSUE_REACH
    _repathInterval = bb and bb:GetFloat("pursue.repathIntervalMs", _REPATH_INTERVAL) or _REPATH_INTERVAL
    _repathDeltaSq = bb and bb:GetFloat("pursue.repathDeltaSq", _REPATH_DELTA_SQ) or _REPATH_DELTA_SQ
    if _pursueReach <= 0 then _pursueReach = _PURSUE_REACH end
    if _repathInterval < 0 then _repathInterval = _REPATH_INTERVAL end
    if _repathDeltaSq < 0 then _repathDeltaSq = _REPATH_DELTA_SQ end
end

function OnInitialize(owner, bb)
    _msSinceRepath = 0
    _ConfigureFromBlackboard(bb)
    if _acc == nil then _acc = Vector3(0, 0, 0) end
    if not owner then return end

    owner:EnterMoveAnim()

    local enemy = bb:GetAgent("enemy")
    if enemy then
        local enemyPos = enemy:GetPosition()
        if MoveHelpers.BuildAndSetPath(owner, owner:GetPosition(), enemyPos) then
            owner:SetMovePosition(enemyPos)
            _lastEnemyPos = enemyPos
        end
    end
end

function OnUpdate(deltaMs, owner, bb)
    if not owner or owner:GetHealth() <= 0 then
        return ActionStatus.TERMINATED
    end
    local enemy = bb:GetAgent("enemy")
    if enemy == nil or enemy:GetHealth() <= 0 then
        return ActionStatus.TERMINATED
    end

    -- 接近到 reach 距离即终止；CanShootEnemy 评估器（3m）会让 DT 切到 ShootAction
    local toEnemy = enemy:GetPosition() - owner:GetPosition()
    toEnemy.y = 0
    if toEnemy:length() < _pursueReach then
        return ActionStatus.TERMINATED
    end

    -- 周期性按需 repath：到了检查间隔且敌人位移超阈值才重建
    _msSinceRepath = _msSinceRepath + deltaMs
    if _repathInterval <= 0 or _msSinceRepath >= _repathInterval then
        _msSinceRepath = 0
        local enemyPos = enemy:GetPosition()
        local needRepath = (_lastEnemyPos == nil)
            or (_repathDeltaSq <= 0)
            or ((enemyPos - _lastEnemyPos):squaredLength() > _repathDeltaSq)
        if needRepath and MoveHelpers.BuildAndSetPath(owner, owner:GetPosition(), enemyPos) then
            owner:SetMovePosition(enemyPos)
            _lastEnemyPos = enemyPos
        end
    end

    MoveHelpers.ApplySteering(owner, _acc, deltaMs)
    -- 仿 Sandbox6 C++ PursueState：红色折线 + 末端圆圈 = pursue reach 距离。
    -- 敌人 GetPosition() 是腰高，必须投到 navmesh 才能贴地，不然圆飘在空中。
    if _G.HELLO_SUPPRESS_AI_PATH_DRAW ~= true then
        local circleCenter = Sandbox:FindClosestPoint("default", enemy:GetPosition())
        MoveHelpers.DrawPath(owner, circleCenter, UtilColors.Red, Vector3(0, 0.1, 0), _pursueReach)
    end
    return ActionStatus.RUNNING
end

function OnCleanUp(owner, bb)
    -- 同 MoveAction：不刹车，velocity 留给下一段。终止条件是"进入射程"，
    -- 此时正要切到 ShootAction，ShootAction 自己会 SetVelocity(0) 站住。
end
