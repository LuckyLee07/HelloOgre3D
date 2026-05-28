#ifndef __HELLO_FAIRY_GUI_SYSTEM_INPUT_HELPERS_H__
#define __HELLO_FAIRY_GUI_SYSTEM_INPUT_HELPERS_H__

#include "ui/fairygui/FairyGuiSystemImpl.h"
#include "ui/fairygui/FairyGuiSystemCommonHelpers.h"
#include "ui/fairygui/FairyGuiSystemFairyIncludes.h"

namespace
{
	const int OIS_KC_ESCAPE = 0x01;
	const int OIS_KC_TAB = 0x0F;
	const int OIS_KC_BACK = 0x0E;
	const int OIS_KC_RETURN = 0x1C;
	const int OIS_KC_NUMPADENTER = 0x9C;
	const int OIS_KC_HOME = 0xC7;
	const int OIS_KC_LEFT = 0xCB;
	const int OIS_KC_RIGHT = 0xCD;
	const int OIS_KC_END = 0xCF;
	const int OIS_KC_DELETE = 0xD3;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	std::map<HWND, FairyGuiSystemImpl*> g_nativeImeSystemsByWindow;
	std::map<HWND, WNDPROC> g_nativeImePreviousWndProcByWindow;

	std::string WideToUtf8(const std::wstring& value)
	{
		if (value.empty())
			return std::string();

		const int required = WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
		if (required <= 0)
			return std::string();

		std::string output(static_cast<size_t>(required), '\0');
		WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), &output[0], required, nullptr, nullptr);
		return output;
	}

	std::string ReadImeCompositionString(HIMC context, DWORD index)
	{
		if (context == nullptr)
			return std::string();

		const LONG byteCount = ImmGetCompositionStringW(context, index, nullptr, 0);
		if (byteCount <= 0)
			return std::string();

		std::wstring text(static_cast<size_t>(byteCount / sizeof(wchar_t)), L'\0');
		ImmGetCompositionStringW(context, index, &text[0], byteCount);
		return WideToUtf8(text);
	}

	bool ReadImeCandidateInfo(HIMC context, int& candidateCount, int& candidateSelection)
	{
		candidateCount = 0;
		candidateSelection = -1;
		if (context == nullptr)
			return false;

		const DWORD byteCount = ImmGetCandidateListW(context, 0, nullptr, 0);
		if (byteCount == 0)
			return false;

		std::vector<char> buffer(byteCount);
		LPCANDIDATELIST candidateList = reinterpret_cast<LPCANDIDATELIST>(&buffer[0]);
		if (ImmGetCandidateListW(context, 0, candidateList, byteCount) == 0)
			return false;

		candidateCount = static_cast<int>(candidateList->dwCount);
		candidateSelection = static_cast<int>(candidateList->dwSelection);
		return true;
	}

	LRESULT CALLBACK FairyGuiNativeImeWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		std::map<HWND, FairyGuiSystemImpl*>::iterator it = g_nativeImeSystemsByWindow.find(hwnd);
		if (it != g_nativeImeSystemsByWindow.end() && it->second != nullptr)
		{
			long long result = 0;
			if (it->second->HandleNativeImeMessage(static_cast<unsigned int>(message), static_cast<unsigned long long>(wParam), static_cast<long long>(lParam), result))
				return static_cast<LRESULT>(result);
		}

		std::map<HWND, WNDPROC>::iterator procIt = g_nativeImePreviousWndProcByWindow.find(hwnd);
		WNDPROC previousProc = procIt != g_nativeImePreviousWndProcByWindow.end() ? procIt->second : nullptr;
		return previousProc != nullptr ? CallWindowProc(previousProc, hwnd, message, wParam, lParam) : DefWindowProc(hwnd, message, wParam, lParam);
	}
#endif

	std::string DescribeObject(fairygui::GObject* object)
	{
		if (object == nullptr)
			return "null";

		std::ostringstream stream;
		stream << "name=" << object->name
			<< " id=" << object->id
			<< " pos=" << object->getX() << "," << object->getY()
			<< " size=" << object->getWidth() << "x" << object->getHeight()
			<< " touchable=" << (object->isTouchable() ? "true" : "false");
		return stream.str();
	}

	void LogInputHit(const char* phase, int rawX, int rawY, float x, float y, fairygui::GObject* target)
	{
		if (!IsEnvironmentEnabled("HELLO_FGUI_INPUT_DEBUG"))
			return;

		std::ostringstream stream;
		stream << "[FGUI] input " << phase
			<< " raw=" << rawX << "," << rawY
			<< " ui=" << x << "," << y
			<< " target=" << DescribeObject(target);
		Ogre::LogManager::getSingleton().logMessage(stream.str());
	}
	bool IsPrintableKeyText(int keyText)
	{
		return keyText >= 32 && keyText != 127 && keyText <= 0x10FFFF
			&& (keyText < 0xD800 || keyText > 0xDFFF);
	}
}

#endif // __HELLO_FAIRY_GUI_SYSTEM_INPUT_HELPERS_H__
