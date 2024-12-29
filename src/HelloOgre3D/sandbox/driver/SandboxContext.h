#ifndef __SANDBOX_CONTEXT_H__  
#define __SANDBOX_CONTEXT_H__

#include <map>
#include "SandboxParamObject.h"

class SandboxContext
{
public:
	// std::string����
	void Set_String(const std::string& key, const std::string& value)
	{
		Set(key, value);
	}
	std::string Get_String(const std::string& key)
	{
		return Get<std::string>(key);
	}

	// number ����
	void Set_Number(const std::string& key, double value)
	{
		Set(key, value);
	}
	double Get_Number(const std::string& key)
	{
		return Get<double>(key);
	}

private:
	// ���ò���
	template<typename T>
	void Set(const std::string& key, const T& value)
	{
		m_params.Set(key, value);
	}

	// ��ȡ��������Ĭ��ֵ��
	template<typename T>
	T Get(const std::string& key, const T& defaultValue = T()) const
	{
		return m_params.Get<T>(key, defaultValue);
	}

	//�������Ƿ����
	bool Has(const std::string& key) const
	{
		return m_params.Has(key);
	}

	// �Ƴ�����
	void Remove(const std::string& key)
	{
		m_params.Remove(key);
	}

	const std::type_info& GetType(const std::string& key) const
	{
		if (!m_params.Has(key))
			throw std::runtime_error("Key not found: " + key);

		return m_params.GetType(key);
	}

private:
	SandboxParamObject m_params;
};


/*
#include <any>
// std::any�汾 C++17
class SandboxContext
{
public:
	void Set(const std::string& key, std::any value)
	{
		m_params[key] = value;
	}

	template<typename T>
	T Get(const std::string& key) const
	{
		return std::any_cast<T>(m_params.at(key));
	}

private:
	std::map<std::string, std::any> m_params;
};
*/

#endif; // __SANDBOX_CONTEXT_H__