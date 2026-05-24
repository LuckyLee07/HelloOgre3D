#ifndef __I_COMPONENT__
#define __I_COMPONENT__

#include <string>

class BaseObject;
class GameObject;
class IComponent
{
public:
	IComponent() : m_gameobj(nullptr) {}
	virtual ~IComponent() {}

	BaseObject* getOwner() const;
	GameObject* getGameObject() const { return m_gameobj; }
	const std::string& getComponentKey() const { return m_componentKey; }

	virtual void onAttach(GameObject* gameobj);
	virtual void onDetach();

	virtual void onDestroy() {}

	virtual void start() {}
	virtual void update(int) {}

protected:
	GameObject* m_gameobj;
	std::string m_componentKey;

private:
	friend class GameObject;
	void setComponentKey(const std::string& key) { m_componentKey = key; }
};

#endif // __I_COMPONENT__
