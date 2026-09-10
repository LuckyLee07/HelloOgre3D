-- Relay Outpost: bounded guards, explicit orders, shared combat actions.
require("res.scripts.ai.behavior.config.SoldierBT.lua")
local function condition(name) return {node = "Condition", condition = name} end
local function action(name) return {node = "Action", action = name} end
local function combat()
	return {node = "Selector", reevaluateMs = 150, children = {
		{node = "Sequence", reevaluateMs = 150, children = {condition("HasAmmo"),
			{node = "Selector", reevaluateMs = 150, children = {
				{node = "Sequence", reevaluateMs = 150, children = {condition("CanShootEnemy"), action("shoot")}}, action("pursue")}}}},
		action("reload")}}
end
Sandbox19CommandBTConfig = {
	actionDir = SoldierBTConfig.actionDir,
	actions = SoldierBTConfig.actions,
	subtrees = SoldierBTConfig.subtrees,
	tree = {node = "Selector", name = "sandbox19Root", reevaluateMs = 150, children = {
		{node = "Sequence", reevaluateMs = 150, children = {condition("IsAlive"),
			{node = "Selector", reevaluateMs = 150, children = {
				{node = "Sequence", name = "commandFocus", reevaluateMs = 150, children = {condition("HasCommandFocus"), combat()}},
				{node = "Sequence", name = "commandRetreat", reevaluateMs = 150, children = {condition("HasCommandRetreat"), action("move")}},
				{node = "Sequence", name = "commandRally", reevaluateMs = 150, children = {condition("HasCommandRally"), action("move")}},
				{node = "Sequence", name = "guardCombat", reevaluateMs = 150, children = {condition("HasEnemy"), combat()}},
				{node = "Sequence", name = "returnToPost", reevaluateMs = 150, children = {condition("ShouldReturnToPost"), action("move")}},
				action("idle")}}}},
		action("die")}}
}
