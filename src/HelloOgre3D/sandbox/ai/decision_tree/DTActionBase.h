#ifndef __DT_ACTION_BASE_H__
#define __DT_ACTION_BASE_H__

#include <string>

struct AgentContext;

// р╤вс╫з╣Ц
class DTActionBase
{
public:
	enum Status
	{
		UNINITIALIZED = 0,
		RUNNING,
		TERMINATED
	};

public:
	DTActionBase(const char* name);
	virtual ~DTActionBase() {}

	void Initialize(AgentContext& ctx);
	Status Update(int deltaMs, AgentContext& ctx);
	void ClearUp(AgentContext& ctx);

	const char* GetName() const { return m_name.c_str(); }

protected:
	virtual void OnInitialize(AgentContext& ctx) {}
	virtual Status OnUpdate(int deltaMs, AgentContext& ctx) = 0;
	virtual void OnClearUp(AgentContext& ctx) {}

private:
	std::string m_name;
	Status m_status;
};

#endif  // __DT_ACTION_BASE_H__
