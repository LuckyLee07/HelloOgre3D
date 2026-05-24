#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include <string>
#include <map>
#include <vector>
#include "component/IComponent.h"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	void setUserData(void* userdata);
	void* getUserData() const;

	bool addComponent(const std::string& key, IComponent* comp);
	IComponent* getComponent(const std::string& key);
	const IComponent* getComponent(const std::string& key) const;
	bool hasComponent(const std::string& key) const;

	template<typename T>
	T* getComponentAs(const std::string& key)
	{
		return dynamic_cast<T*>(getComponent(key));
	}

	template<typename T>
	const T* getComponentAs(const std::string& key) const
	{
		return dynamic_cast<const T*>(getComponent(key));
	}

	template<typename T>
	T* findComponent()
	{
		for (std::map<std::string, IComponent*>::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			T* component = dynamic_cast<T*>(iter->second);
			if (component != nullptr)
				return component;
		}
		return nullptr;
	}

	template<typename T>
	const T* findComponent() const
	{
		for (std::map<std::string, IComponent*>::const_iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
		{
			const T* component = dynamic_cast<const T*>(iter->second);
			if (component != nullptr)
				return component;
		}
		return nullptr;
	}

	bool removeComponent(const std::string& key);
	bool removeComponent(IComponent* comp);

	int getComponentCount() const;
	std::vector<std::string> getComponentKeys() const;
	std::string buildComponentDebugString() const;
	
	void update(int deltaMs);
	
private:
	void* m_userdata;
	// GameObject owns every component inserted through addComponent.
	std::map<std::string, IComponent*> m_components;
};

#endif // __GAME_OBJECT__
