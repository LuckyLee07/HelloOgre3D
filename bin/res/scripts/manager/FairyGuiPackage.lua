local FairyGuiPackage = Class("FairyGuiPackage")

local function isBlank(value)
	return value == nil or value == ""
end

local function normalizePath(path)
	if type(path) ~= "string" then
		return ""
	end
	return string.gsub(path, "\\", "/")
end

local function stripPackageExtension(path)
	return string.gsub(path, "%.fui$", "")
end

local function nowMs()
	return os.clock and os.clock() * 1000 or 0
end

local function addSetValue(set, value)
	if set == nil or isBlank(value) then
		return
	end
	value = tostring(value)
	for item in string.gmatch(value, "[^,%s]+") do
		if not isBlank(item) then
			set[item] = true
		end
	end
end

local function addSetValues(set, values)
	if set == nil or values == nil then
		return
	end
	if type(values) == "table" then
		if #values > 0 then
			for _, value in ipairs(values) do
				addSetValue(set, value)
			end
		else
			for _, value in pairs(values) do
				addSetValue(set, value)
			end
		end
	else
		addSetValue(set, values)
	end
end

local function setHasValue(set, value)
	return set ~= nil and value ~= nil and set[tostring(value)] == true
end

local function setToList(set)
	local list = {}
	if type(set) ~= "table" then
		return list
	end
	for value, enabled in pairs(set) do
		if enabled == true then
			table.insert(list, value)
		end
	end
	table.sort(list)
	return list
end

local function setToString(set)
	return table.concat(setToList(set), ",")
end

local function mergePackageMeta(packageInfo, meta)
	if packageInfo == nil or type(meta) ~= "table" then
		return packageInfo
	end

	packageInfo.groups = packageInfo.groups or {}
	packageInfo.tags = packageInfo.tags or {}
	packageInfo.scenes = packageInfo.scenes or {}
	packageInfo.sourceUIs = packageInfo.sourceUIs or {}

	addSetValues(packageInfo.groups, meta.group)
	addSetValues(packageInfo.groups, meta.groupName)
	addSetValues(packageInfo.groups, meta.packageGroup)
	addSetValues(packageInfo.tags, meta.tag)
	addSetValues(packageInfo.tags, meta.tags)
	addSetValues(packageInfo.tags, meta.packageTag)
	addSetValues(packageInfo.tags, meta.packageTags)
	addSetValues(packageInfo.scenes, meta.scene)
	addSetValues(packageInfo.scenes, meta.sceneName)
	addSetValues(packageInfo.sourceUIs, meta.uiName)

	if meta.preloaded == true then
		packageInfo.preloaded = true
		packageInfo.preloadCount = (packageInfo.preloadCount or 0) + 1
	end
	return packageInfo
end

function FairyGuiPackage:Init(owner)
	self.owner = owner
end

function FairyGuiPackage:GetPackageNameByPath(packagePath)
	packagePath = normalizePath(packagePath)
	packagePath = stripPackageExtension(packagePath)

	return string.match(packagePath, "([^/]+)$") or packagePath
end

function FairyGuiPackage:SetPackageRoot(packageRoot)
	local owner = self.owner
	if owner == nil then
		return
	end

	packageRoot = stripPackageExtension(normalizePath(packageRoot))
	packageRoot = string.gsub(packageRoot, "/$", "")
	if not isBlank(packageRoot) then
		owner.packageRoot = packageRoot
	end
end

function FairyGuiPackage:ResolvePackagePath(packagePath)
	local owner = self.owner
	packagePath = stripPackageExtension(normalizePath(packagePath))
	if isBlank(packagePath) then
		return ""
	end
	if string.find(packagePath, "/", 1, true) ~= nil or string.find(packagePath, ":", 1, true) ~= nil then
		return packagePath
	end
	return (owner ~= nil and owner.packageRoot or "res/fuires") .. "/" .. packagePath
end

function FairyGuiPackage:AddPackage(packageList)
	if type(packageList) == "string" then
		return self:LoadPackage(packageList)
	end

	if type(packageList) ~= "table" then
		return nil
	end

	local lastPackageName = nil
	if #packageList > 0 then
		for index = 1, #packageList do
			lastPackageName = self:LoadPackage(packageList[index])
		end
	else
		for _, packagePath in pairs(packageList) do
			lastPackageName = self:LoadPackage(packagePath)
		end
	end
	return lastPackageName
end

function FairyGuiPackage:LoadPackage(packagePath, packageName, meta)
	local owner = self.owner
	if owner == nil or not owner:IsAvailable() then
		return nil
	end

	packagePath = self:ResolvePackagePath(packagePath)
	if isBlank(packagePath) then
		return nil
	end

	local packageInfo = owner.packages[packagePath]
	if packageInfo then
		mergePackageMeta(packageInfo, meta)
		return packageInfo.packageName
	end

	local startMs = nowMs()
	local loadedPackageName = GameManager:loadFairyGuiPackage(packagePath)
	owner:RecordPerf("loadPackage", nowMs() - startMs, packagePath, not isBlank(loadedPackageName))
	if isBlank(loadedPackageName) then
		print("[FGUI] load package failed:", packagePath)
		return nil
	end

	local resolvedPackageName = packageName
	if isBlank(resolvedPackageName) then
		resolvedPackageName = loadedPackageName
	end
	if isBlank(resolvedPackageName) then
		resolvedPackageName = self:GetPackageNameByPath(packagePath)
	end

	packageInfo = {
		packagePath = packagePath,
		packageName = resolvedPackageName,
		loadedPackageName = loadedPackageName,
		refCount = 0,
		groups = {},
		tags = {},
		scenes = {},
		sourceUIs = {},
		preloaded = false,
		preloadCount = 0,
	}
	mergePackageMeta(packageInfo, meta)
	owner.packages[packagePath] = packageInfo
	owner.packages[resolvedPackageName] = packageInfo
	owner.packagesByName[resolvedPackageName] = packageInfo
	return resolvedPackageName
end

function FairyGuiPackage:RetainPackage(packagePath, packageName)
	local owner = self.owner
	if owner == nil then
		return nil
	end

	packagePath = self:ResolvePackagePath(packagePath)
	local packageInfo = owner.packages[packagePath] or owner.packages[packageName]
	if packageInfo == nil then
		return nil
	end

	packageInfo.refCount = (packageInfo.refCount or 0) + 1
	return packageInfo
end

function FairyGuiPackage:RemovePackageInfo(packageInfo)
	local owner = self.owner
	if owner == nil or packageInfo == nil then
		return false
	end

	if GameManager ~= nil and GameManager.removeFairyGuiPackage ~= nil then
		GameManager:removeFairyGuiPackage(packageInfo.loadedPackageName or packageInfo.packageName or "")
	end
	owner.packages[packageInfo.packagePath] = nil
	owner.packages[packageInfo.packageName] = nil
	owner.packagesByName[packageInfo.packageName] = nil
	return true
end

function FairyGuiPackage:ReleasePackage(packagePath, packageName, unloadWhenZero)
	local owner = self.owner
	if owner == nil then
		return false
	end

	packagePath = self:ResolvePackagePath(packagePath)
	local packageInfo = owner.packages[packagePath] or owner.packages[packageName]
	if packageInfo == nil then
		return false
	end

	packageInfo.refCount = math.max((packageInfo.refCount or 0) - 1, 0)
	if packageInfo.refCount > 0 or unloadWhenZero ~= true then
		return true
	end

	return self:RemovePackageInfo(packageInfo)
end

function FairyGuiPackage:PreloadPackage(packagePath, packageName, meta)
	meta = meta or {}
	meta.preloaded = true
	return self:LoadPackage(packagePath, packageName, meta)
end

function FairyGuiPackage:MatchesConfigScene(config, sceneName)
	if type(config) ~= "table" then
		return false
	end
	local configScene = config.scene or config.sceneName or "Default"
	if configScene == sceneName then
		return true
	end
	if config.preloadScene == sceneName then
		return true
	end
	local preloadScenes = config.preloadScenes
	if type(preloadScenes) == "table" then
		for _, preloadScene in pairs(preloadScenes) do
			if preloadScene == sceneName then
				return true
			end
		end
	end
	return false
end

function FairyGuiPackage:MatchesConfigFilter(config, options)
	if type(config) ~= "table" then
		return false
	end
	options = options or {}
	if options.groupName ~= nil and config.group ~= options.groupName and config.uiGroup ~= options.groupName and config.packageGroup ~= options.groupName then
		return false
	end
	if options.tagName ~= nil then
		local tags = {}
		addSetValues(tags, config.packageTags)
		addSetValues(tags, config.packageTag)
		addSetValues(tags, config.tags)
		addSetValues(tags, config.tag)
		if not setHasValue(tags, options.tagName) then
			return false
		end
	end
	return true
end

function FairyGuiPackage:PreloadScene(sceneName, options)
	local owner = self.owner
	options = options or {}
	sceneName = sceneName or owner and owner.currentSceneName or "Default"
	local stats = {
		sceneName = sceneName,
		requested = 0,
		loaded = 0,
		skipped = 0,
		packages = {},
	}
	if owner == nil then
		return stats
	end

	for uiName, config in pairs(owner.uiRegistry or {}) do
		if type(config) == "table"
			and config.preload ~= false
			and self:MatchesConfigScene(config, sceneName)
			and self:MatchesConfigFilter(config, options) then
			local packagePath = config.packagePath or config.package
			if not isBlank(packagePath) then
				stats.requested = stats.requested + 1
				local packageName = self:PreloadPackage(packagePath, config.packageName, {
					uiName = uiName,
					group = config.group or config.uiGroup,
					packageGroup = config.packageGroup,
					tags = config.tags,
					tag = config.tag,
					packageTags = config.packageTags,
					packageTag = config.packageTag,
					scene = config.scene or config.sceneName,
				})
				if isBlank(packageName) then
					stats.skipped = stats.skipped + 1
				else
					stats.loaded = stats.loaded + 1
					stats.packages[packageName] = true
				end
			end
		end
	end
	print("[FGUI] PreloadScene", sceneName, "requested=", stats.requested, "loaded=", stats.loaded, "skipped=", stats.skipped)
	return stats
end

function FairyGuiPackage:CreateObject(packageName, objectName)
	local owner = self.owner
	if owner == nil or not owner:IsAvailable() or isBlank(packageName) or isBlank(objectName) then
		return 0
	end
	local startMs = nowMs()
	local handle = GameManager:createFairyGuiObject(packageName, objectName)
	owner:RecordPerf("createObject", nowMs() - startMs, tostring(packageName) .. "/" .. tostring(objectName), handle ~= nil and handle > 0)
	if handle == nil or handle <= 0 then
		print("[FGUI] create object failed:", packageName, objectName)
	end
	return handle
end

function FairyGuiPackage:DumpPackages()
	local owner = self.owner
	if owner == nil then
		return
	end

	print("[FGUI] DumpPackages begin")
	local printed = {}
	for _, packageInfo in pairs(owner.packagesByName or {}) do
		if packageInfo ~= nil and printed[packageInfo.packageName] ~= true then
			print("[FGUI] Package", packageInfo.packageName, "path=", packageInfo.packagePath, "refCount=", packageInfo.refCount or 0, "groups=", setToString(packageInfo.groups), "tags=", setToString(packageInfo.tags), "scenes=", setToString(packageInfo.scenes), "preloaded=", packageInfo.preloaded == true)
			printed[packageInfo.packageName] = true
		end
	end
	print("[FGUI] DumpPackages end")
end

function FairyGuiPackage:GetPackageRef(refsByPackage, packageInfo)
	if packageInfo == nil then
		return nil
	end
	local packageKey = packageInfo.packageName or packageInfo.packagePath
	return refsByPackage ~= nil and refsByPackage[packageKey] or nil
end

function FairyGuiPackage:MatchesPackageFilter(packageInfo, filter)
	if packageInfo == nil then
		return false
	end
	filter = filter or {}
	if filter.packageName ~= nil and packageInfo.packageName ~= filter.packageName then
		return false
	end
	if filter.packagePath ~= nil and packageInfo.packagePath ~= self:ResolvePackagePath(filter.packagePath) then
		return false
	end
	if filter.groupName ~= nil and not setHasValue(packageInfo.groups, filter.groupName) then
		return false
	end
	if filter.tagName ~= nil and not setHasValue(packageInfo.tags, filter.tagName) then
		return false
	end
	if filter.sceneName ~= nil and not setHasValue(packageInfo.scenes, filter.sceneName) then
		return false
	end
	if filter.preloaded ~= nil and (packageInfo.preloaded == true) ~= (filter.preloaded == true) then
		return false
	end
	return true
end

function FairyGuiPackage:CanUnloadPackage(packageInfo, refsByPackage, force)
	if packageInfo == nil then
		return false, "missing"
	end
	if force == true then
		return true
	end
	local ref = self:GetPackageRef(refsByPackage, packageInfo)
	local openCount = ref ~= nil and ref.openCount or 0
	local hiddenCount = ref ~= nil and ref.hiddenCount or 0
	local stackCount = ref ~= nil and ref.stackCount or 0
	local refCount = packageInfo.refCount or 0
	if refCount > 0 or openCount > 0 or hiddenCount > 0 or stackCount > 0 then
		return false, "inUse ref=" .. tostring(refCount) .. " open=" .. tostring(openCount) .. " hidden=" .. tostring(hiddenCount) .. " stack=" .. tostring(stackCount)
	end
	return true
end

function FairyGuiPackage:UnloadPackagesByFilter(filter)
	local owner = self.owner
	filter = filter or {}
	local stats = {
		matched = 0,
		unloaded = 0,
		skipped = 0,
		skippedDetails = {},
	}
	if owner == nil then
		return stats
	end

	local packages = {}
	local printed = {}
	for _, packageInfo in pairs(owner.packagesByName or {}) do
		if packageInfo ~= nil and printed[packageInfo.packageName] ~= true and self:MatchesPackageFilter(packageInfo, filter) then
			table.insert(packages, packageInfo)
			printed[packageInfo.packageName] = true
		end
	end

	local refsByPackage = self:GetPackageRefs()
	for _, packageInfo in ipairs(packages) do
		stats.matched = stats.matched + 1
		local canUnload, reason = self:CanUnloadPackage(packageInfo, refsByPackage, filter.force == true)
		if canUnload then
			if self:RemovePackageInfo(packageInfo) then
				stats.unloaded = stats.unloaded + 1
			else
				stats.skipped = stats.skipped + 1
			end
		else
			stats.skipped = stats.skipped + 1
			stats.skippedDetails[#stats.skippedDetails + 1] = {
				packageName = packageInfo.packageName,
				reason = reason,
			}
		end
	end
	print("[FGUI] UnloadPackages matched=", stats.matched, "unloaded=", stats.unloaded, "skipped=", stats.skipped)
	return stats
end

function FairyGuiPackage:UnloadPackageGroup(groupName, force)
	return self:UnloadPackagesByFilter({ groupName = groupName, force = force == true })
end

function FairyGuiPackage:UnloadPackageTag(tagName, force)
	return self:UnloadPackagesByFilter({ tagName = tagName, force = force == true })
end

function FairyGuiPackage:GetPackageRefs()
	local owner = self.owner
	local refsByPackage = {}
	if owner == nil then
		return refsByPackage
	end

	local printed = {}
	for _, packageInfo in pairs(owner.packagesByName or {}) do
		local packageKey = packageInfo ~= nil and (packageInfo.packageName or packageInfo.packagePath) or nil
		if packageInfo ~= nil and packageKey ~= nil and printed[packageKey] ~= true then
			refsByPackage[packageKey] = {
				packageName = packageInfo.packageName,
				packagePath = packageInfo.packagePath,
				refCount = packageInfo.refCount or 0,
				groups = setToList(packageInfo.groups),
				tags = setToList(packageInfo.tags),
				scenes = setToList(packageInfo.scenes),
				preloaded = packageInfo.preloaded == true,
				preloadCount = packageInfo.preloadCount or 0,
				openCount = 0,
				hiddenCount = 0,
				cacheCount = 0,
				stackCount = 0,
				stackedKeys = {},
				objects = {},
			}
			printed[packageKey] = true
		end
	end

	local function getRef(objectInfo)
		local packageName = objectInfo.packageName
		local packagePath = objectInfo.packagePath
		local packageInfo = packagePath ~= nil and owner.packages[packagePath] or nil
		if packageInfo == nil and packageName ~= nil then
			packageInfo = owner.packagesByName[packageName] or owner.packages[packageName]
		end
		local packageKey = packageInfo ~= nil and (packageInfo.packageName or packageInfo.packagePath) or (packageName or packagePath or "<dynamic>")
		local ref = refsByPackage[packageKey]
		if ref == nil then
			ref = {
				packageName = packageInfo ~= nil and packageInfo.packageName or packageName,
				packagePath = packageInfo ~= nil and packageInfo.packagePath or packagePath,
				refCount = packageInfo ~= nil and (packageInfo.refCount or 0) or 0,
				groups = packageInfo ~= nil and setToList(packageInfo.groups) or {},
				tags = packageInfo ~= nil and setToList(packageInfo.tags) or {},
				scenes = packageInfo ~= nil and setToList(packageInfo.scenes) or {},
				preloaded = packageInfo ~= nil and packageInfo.preloaded == true or false,
				preloadCount = packageInfo ~= nil and packageInfo.preloadCount or 0,
				openCount = 0,
				hiddenCount = 0,
				cacheCount = 0,
				stackCount = 0,
				stackedKeys = {},
				objects = {},
			}
			refsByPackage[packageKey] = ref
		end
		return ref
	end

	for key, objectInfo in pairs(owner.objects or {}) do
		local ref = getRef(objectInfo)
		ref.openCount = ref.openCount + 1
		if owner.hiddenObjects[key] ~= nil then
			ref.hiddenCount = ref.hiddenCount + 1
		end
		if objectInfo.cache == true then
			ref.cacheCount = ref.cacheCount + 1
		end
		table.insert(ref.objects, objectInfo)
	end

	for _, entry in ipairs(owner.uiStack or {}) do
		local objectInfo = entry ~= nil and owner.objects[entry.key] or nil
		if objectInfo ~= nil then
			local ref = getRef(objectInfo)
			ref.stackCount = ref.stackCount + 1
			ref.stackedKeys[objectInfo.key] = true
		end
	end
	for _, entry in ipairs(owner.popupStack or {}) do
		local objectInfo = entry ~= nil and owner.objects[entry.key] or nil
		if objectInfo ~= nil then
			local ref = getRef(objectInfo)
			ref.stackCount = ref.stackCount + 1
			ref.stackedKeys[objectInfo.key] = true
		end
	end
	return refsByPackage
end

function FairyGuiPackage:DumpResourceRefs()
	print("[FGUI] DumpResourceRefs begin")
	local refsByPackage = self:GetPackageRefs()
	for packageKey, ref in pairs(refsByPackage) do
		print("[FGUI] ResourcePackage", packageKey, "path=", ref.packagePath, "refCount=", ref.refCount, "open=", ref.openCount, "hidden=", ref.hiddenCount, "cache=", ref.cacheCount, "stackRefs=", ref.stackCount, "groups=", table.concat(ref.groups or {}, ","), "tags=", table.concat(ref.tags or {}, ","), "scenes=", table.concat(ref.scenes or {}, ","), "preloaded=", ref.preloaded == true, "preloadCount=", ref.preloadCount or 0)
		for _, objectInfo in ipairs(ref.objects) do
			print("[FGUI] ResourceUI", packageKey, "key=", objectInfo.key, "handle=", objectInfo.handle, "uiName=", objectInfo.uiName, "object=", objectInfo.objectName, "layer=", objectInfo.layer, "group=", objectInfo.uiGroup, "scene=", objectInfo.sceneName, "cache=", objectInfo.cache, "hidden=", self.owner.hiddenObjects[objectInfo.key] ~= nil)
		end
	end
	print("[FGUI] DumpResourceRefs end")
end

function FairyGuiPackage:GetResourceWarnings()
	local owner = self.owner
	local warnings = {}
	if owner == nil then
		return warnings
	end

	local refsByPackage = self:GetPackageRefs()
	local policy = owner.cachePolicy or {}
	local maxHiddenPerPackage = tonumber(policy.maxHiddenPerPackage) or 0
	local maxHiddenTotal = tonumber(policy.maxHiddenTotal) or 0
	local warnHiddenMs = (tonumber(policy.warnHiddenSeconds) or 0) * 1000
	local hiddenTotal = 0
	local currentMs = nowMs()
	for packageKey, ref in pairs(refsByPackage) do
		local isPackageRef = ref.packageName ~= nil or ref.packagePath ~= nil
		hiddenTotal = hiddenTotal + (ref.hiddenCount or 0)
		if isPackageRef and ref.openCount > 0 and ref.refCount < ref.openCount then
			table.insert(warnings, {
				kind = "refCountTooLow",
				packageKey = packageKey,
				detail = "refCount=" .. tostring(ref.refCount) .. " open=" .. tostring(ref.openCount),
			})
		end
		if isPackageRef and ref.refCount > ref.openCount and ref.openCount > 0 then
			table.insert(warnings, {
				kind = "refCountTooHigh",
				packageKey = packageKey,
				detail = "refCount=" .. tostring(ref.refCount) .. " open=" .. tostring(ref.openCount),
			})
		end
		if isPackageRef and ref.openCount <= 0 and ref.refCount > 0 then
			table.insert(warnings, {
				kind = "retainedWithoutOpen",
				packageKey = packageKey,
				detail = "refCount=" .. tostring(ref.refCount),
			})
		end
		if isPackageRef and ref.hiddenCount > 0 and ref.refCount <= 0 then
			table.insert(warnings, {
				kind = "hiddenWithoutRef",
				packageKey = packageKey,
				detail = "hidden=" .. tostring(ref.hiddenCount),
			})
		end
		if isPackageRef and maxHiddenPerPackage > 0 and ref.hiddenCount > maxHiddenPerPackage then
			table.insert(warnings, {
				kind = "hiddenCacheOverPackageBudget",
				packageKey = packageKey,
				detail = "hidden=" .. tostring(ref.hiddenCount) .. " max=" .. tostring(maxHiddenPerPackage),
			})
		end
		for _, objectInfo in ipairs(ref.objects) do
			if owner.hiddenObjects[objectInfo.key] == nil and ref.stackedKeys[objectInfo.key] ~= true and owner:GetStackMode(objectInfo) ~= "None" then
				table.insert(warnings, {
					kind = "visibleWithoutStack",
					packageKey = packageKey,
					detail = "key=" .. tostring(objectInfo.key),
				})
			end
			if owner.hiddenObjects[objectInfo.key] ~= nil and warnHiddenMs > 0 and objectInfo.cacheHiddenAtMs ~= nil and currentMs - objectInfo.cacheHiddenAtMs > warnHiddenMs then
				table.insert(warnings, {
					kind = "hiddenCacheLongLived",
					packageKey = packageKey,
					detail = "key=" .. tostring(objectInfo.key) .. " hiddenMs=" .. tostring(math.floor(currentMs - objectInfo.cacheHiddenAtMs)),
				})
			end
		end
	end
	if maxHiddenTotal > 0 and hiddenTotal > maxHiddenTotal then
		table.insert(warnings, {
			kind = "hiddenCacheOverTotalBudget",
			packageKey = "*",
			detail = "hidden=" .. tostring(hiddenTotal) .. " max=" .. tostring(maxHiddenTotal),
		})
	end
	return warnings
end

function FairyGuiPackage:DumpResourceWarnings()
	local warnings = self:GetResourceWarnings()
	print("[FGUI] DumpResourceWarnings count=", #warnings)
	for _, warning in ipairs(warnings) do
		print("[FGUI] ResourceWarning", warning.kind, "package=", warning.packageKey, warning.detail or "")
	end
	return #warnings == 0, warnings
end
