#ifndef __I_COMPONENT__
#define __I_COMPONENT__

class GameObject;
class IComponent
{
public:
	IComponent() : m_owner(nullptr) {}
	virtual ~IComponent() {}

	virtual void onAttach(GameObject* owner);
	virtual void onDetach();

	virtual void start() {}
	virtual void update(int deltaMs);

	GameObject* getOwner() const;

protected:
	GameObject* m_owner;
};

#endif // __I_COMPONENT__