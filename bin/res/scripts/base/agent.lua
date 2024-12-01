-- 将计算得到的转向力转化为只能体的运动变化
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

function Agent_Initialize(agent)
    --print("-------Initialize-------")
    local agentType = agent:getAgentType()
    if agentType == AGENT_OBJ_SEEKING then
        Agent_Seeking_Initialize(agent)
    elseif agentType == AGENT_OBJ_PURSUING then
        Agent_Pursuing_Initialize(agent)
    elseif agentType == AGENT_OBJ_PATHING then
        Agent_Pathing_Initialize(agent)
    elseif agentType == AGENT_OBJ_FOLLOWER then
        Agent_Follower_Initialize(agent)
    end
end

function Agent_Seeking_Initialize(agent)
    agent:SetTarget(Vector3(50.0, 0.0, 0.0))
    agent:SetTargetRadius(1.5)
end

local enemy;
function Agent_Pursuing_Initialize(agent)
    agent:SetTargetRadius(1.0);

    local randPosx = math.random(-50, 50)
    local randPosz = math.random(-50, 50)
    agent:SetPosition(Vector3(randPosx, 0, randPosz))

    seekings = GameManager:getSpecifyAgents(AGENT_OBJ_SEEKING)
    if seekings:size() > 0 then enemy = seekings[0] end
    agent:SetTarget(enemy:GetPosition());

    agent:SetMaxSpeed(enemy:GetMaxSpeed() * 0.8)
end

function Agent_Pathing_Initialize(agent)
    local randPosx = math.random(-50, 50)
    local randPosz = math.random(-50, 50)
    agent:SetPosition(Vector3(randPosx, 0, randPosz))
end

local leaders;
function Agent_Follower_Initialize(agent)
    local randPosx = math.random(-50, 50)
    local randPosz = math.random(-50, 50)
    agent:SetPosition(Vector3(randPosx, 0, randPosz))

    leaders = GameManager:getSpecifyAgents(AGENT_OBJ_SEEKING)
end

function Agent_Update(agent, deltaTimeInMillis)
    local agentType = agent:getAgentType()
    if agentType == AGENT_OBJ_SEEKING then
        Agent_Seeking_Update(agent, deltaTimeInMillis)
    elseif agentType == AGENT_OBJ_PURSUING then
        Agent_Pursuing_Update(agent, deltaTimeInMillis)
    elseif agentType == AGENT_OBJ_PATHING then
        Agent_Pathing_Update(agent, deltaTimeInMillis)
    elseif agentType == AGENT_OBJ_FOLLOWER then
        Agent_Follower_Update(agent, deltaTimeInMillis)
    end
end


function Agent_Seeking_Update(agent, deltaTimeInMillis)
    local destination = agent:GetTarget();
    local deltaTimeInSeconds = deltaTimeInMillis / 1000;
    local avoidAgentForce = agent:ForceToAvoidAgents(1.5);
    local avoidObjectForce = agent:ForceToAvoidObjects(1.5);
    local seekForce = agent:ForceToPosition(destination);
    local targetRadius = agent:GetTargetRadius();
    local radius = agent:GetRadius();
    local position = agent:GetPosition();
    local avoidanceMultiplier = 3;
    
    -- Sum all forces and apply higher priorty to avoidance forces.
    local steeringForces = seekForce +
        avoidAgentForce * avoidanceMultiplier +
        avoidObjectForce * avoidanceMultiplier;

    -- Apply all steering forces.
    AgentUtilities_ApplyPhysicsSteeringForce(
        agent, steeringForces, deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);

    local targetRadiusSquared = (targetRadius + radius) * (targetRadius + radius);
    
    -- Calculate the position where the Agent touches the ground.
    local adjustedPosition = agent:GetPosition() - Vector3(0, agent:GetHeight()/2, 0);

    -- If the agent is within the target radius pick a new 
    -- random position to move to.
    if (DistanceSquared(adjustedPosition, destination) < targetRadiusSquared) then

        -- New target is within the 100 meter squared movement space.
        local targetPos = agent:GetTarget();
        targetPos.x = math.random(-50, 50);
        targetPos.z = math.random(-50, 50);
        
        agent:SetTarget(targetPos);
    end

    -- Draw debug information for target and target radius.
    DebugDrawer:drawCircle(destination, targetRadius, 10, UtilColors.Red);
    DebugDrawer:drawLine(position, destination, UtilColors.Green);

    -- Debug outline representing the space the Agent can move within.
    DebugDrawer:drawSquare(Vector3(0, 0, 0), 100, UtilColors.Red);
end

function Agent_Pursuing_Update(agent, deltaTimeInMillis)
    agent:SetTarget(enemy:PredictFuturePosition(1))

    local destination = agent:GetTarget()
    local deltaTimeInSeconds = deltaTimeInMillis / 1000;
    local seekForce = agent:ForceToPosition(destination);
    local targetRadius = agent:GetTargetRadius();
    local position = agent:GetPosition();

    AgentUtilities_ApplyPhysicsSteeringForce(
        agent, seekForce, deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);

    DebugDrawer:drawCircle(destination, targetRadius, 10, UtilColors.Orange);
    DebugDrawer:drawLine(position, destination, UtilColors.Yellow);
end

function Agent_Pathing_Update(agent, deltaTimeInMillis)
    local deltaTimeInSeconds = deltaTimeInMillis / 1000;

    local followForce = agent:ForceToFollowPath(1.25);
    local stayForce = agent:ForceToStayOnPath(1);
    local wanderForce = agent:ForceToWander(deltaTimeInMillis);

    local totalForces = Vector.Normalize(followForce) + Vector.Normalize(stayForce) * 0.25
                         + Vector.Normalize(wanderForce) * 0.25;

    local targetSpeed = 3
    -- Accelerate pathing agents to a minimun speed
    if (agent:GetSpeed() < targetSpeed) then
        local speedForce = agent:ForceToTargetSpeed(targetSpeed)
        totalForces = totalForces + Vector.Normalize(speedForce);
    end

    AgentUtilities_ApplyPhysicsSteeringForce(
        agent, totalForces, deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);

    DebugDrawer:drawPath(agent:GetPath(), UtilColors.Blue, true, Vector3(0.0))
end

function Agent_Follower_Update(agent, deltaTimeInMillis)
    local deltaTimeInSeconds = deltaTimeInMillis / 1000;

    local forceToCombine = agent:ForceToCombine(leaders, 100, 180);

    local agents = GameManager:getAllAgents()
    local forceToSeparate = agent:ForceToSeparate(agents, 2, 180);
    
    local forceToAlign = agent:ForceToSeparate(leaders, 5, 45);

    local totalForces = forceToCombine + forceToSeparate * 1.15 + forceToAlign;

    AgentUtilities_ApplyPhysicsSteeringForce(
        agent, totalForces, deltaTimeInSeconds);
    AgentUtilities_ClampHorizontalSpeed(agent);


    local position = agent:GetPosition();
    local destination = leaders[0]:GetPosition();
    local targetRadius = agent:GetTargetRadius();

    DebugDrawer:drawCircle(position, 1, 10, UtilColors.Yellow);
    DebugDrawer:drawCircle(destination, targetRadius, 10, UtilColors.White);
    DebugDrawer:drawLine(position, destination, UtilColors.Green);
end