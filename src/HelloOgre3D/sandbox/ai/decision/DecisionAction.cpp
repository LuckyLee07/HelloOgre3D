#include "DecisionAction.h"

DecisionAction::DecisionAction(const std::string& name)
	: m_name(name), m_status(STATUS_UNINITIALIZED)
{
}

DecisionAction::~DecisionAction()
{
}

void DecisionAction::Initialize()
{
	if (m_status != STATUS_UNINITIALIZED) return;

	OnInitialize();
	// OnInitialize may choose to immediately terminate (e.g. fire-and-forget actions).
	// If it did not set status explicitly, advance to RUNNING.
	if (m_status == STATUS_UNINITIALIZED)
	{
		m_status = STATUS_RUNNING;
	}
}

DecisionAction::Status DecisionAction::Update(float deltaMs)
{
	if (m_status == STATUS_TERMINATED) return STATUS_TERMINATED;
	if (m_status == STATUS_UNINITIALIZED)
	{
		// Someone called Update without Initialize; tolerate by initializing now.
		Initialize();
		if (m_status == STATUS_TERMINATED) return STATUS_TERMINATED;
	}

	m_status = OnUpdate(deltaMs);
	return m_status;
}

void DecisionAction::CleanUp()
{
	if (m_status == STATUS_TERMINATED)
	{
		OnCleanUp();
	}
	m_status = STATUS_UNINITIALIZED;
}
