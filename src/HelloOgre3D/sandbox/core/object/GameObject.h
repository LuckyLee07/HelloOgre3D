#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include <string>
#include <map>
#include <vector>

class IComponent;
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	void setUserData(void* userdata);
	void* getUserData() const;

	bool addComponent(const std::string& key, IComponent* comp);
	IComponent* getComponent(const std::string& key);

	bool removeComponent(const std::string& key);
	bool removeComponent(IComponent* comp);

	int getComponentCount() const;
	std::vector<std::string> getComponentKeys() const;
	std::string buildComponentDebugString() const;
	
	void update(int deltaMs);
	
private:
	void* m_userdata;
	std::map<std::string, IComponent*> m_components;
};

#endif // __GAME_OBJECT__
