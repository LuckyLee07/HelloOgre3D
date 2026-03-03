#ifndef __AGENT_ATTRIB_H__
#define __AGENT_ATTRIB_H__

class GameObject;
class AgentAttrib
{
public:
	AgentAttrib() : m_owner(nullptr) {}
	virtual ~AgentAttrib() {}

	virtual void onAttach(GameObject* owner);
	virtual void onDetach();

	virtual void start() {}
	virtual void update(int deltaMs);

	GameObject* getOwner() const;

protected:
	GameObject* m_owner;
};

#endif // __AGENT_ATTRIB_H__