#ifndef __HELLO_FAIRY_GUI_SYSTEM_OBJECT_HELPERS_H__
#define __HELLO_FAIRY_GUI_SYSTEM_OBJECT_HELPERS_H__

#include "ui/fairygui/FairyGuiSystemImpl.h"
#include "ui/fairygui/FairyGuiSystemCommonHelpers.h"
#include "ui/fairygui/FairyGuiSystemFairyIncludes.h"

#include <fstream>

namespace
{
	bool SetRangeObjectValue(fairygui::GObject* object, double value)
	{
		fairygui::GProgressBar* progressBar = dynamic_cast<fairygui::GProgressBar*>(object);
		if (progressBar != nullptr)
		{
			progressBar->setValue(value);
			return true;
		}

		fairygui::GSlider* slider = dynamic_cast<fairygui::GSlider*>(object);
		if (slider != nullptr)
		{
			slider->setValue(value);
			return true;
		}

		return false;
	}

	float GetRangeObjectValue(fairygui::GObject* object)
	{
		fairygui::GProgressBar* progressBar = dynamic_cast<fairygui::GProgressBar*>(object);
		if (progressBar != nullptr)
			return static_cast<float>(progressBar->getValue());

		fairygui::GSlider* slider = dynamic_cast<fairygui::GSlider*>(object);
		if (slider != nullptr)
			return static_cast<float>(slider->getValue());

		return 0.0f;
	}

	bool SetRangeObjectMin(fairygui::GObject* object, double minValue)
	{
		fairygui::GProgressBar* progressBar = dynamic_cast<fairygui::GProgressBar*>(object);
		if (progressBar != nullptr)
		{
			progressBar->setMin(minValue);
			return true;
		}

		fairygui::GSlider* slider = dynamic_cast<fairygui::GSlider*>(object);
		if (slider != nullptr)
		{
			slider->setMin(minValue);
			return true;
		}

		return false;
	}

	float GetRangeObjectMin(fairygui::GObject* object)
	{
		fairygui::GProgressBar* progressBar = dynamic_cast<fairygui::GProgressBar*>(object);
		if (progressBar != nullptr)
			return static_cast<float>(progressBar->getMin());

		fairygui::GSlider* slider = dynamic_cast<fairygui::GSlider*>(object);
		if (slider != nullptr)
			return static_cast<float>(slider->getMin());

		return 0.0f;
	}

	bool SetRangeObjectMax(fairygui::GObject* object, double maxValue)
	{
		fairygui::GProgressBar* progressBar = dynamic_cast<fairygui::GProgressBar*>(object);
		if (progressBar != nullptr)
		{
			progressBar->setMax(maxValue);
			return true;
		}

		fairygui::GSlider* slider = dynamic_cast<fairygui::GSlider*>(object);
		if (slider != nullptr)
		{
			slider->setMax(maxValue);
			return true;
		}

		return false;
	}

	float GetRangeObjectMax(fairygui::GObject* object)
	{
		fairygui::GProgressBar* progressBar = dynamic_cast<fairygui::GProgressBar*>(object);
		if (progressBar != nullptr)
			return static_cast<float>(progressBar->getMax());

		fairygui::GSlider* slider = dynamic_cast<fairygui::GSlider*>(object);
		if (slider != nullptr)
			return static_cast<float>(slider->getMax());

		return 0.0f;
	}

	fairygui::Transition* FindTransition(fairygui::GComponent* component, const std::string& transitionName)
	{
		if (component == nullptr)
			return nullptr;

		if (!transitionName.empty())
			return component->getTransition(transitionName);

		return component->getTransitions().empty() ? nullptr : component->getTransitionAt(0);
	}

	std::string NormalizePackagePath(const std::string& packagePath)
	{
		std::string normalized = packagePath;
		for (size_t index = 0; index < normalized.size(); ++index)
		{
			if (normalized[index] == '\\')
				normalized[index] = '/';
		}
		if (normalized.size() >= 4 && normalized.compare(normalized.size() - 4, 4, ".fui") == 0)
			normalized = normalized.substr(0, normalized.size() - 4);
		return normalized;
	}

	bool ContainsDirectorySeparator(const std::string& packagePath)
	{
		return packagePath.find('/') != std::string::npos || packagePath.find(':') != std::string::npos;
	}

	bool FileExists(const std::string& filePath)
	{
		std::ifstream file(filePath.c_str(), std::ios::binary);
		return file.good();
	}

	std::string ResolvePackagePath(const std::string& packagePath)
	{
		const std::string normalized = NormalizePackagePath(packagePath);
		if (FileExists(normalized + ".fui"))
			return normalized;
		if (!ContainsDirectorySeparator(normalized))
		{
			const std::string runtimePath = "res/fuires/" + normalized;
			if (FileExists(runtimePath + ".fui"))
				return runtimePath;
		}
		return normalized;
	}

	fairygui::UIPackage* AddPackage(const std::string& packagePath)
	{
		return fairygui::UIPackage::addPackage(ResolvePackagePath(packagePath));
	}
}

#endif // __HELLO_FAIRY_GUI_SYSTEM_OBJECT_HELPERS_H__
