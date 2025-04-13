-- 将计算得到的转向力转化为智能体的运动变化
function AgentUtilities_ApplyPhysicsSteeringForce(agent, steeringForce, deltaTimeInSeconds)

    -- Ignore very weak steering forces.
    if (Vector.LengthSquared(steeringForce) < 0.1) then
        return;
    end
    
    -- Agents with 0 mass are immovable.
    if (agent:GetMass() <= 0) then
        return;
    end

    -- Zero out any steering in the y direction
    steeringForce.y = 0;

    -- Maximize the steering force, essentially forces the agent to max acceleration.
    steeringForce = Vector.Normalize(steeringForce) * agent:GetMaxForce();

    -- Apply force to the physics representation.
    agent:ApplyForce(steeringForce);

    -- Newtons(kg*m/s^2) divided by mass(kg) results in acceleration(m/s^2).
    local acceleration = steeringForce / agent:GetMass();
    
    -- Velocity is measured in meters per second(m/s).
    local currentVelocity = agent:GetVelocity();
    
    -- Acceleration(m/s^2) multiplied by seconds results in velocity(m/s).
    local newVelocity = currentVelocity + (acceleration * deltaTimeInSeconds);

    -- Zero out any pitch changes to keep the Agent upright.
    -- NOTE: This implies that agents can immediately turn in any direction.
    newVelocity.y = 0;

    -- Point the agent in the direction of movement.
    agent:SetForward(newVelocity);
end

-- 将计算得到的转向力转化为智能体的运动变化
-- 基于物理运算（牛顿力学）的运动更新，包括加速度、速度和朝向的平滑过渡
function AgentUtilities_ApplySteeringForce2(
    agent, steeringForce, accelerationAccumulator, deltaTimeInSeconds)

    -- Ignore very weak steering forces.
    if (Vector.LengthSquared(steeringForce) < 0.1) then
        return;
    end
    
    -- Agents with 0 mass are immovable.
    if (agent:GetMass() <= 0) then
        return;
    end

    -- Zero out any steering changes in the y axis.
    steeringForce.y = 0;
    
    -- Maximize the steering force, essentially forces the agent to max
    -- acceleration.
    steeringForce = Vector.Normalize(steeringForce) * agent:GetMaxForce();
    
    -- Newtons(kg*m/s^2) divided by mass(kg) results in acceleration(m/s^2).
    local acceleration = steeringForce / agent:GetMass();
    
    -- Interpolate to the new acceleration to dampen jitter in velocity and
    -- forward direction.
    acceleration = accelerationAccumulator +
        (acceleration - accelerationAccumulator) * 0.4;
    
    -- Reassign the new acceleration back to the accumulator.
    accelerationAccumulator.x = acceleration.x;
    accelerationAccumulator.y = acceleration.y;
    accelerationAccumulator.z = acceleration.z;
    
    -- Calculate the new velocity in (m/s)
    local velocity = agent:GetVelocity() + (acceleration * deltaTimeInSeconds);
    
    -- Assign the velocity directly, and orient toward the movement.
    agent:SetVelocity(velocity);
    
    -- Prevent trying to set the forward to a Zero vector.
    if (Vector.LengthSquared(velocity) > 0.1) then
        velocity.y = 0;
        
        -- Interpolate to the new forward direction to dampen jitter.
        local forward = agent:GetForward();
        forward = forward + (Vector.Normalize(velocity) - forward) * 0.2;
        agent:SetForward(forward);
    end
end

-- 限制智能体的速度 防止超过定义的最大值
function AgentUtilities_ClampHorizontalSpeed(agent)
    local velocity = agent:GetVelocity();
    -- Store downward velocity to apply after clamping.
    local downwardVelocity = velocity.y;

    -- Ignore downward velocity since Agents never apply downward velocity
    -- themselves.
    velocity.y = 0;

    local maxSpeed = agent:GetMaxSpeed();
    local squaredSpeed = maxSpeed * maxSpeed;

    -- Using squared values avoids the cost of using the square
    -- root when calculating the magnitude of the velocity vector.
    if (Vector.LengthSquared(velocity) > squaredSpeed) then
        local newVelocity = Vector.Normalize(velocity) * maxSpeed;

        -- Reapply the original downward velocity after clamping.
        newVelocity.y = downwardVelocity;

        agent:SetVelocity(newVelocity);
    end
end

function Soldier_CalculateSteering(agent, deltaTimeInSeconds)
    local avoidForce = agent:ForceToAvoidAgents(0.5);
    local avoidObjectForce = agent:ForceToAvoidObjects(0.5);
    local followForce = agent:ForceToFollowPath(0.5);
    local stayForce = agent:ForceToStayOnPath(0.5);
    
    local totalForces = followForce * 1.5 + stayForce * 0.4 + avoidForce * 1 + avoidObjectForce * 2;
    totalForces.y = 0;

    local targetSpeed = agent:GetMaxSpeed();

    if (agent:GetSpeed() < targetSpeed) then
        local speedForce = agent:ForceToTargetSpeed(targetSpeed);
        totalForces = totalForces + speedForce * 7;
    end
    
    return totalForces;
end

function Soldier_CalculateSlowSteering(agent, deltaTimeInSeconds)
    local avoidForce = agent:ForceToAvoidAgents(0.5);
    local avoidObjectForce = agent:ForceToAvoidObjects(0.5);
    local followForce = agent:ForceToFollowPath(0.5);
    local stayForce = agent:ForceToStayOnPath(0.5);
    
    local totalForces = Vector.Normalize(followForce) +
        Vector.Normalize(stayForce) * 0.2 +
        avoidForce * 1 +
        avoidObjectForce * 1;

    local targetSpeed = agent:GetMaxSpeed();
    if (agent:GetSpeed() < targetSpeed) then
        local speedForce = agent:ForceToTargetSpeed(targetSpeed);
        totalForces = totalForces + speedForce * 5;
    end
    
    return totalForces;
end