#ifndef __BEHAVIOR_TREE_H__
#define __BEHAVIOR_TREE_H__

#include "script/LuaClassNameTraits.h"

class BehaviorNode;

// 行为树容器：持有 root + 每帧从根 tick。
// 节点本身的所有权在 BehaviorTreeDriver 那边（同 DT 设计），本类不释放任何子节点。
class BehaviorTree //tolua_exports
{ //tolua_exports
public:
	BehaviorTree();
	~BehaviorTree();

	//tolua_begin
	void SetRoot(BehaviorNode* root);
	void Tick(float deltaMs);
	//tolua_end

	BehaviorNode* GetRoot() const { return m_root; }

private:
	BehaviorNode* m_root;
}; //tolua_exports

REGISTER_LUA_CLASS_NAME(BehaviorTree);

#endif  // __BEHAVIOR_TREE_H__
