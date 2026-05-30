-- KnowledgeSource.lua
-- Chapter 7 知识源的 Lua 薄壳：按固定频率调用 evaluator，缓存最近一次结果。

local KnowledgeSource = {}
KnowledgeSource.__index = KnowledgeSource

function KnowledgeSource.New(name, evaluator, updateFrequencyMs)
	local source = {
		name = name or "unnamed",
		evaluator = evaluator,
		updateFrequencyMs = updateFrequencyMs or 0,
		lastUpdateTimeMs = nil,
		evaluation = nil,
		confidence = 0.0,
		evaluateCount = 0,
	}
	setmetatable(source, KnowledgeSource)
	return source
end

function KnowledgeSource:Evaluate(context, nowMs)
	if self.evaluator == nil then
		return {
			evaluation = nil,
			confidence = 0.0,
			source = self.name,
			updated = false,
			evaluateCount = self.evaluateCount,
		}
	end

	local shouldUpdate = self.lastUpdateTimeMs == nil
		or self.updateFrequencyMs <= 0
		or (nowMs - self.lastUpdateTimeMs) >= self.updateFrequencyMs

	if shouldUpdate then
		local result = self.evaluator(context) or {}
		self.evaluation = result.evaluation
		self.confidence = result.confidence or 0.0
		self.lastUpdateTimeMs = nowMs
		self.evaluateCount = self.evaluateCount + 1
	end

	return {
		evaluation = self.evaluation,
		confidence = self.confidence,
		source = self.name,
		updated = shouldUpdate,
		evaluateCount = self.evaluateCount,
	}
end

_G.KnowledgeSource = KnowledgeSource
return KnowledgeSource
