#ifndef __DTREE_ACTION_BASE_H__
#define __DTREE_ACTION_BASE_H__

#include <string>

struct AgentContext;

// 叶子节点
class DTreeActionBase
{
public:
	enum Status
	{
		UNINITIALIZED = 0,
		RUNNING,
		TERMINATED
	};

public:
	DTreeActionBase(const char* name);
	virtual ~DTreeActionBase() {}

	void Initialize(AgentContext& ctx);
	Status Update(int deltaMs, AgentContext& ctx);
	void ClearUp(AgentContext& ctx);

	const char* GetName() const { return m_name.c_str(); }

protected:
	virtual void OnInitialize(AgentContext& ctx) { (void)ctx; }
	virtual Status OnUpdate(int deltaMs, AgentContext& ctx) = 0;
	virtual void OnClearUp(AgentContext& ctx) { (void)ctx; }

private:
	std::string m_name;
	Status m_status;
};

#endif  // __DT_ACTION_BASE_H__
