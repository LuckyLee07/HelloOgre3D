#ifndef __SANDBOX_CONTEXT_H__  
#define __SANDBOX_CONTEXT_H__

#include <map>
#include "SandboxParamObject.h"

class SandboxContext
{
public:
	// std::string类型
	SandboxContext& Set_String(const std::string& key, const std::string& value)
	{
		return Set(key, value);
	}
	std::string Get_String(const std::string& key) const
	{
		return Get<std::string>(key);
	}

	// number 类型
	SandboxContext& Set_Number(const std::string& key, double value)
	{
		return Set(key, value);
	}
	double Get_Number(const std::string& key) const
	{
		return Get<double>(key);
	}

	// Reference 类型
	template<typename T>
	SandboxContext& Set_Ref(const std::string& key, T& value)
	{
		return Set(key, std::make_shared<T>(value)); // 使用 shared_ptr 存储引用
	}
	template<typename T>
	T& Get_Ref(const std::string& key) const
	{
		auto ptr = Get<std::shared_ptr<T>>(key);
		return *ptr;
	}

	// Pointer 类型
	template<typename T>
	SandboxContext& Set_Ptr(const std::string& key, T* value)
	{
		return Set(key, value); // 存储指针类型
	}
	template<typename T>
	T* Get_Ptr(const std::string& key) const
	{
		return Get<T>(key);
	}

private:
	// 设置参数
	template<typename T>
	SandboxContext& Set(const std::string& key, const T& value)
	{
		m_params.Set(key, value);
		return *this;
	}

	// 获取参数（带默认值）
	template<typename T>
	T Get(const std::string& key, const T& defaultValue = T()) const
	{
		return m_params.Get<T>(key, defaultValue);
	}

	//检查参数是否存在
	bool Has(const std::string& key) const
	{
		return m_params.Has(key);
	}

	// 移除参数
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
// std::any版本 C++17
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