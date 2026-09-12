require("res.scripts.agent.SoldierAgent.lua")
local Access=require("res.scripts.agent.AgentComponentAccess.lua")
local actor,anim,asm
local time=0
local sent={}
function Agent_Initialize(agent) end
function Agent_Update(agent,dt) end
function Agent_EventHandle(agent,key) end
function EventHandle_Keyboard(key,pressed) end
function EventHandle_Mouse(ctype,x,y,button) return false end
function EventHandle_WindowResized(w,h) end
local function snapshot(label)
 print(string.format('[AnimAudit] t=%d label=%s current=%s next=%s desired=%s clipTime=%.4f progress=%.4f',time,label,asm:GetCurrStateName(),asm:GetNextStateName(),asm:GetDesiredStateName(),asm:GetCurrStateTime(),asm:GetCurrStateProgress()))
end
function Sandbox_Initialize()
 SandboxAgentConfig:SetUseCppFsmFlag(true)
 SandboxScene:SetAmbientLight(Vector3(.5))
 local plane=SandboxObjects:CreatePlane(30,30)
 plane:setPosition(Vector3(0,0,0)); SandboxScene:setMaterial(plane,'Ground2')
 actor=SandboxObjects:CreateSoldier('models/futuristic_soldier/futuristic_soldier_dark_anim.mesh')
 actor:GetAIComponent():SetDriverByType('dt') -- Empty tree: probe exclusively owns intent.
 Soldier_InitSoldierAsm(actor)
 actor:initWeapon('models/futuristic_soldier/soldier_weapon.mesh'); Soldier_InitWeaponAsm(actor)
 actor:setPosition(Vector3(0,0,0))
 actor:SetForward(Vector3(0,3,10))
 local f=actor:GetForward()
 print(string.format('[AnimAudit] elevated_target_body_forward=(%.6f,%.6f,%.6f) pitch_degrees=%.3f',f.x,f.y,f.z,math.asin(f.y)*180/math.pi))
 actor:SetForward(Vector3(0,0,1))
 anim=Access.GetAnim(actor); asm=Access.GetBodyAsm(actor)
 SandboxCamera:GetCamera():setPosition(Vector3(0,2,-5))
 SandboxCamera:GetCamera():setOrientation(GetForward(Vector3(0,0,-1)))
 snapshot('ready')
end
function Sandbox_Update(dt)
 if not asm then return end
 time=time+dt
 if time>=330 and not sent.reload then sent.reload=true; anim:EnterReloadIntent();snapshot('request_reload') end
 if time>=363 and not sent.cancel then sent.cancel=true;snapshot('before_cancel');anim:EnterIdleIntent();snapshot('cancel_to_idle') end
 if time>=660 and not sent.aftercancel then sent.aftercancel=true;snapshot('after_cancel') end
 if time>=990 and not sent.fire then sent.fire=true;anim:EnterShootIntent();snapshot('request_fire') end
 if time>=1590 and not sent.deadline then sent.deadline=true;snapshot('600ms_after_fire_request') end
 if time%99<33 then snapshot('sample') end
 if time>=3300 then snapshot('done');GameManager:RequestQuit() end
end
