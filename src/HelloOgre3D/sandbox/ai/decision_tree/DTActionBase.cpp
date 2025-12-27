#include "DTActionBase.h"
#include "ai/AgentContext.h"

DTActionBase::DTActionBase(const char* name) : m_name(name ? name : ""), m_status(TERMINATED)
{
}

void DTActionBase::Initialize(AgentContext& ctx)
{
	if (m_status == UNINITIALIZED)
	{
		OnInitialize(ctx);
		m_status = RUNNING;
	}
}

DTActionBase::Status DTActionBase::Update(int deltaMs, AgentContext& ctx)
{
	if (m_status == TERMINATED)
		return TERMINATED;
	if (m_status == UNINITIALIZED)
		Initialize(ctx);

	Status result = OnUpdate(deltaMs, ctx);
	m_status = result;
	return m_status;
}

void DTActionBase::ClearUp(AgentContext& ctx)
{
	if (m_status == TERMINATED)
	{
		OnClearUp(ctx);
		m_status = UNINITIALIZED;
	}
}
