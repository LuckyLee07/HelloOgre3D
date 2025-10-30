#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include "SandboxObject.h"
#include <string>
#include <map>

class IComponent;
class GameObject : public SandboxObject
{
public:
	GameObject();
	virtual ~GameObject();

	bool addComponent(const std::string& key, IComponent* comp);
	IComponent* getComponent(const std::string& key);

	bool removeComponent(const std::string& key);
	bool removeComponent(IComponent* comp);
	
private:
	std::map<std::string, IComponent*> m_components;
};

#endif // __GAME_OBJECT__