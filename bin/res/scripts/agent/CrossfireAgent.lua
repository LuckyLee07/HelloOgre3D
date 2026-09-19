-- Non-skeletal actors use the existing C++ BT, perception and locomotion.
require("res.scripts.ai.behavior.BehaviorTreeLoader.lua")
local config = {
 actionDir = "res/scripts/ai/decision/actions/crossfire/",
 actions = {move = "Move.lua", guard = "Guard.lua"},
 tree = {node = "Selector", reevaluateMs = 60, children = {
  {node = "Sequence", children = {
   {node = "Condition", condition = "Moving"}, {node = "Action", action = "move"}}},
  {node = "Action", action = "guard"}
 }}
}
function Agent_Initialize(agent)
 local ai = agent:GetAIComponent()
 ai:SetDriverByType("bt")
 local bb = ai:GetBlackboard()
 bb:SetBool("crossfire.enabled", true)
 bb:SetFloat("perception.visionRange", 22)
 bb:SetFloat("perception.fieldOfViewDegrees", 360)
 bb:SetBool("perception.requirePath", false)
 local driver = ai:GetBehaviorTreeDriver()
 local tree=BehaviorTreeLoader.Build(config, agent, driver, bb, {
  Moving = function(owner, board) return owner:GetHealth() > 0 and board:Has("movePos") end
 })
 driver:SetTree(tree)
 agent:GetLocomotionComponent():SetMaxSpeed(3.2)
 agent:GetLocomotionComponent():SetMaxForce(160)
 bb:SetString("crossfire.state", "READY")
end
function Agent_Update(agent, deltaMs) end
function Agent_EventHandle(agent, keycode) end
