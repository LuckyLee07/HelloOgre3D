-- Exercise the real Lua -> C++ locomotion path without advancing physics while
-- test actors are displaced. Ordinary play never enables this test.
local Test = {}

function Test.Run(a, b)
	local function finite(v)
		return v ~= nil and v.x == v.x and v.y == v.y and v.z == v.z
			and math.abs(v.x) < math.huge and math.abs(v.y) < math.huge and math.abs(v.z) < math.huge
	end
	local positionA, positionB = a:GetPosition(), b:GetPosition()
	local velocityA, velocityB = a:GetVelocity(), b:GetVelocity()
	local ok, detail = pcall(function()
		local agents = ObjectManager:getAllAgents()
		local function force(agent)
			return agent:GetLocomotionComponent():ForceToSeparate(agents, 1.25, 180)
		end
		local function check(name, condition)
			print("[LocomotionSeparationSelfTest] case=" .. name .. " result=" .. tostring(condition))
			assert(condition, name)
		end
		a:setPosition(Vector3(0, 100, 0))
		b:setPosition(Vector3(0.5, 100, 0))
		local fa, fb = force(a), force(b)
		check("separated", finite(fa) and finite(fb) and fa.x < -0.99 and fb.x > 0.99)
		b:setPosition(Vector3(0, 100, 0))
		fa, fb = force(a), force(b)
		check("coincident", finite(fa) and finite(fb) and fa:squaredLength() > 0.99
			and (fa + fb):squaredLength() < 0.000001 and math.abs(fa.y) < 0.000001)
		check("repeatable", (force(a) - fa):squaredLength() < 0.000001)
		b:setPosition(Vector3(0.00001, 100, 0))
		fa, fb = force(a), force(b)
		check("near-coincident", finite(fa) and finite(fb) and fa.x < -0.99 and fb.x > 0.99)
		b:setPosition(Vector3(10, 100, 0))
		fa, fb = force(a), force(b)
		check("outside-neighborhood", finite(fa) and finite(fb)
			and fa:squaredLength() == 0 and fb:squaredLength() == 0)
	end)
	a:setPosition(positionA)
	b:setPosition(positionB)
	a:SetVelocity(velocityA)
	b:SetVelocity(velocityB)
	print("[LocomotionSeparationSelfTest] " .. (ok and "PASS" or "FAIL") .. " detail=" .. tostring(detail or "all"))
	return ok
end

return Test
