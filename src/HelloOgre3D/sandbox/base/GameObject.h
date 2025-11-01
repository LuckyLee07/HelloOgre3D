#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include <string>
#include <map>

class IComponent;
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	bool addComponent(const std::string& key, IComponent* comp);
	IComponent* getComponent(const std::string& key);

	bool removeComponent(const std::string& key);
	bool removeComponent(IComponent* comp);
	
	void update(int deltaMs);

private:
	std::map<std::string, IComponent*> m_components;
};

#endif // __GAME_OBJECT__