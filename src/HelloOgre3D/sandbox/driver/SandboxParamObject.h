#ifndef __SANDBOX_PARAM_OBJECT_H__  
#define __SANDBOX_PARAM_OBJECT_H__

#include <typeinfo>
#include <string>
#include <memory>
#include <map>

class SandboxParamBase
{
public:
	virtual ~SandboxParamBase() = default;

	// 获取存储类型信息
	virtual const std::type_info& Type() const = 0;
};

// 模板派生类：存储具体类型的值
template<typename T>
class SandboxParam : public SandboxParamBase
{
public:
	explicit SandboxParam(const T& value) : m_value(value) {}
	const std::type_info& Type() const override { return typeid(T); }
	const T& GetValue() const { return m_value; }

private:
	T m_value; //存储对应的值
};

class SandboxParamObject
{
public:
	// 设置参数 （通用模板接口）
	template<typename T>
	void Set(const std::string& key, const T& value)
	{
		m_params[key] = std::make_shared<SandboxParam<T>>(value);
	}

	// 获取参数（带默认值）
	template<typename T>
	T Get(const std::string& key, const T& defaultValue = T()) const
	{
		auto iter = m_params.find(key);
		if (iter == m_params.end())
			return defaultValue;

		auto param = std::dynamic_pointer_cast<SandboxParam<T>>(iter->second);
		if (param == nullptr) // 类型不匹配
			return defaultValue;

		return param->GetValue();
	}

	const std::type_info& GetType(const std::string& key) const
	{
		auto iter = m_params.find(key);
		if (iter == m_params.end())
			throw std::runtime_error("Key not found: " + key);

		return iter->second->Type();
	}

	// 检查参数是否存在
	bool Has(const std::string& key) const
	{
		return m_params.find(key) != m_params.end();
	}

	// 移除参数
	void Remove(const std::string& key)
	{
		m_params.erase(key);
	}

private:
	std::map<std::string, std::shared_ptr<SandboxParamBase>> m_params;
};

#endif; // __SANDBOX_PARAM_OBJECT_H__