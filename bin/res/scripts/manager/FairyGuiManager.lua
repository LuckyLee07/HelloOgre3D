local FairyGuiManager = Class("FairyGuiManager")

local instance = nil

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

function FairyGuiManager:GetInst()
	if instance == nil then
		instance = ClassList.FairyGuiManager.instance()
	end
	return instance
end

function FairyGuiManager:Init()
	self.packageRoot = "res/fuires"
	self.packages = {}
	self.objects = {}
	self.objectsByHandle = {}
end

function FairyGuiManager:IsAvailable()
	if GameManager == nil then
		return false
	end
	return GameManager:isFairyGuiAvailable()
end

function FairyGuiManager:GetPackageNameByPath(packagePath)
	packagePath = normalizePath(packagePath)
	packagePath = stripPackageExtension(packagePath)

	return string.match(packagePath, "([^/]+)$") or packagePath
end

function FairyGuiManager:SetPackageRoot(packageRoot)
	packageRoot = stripPackageExtension(normalizePath(packageRoot))
	packageRoot = string.gsub(packageRoot, "/$", "")
	if not isBlank(packageRoot) then
		self.packageRoot = packageRoot
	end
end

function FairyGuiManager:ResolvePackagePath(packagePath)
	packagePath = stripPackageExtension(normalizePath(packagePath))
	if isBlank(packagePath) then
		return ""
	end
	if string.find(packagePath, "/", 1, true) ~= nil or string.find(packagePath, ":", 1, true) ~= nil then
		return packagePath
	end
	return self.packageRoot .. "/" .. packagePath
end

function FairyGuiManager:AddPackage(packageList)
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

function FairyGuiManager:LoadPackage(packagePath, packageName)
	if not self:IsAvailable() then
		return nil
	end

	packagePath = self:ResolvePackagePath(packagePath)
	if isBlank(packagePath) then
		return nil
	end

	local packageInfo = self.packages[packagePath]
	if packageInfo then
		return packageInfo.packageName
	end

	local loadedPackageName = GameManager:loadFairyGuiPackage(packagePath)
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
	}
	self.packages[packagePath] = packageInfo
	self.packages[resolvedPackageName] = packageInfo
	return resolvedPackageName
end

function FairyGuiManager:CreateObject(packageName, objectName)
	if not self:IsAvailable() or isBlank(packageName) or isBlank(objectName) then
		return 0
	end
	return GameManager:createFairyGuiObject(packageName, objectName)
end

function FairyGuiManager:OpenUI(name, packagePath, objectName, param)
	param = param or {}
	objectName = objectName or name

	local packageName = self:LoadPackage(packagePath, param.packageName)
	if isBlank(packageName) then
		return nil
	end

	local handle = self:CreateObject(packageName, objectName)
	if handle == nil or handle <= 0 then
		return nil
	end

	if not GameManager:addFairyGuiObjectToRoot(handle) then
		GameManager:removeFairyGuiObject(handle)
		return nil
	end

	if param.width ~= nil and param.height ~= nil then
		GameManager:setFairyGuiObjectSize(handle, param.width, param.height)
	end

	if param.x ~= nil and param.y ~= nil then
		GameManager:setFairyGuiObjectPosition(handle, param.x, param.y)
	elseif param.center then
		GameManager:centerFairyGuiObject(handle, param.restraint == true)
	end

	if param.visible ~= nil then
		GameManager:setFairyGuiObjectVisible(handle, param.visible == true)
	end

	local key = param.key or name or tostring(handle)
	local objectInfo = {
		handle = handle,
		key = key,
		name = name,
		packagePath = normalizePath(packagePath),
		packageName = packageName,
		objectName = objectName,
		param = param,
	}
	self.objects[key] = objectInfo
	self.objectsByHandle[handle] = objectInfo
	return handle
end

function FairyGuiManager:GetUI(key)
	local objectInfo = self.objects[key]
	return objectInfo and objectInfo.handle or nil
end

function FairyGuiManager:SetPosition(handle, x, y)
	if handle == nil then
		return false
	end
	return GameManager:setFairyGuiObjectPosition(handle, x, y)
end

function FairyGuiManager:SetSize(handle, width, height)
	if handle == nil then
		return false
	end
	return GameManager:setFairyGuiObjectSize(handle, width, height)
end

function FairyGuiManager:SetVisible(handle, visible)
	if handle == nil then
		return false
	end
	return GameManager:setFairyGuiObjectVisible(handle, visible == true)
end

function FairyGuiManager:CloseUI(keyOrHandle)
	local objectInfo = nil
	if type(keyOrHandle) == "number" then
		objectInfo = self.objectsByHandle[keyOrHandle]
	else
		objectInfo = self.objects[keyOrHandle]
	end

	if not objectInfo then
		return false
	end

	local handle = objectInfo.handle
	local removed = GameManager:removeFairyGuiObject(handle)
	self.objects[objectInfo.key] = nil
	self.objectsByHandle[handle] = nil
	return removed
end

function FairyGuiManager:CloseAll()
	if GameManager ~= nil then
		GameManager:clearFairyGuiObjects()
	end
	self.objects = {}
	self.objectsByHandle = {}
end

return FairyGuiManager
