#ifndef _SINGLETON_H_  
#define _SINGLETON_H_  

#include <assert.h>

template <typename T> class Singleton
{
protected:

	static T* m_Instance;

public:
	Singleton()
	{
		assert(!m_Instance);
		m_Instance = static_cast<T*>(this);
	}
	~Singleton(void)
	{
		assert(m_Instance);  m_Instance = 0;
	}
	static T * GetInstance()
	{
		if (m_Instance == 0)
		{
			m_Instance = new T();
		}
		return m_Instance;
	}
	static void Release()
	{
		if (m_Instance)
		{
			delete m_Instance;
			m_Instance = NULL;
		}
	}
};

template <typename T>
T* Singleton<T>::m_Instance = 0;

#endif;