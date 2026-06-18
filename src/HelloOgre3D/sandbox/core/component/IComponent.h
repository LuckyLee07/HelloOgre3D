#ifndef __I_COMPONENT__
#define __I_COMPONENT__

#include <string>

class BaseObject;
struct SandboxServices;

namespace ComponentUpdateOrder
{
	static const int Default = 0;
	static const int AI = 100;
	static const int Render = 200;
	static const int Anim = 300;
	static const int Weapon = 400;
}

class IComponent
{
public:
	enum LifecycleState
	{
		LIFECYCLE_CONSTRUCTED = 0,
		LIFECYCLE_ATTACHING,
		LIFECYCLE_ATTACHED,
		LIFECYCLE_DESTROYING,
		LIFECYCLE_DETACHED,
		LIFECYCLE_DESTROYED,
	};

	IComponent() : m_owner(nullptr) {}
	virtual ~IComponent() {}

	BaseObject* getOwner() const;
	const SandboxServices* GetSandboxServices() const;
	const std::string& getComponentKey() const { return m_componentKey; }
	LifecycleState getLifecycleState() const { return m_lifecycleState; }
	const char* getLifecycleStateName() const;

	virtual void onAttach(BaseObject* owner);
	virtual void onDetach();
	virtual void onSandboxServicesChanged(const SandboxServices*) {}

	virtual void onDestroy() {}

	virtual void start() {}
	virtual int getUpdateOrder() const { return ComponentUpdateOrder::Default; }
	virtual void update(int) {}

protected:
	BaseObject* m_owner;
	std::string m_componentKey;

private:
	friend class BaseObject;
	void setComponentKey(const std::string& key) { m_componentKey = key; }
	void setLifecycleState(LifecycleState state) { m_lifecycleState = state; }

	LifecycleState m_lifecycleState = LIFECYCLE_CONSTRUCTED;
};

#endif // __I_COMPONENT__
