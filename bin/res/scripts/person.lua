
local Person = Class("Person")
function Person:Init()
	self.age = 0
	self.name = "P"
end

function Person:GetAge()
	return self.age
end

function Person:GetName()
	return self.name
end

local Male = Class("Male", ClassList['Person'])
function Male:Init()
	self.age = 99
	self.sex = "male"
end

function Male:GetSex()
	return self.male
end

local Female = Class("Female", ClassList['Person'])
function Female:Init()
	self.age = 99
	self.sex = "female"
end

function Female:GetSex()
	return self.male
end

