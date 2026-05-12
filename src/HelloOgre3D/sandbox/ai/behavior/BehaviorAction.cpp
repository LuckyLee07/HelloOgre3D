#include "BehaviorAction.h"

BehaviorNode::Status BehaviorAction::Tick(float deltaMs)
{
	if (!m_initialized)
	{
		OnInitialize();
		m_initialized = true;
	}

	const Status s = OnUpdate(deltaMs);
	if (s == STATUS_SUCCESS || s == STATUS_FAILURE)
	{
		OnCleanUp();
		m_initialized = false;
	}
	return s;
}

void BehaviorAction::Reset()
{
	// 父节点中断时收尾：如果当前还 RUNNING，给一次 OnCleanUp 机会，
	// 然后回到未初始化态等待下次 Tick 重新 Initialize。
	if (m_initialized)
	{
		OnCleanUp();
		m_initialized = false;
	}
}
