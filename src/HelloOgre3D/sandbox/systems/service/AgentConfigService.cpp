#include "AgentConfigService.h"

AgentConfigService::AgentConfigService()
	: m_useCppFsmFlag(true)
{
}

AgentConfigService::~AgentConfigService()
{
}

void AgentConfigService::SetUseCppFsmFlag(bool value)
{
	m_useCppFsmFlag = value;
}

bool AgentConfigService::GetUseCppFsmFlag() const
{
	return m_useCppFsmFlag;
}
