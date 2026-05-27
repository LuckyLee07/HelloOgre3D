#ifndef __I_COMPONENT__
#define __I_COMPONENT__

#include <string>

class BaseObject;
class IComponent
{
public:
	IComponent() : m_owner(nullptr) {}
	virtual ~IComponent() {}

	BaseObject* getOwner() const;
	const std::string& getComponentKey() const { return m_componentKey; }

	virtual void onAttach(BaseObject* owner);
	virtual void onDetach();

	virtual void onDestroy() {}

	virtual void start() {}
	virtual void update(int) {}

protected:
	BaseObject* m_owner;
	std::string m_componentKey;

private:
	friend class BaseObject;
	void setComponentKey(const std::string& key) { m_componentKey = key; }
};

#endif // __I_COMPONENT__
