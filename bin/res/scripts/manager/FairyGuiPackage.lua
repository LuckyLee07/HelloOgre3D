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

function FairyGuiPackage:LoadPackage(packagePath, packageName)
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
		return packageInfo.packageName
	end

	local startMs = nowMs()
	local loadedPackageName = GameManager:loadFairyGuiPackage(packagePath)
	owner:RecordPerf("loadPackage", nowMs() - startMs, packagePath, not isBlank(loadedPackageName))
	if isBlank(loadedPackageName) then
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
	}
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

	if GameManager ~= nil and GameManager.removeFairyGuiPackage ~= nil then
		GameManager:removeFairyGuiPackage(packageInfo.packageName or packageName or "")
	end
	owner.packages[packageInfo.packagePath] = nil
	owner.packages[packageInfo.packageName] = nil
	owner.packagesByName[packageInfo.packageName] = nil
	return true
end

function FairyGuiPackage:PreloadPackage(packagePath, packageName)
	return self:LoadPackage(packagePath, packageName)
end

function FairyGuiPackage:CreateObject(packageName, objectName)
	local owner = self.owner
	if owner == nil or not owner:IsAvailable() or isBlank(packageName) or isBlank(objectName) then
		return 0
	end
	local startMs = nowMs()
	local handle = GameManager:createFairyGuiObject(packageName, objectName)
	owner:RecordPerf("createObject", nowMs() - startMs, tostring(packageName) .. "/" .. tostring(objectName), handle ~= nil and handle > 0)
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
			print("[FGUI] Package", packageInfo.packageName, "path=", packageInfo.packagePath, "refCount=", packageInfo.refCount or 0)
			printed[packageInfo.packageName] = true
		end
	end
	print("[FGUI] DumpPackages end")
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
		print("[FGUI] ResourcePackage", packageKey, "path=", ref.packagePath, "refCount=", ref.refCount, "open=", ref.openCount, "hidden=", ref.hiddenCount, "cache=", ref.cacheCount, "stackRefs=", ref.stackCount)
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
	for packageKey, ref in pairs(refsByPackage) do
		local isPackageRef = ref.packageName ~= nil or ref.packagePath ~= nil
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
		for _, objectInfo in ipairs(ref.objects) do
			if owner.hiddenObjects[objectInfo.key] == nil and ref.stackedKeys[objectInfo.key] ~= true and owner:GetStackMode(objectInfo) ~= "None" then
				table.insert(warnings, {
					kind = "visibleWithoutStack",
					packageKey = packageKey,
					detail = "key=" .. tostring(objectInfo.key),
				})
			end
		end
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
