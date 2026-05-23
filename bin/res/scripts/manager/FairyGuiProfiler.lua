local FairyGuiProfiler = Class("FairyGuiProfiler")

local function isBlank(value)
	return value == nil or value == ""
end

local function copyTable(source, target)
	target = target or {}
	if type(source) ~= "table" then
		return target
	end

	for name, value in pairs(source) do
		target[name] = value
	end
	return target
end

local function tableCount(source)
	local count = 0
	if type(source) ~= "table" then
		return count
	end
	for _, _ in pairs(source) do
		count = count + 1
	end
	return count
end

local function formatMs(value)
	return string.format("%.3f", tonumber(value) or 0)
end

local function clampText(value, maxLen)
	value = tostring(value or "")
	maxLen = tonumber(maxLen) or 0
	if maxLen <= 0 or string.len(value) <= maxLen then
		return value
	end
	if maxLen <= 3 then
		return string.sub(value, 1, maxLen)
	end
	return string.sub(value, 1, maxLen - 3) .. "..."
end

local function joinBriefs(values, emptyText)
	if type(values) ~= "table" or #values <= 0 then
		return emptyText or "-"
	end
	return table.concat(values, " | ")
end

local function pushLimited(values, value, maxCount)
	if type(values) ~= "table" then
		return
	end
	maxCount = tonumber(maxCount) or 0
	if maxCount > 0 and #values >= maxCount then
		return
	end
	table.insert(values, value)
end

local function objectBrief(owner, objectInfo)
	if objectInfo == nil then
		return "-"
	end
	local hidden = owner ~= nil and owner.hiddenObjects ~= nil and owner.hiddenObjects[objectInfo.key] ~= nil
	local state = hidden and "H" or "V"
	local service = objectInfo.serviceType ~= nil and ":" .. tostring(objectInfo.serviceType) or ""
	return string.format("%s%s[%s,%s,#%s]", tostring(objectInfo.key or objectInfo.uiName or objectInfo.handle), service, tostring(objectInfo.layer or ""), state, tostring(objectInfo.sortingOrder or 0))
end

local function sortedObjectInfos(owner, includeHidden)
	local objects = {}
	if owner == nil then
		return objects
	end
	for key, objectInfo in pairs(owner.objects or {}) do
		if includeHidden == true or owner.hiddenObjects[key] == nil then
			table.insert(objects, objectInfo)
		end
	end
	table.sort(objects, function(a, b)
		local sortA = tonumber(a.sortingOrder) or 0
		local sortB = tonumber(b.sortingOrder) or 0
		if sortA == sortB then
			return tostring(a.key or "") < tostring(b.key or "")
		end
		return sortA > sortB
	end)
	return objects
end

local function sortedKeys(source)
	local keys = {}
	if type(source) ~= "table" then
		return keys
	end
	for key, _ in pairs(source) do
		table.insert(keys, key)
	end
	table.sort(keys, function(a, b)
		return tostring(a) < tostring(b)
	end)
	return keys
end

local function createPerfStat()
	return {
		count = 0,
		success = 0,
		fail = 0,
		totalMs = 0,
		maxMs = 0,
		lastMs = 0,
		lastName = "",
	}
end

function FairyGuiProfiler:Init(owner)
	self.owner = owner
end

function FairyGuiProfiler:GetRenderStats()
	local commandCount = 0
	local triangleCount = 0
	local materialCount = 0
	local textureCount = 0
	local materialAliasCount = 0
	local textureAliasCount = 0
	local runtimeObjectHandle = 0
	local runtimeBinding = 0
	local materialDetail = ""
	local textureDetail = ""
	if GameManager ~= nil and GameManager.getFairyGuiLastRenderCommandCount ~= nil then
		commandCount = GameManager:getFairyGuiLastRenderCommandCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiLastTriangleCount ~= nil then
		triangleCount = GameManager:getFairyGuiLastTriangleCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiMaterialCount ~= nil then
		materialCount = GameManager:getFairyGuiMaterialCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiTextureCount ~= nil then
		textureCount = GameManager:getFairyGuiTextureCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiMaterialAliasCount ~= nil then
		materialAliasCount = GameManager:getFairyGuiMaterialAliasCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiTextureAliasCount ~= nil then
		textureAliasCount = GameManager:getFairyGuiTextureAliasCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiRuntimeObjectHandleCount ~= nil then
		runtimeObjectHandle = GameManager:getFairyGuiRuntimeObjectHandleCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiRuntimeListenerBindingCount ~= nil then
		runtimeBinding = GameManager:getFairyGuiRuntimeListenerBindingCount()
	end
	if GameManager ~= nil and GameManager.getFairyGuiMaterialDetailString ~= nil then
		materialDetail = GameManager:getFairyGuiMaterialDetailString() or ""
	end
	if GameManager ~= nil and GameManager.getFairyGuiTextureDetailString ~= nil then
		textureDetail = GameManager:getFairyGuiTextureDetailString() or ""
	end
	return {
		commandCount = commandCount or 0,
		triangleCount = triangleCount or 0,
		materialCount = materialCount or 0,
		textureCount = textureCount or 0,
		materialAliasCount = materialAliasCount or 0,
		textureAliasCount = textureAliasCount or 0,
		runtimeObjectHandle = runtimeObjectHandle or 0,
		runtimeBinding = runtimeBinding or 0,
		materialDetail = materialDetail,
		textureDetail = textureDetail,
	}
end

function FairyGuiProfiler:DumpRenderStats()
	local stats = self:GetRenderStats()
	print("[FGUI] RenderStats commandCount=", stats.commandCount, "triangleCount=", stats.triangleCount, "material=", stats.materialCount, "texture=", stats.textureCount, "materialAlias=", stats.materialAliasCount, "textureAlias=", stats.textureAliasCount, "runtimeObjectHandle=", stats.runtimeObjectHandle, "runtimeBinding=", stats.runtimeBinding)
	if not isBlank(stats.materialDetail) then
		print("[FGUI] RenderMaterialDetail", stats.materialDetail)
	end
	if not isBlank(stats.textureDetail) then
		print("[FGUI] RenderTextureDetail", stats.textureDetail)
	end
end

function FairyGuiProfiler:RecordPerf(category, elapsedMs, name, success)
	if isBlank(category) then
		return nil
	end
	local owner = self.owner
	if owner == nil then
		return nil
	end
	if owner.perfStats == nil then
		owner.perfStats = {}
	end

	local stat = owner.perfStats[category]
	if stat == nil then
		stat = createPerfStat()
		owner.perfStats[category] = stat
	end

	elapsedMs = tonumber(elapsedMs) or 0
	stat.count = stat.count + 1
	stat.totalMs = stat.totalMs + elapsedMs
	stat.lastMs = elapsedMs
	stat.lastName = name or ""
	if elapsedMs > stat.maxMs then
		stat.maxMs = elapsedMs
		stat.maxName = name or ""
	end
	if success == false then
		stat.fail = stat.fail + 1
	else
		stat.success = stat.success + 1
	end
	return stat
end

function FairyGuiProfiler:GetPerfStat(category)
	local owner = self.owner
	local stat = owner ~= nil and owner.perfStats ~= nil and owner.perfStats[category] or nil
	if stat == nil then
		stat = createPerfStat()
	end
	return {
		count = stat.count or 0,
		success = stat.success or 0,
		fail = stat.fail or 0,
		totalMs = stat.totalMs or 0,
		avgMs = (stat.count or 0) > 0 and (stat.totalMs or 0) / stat.count or 0,
		maxMs = stat.maxMs or 0,
		maxName = stat.maxName or "",
		lastMs = stat.lastMs or 0,
		lastName = stat.lastName or "",
	}
end

function FairyGuiProfiler:GetPerfStats()
	local stats = {}
	for _, category in ipairs({ "open", "close", "event", "loadPackage", "createObject", "service" }) do
		stats[category] = self:GetPerfStat(category)
	end
	local owner = self.owner
	if owner ~= nil and owner.perfStats ~= nil then
		for category, _ in pairs(owner.perfStats) do
			if stats[category] == nil then
				stats[category] = self:GetPerfStat(category)
			end
		end
	end
	return stats
end

function FairyGuiProfiler:DumpPerfStats()
	local stats = self:GetPerfStats()
	for _, category in ipairs({ "open", "close", "event", "loadPackage", "createObject", "service" }) do
		local stat = stats[category]
		print("[FGUI] PerfStat", category, "count=", stat.count, "success=", stat.success, "fail=", stat.fail, "avgMs=", formatMs(stat.avgMs), "maxMs=", formatMs(stat.maxMs), "lastMs=", formatMs(stat.lastMs), "last=", stat.lastName or "", "max=", stat.maxName or "")
	end
	return stats
end

function FairyGuiProfiler:GetDebugStats()
	local owner = self.owner
	if owner == nil then
		return {
			openUI = 0,
			hiddenUI = 0,
			package = 0,
			layerRoot = 0,
			binding = 0,
			transitionCallback = 0,
			timer = 0,
			objectHandle = 0,
			childCache = 0,
			view = 0,
			controller = 0,
		}
	end

	local childCacheCount = 0
	for _, children in pairs(owner.childrenByHandle or {}) do
		childCacheCount = childCacheCount + tableCount(children)
	end

	local packageCount = 0
	local printed = {}
	for _, packageInfo in pairs(owner.packagesByName or {}) do
		if packageInfo ~= nil and printed[packageInfo.packageName] ~= true then
			packageCount = packageCount + 1
			printed[packageInfo.packageName] = true
		end
	end

	return {
		openUI = tableCount(owner.objects),
		hiddenUI = tableCount(owner.hiddenObjects),
		package = packageCount,
		layerRoot = tableCount(owner.layerRoots),
		binding = tableCount(owner.bindings),
		transitionCallback = tableCount(owner.transitionCallbacks),
		timer = tableCount(owner.timers),
		objectHandle = tableCount(owner.objectsByHandle),
		childCache = childCacheCount,
		view = tableCount(owner.views),
		controller = tableCount(owner.controllers),
	}
end

function FairyGuiProfiler:DumpDebugStats()
	local stats = self:GetDebugStats()
	print("[FGUI] DebugStats openUI=", stats.openUI, "hiddenUI=", stats.hiddenUI, "package=", stats.package, "layerRoot=", stats.layerRoot, "binding=", stats.binding, "transitionCallback=", stats.transitionCallback, "timer=", stats.timer, "objectHandle=", stats.objectHandle, "childCache=", stats.childCache, "view=", stats.view, "controller=", stats.controller)
end

function FairyGuiProfiler:PublishTracyCounters(healthStats, serviceMeta)
	if GameManager == nil or GameManager.plotFairyGuiServiceStats == nil then
		return false
	end

	healthStats = healthStats or {}
	serviceMeta = serviceMeta or {}
	return GameManager:plotFairyGuiServiceStats(
		tonumber(serviceMeta.serviceOpenTotal) or 0,
		tonumber(serviceMeta.serviceKindCount) or 0,
		tonumber(serviceMeta.toastQueue) or 0,
		tonumber(serviceMeta.loadingRefTotal) or 0,
		tonumber(serviceMeta.createdTotal) or 0,
		tonumber(serviceMeta.closedTotal) or 0,
		tonumber(serviceMeta.failedTotal) or 0,
		tonumber(serviceMeta.peakOpen) or 0)
end

function FairyGuiProfiler:GetHealthStats()
	local owner = self.owner
	local debugStats = self:GetDebugStats()
	local renderStats = self:GetRenderStats()
	local perfStats = self:GetPerfStats()
	local serviceStats = owner ~= nil and owner:GetServiceStats() or { __meta = {} }
	local serviceMeta = serviceStats.__meta or {}
	local threadTimerCount = 0
	if threadpool ~= nil and threadpool.get_timer_count ~= nil then
		threadTimerCount = threadpool:get_timer_count()
	end

	local stats = {
		openUI = debugStats.openUI,
		hiddenUI = debugStats.hiddenUI,
		package = debugStats.package,
		layerRoot = debugStats.layerRoot,
		binding = debugStats.binding,
		transitionCallback = debugStats.transitionCallback,
		timer = debugStats.timer,
		threadTimer = threadTimerCount,
		objectHandle = debugStats.objectHandle,
		childCache = debugStats.childCache,
		view = debugStats.view,
		controller = debugStats.controller,
		focusedHandle = owner ~= nil and owner.GetFocusedHandle ~= nil and owner:GetFocusedHandle() or nil,
		commandCount = renderStats.commandCount,
		triangleCount = renderStats.triangleCount,
		materialCount = renderStats.materialCount,
		textureCount = renderStats.textureCount,
		materialAliasCount = renderStats.materialAliasCount,
		textureAliasCount = renderStats.textureAliasCount,
		runtimeObjectHandle = renderStats.runtimeObjectHandle,
		runtimeBinding = renderStats.runtimeBinding,
		eventDispatchTotal = owner ~= nil and owner.eventDispatchTotal or 0,
		openPerfCount = perfStats.open.count,
		openAvgMs = perfStats.open.avgMs,
		openMaxMs = perfStats.open.maxMs,
		closePerfCount = perfStats.close.count,
		closeAvgMs = perfStats.close.avgMs,
		closeMaxMs = perfStats.close.maxMs,
		eventAvgMs = perfStats.event.avgMs,
		eventMaxMs = perfStats.event.maxMs,
		loadPackageAvgMs = perfStats.loadPackage.avgMs,
		loadPackageMaxMs = perfStats.loadPackage.maxMs,
		serviceOpenTotal = serviceMeta.serviceOpenTotal or 0,
		serviceHiddenTotal = serviceMeta.serviceHiddenTotal or 0,
		serviceKindCount = serviceMeta.serviceKindCount or 0,
		serviceCreatedTotal = serviceMeta.createdTotal or 0,
		serviceClosedTotal = serviceMeta.closedTotal or 0,
		serviceFailedTotal = serviceMeta.failedTotal or 0,
		servicePeakOpen = serviceMeta.peakOpen or 0,
		toastQueue = serviceMeta.toastQueue or 0,
		loadingRefTotal = serviceMeta.loadingRefTotal or 0,
		servicePerfCount = perfStats.service.count,
		serviceAvgMs = perfStats.service.avgMs,
		serviceMaxMs = perfStats.service.maxMs,
	}
	self:PublishTracyCounters(stats, serviceMeta)
	return stats
end

function FairyGuiProfiler:FindFocusOwner(focusedHandle)
	local owner = self.owner
	if owner == nil or focusedHandle == nil or focusedHandle <= 0 then
		return nil
	end

	local directOwner = owner.objectsByHandle ~= nil and owner.objectsByHandle[focusedHandle] or nil
	if directOwner ~= nil then
		return directOwner
	end

	for _, objectInfo in pairs(owner.objects or {}) do
		local _, handleSet = owner:CollectOwnedHandles(objectInfo)
		if handleSet ~= nil and handleSet[focusedHandle] == true then
			return objectInfo
		end
	end
	return nil
end

function FairyGuiProfiler:CollectLayerSummary()
	local owner = self.owner
	local summaries = {}
	if owner == nil then
		return summaries
	end

	local counts = {}
	for layerName, _ in pairs(owner.layers or {}) do
		counts[layerName] = counts[layerName] or { visible = 0, hidden = 0, root = owner:GetLayerRoot(layerName) }
	end
	for key, objectInfo in pairs(owner.objects or {}) do
		local layerName = objectInfo.layer or "Normal"
		local info = counts[layerName]
		if info == nil then
			info = { visible = 0, hidden = 0, root = owner:GetLayerRoot(layerName) }
			counts[layerName] = info
		end
		if owner.hiddenObjects[key] ~= nil then
			info.hidden = info.hidden + 1
		else
			info.visible = info.visible + 1
		end
	end

	for layerName, info in pairs(counts) do
		table.insert(summaries, {
			layer = layerName,
			baseOrder = owner:GetLayerBaseOrder(layerName),
			root = info.root,
			visible = info.visible,
			hidden = info.hidden,
		})
	end
	table.sort(summaries, function(a, b)
		return (tonumber(a.baseOrder) or 0) < (tonumber(b.baseOrder) or 0)
	end)
	return summaries
end

function FairyGuiProfiler:CollectBindingSummary(maxCount)
	local owner = self.owner
	local byType = {}
	if owner == nil then
		return {}
	end
	for _, binding in pairs(owner.bindings or {}) do
		local eventType = tostring(binding.eventType or "")
		byType[eventType] = (byType[eventType] or 0) + 1
	end

	local summaries = {}
	for eventType, count in pairs(byType) do
		table.insert(summaries, {
			eventType = eventType,
			count = count,
		})
	end
	table.sort(summaries, function(a, b)
		if a.count == b.count then
			return a.eventType < b.eventType
		end
		return a.count > b.count
	end)
	if maxCount ~= nil and #summaries > maxCount then
		local limited = {}
		for index = 1, maxCount do
			limited[index] = summaries[index]
		end
		return limited
	end
	return summaries
end

function FairyGuiProfiler:CollectPackageSummary(maxCount)
	local owner = self.owner
	local packageRefs = owner ~= nil and owner:GetPackageRefs() or {}
	local summaries = {}
	for packageKey, ref in pairs(packageRefs or {}) do
		table.insert(summaries, {
			packageKey = packageKey,
			packageName = ref.packageName,
			refCount = ref.refCount or 0,
			openCount = ref.openCount or 0,
			hiddenCount = ref.hiddenCount or 0,
			cacheCount = ref.cacheCount or 0,
			stackCount = ref.stackCount or 0,
			preloaded = ref.preloaded == true,
		})
	end
	table.sort(summaries, function(a, b)
		if a.openCount == b.openCount then
			return tostring(a.packageKey) < tostring(b.packageKey)
		end
		return a.openCount > b.openCount
	end)
	if maxCount ~= nil and #summaries > maxCount then
		local limited = {}
		for index = 1, maxCount do
			limited[index] = summaries[index]
		end
		return limited
	end
	return summaries
end

function FairyGuiProfiler:GetDebugPanelSnapshot(options)
	local owner = self.owner
	options = options or {}
	local health = self:GetHealthStats()
	local perf = self:GetPerfStats()
	local render = self:GetRenderStats()
	local eventStats = owner ~= nil and owner:GetEventStats() or { total = 0, events = {}, lastEvent = nil }
	local warnings = owner ~= nil and owner:GetResourceWarnings() or {}
	local focusOwner = self:FindFocusOwner(health.focusedHandle)
	local topUI = owner ~= nil and owner:GetTopStackObject(owner.uiStack or {}) or nil
	local topPopup = owner ~= nil and owner:GetTopStackObject(owner.popupStack or {}) or nil
	local openObjects = sortedObjectInfos(owner, false)
	local allObjects = sortedObjectInfos(owner, true)
	local hiddenObjects = {}
	if owner ~= nil then
		for _, objectInfo in ipairs(allObjects) do
			if owner.hiddenObjects[objectInfo.key] ~= nil then
				table.insert(hiddenObjects, objectInfo)
			end
		end
	end

	return {
		health = health,
		perf = perf,
		render = render,
		eventStats = eventStats,
		resourceWarnings = warnings,
		focusOwner = focusOwner,
		topUI = topUI,
		topPopup = topPopup,
		openObjects = openObjects,
		hiddenObjects = hiddenObjects,
		layerSummary = self:CollectLayerSummary(),
		bindingSummary = self:CollectBindingSummary(options.maxBindings or 6),
		packageSummary = self:CollectPackageSummary(options.maxPackages or 5),
	}
end

function FairyGuiProfiler:DumpHealth(verbose)
	local stats = self:GetHealthStats()
	print("[FGUI] Health openUI=", stats.openUI, "hiddenUI=", stats.hiddenUI, "package=", stats.package, "layerRoot=", stats.layerRoot, "binding=", stats.binding, "transitionCallback=", stats.transitionCallback, "timer=", stats.timer, "threadTimer=", stats.threadTimer, "objectHandle=", stats.objectHandle, "childCache=", stats.childCache, "view=", stats.view, "controller=", stats.controller, "focusedHandle=", stats.focusedHandle, "runtimeObjectHandle=", stats.runtimeObjectHandle, "runtimeBinding=", stats.runtimeBinding, "eventTotal=", stats.eventDispatchTotal, "material=", stats.materialCount, "texture=", stats.textureCount, "materialAlias=", stats.materialAliasCount, "textureAlias=", stats.textureAliasCount, "commandCount=", stats.commandCount, "triangleCount=", stats.triangleCount, "service=", tostring(stats.serviceOpenTotal) .. "/" .. tostring(stats.serviceKindCount) .. "/" .. tostring(stats.servicePeakOpen), "toastQueue=", stats.toastQueue, "loadingRefs=", stats.loadingRefTotal, "openPerf=", tostring(stats.openPerfCount) .. "/" .. formatMs(stats.openAvgMs) .. "/" .. formatMs(stats.openMaxMs), "closePerf=", tostring(stats.closePerfCount) .. "/" .. formatMs(stats.closeAvgMs) .. "/" .. formatMs(stats.closeMaxMs), "eventMs=", formatMs(stats.eventAvgMs) .. "/" .. formatMs(stats.eventMaxMs), "loadPackageMs=", formatMs(stats.loadPackageAvgMs) .. "/" .. formatMs(stats.loadPackageMaxMs), "serviceMs=", formatMs(stats.serviceAvgMs) .. "/" .. formatMs(stats.serviceMaxMs))
	local owner = self.owner
	if verbose == true and owner ~= nil then
		self:DumpPerfStats()
		owner:DumpResourceRefs()
		owner:DumpResourceWarnings()
		owner:DumpStacks()
		owner:DumpLayerRoots()
	end
	return stats
end

function FairyGuiProfiler:BuildDebugPanelLines(options)
	local owner = self.owner
	options = options or {}
	local snapshot = self:GetDebugPanelSnapshot(options)
	local health = snapshot.health
	local perf = snapshot.perf
	local render = snapshot.render
	local eventStats = snapshot.eventStats or { total = 0, events = {}, lastEvent = nil }
	local lastEvent = eventStats.lastEvent
	local lines = {
		string.format("UI open=%s hidden=%s pkg=%s obj=%s layer=%s", tostring(health.openUI), tostring(health.hiddenUI), tostring(health.package), tostring(health.objectHandle), tostring(health.layerRoot)),
		string.format("Top ui=%s popup=%s focus=%s owner=%s", objectBrief(owner, snapshot.topUI), objectBrief(owner, snapshot.topPopup), tostring(health.focusedHandle or 0), snapshot.focusOwner ~= nil and tostring(snapshot.focusOwner.key or snapshot.focusOwner.uiName or "") or "-"),
		string.format("Life binding=%s timer=%s thread=%s child=%s ctrl=%s view=%s", tostring(health.binding), tostring(health.timer), tostring(health.threadTimer), tostring(health.childCache), tostring(health.controller), tostring(health.view)),
		string.format("Render cmd=%s tri=%s mat=%s/%s tex=%s/%s", tostring(health.commandCount), tostring(health.triangleCount), tostring(health.materialCount), tostring(health.materialAliasCount), tostring(health.textureCount), tostring(health.textureAliasCount)),
		string.format("Stacks ui=%s popup=%s warnings=%s eventTotal=%s", tostring(owner ~= nil and #(owner.uiStack or {}) or 0), tostring(owner ~= nil and #(owner.popupStack or {}) or 0), tostring(#(snapshot.resourceWarnings or {})), tostring(eventStats.total or 0)),
		string.format("Last event=%s root=%s sender=%s item=%s xy=%s,%s", lastEvent ~= nil and tostring(lastEvent.eventType) or "-", lastEvent ~= nil and tostring(lastEvent.rootHandle) or "-", lastEvent ~= nil and tostring(lastEvent.senderHandle) or "-", lastEvent ~= nil and tostring(lastEvent.itemHandle or "") or "-", lastEvent ~= nil and tostring(lastEvent.x or "") or "-", lastEvent ~= nil and tostring(lastEvent.y or "") or "-"),
		string.format("Perf open %s avg/max=%s/%s", tostring(perf.open.count), formatMs(perf.open.avgMs), formatMs(perf.open.maxMs)),
		string.format("Perf close %s avg/max=%s/%s", tostring(perf.close.count), formatMs(perf.close.avgMs), formatMs(perf.close.maxMs)),
		string.format("Perf event %s avg/max=%s/%s", tostring(perf.event.count), formatMs(perf.event.avgMs), formatMs(perf.event.maxMs)),
		string.format("Load pkg %s avg/max=%s/%s svc %s avg/max=%s/%s", tostring(perf.loadPackage.count), formatMs(perf.loadPackage.avgMs), formatMs(perf.loadPackage.maxMs), tostring(perf.service.count), formatMs(perf.service.avgMs), formatMs(perf.service.maxMs)),
		string.format("Svc open=%s kind=%s peak=%s total=%s/%s/%s tq=%s lr=%s", tostring(health.serviceOpenTotal), tostring(health.serviceKindCount), tostring(health.servicePeakOpen), tostring(health.serviceCreatedTotal), tostring(health.serviceClosedTotal), tostring(health.serviceFailedTotal), tostring(health.toastQueue), tostring(health.loadingRefTotal)),
	}

	local layerBriefs = {}
	for _, layer in ipairs(snapshot.layerSummary or {}) do
		if layer.visible > 0 or layer.hidden > 0 or layer.root ~= nil then
			pushLimited(layerBriefs, string.format("%s=%s/%s", tostring(layer.layer), tostring(layer.visible), tostring(layer.hidden)), options.maxLayerBriefs or 6)
		end
	end
	table.insert(lines, string.format("Layers %s", joinBriefs(layerBriefs)))

	local bindingBriefs = {}
	for _, binding in ipairs(snapshot.bindingSummary or {}) do
		pushLimited(bindingBriefs, tostring(binding.eventType) .. "=" .. tostring(binding.count), options.maxBindingBriefs or 5)
	end
	table.insert(lines, string.format("Bindings %s", joinBriefs(bindingBriefs)))

	local openBriefs = {}
	for _, objectInfo in ipairs(snapshot.openObjects or {}) do
		pushLimited(openBriefs, objectBrief(owner, objectInfo), options.maxOpenBriefs or 4)
	end
	table.insert(lines, clampText("Open " .. joinBriefs(openBriefs), options.lineMaxLen or 96))

	local hiddenBriefs = {}
	for _, objectInfo in ipairs(snapshot.hiddenObjects or {}) do
		pushLimited(hiddenBriefs, objectBrief(owner, objectInfo), options.maxHiddenBriefs or 3)
	end
	table.insert(lines, clampText("Hidden " .. joinBriefs(hiddenBriefs), options.lineMaxLen or 96))

	local packageBriefs = {}
	for _, packageInfo in ipairs(snapshot.packageSummary or {}) do
		pushLimited(packageBriefs, string.format("%s r%s o%s h%s", tostring(packageInfo.packageKey), tostring(packageInfo.refCount), tostring(packageInfo.openCount), tostring(packageInfo.hiddenCount)), options.maxPackageBriefs or 3)
	end
	table.insert(lines, clampText("Packages " .. joinBriefs(packageBriefs), options.lineMaxLen or 96))

	local warningBriefs = {}
	for _, warning in ipairs(snapshot.resourceWarnings or {}) do
		pushLimited(warningBriefs, tostring(warning.kind) .. ":" .. tostring(warning.packageKey), options.maxWarningBriefs or 2)
	end
	table.insert(lines, clampText("Warnings " .. joinBriefs(warningBriefs, "none"), options.lineMaxLen or 96))

	if not isBlank(render.materialDetail) then
		table.insert(lines, clampText("Mat " .. render.materialDetail, options.lineMaxLen or 96))
	end
	if not isBlank(render.textureDetail) then
		table.insert(lines, clampText("Tex " .. render.textureDetail, options.lineMaxLen or 96))
	end

	local lineCount = tonumber(options.lineCount)
	if lineCount ~= nil and lineCount > 0 and #lines > lineCount then
		local limited = {}
		for index = 1, lineCount do
			limited[index] = lines[index]
		end
		return limited
	end
	return lines
end

function FairyGuiProfiler:RefreshDebugPanel(key)
	local owner = self.owner
	if owner == nil then
		return false
	end

	local objectInfo = owner:GetObjectInfo(key or "__DebugPanel")
	if objectInfo == nil then
		return false
	end

	if objectInfo.debugPanelTitleHandle ~= nil then
		owner:SetText(objectInfo.debugPanelTitleHandle, nil, string.format("%s  %s", tostring(objectInfo.param.title or "FGUI Debug"), os.date and os.date("%H:%M:%S") or ""))
	end

	local lineHandles = objectInfo.debugPanelLineHandles or {}
	local lines = self:BuildDebugPanelLines({
		lineCount = #lineHandles,
		lineMaxLen = objectInfo.param.lineMaxLen or 96,
	})
	for index, handle in ipairs(lineHandles) do
		owner:SetText(handle, nil, lines[index] or "")
	end
	return true
end

function FairyGuiProfiler:ShowDebugPanel(param)
	local owner = self.owner
	if owner == nil then
		return nil
	end

	param = copyTable(param)
	param.key = param.key or "__DebugPanel"
	param.layer = param.layer or "Top"
	param.stackMode = param.stackMode or "Popup"
	param.popupGroup = param.popupGroup or "DebugPanel"
	param.popupMode = param.popupMode or "replace"
	param.width = param.width or 640
	param.lineCount = param.lineCount or 16
	param.lineHeight = param.lineHeight or 21
	param.height = param.height or math.max(236, 54 + param.lineCount * param.lineHeight)
	param.top = param.top or 24
	param.right = param.right or 24
	param.modal = false
	param.touchable = param.touchable ~= false
	param.closeOnEscape = false
	param.closeOnSceneChange = param.closeOnSceneChange == true
	param.serviceType = "DebugPanel"

	local objectInfo = owner:OpenServiceContainer(param.key, param)
	if objectInfo == nil then
		return nil
	end

	objectInfo.debugPanelBackgroundHandle = owner:AddServiceImage(objectInfo, "debug_panel_bg", param.background or "res/assets/act_38/_imgs/board_task.png", 0, 0, param.width, param.height, param.alpha or 0.92)
	objectInfo.debugPanelTitleHandle = owner:AddServiceText(objectInfo, "debug_panel_title", param.title or "FGUI Debug", 18, 12, param.width - 88, 28, 20, 255, 236, 180)
	objectInfo.debugPanelLineHandles = {}
	for index = 1, param.lineCount do
		objectInfo.debugPanelLineHandles[index] = owner:AddServiceText(objectInfo, "debug_panel_line_" .. tostring(index), "", 18, 42 + (index - 1) * param.lineHeight, param.width - 36, param.lineHeight, 15, 210, 235, 255)
	end
	owner:AddServiceButton(objectInfo, "debug_panel_close", "X", param.width - 60, 10, 42, 30, function()
		self:HideDebugPanel(param.key)
	end)
	owner:ApplyServiceLayout(objectInfo)
	self:RefreshDebugPanel(param.key)

	if param.autoRefresh ~= false then
		objectInfo.debugPanelTimer = owner:AddTimer(param.key, param.duration or 3600, param.refreshInterval or 1, function()
			self:RefreshDebugPanel(param.key)
		end)
	end
	return objectInfo.handle
end

function FairyGuiProfiler:HideDebugPanel(key)
	local owner = self.owner
	if owner == nil then
		return false
	end
	return owner:Close(key or "__DebugPanel", true, "hideDebugPanel")
end
