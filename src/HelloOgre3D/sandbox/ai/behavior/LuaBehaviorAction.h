#ifndef __LUA_BEHAVIOR_ACTION_H__
#define __LUA_BEHAVIOR_ACTION_H__

#include <string>

#include "BehaviorAction.h"
#include "object/LuaEnvObject.h"
#include "script/LuaClassNameTraits.h"

class SoldierObject;
class Blackboard;

// Lua 实现的 BT 叶动作 —— 跟 LuaDecisionAction 平行，不同点：
//   - OnUpdate Lua 返回值：1=RUNNING / 2=SUCCESS / 3=FAILURE（DT 那边只有 RUNNING/TERMINATED）
//   - 其余生命周期 hook 与 LuaDecisionAction 一致
class LuaBehaviorAction : public BehaviorAction //tolua_exports
	, public LuaEnvObject
{ //tolua_exports
public:
	LuaBehaviorAction(const std::string& name, SoldierObject* owner);
	virtual ~LuaBehaviorAction();

	//tolua_begin
	bool BindToScript(const std::string& filepath);
	SoldierObject* GetOwner() const { return m_owner; }
	//tolua_end

protected:
	virtual void   OnInitialize() override;
	virtual Status OnUpdate(float deltaMs) override;
	virtual void   OnCleanUp() override;

private:
	SoldierObject* m_owner;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(LuaBehaviorAction);

#endif  // __LUA_BEHAVIOR_ACTION_H__
