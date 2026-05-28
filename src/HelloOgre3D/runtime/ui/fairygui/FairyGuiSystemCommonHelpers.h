#ifndef __HELLO_FAIRY_GUI_SYSTEM_COMMON_HELPERS_H__
#define __HELLO_FAIRY_GUI_SYSTEM_COMMON_HELPERS_H__

#include "ui/fairygui/FairyGuiSystemImpl.h"
#include "ui/fairygui/FairyGuiSystemFairyIncludes.h"

namespace
{
	std::string GetFileExtension(const std::string& filePath)
	{
		const size_t pos = filePath.find_last_of('.');
		if (pos == std::string::npos)
			return std::string();
		return filePath.substr(pos + 1);
	}

	std::string GetEnvironmentString(const char* name)
	{
		const char* value = std::getenv(name);
		return value != nullptr ? value : std::string();
	}

	bool IsEnvironmentEnabled(const char* name)
	{
		const std::string value = GetEnvironmentString(name);
		return value == "1" || value == "true" || value == "TRUE" || value == "True";
	}

	float GetEnvironmentFloat(const char* name, float fallback)
	{
		const std::string value = GetEnvironmentString(name);
		return value.empty() ? fallback : static_cast<float>(std::atof(value.c_str()));
	}

	unsigned int ToLogicalWindowPixels(Ogre::RenderWindow* renderWindow, unsigned int physicalPixels)
	{
		if (physicalPixels == 0)
			return 0;

		if (renderWindow != nullptr)
		{
			const float pointToPixelScale = renderWindow->getViewPointToPixelScale();
			if (pointToPixelScale > 1.0f)
				return static_cast<unsigned int>(static_cast<float>(physicalPixels) / pointToPixelScale + 0.5f);
		}
		return Ogre::DpiHelper::toLogicalPixels(physicalPixels);
	}

	float GetWindowInputScale(Ogre::RenderWindow* renderWindow, unsigned int logicalPixels, bool horizontal)
	{
		if (renderWindow == nullptr || logicalPixels == 0)
			return 1.0f;

		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int colourDepth = 0;
		int left = 0;
		int top = 0;
		renderWindow->getMetrics(width, height, colourDepth, left, top);
		const unsigned int physicalPixels = horizontal ? width : height;
		if (physicalPixels > logicalPixels)
			return static_cast<float>(physicalPixels) / static_cast<float>(logicalPixels);

		const float pointToPixelScale = renderWindow->getViewPointToPixelScale();
		return pointToPixelScale > 1.0f ? pointToPixelScale : 1.0f;
	}

	int ToRawInputCoordinate(int value, float scale)
	{
		return scale > 1.0f ? static_cast<int>(static_cast<float>(value) * scale + 0.5f) : value;
	}

	int ClampClientCoordinate(float value, unsigned int logicalPixels)
	{
		if (logicalPixels == 0)
			return static_cast<int>(value >= 0.0f ? value + 0.5f : value - 0.5f);

		const float maxValue = static_cast<float>(logicalPixels - 1);
		const float clamped = std::max(0.0f, std::min(maxValue, value));
		return static_cast<int>(clamped + 0.5f);
	}
	bool EndsWith(const std::string& value, const std::string& suffix)
	{
		if (value.size() < suffix.size())
			return false;
		return value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
	}

	void AppendUtf8Codepoint(std::string& text, unsigned int codepoint)
	{
		if (codepoint <= 0x7F)
		{
			text.push_back(static_cast<char>(codepoint));
		}
		else if (codepoint <= 0x7FF)
		{
			text.push_back(static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F)));
			text.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
		}
		else if (codepoint <= 0xFFFF)
		{
			text.push_back(static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F)));
			text.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
			text.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
		}
		else if (codepoint <= 0x10FFFF)
		{
			text.push_back(static_cast<char>(0xF0 | ((codepoint >> 18) & 0x07)));
			text.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
			text.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
			text.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
		}
	}

	bool IsUtf8Continuation(unsigned char value)
	{
		return (value & 0xC0) == 0x80;
	}

	size_t GetNextUtf8ByteOffset(const std::string& text, size_t byteOffset)
	{
		if (byteOffset >= text.size())
			return text.size();

		++byteOffset;
		while (byteOffset < text.size() && IsUtf8Continuation(static_cast<unsigned char>(text[byteOffset])))
			++byteOffset;
		return byteOffset;
	}

	size_t GetUtf8CodepointCount(const std::string& text)
	{
		size_t count = 0;
		size_t byteOffset = 0;
		while (byteOffset < text.size())
		{
			byteOffset = GetNextUtf8ByteOffset(text, byteOffset);
			++count;
		}
		return count;
	}

	size_t ClampUtf8Caret(const std::string& text, size_t caret)
	{
		const size_t count = GetUtf8CodepointCount(text);
		return std::min(caret, count);
	}

	size_t GetUtf8ByteOffsetForCaret(const std::string& text, size_t caret)
	{
		size_t index = 0;
		size_t byteOffset = 0;
		while (byteOffset < text.size() && index < caret)
		{
			byteOffset = GetNextUtf8ByteOffset(text, byteOffset);
			++index;
		}
		return byteOffset;
	}

	bool RemoveUtf8CodepointBeforeCaret(std::string& text, size_t& caret)
	{
		if (caret == 0 || text.empty())
			return false;

		const size_t eraseStart = GetUtf8ByteOffsetForCaret(text, caret - 1);
		const size_t eraseEnd = GetUtf8ByteOffsetForCaret(text, caret);
		if (eraseStart >= eraseEnd || eraseStart >= text.size())
			return false;

		text.erase(eraseStart, eraseEnd - eraseStart);
		--caret;
		return true;
	}

	bool RemoveUtf8CodepointAtCaret(std::string& text, size_t& caret)
	{
		if (text.empty() || caret >= GetUtf8CodepointCount(text))
			return false;

		const size_t eraseStart = GetUtf8ByteOffsetForCaret(text, caret);
		const size_t eraseEnd = GetNextUtf8ByteOffset(text, eraseStart);
		if (eraseStart >= eraseEnd || eraseStart >= text.size())
			return false;

		text.erase(eraseStart, eraseEnd - eraseStart);
		return true;
	}

	bool InsertUtf8CodepointAtCaret(std::string& text, size_t& caret, unsigned int codepoint)
	{
		std::string encoded;
		AppendUtf8Codepoint(encoded, codepoint);
		if (encoded.empty())
			return false;

		const size_t byteOffset = GetUtf8ByteOffsetForCaret(text, caret);
		text.insert(byteOffset, encoded);
		++caret;
		return true;
	}

	size_t CountUtf8CodepointsInString(const std::string& text)
	{
		return GetUtf8CodepointCount(text);
	}

	bool InsertUtf8TextAtCaret(std::string& text, size_t& caret, const std::string& committedText)
	{
		if (committedText.empty())
			return false;

		const size_t byteOffset = GetUtf8ByteOffsetForCaret(text, caret);
		text.insert(byteOffset, committedText);
		caret += CountUtf8CodepointsInString(committedText);
		return true;
	}

	std::string NormalizePackagePath(const std::string& packagePath)
	{
		std::string normalized = packagePath;
		for (size_t index = 0; index < normalized.size(); ++index)
		{
			if (normalized[index] == '\\')
				normalized[index] = '/';
		}
		if (EndsWith(normalized, ".fui"))
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

	std::string TrimRenderDetailKey(const std::string& key)
	{
		if (key.empty())
			return "-";

		const std::string::size_type slash = key.find_last_of("/\\");
		if (slash != std::string::npos && slash + 1 < key.size())
			return key.substr(slash + 1);

		const std::string prefix = "file:";
		if (key.compare(0, prefix.size(), prefix) == 0)
			return key.substr(prefix.size());

		if (key.size() > 36)
			return key.substr(0, 33) + "...";
		return key;
	}

	std::string BuildCountBrief(const std::map<std::string, int>& counts, int maxCount)
	{
		std::vector<std::pair<std::string, int> > entries;
		for (std::map<std::string, int>::const_iterator it = counts.begin(); it != counts.end(); ++it)
			entries.push_back(*it);

		std::sort(entries.begin(), entries.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
			if (a.second == b.second)
				return a.first < b.first;
			return a.second > b.second;
		});

		std::ostringstream stream;
		for (size_t index = 0; index < entries.size() && index < static_cast<size_t>(maxCount); ++index)
		{
			if (index > 0)
				stream << ",";
			stream << TrimRenderDetailKey(entries[index].first) << ":" << entries[index].second;
		}
		return stream.str();
	}
}

#endif // __HELLO_FAIRY_GUI_SYSTEM_COMMON_HELPERS_H__
