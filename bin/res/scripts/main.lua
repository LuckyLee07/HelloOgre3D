
local xman = ClassList.Male.new()
local fman = ClassList.Female.new()
print(xman)
print(xman.super)
print(fman.super)

function sayhello(str, num1, num2)
	print('Fxkk=====>>>', str, num1, num2)
	return num1+num2;
end

local xx = {8, nil, 9}
print('fxkk====>>>', unpack(xx))

funct = {
	func1 = function()
		print('fx==>>>1')
		func2()
	end,
	func2 = function()
		print('fx==>>>2')
	end,
}
print('fxkk=====>>>', funct)