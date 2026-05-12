#ifndef __BEHAVIOR_ACTION_H__
#define __BEHAVIOR_ACTION_H__

#include <string>

#include "BehaviorNode.h"

// BT 叶动作基类。包装"首次 tick 时 Initialize → 每帧 Update → 终结时 CleanUp"的生命周期。
// 复用 DecisionAction 一套思路，但状态码扩到三态（SUCCESS / FAILURE / RUNNING），
// 让父 Composite 能区分成功失败。
//
// 子类只需实现 OnInitialize / OnUpdate / OnCleanUp 三个 hook：
//   - OnInitialize：进入 RUNNING 之前的一次性准备
//   - OnUpdate：每帧调用，返回 RUNNING 继续 / SUCCESS / FAILURE
//   - OnCleanUp：进入终结态（成功或失败、或被父节点 Reset 中断）时一次性收尾
class BehaviorAction : public BehaviorNode //tolua_exports
{ //tolua_exports
public:
	BehaviorAction(const std::string& name) : m_name(name), m_initialized(false) {}
	virtual ~BehaviorAction() {}

	//tolua_begin
	const std::string& GetName() const { return m_name; }
	//tolua_end

	virtual Status Tick(float deltaMs) override;
	virtual void Reset() override;

protected:
	virtual void   OnInitialize() = 0;
	virtual Status OnUpdate(float deltaMs) = 0;
	virtual void   OnCleanUp() = 0;

private:
	std::string m_name;
	bool m_initialized;  // false → 下一次 Tick 会先 OnInitialize
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorAction);

#endif  // __BEHAVIOR_ACTION_H__
