#ifndef __I_COMPONENT__
#define __I_COMPONENT__

class BaseObject;
class GameObject;
class IComponent
{
public:
	IComponent() : m_gameobj(nullptr) {}
	virtual ~IComponent() {}

	BaseObject* getOwner() const;

	virtual void onAttach(GameObject* gameobj);
	virtual void onDetach();

	virtual void onDestroy() {}

	virtual void start() {}
	virtual void update(int deltaMs) {}

protected:
	GameObject* m_gameobj;
};

#endif // __I_COMPONENT__