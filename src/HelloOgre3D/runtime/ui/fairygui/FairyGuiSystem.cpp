#include "ui/fairygui/FairyGuiSystem.h"

#include "Controller.h"
#include "cocos2d.h"
#include "GComboBox.h"
#include "GComponent.h"
#include "GGraph.h"
#include "GList.h"
#include "GLoader.h"
#include "GObject.h"
#include "GProgressBar.h"
#include "GRoot.h"
#include "GSlider.h"
#include "GTextField.h"
#include "GTextInput.h"
#include "Transition.h"
#include "UIPackage.h"
#include "event/EventContext.h"
#include "event/InputEvent.h"
#include "event/UIEventType.h"

#include "Ogre.h"
#include "OgreDpiHelper.h"
#include "OgreRenderWindow.h"
#include "ScriptLuaVM.h"
#include "profiling/Profile.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#endif

namespace
{
	const std::string FAIRYGUI_FALLBACK_MATERIAL = "Hello/FairyGUI/Fallback";
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
	std::map<HWND, FairyGuiSystem*> g_nativeImeSystemsByWindow;
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
		std::map<HWND, FairyGuiSystem*>::iterator it = g_nativeImeSystemsByWindow.find(hwnd);
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

	float TransformX(const cocos2d::Mat4& transform, const cocos2d::Vec3& value)
	{
		return value.x * transform.m[0] + value.y * transform.m[4] + value.z * transform.m[8] + transform.m[12];
	}

	float TransformY(const cocos2d::Mat4& transform, const cocos2d::Vec3& value)
	{
		return value.x * transform.m[1] + value.y * transform.m[5] + value.z * transform.m[9] + transform.m[13];
	}

	Ogre::ColourValue ToOgreColor(const cocos2d::Color4B& color)
	{
		return Ogre::ColourValue(
			color.r / 255.0f,
			color.g / 255.0f,
			color.b / 255.0f,
			color.a / 255.0f);
	}

	struct ClipVertex
	{
		float x;
		float y;
		float u;
		float v;
		Ogre::ColourValue color;
	};

	enum ClipEdge
	{
		CLIP_LEFT,
		CLIP_RIGHT,
		CLIP_BOTTOM,
		CLIP_TOP
	};

	float GetClipAxisValue(const ClipVertex& vertex, ClipEdge edge)
	{
		return edge == CLIP_LEFT || edge == CLIP_RIGHT ? vertex.x : vertex.y;
	}

	bool IsClipVertexInside(const ClipVertex& vertex, ClipEdge edge, float value)
	{
		if (edge == CLIP_LEFT || edge == CLIP_BOTTOM)
			return GetClipAxisValue(vertex, edge) >= value;
		return GetClipAxisValue(vertex, edge) <= value;
	}

	ClipVertex InterpolateClipVertex(const ClipVertex& from, const ClipVertex& to, float t)
	{
		ClipVertex result;
		result.x = from.x + (to.x - from.x) * t;
		result.y = from.y + (to.y - from.y) * t;
		result.u = from.u + (to.u - from.u) * t;
		result.v = from.v + (to.v - from.v) * t;
		result.color.r = from.color.r + (to.color.r - from.color.r) * t;
		result.color.g = from.color.g + (to.color.g - from.color.g) * t;
		result.color.b = from.color.b + (to.color.b - from.color.b) * t;
		result.color.a = from.color.a + (to.color.a - from.color.a) * t;
		return result;
	}

	ClipVertex IntersectClipEdge(const ClipVertex& from, const ClipVertex& to, ClipEdge edge, float value)
	{
		const float fromValue = GetClipAxisValue(from, edge);
		const float toValue = GetClipAxisValue(to, edge);
		const float delta = toValue - fromValue;
		const float t = delta != 0.0f ? (value - fromValue) / delta : 0.0f;
		return InterpolateClipVertex(from, to, std::max(0.0f, std::min(1.0f, t)));
	}

	void ClipPolygonByEdge(const std::vector<ClipVertex>& input, std::vector<ClipVertex>& output, ClipEdge edge, float value)
	{
		output.clear();
		if (input.empty())
			return;

		ClipVertex previous = input.back();
		bool previousInside = IsClipVertexInside(previous, edge, value);
		for (std::vector<ClipVertex>::const_iterator it = input.begin(); it != input.end(); ++it)
		{
			const ClipVertex& current = *it;
			const bool currentInside = IsClipVertexInside(current, edge, value);
			if (currentInside != previousInside)
				output.push_back(IntersectClipEdge(previous, current, edge, value));
			if (currentInside)
				output.push_back(current);
			previous = current;
			previousInside = currentInside;
		}
	}

	void ClipTriangleToRect(const ClipVertex* triangle, const cocos2d::Rect& rect, std::vector<ClipVertex>& output, std::vector<ClipVertex>& scratch)
	{
		scratch.clear();
		output.clear();
		scratch.push_back(triangle[0]);
		scratch.push_back(triangle[1]);
		scratch.push_back(triangle[2]);

		ClipPolygonByEdge(scratch, output, CLIP_LEFT, rect.getMinX());
		ClipPolygonByEdge(output, scratch, CLIP_RIGHT, rect.getMaxX());
		ClipPolygonByEdge(scratch, output, CLIP_BOTTOM, rect.getMinY());
		ClipPolygonByEdge(output, scratch, CLIP_TOP, rect.getMaxY());
		output.swap(scratch);
	}

	bool IntersectRect(const cocos2d::Rect& lhs, const cocos2d::Rect& rhs, cocos2d::Rect& output)
	{
		const float minX = std::max(lhs.getMinX(), rhs.getMinX());
		const float minY = std::max(lhs.getMinY(), rhs.getMinY());
		const float maxX = std::min(lhs.getMaxX(), rhs.getMaxX());
		const float maxY = std::min(lhs.getMaxY(), rhs.getMaxY());
		if (maxX <= minX || maxY <= minY)
			return false;

		output.setRect(minX, minY, maxX - minX, maxY - minY);
		return true;
	}

	void AppendRectIfValid(std::vector<cocos2d::Rect>& rects, float x, float y, float width, float height)
	{
		if (width <= 0.0f || height <= 0.0f)
			return;

		rects.push_back(cocos2d::Rect(x, y, width, height));
	}

	void SubtractRect(const cocos2d::Rect& source, const cocos2d::Rect& cut, std::vector<cocos2d::Rect>& output)
	{
		cocos2d::Rect intersection;
		if (!IntersectRect(source, cut, intersection))
		{
			output.push_back(source);
			return;
		}

		AppendRectIfValid(output, source.getMinX(), source.getMinY(), intersection.getMinX() - source.getMinX(), source.size.height);
		AppendRectIfValid(output, intersection.getMaxX(), source.getMinY(), source.getMaxX() - intersection.getMaxX(), source.size.height);
		AppendRectIfValid(output, intersection.getMinX(), source.getMinY(), intersection.size.width, intersection.getMinY() - source.getMinY());
		AppendRectIfValid(output, intersection.getMinX(), intersection.getMaxY(), intersection.size.width, source.getMaxY() - intersection.getMaxY());
	}

	float PolygonSignedArea(const std::vector<ClipVertex>& polygon)
	{
		if (polygon.size() < 3)
			return 0.0f;

		float area = 0.0f;
		for (size_t index = 0; index < polygon.size(); ++index)
		{
			const ClipVertex& current = polygon[index];
			const ClipVertex& next = polygon[(index + 1) % polygon.size()];
			area += current.x * next.y - next.x * current.y;
		}
		return area * 0.5f;
	}

	bool IsValidClipPolygon(const std::vector<ClipVertex>& polygon)
	{
		return polygon.size() >= 3 && std::abs(PolygonSignedArea(polygon)) > 0.001f;
	}

	float Cross(const ClipVertex& a, const ClipVertex& b, const ClipVertex& point)
	{
		return (b.x - a.x) * (point.y - a.y) - (b.y - a.y) * (point.x - a.x);
	}

	bool IsInsideConvexEdge(const ClipVertex& vertex, const ClipVertex& edgeStart, const ClipVertex& edgeEnd, float orientation, bool keepInside)
	{
		const float side = Cross(edgeStart, edgeEnd, vertex);
		if (orientation >= 0.0f)
			return keepInside ? side >= -0.001f : side < -0.001f;
		return keepInside ? side <= 0.001f : side > 0.001f;
	}

	ClipVertex IntersectConvexEdge(const ClipVertex& from, const ClipVertex& to, const ClipVertex& edgeStart, const ClipVertex& edgeEnd)
	{
		const float fromSide = Cross(edgeStart, edgeEnd, from);
		const float toSide = Cross(edgeStart, edgeEnd, to);
		const float denominator = fromSide - toSide;
		const float t = denominator != 0.0f ? fromSide / denominator : 0.0f;
		return InterpolateClipVertex(from, to, std::max(0.0f, std::min(1.0f, t)));
	}

	void ClipPolygonByConvexEdge(const std::vector<ClipVertex>& input, std::vector<ClipVertex>& output, const ClipVertex& edgeStart, const ClipVertex& edgeEnd, float orientation, bool keepInside)
	{
		output.clear();
		if (input.empty())
			return;

		ClipVertex previous = input.back();
		bool previousInside = IsInsideConvexEdge(previous, edgeStart, edgeEnd, orientation, keepInside);
		for (std::vector<ClipVertex>::const_iterator it = input.begin(); it != input.end(); ++it)
		{
			const ClipVertex& current = *it;
			const bool currentInside = IsInsideConvexEdge(current, edgeStart, edgeEnd, orientation, keepInside);
			if (currentInside != previousInside)
				output.push_back(IntersectConvexEdge(previous, current, edgeStart, edgeEnd));
			if (currentInside)
				output.push_back(current);
			previous = current;
			previousInside = currentInside;
		}
	}

	void ClipPolygonToConvexPolygon(const std::vector<ClipVertex>& input, const std::vector<ClipVertex>& clipPolygon, std::vector<ClipVertex>& output, std::vector<ClipVertex>& scratch)
	{
		output.clear();
		if (!IsValidClipPolygon(input) || !IsValidClipPolygon(clipPolygon))
			return;

		const float orientation = PolygonSignedArea(clipPolygon);
		scratch = input;
		for (size_t index = 0; index < clipPolygon.size(); ++index)
		{
			const ClipVertex& edgeStart = clipPolygon[index];
			const ClipVertex& edgeEnd = clipPolygon[(index + 1) % clipPolygon.size()];
			ClipPolygonByConvexEdge(scratch, output, edgeStart, edgeEnd, orientation, true);
			if (!IsValidClipPolygon(output))
			{
				output.clear();
				return;
			}
			scratch.swap(output);
		}
		output.swap(scratch);
	}

	void SubtractConvexPolygon(const std::vector<ClipVertex>& source, const std::vector<ClipVertex>& cut, std::vector<std::vector<ClipVertex> >& output, std::vector<ClipVertex>& insideScratch, std::vector<ClipVertex>& outsideScratch)
	{
		if (!IsValidClipPolygon(source))
			return;
		if (!IsValidClipPolygon(cut))
		{
			output.push_back(source);
			return;
		}

		const float orientation = PolygonSignedArea(cut);
		insideScratch = source;
		for (size_t index = 0; index < cut.size(); ++index)
		{
			const ClipVertex& edgeStart = cut[index];
			const ClipVertex& edgeEnd = cut[(index + 1) % cut.size()];
			ClipPolygonByConvexEdge(insideScratch, outsideScratch, edgeStart, edgeEnd, orientation, false);
			if (IsValidClipPolygon(outsideScratch))
				output.push_back(outsideScratch);

			std::vector<ClipVertex> nextInside;
			ClipPolygonByConvexEdge(insideScratch, nextInside, edgeStart, edgeEnd, orientation, true);
			if (!IsValidClipPolygon(nextInside))
				return;
			insideScratch.swap(nextInside);
		}
	}

	ClipVertex MakeClipVertex(float x, float y)
	{
		ClipVertex vertex;
		vertex.x = x;
		vertex.y = y;
		vertex.u = 0.0f;
		vertex.v = 0.0f;
		vertex.color = Ogre::ColourValue::White;
		return vertex;
	}

	std::vector<ClipVertex> BuildClipPolygon(const std::vector<cocos2d::Vec2>& points)
	{
		std::vector<ClipVertex> polygon;
		polygon.reserve(points.size());
		for (std::vector<cocos2d::Vec2>::const_iterator it = points.begin(); it != points.end(); ++it)
			polygon.push_back(MakeClipVertex(it->x, it->y));
		if (!IsValidClipPolygon(polygon))
			polygon.clear();
		return polygon;
	}

	std::vector<ClipVertex> BuildRectClipPolygon(const cocos2d::Rect& rect)
	{
		std::vector<ClipVertex> polygon;
		if (rect.size.width <= 0.0f || rect.size.height <= 0.0f)
			return polygon;

		polygon.push_back(MakeClipVertex(rect.getMinX(), rect.getMinY()));
		polygon.push_back(MakeClipVertex(rect.getMaxX(), rect.getMinY()));
		polygon.push_back(MakeClipVertex(rect.getMaxX(), rect.getMaxY()));
		polygon.push_back(MakeClipVertex(rect.getMinX(), rect.getMaxY()));
		return polygon;
	}

	void AppendPolygonTriangles(const std::vector<ClipVertex>& polygon, std::vector<ClipVertex>& outputVertices)
	{
		if (!IsValidClipPolygon(polygon))
			return;

		for (size_t index = 1; index + 1 < polygon.size(); ++index)
		{
			outputVertices.push_back(polygon[0]);
			outputVertices.push_back(polygon[index]);
			outputVertices.push_back(polygon[index + 1]);
		}
	}

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

	bool EndsWith(const std::string& value, const std::string& suffix)
	{
		if (value.size() < suffix.size())
			return false;
		return value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
	}

	bool IsPrintableKeyText(int keyText)
	{
		return keyText >= 32 && keyText != 127 && keyText <= 0x10FFFF
			&& (keyText < 0xD800 || keyText > 0xDFFF);
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

FairyGuiSystem::FairyGuiSystem()
	: m_pRenderWindow(nullptr), m_pSceneManager(nullptr), m_pManualNode(nullptr), m_pManualObject(nullptr),
	m_pScene(nullptr), m_pRoot(nullptr), m_initialized(false), m_screenWidth(0), m_screenHeight(0),
	m_lastMouseX(0.0f), m_lastMouseY(0.0f), m_hasLastMousePosition(false), m_leftMouseDownOnUi(false),
	m_focusedObjectHandle(0),
	m_scissorEnabled(false), m_scissorRect(cocos2d::Rect::ZERO),
	m_stencilStage(cocos2d::STENCIL_STAGE_DISABLED), m_stencilDepth(0), m_stencilRevision(0),
	m_pendingStencilDepth(0), m_pendingStencilInverted(false), m_pendingStencilValid(false), m_pendingStencilRect(cocos2d::Rect::ZERO), m_pendingStencilPolygons(), m_stencilScopes(),
	m_lastRenderCommandCount(0), m_lastTriangleCount(0), m_materialCounter(0), m_nextObjectHandle(1), m_nextListenerBindingId(1),
	m_objectHandles(), m_listenerBindings(), m_textInputCarets(), m_materialNames(), m_textureNames(), m_materialNamesBySource(), m_textureNamesBySource(), m_textureDetailsBySource(),
	m_currentFrameStats(), m_lastFrameStats(), m_imeStats()
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	, m_nativeWindowHandle(nullptr), m_previousWindowProc(nullptr), m_nativeImeHookInstalled(false)
#endif
{
}

FairyGuiSystem::~FairyGuiSystem()
{
	Shutdown();
}

bool FairyGuiSystem::Initialize(Ogre::RenderWindow* renderWindow, Ogre::SceneManager* sceneManager)
{
	m_pRenderWindow = renderWindow;
	m_pSceneManager = sceneManager;
	if (m_pRenderWindow == nullptr || m_pSceneManager == nullptr)
		return false;

	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int colourDepth = 0;
	int left = 0;
	int top = 0;
	m_pRenderWindow->getMetrics(width, height, colourDepth, left, top);
	HandleWindowResized(ToLogicalWindowPixels(m_pRenderWindow, width), ToLogicalWindowPixels(m_pRenderWindow, height));

	m_pScene = cocos2d::Scene::create();
	m_pRoot = fairygui::GRoot::create(m_pScene);
	m_initialized = (m_pScene != nullptr && m_pRoot != nullptr);
	if (m_initialized)
	{
		m_pRoot->setSize((float)m_screenWidth, (float)m_screenHeight);
		m_pManualNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
		m_pManualObject = m_pSceneManager->createManualObject();
		m_pManualObject->setDynamic(true);
		m_pManualObject->setCastShadows(false);
		m_pManualObject->setUseIdentityProjection(true);
		m_pManualObject->setUseIdentityView(true);
		m_pManualObject->setKeepDeclarationOrder(true);
		m_pManualObject->setRenderQueueGroupAndPriority(Ogre::RENDER_QUEUE_OVERLAY, 100);
		m_pManualObject->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
		m_pManualNode->attachObject(m_pManualObject);
		m_pManualNode->_updateBounds();
		InstallNativeImeHook();
		if (!CreateConfiguredPackageObject() && IsEnvironmentEnabled("HELLO_FGUI_SMOKE_TEST"))
			CreateSmokeTestImage("media/fonts/dejavu/dejavu.png");
	}
	return m_initialized;
}

void FairyGuiSystem::Shutdown()
{
	RemoveNativeImeHook();
	ClearObjectHandles();

	if (m_pScene != nullptr)
		m_pScene->removeAllChildren();

	delete m_pRoot;
	delete m_pScene;
	DestroyOgreResources();
	m_pRoot = nullptr;
	m_pScene = nullptr;
	m_pManualObject = nullptr;
	m_pManualNode = nullptr;
	m_initialized = false;
	m_pRenderWindow = nullptr;
	m_pSceneManager = nullptr;
	m_screenWidth = 0;
	m_screenHeight = 0;
	m_lastMouseX = 0.0f;
	m_lastMouseY = 0.0f;
	m_hasLastMousePosition = false;
	m_leftMouseDownOnUi = false;
	m_focusedObjectHandle = 0;
	m_lastRenderCommandCount = 0;
	m_lastTriangleCount = 0;
	m_imeStats = ImeStats();
	ResetFrameRenderStats();
	m_lastFrameStats = FrameRenderStats();
	m_nextObjectHandle = 1;
	m_nextListenerBindingId = 1;
	m_textInputCarets.clear();
}

void FairyGuiSystem::Update(float deltaSeconds)
{
	H3D_PROFILE_SCOPE("FairyGuiSystem::Update");
	if (!m_initialized)
		return;

	cocos2d::Director::getInstance()->mainLoop(deltaSeconds);
	H3D_PROFILE_PLOT("FGUIObjectHandles", static_cast<double>(m_objectHandles.size()));
	H3D_PROFILE_PLOT("FGUIListenerBindings", static_cast<double>(m_listenerBindings.size()));
}

void FairyGuiSystem::Render()
{
	H3D_PROFILE_SCOPE("FairyGuiSystem::Render");
	if (!m_initialized)
		return;

	cocos2d::Renderer* renderer = cocos2d::Director::getInstance()->getRenderer();
	ResetFrameRenderStats();
	renderer->beginFrame();
	renderer->setCommandSink(this);
	BeginOgreRender();
	if (m_pScene != nullptr)
		m_pScene->visit(renderer, cocos2d::Mat4::IDENTITY, 0);
	EndOgreRender();
	renderer->setCommandSink(nullptr);
	m_lastRenderCommandCount = renderer->getTriangleCommandCount();
	m_lastTriangleCount = renderer->getSubmittedTriangleCount();
	FinalizeFrameRenderStats();
	H3D_PROFILE_PLOT("FGUIRenderCommands", static_cast<double>(m_lastRenderCommandCount));
	H3D_PROFILE_PLOT("FGUITriangles", static_cast<double>(m_lastTriangleCount));
	H3D_PROFILE_PLOT("FGUIDrawCommands", static_cast<double>(m_lastFrameStats.drawCommandCount));
	H3D_PROFILE_PLOT("FGUIDrawTriangles", static_cast<double>(m_lastFrameStats.drawTriangleCount));
	H3D_PROFILE_PLOT("FGUIMaterialSwitches", static_cast<double>(m_lastFrameStats.materialSwitchCount));
	H3D_PROFILE_PLOT("FGUITextureSwitches", static_cast<double>(m_lastFrameStats.textureSwitchCount));
	H3D_PROFILE_PLOT("FGUIClippedCommands", static_cast<double>(m_lastFrameStats.clippedCommandCount));
	H3D_PROFILE_PLOT("FGUIStencilCommands", static_cast<double>(m_lastFrameStats.stencilCommandCount));
	H3D_PROFILE_PLOT("FGUICpuClipSourceTriangles", static_cast<double>(m_lastFrameStats.cpuClipSourceTriangleCount));
	H3D_PROFILE_PLOT("FGUICpuClipOutputTriangles", static_cast<double>(m_lastFrameStats.cpuClipOutputTriangleCount));
	H3D_PROFILE_PLOT("FGUIStencilClipPolygons", static_cast<double>(m_lastFrameStats.stencilClipPolygonCount));
	H3D_PROFILE_PLOT("FGUIMaxBatchTriangles", static_cast<double>(m_lastFrameStats.maxBatchTriangles));
}

void FairyGuiSystem::HandleWindowResized(unsigned int width, unsigned int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
	cocos2d::Director::getInstance()->getOpenGLView()->setFrameSize((float)width, (float)height);
	if (m_pRoot != nullptr)
		m_pRoot->setSize((float)width, (float)height);
	UpdateNativeImeCandidatePosition();
}

bool FairyGuiSystem::InjectMouseMove(int x, int y)
{
	if (!m_initialized || m_pRoot == nullptr || m_pRoot->getInputProcessor() == nullptr)
		return false;

	float mouseX = 0.0f;
	float mouseY = 0.0f;
	ConvertMousePosition(x, y, mouseX, mouseY);
	const float previousX = m_hasLastMousePosition ? m_lastMouseX : mouseX;
	const float previousY = m_hasLastMousePosition ? m_lastMouseY : mouseY;

	cocos2d::Touch touch;
	touch.setTouchInfo(0, mouseX, mouseY, previousX, previousY);
	m_pRoot->getInputProcessor()->touchMove(&touch, nullptr);

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;
	m_hasLastMousePosition = true;
	return IsMouseOnUi(mouseX, mouseY) || m_leftMouseDownOnUi;
}

bool FairyGuiSystem::InjectMouseDown(int x, int y, int button)
{
	if (!m_initialized || m_pRoot == nullptr || m_pRoot->getInputProcessor() == nullptr)
		return false;

	float mouseX = 0.0f;
	float mouseY = 0.0f;
	ConvertMousePosition(x, y, mouseX, mouseY);
	const bool mouseOnUi = IsMouseOnUi(mouseX, mouseY);
	LogInputHit("down", x, y, mouseX, mouseY, m_pRoot->hitTest(cocos2d::Vec2(mouseX, mouseY), cocos2d::Camera::getVisitingCamera()));

	if (button != 0)
	{
		m_lastMouseX = mouseX;
		m_lastMouseY = mouseY;
		m_hasLastMousePosition = true;
		m_leftMouseDownOnUi = mouseOnUi;
		return mouseOnUi;
	}

	cocos2d::Touch touch;
	touch.setTouchInfo(0, mouseX, mouseY, mouseX, mouseY);
	m_pRoot->getInputProcessor()->touchDown(&touch, nullptr);

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;
	m_hasLastMousePosition = true;
	m_leftMouseDownOnUi = mouseOnUi;
	return mouseOnUi;
}

bool FairyGuiSystem::InjectMouseUp(int x, int y, int button)
{
	if (!m_initialized || m_pRoot == nullptr || m_pRoot->getInputProcessor() == nullptr)
		return false;

	float mouseX = 0.0f;
	float mouseY = 0.0f;
	ConvertMousePosition(x, y, mouseX, mouseY);
	const float previousX = m_hasLastMousePosition ? m_lastMouseX : mouseX;
	const float previousY = m_hasLastMousePosition ? m_lastMouseY : mouseY;
	const bool consumed = IsMouseOnUi(mouseX, mouseY) || m_leftMouseDownOnUi;
	fairygui::GObject* hitTarget = m_pRoot->hitTest(cocos2d::Vec2(mouseX, mouseY), cocos2d::Camera::getVisitingCamera());
	LogInputHit("up", x, y, mouseX, mouseY, hitTarget);

	if (button != 0)
	{
		if (consumed && hitTarget != nullptr && hitTarget != m_pRoot)
			hitTarget->bubbleEvent(button == 2 ? fairygui::UIEventType::MiddleClick : fairygui::UIEventType::RightClick);
		m_lastMouseX = mouseX;
		m_lastMouseY = mouseY;
		m_hasLastMousePosition = true;
		m_leftMouseDownOnUi = false;
		return consumed;
	}

	cocos2d::Touch touch;
	touch.setTouchInfo(0, mouseX, mouseY, previousX, previousY);
	m_pRoot->getInputProcessor()->touchUp(&touch, nullptr);
	if (consumed)
		FocusTextInput(FindTextInputTarget(hitTarget));
	else
		ClearObjectHandleFocus();

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;
	m_hasLastMousePosition = true;
	m_leftMouseDownOnUi = false;
	return consumed;
}

bool FairyGuiSystem::InjectMouseWheel(int x, int y, int wheelDelta)
{
	if (!m_initialized || m_pRoot == nullptr || m_pRoot->getInputProcessor() == nullptr || wheelDelta == 0)
		return false;

	float mouseX = 0.0f;
	float mouseY = 0.0f;
	ConvertMousePosition(x, y, mouseX, mouseY);
	fairygui::GObject* target = m_pRoot->hitTest(cocos2d::Vec2(mouseX, mouseY), cocos2d::Camera::getVisitingCamera());
	const bool mouseOnUi = target != nullptr && target != m_pRoot;
	LogInputHit("wheel", x, y, mouseX, mouseY, target);

	if (mouseOnUi)
	{
		cocos2d::EventMouse event;
		event.setCursorPosition(mouseX, mouseY);
		event.setScrollData(0.0f, static_cast<float>(wheelDelta));
		m_pRoot->getInputProcessor()->mouseWheel(&event);
	}

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;
	m_hasLastMousePosition = true;
	return mouseOnUi;
}

bool FairyGuiSystem::InjectLogicalMouseMove(int x, int y)
{
	return InjectMouseMove(ToRawInputX(x), ToRawInputY(y));
}

bool FairyGuiSystem::InjectLogicalMouseDown(int x, int y, int button)
{
	return InjectMouseDown(ToRawInputX(x), ToRawInputY(y), button);
}

bool FairyGuiSystem::InjectLogicalMouseUp(int x, int y, int button)
{
	return InjectMouseUp(ToRawInputX(x), ToRawInputY(y), button);
}

bool FairyGuiSystem::InjectLogicalMouseWheel(int x, int y, int wheelDelta)
{
	return InjectMouseWheel(ToRawInputX(x), ToRawInputY(y), wheelDelta);
}

bool FairyGuiSystem::InjectKeyPressed(int keyCode, int keyText)
{
	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input == nullptr)
	{
		m_focusedObjectHandle = 0;
		return false;
	}

	input->dispatchEvent(fairygui::UIEventType::KeyDown, nullptr, cocos2d::Value(keyCode));
	if (keyCode == OIS_KC_TAB)
		return false;
	if (keyCode == OIS_KC_ESCAPE)
	{
		ClearObjectHandleFocus();
		return true;
	}
	return ApplyTextInputKey(input, keyCode, keyText);
}

bool FairyGuiSystem::InjectKeyReleased(int keyCode, int keyText)
{
	(void)keyText;
	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input == nullptr)
	{
		m_focusedObjectHandle = 0;
		return false;
	}

	input->dispatchEvent(fairygui::UIEventType::KeyUp, nullptr, cocos2d::Value(keyCode));
	return true;
}

bool FairyGuiSystem::LoadPackage(const std::string& packagePath)
{
	H3D_PROFILE_SCOPE_NAMED(loadPackageZone, "FairyGuiSystem::LoadPackage");
	if (!packagePath.empty())
	{
		H3D_PROFILE_TEXT(loadPackageZone, packagePath.c_str(), packagePath.size());
	}
	if (!m_initialized || packagePath.empty())
		return false;

	return AddPackage(packagePath) != nullptr;
}

std::string FairyGuiSystem::LoadPackageAndGetName(const std::string& packagePath)
{
	H3D_PROFILE_SCOPE_NAMED(loadPackageZone, "FairyGuiSystem::LoadPackageAndGetName");
	if (!packagePath.empty())
	{
		H3D_PROFILE_TEXT(loadPackageZone, packagePath.c_str(), packagePath.size());
	}
	if (!m_initialized || packagePath.empty())
		return std::string();

	fairygui::UIPackage* package = AddPackage(packagePath);
	return package != nullptr ? package->getName() : std::string();
}

bool FairyGuiSystem::RemovePackage(const std::string& packageName)
{
	H3D_PROFILE_SCOPE_NAMED(removePackageZone, "FairyGuiSystem::RemovePackage");
	if (!packageName.empty())
	{
		H3D_PROFILE_TEXT(removePackageZone, packageName.c_str(), packageName.size());
	}
	if (!m_initialized || packageName.empty())
		return false;

	fairygui::UIPackage::removePackage(packageName);
	return true;
}

fairygui::GObject* FairyGuiSystem::CreateObject(const std::string& packageName, const std::string& objectName)
{
	H3D_PROFILE_SCOPE_NAMED(createObjectZone, "FairyGuiSystem::CreateObject");
	if (!objectName.empty())
	{
		H3D_PROFILE_TEXT(createObjectZone, objectName.c_str(), objectName.size());
	}
	if (!m_initialized || packageName.empty() || objectName.empty())
		return nullptr;

	return fairygui::UIPackage::createObject(packageName, objectName);
}

bool FairyGuiSystem::AddToRoot(fairygui::GObject* object)
{
	if (!m_initialized || m_pRoot == nullptr || object == nullptr)
		return false;

	m_pRoot->addChild(object);
	return true;
}

int FairyGuiSystem::CreateObjectHandle(const std::string& packageName, const std::string& objectName)
{
	fairygui::GObject* object = CreateObject(packageName, objectName);
	if (object == nullptr)
		return 0;

	const int objectHandle = m_nextObjectHandle++;
	object->retain();

	ObjectHandleInfo handleInfo;
	handleInfo.object = object;
	handleInfo.ownerHandle = 0;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystem::CreateContainerHandle(const std::string& name)
{
	if (!m_initialized)
		return 0;

	fairygui::GComponent* container = fairygui::GComponent::create();
	if (container == nullptr)
		return 0;

	container->name = name;
	container->setSize(static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight));
	container->setOpaque(false);
	container->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = container;
	handleInfo.ownerHandle = 0;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystem::CreateChildContainerHandle(int ownerHandle, const std::string& name)
{
	if (!m_initialized)
		return 0;

	fairygui::GComponent* container = fairygui::GComponent::create();
	if (container == nullptr)
		return 0;

	container->name = name;
	container->setOpaque(false);
	container->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = container;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystem::CreateLoaderHandle(int ownerHandle, const std::string& name, const std::string& url)
{
	if (!m_initialized)
		return 0;

	fairygui::GLoader* loader = fairygui::GLoader::create();
	if (loader == nullptr)
		return 0;

	loader->name = name;
	loader->setURL(url);
	loader->setFill(fairygui::LoaderFillType::SCALE_FREE);
	loader->setTouchable(false);
	loader->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = loader;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystem::CreateTextHandle(int ownerHandle, const std::string& name, const std::string& text, float fontSize, float red, float green, float blue)
{
	if (!m_initialized)
		return 0;

	fairygui::GBasicTextField* label = fairygui::GBasicTextField::create();
	if (label == nullptr)
		return 0;

	label->name = name;
	label->setText(text);
	label->setFontSize(fontSize);
	label->setColor(cocos2d::Color3B(
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, red))),
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, green))),
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, blue)))));
	label->setTouchable(false);
	label->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = label;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystem::CreateTextInputHandle(int ownerHandle, const std::string& name, const std::string& text, float fontSize, float red, float green, float blue)
{
	if (!m_initialized)
		return 0;

	fairygui::GTextInput* input = fairygui::GTextInput::create();
	if (input == nullptr)
		return 0;

	input->name = name;
	input->setText(text);
	input->setFontSize(fontSize);
	input->setColor(cocos2d::Color3B(
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, red))),
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, green))),
		static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, blue)))));
	input->setTouchable(true);
	input->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = input;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystem::CreateGraphRectHandle(int ownerHandle, const std::string& name, float width, float height, float red, float green, float blue, float alpha)
{
	if (!m_initialized || width <= 0.0f || height <= 0.0f)
		return 0;

	fairygui::GGraph* graph = fairygui::GGraph::create();
	if (graph == nullptr)
		return 0;

	graph->name = name;
	graph->drawRect(width, height, 0, cocos2d::Color4F(red, green, blue, alpha), cocos2d::Color4F(red, green, blue, alpha));
	graph->setTouchable(false);
	graph->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = graph;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystem::CreateGraphRegularPolygonHandle(int ownerHandle, const std::string& name, float width, float height, int sides, float red, float green, float blue, float alpha)
{
	if (!m_initialized || width <= 0.0f || height <= 0.0f || sides < 3)
		return 0;

	fairygui::GGraph* graph = fairygui::GGraph::create();
	if (graph == nullptr)
		return 0;

	graph->name = name;
	graph->setSize(width, height);
	graph->drawRegularPolygon(0, cocos2d::Color4F(red, green, blue, alpha), cocos2d::Color4F(red, green, blue, alpha), sides, -90.0f);
	graph->setTouchable(false);
	graph->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = graph;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystem::CreateModalMaskHandle(float red, float green, float blue, float alpha)
{
	if (!m_initialized || m_screenWidth == 0 || m_screenHeight == 0)
		return 0;

	fairygui::GGraph* graph = fairygui::GGraph::create();
	if (graph == nullptr)
		return 0;

	graph->drawRect(
		static_cast<float>(m_screenWidth),
		static_cast<float>(m_screenHeight),
		0,
		cocos2d::Color4F(red, green, blue, alpha),
		cocos2d::Color4F(red, green, blue, alpha));
	graph->setTouchable(true);
	graph->retain();

	const int objectHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = graph;
	handleInfo.ownerHandle = 0;
	handleInfo.retained = true;
	m_objectHandles[objectHandle] = handleInfo;
	return objectHandle;
}

int FairyGuiSystem::GetObjectHandleChild(int objectHandle, const std::string& childPath)
{
	if (childPath.empty())
		return objectHandle;

	fairygui::GObject* child = FindEventTarget(objectHandle, childPath);
	if (child == nullptr)
		return 0;

	return GetOrCreateObjectAlias(GetObjectHandleOwner(objectHandle), child);
}

int FairyGuiSystem::GetObjectHandleListItem(int objectHandle, int itemIndex)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr || itemIndex < 0 || itemIndex >= list->getNumItems())
		return 0;

	const int childIndex = list->itemIndexToChildIndex(itemIndex);
	if (childIndex < 0 || childIndex >= list->numChildren())
		return 0;

	fairygui::GObject* item = list->getChildAt(childIndex);
	return GetOrCreateObjectAlias(GetObjectHandleOwner(objectHandle), item);
}

int FairyGuiSystem::GetObjectHandleListItemCount(int objectHandle)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr)
		return 0;

	return list->getNumItems();
}

bool FairyGuiSystem::AddObjectHandleToRoot(int objectHandle)
{
	return AddToRoot(FindObjectHandle(objectHandle));
}

bool FairyGuiSystem::AddObjectHandleToParent(int objectHandle, int parentHandle)
{
	if (parentHandle <= 0)
		return AddObjectHandleToRoot(objectHandle);

	fairygui::GObject* object = FindObjectHandle(objectHandle);
	fairygui::GComponent* parent = dynamic_cast<fairygui::GComponent*>(FindObjectHandle(parentHandle));
	if (object == nullptr || parent == nullptr)
		return false;

	parent->addChild(object);
	return true;
}

bool FairyGuiSystem::SetObjectHandlePosition(int objectHandle, float x, float y)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setPosition(x, y);
	return true;
}

bool FairyGuiSystem::SetObjectHandleSize(int objectHandle, float width, float height)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setSize(width, height);
	return true;
}

bool FairyGuiSystem::SetObjectHandleVisible(int objectHandle, bool visible)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setVisible(visible);
	return true;
}

bool FairyGuiSystem::SetObjectHandleAlpha(int objectHandle, float alpha)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setAlpha(alpha);
	return true;
}

bool FairyGuiSystem::SetObjectHandleTouchable(int objectHandle, bool touchable)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setTouchable(touchable);
	return true;
}

bool FairyGuiSystem::SetObjectHandleMask(int objectHandle, int maskHandle, bool inverted)
{
	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(FindObjectHandle(objectHandle));
	fairygui::GObject* mask = FindObjectHandle(maskHandle);
	if (component == nullptr || mask == nullptr)
		return false;

	component->setMask(mask->displayObject(), inverted);
	return true;
}

bool FairyGuiSystem::SetObjectHandleSortingOrder(int objectHandle, int sortingOrder)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setSortingOrder(sortingOrder);
	return true;
}

bool FairyGuiSystem::SetObjectHandleText(int objectHandle, const std::string& text)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setText(text);
	if (dynamic_cast<fairygui::GTextInput*>(object) != nullptr)
		m_textInputCarets[objectHandle] = GetUtf8CodepointCount(text);
	return true;
}

std::string FairyGuiSystem::GetObjectHandleText(int objectHandle) const
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	return object != nullptr ? object->getText() : std::string();
}

bool FairyGuiSystem::SetObjectHandleIcon(int objectHandle, const std::string& icon)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->setIcon(icon);
	return true;
}

bool FairyGuiSystem::SetObjectHandleLoaderUrl(int objectHandle, const std::string& url)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	fairygui::GLoader* loader = dynamic_cast<fairygui::GLoader*>(object);
	if (loader == nullptr)
		return false;

	loader->setURL(url);
	return true;
}

bool FairyGuiSystem::SetObjectHandleControllerIndex(int objectHandle, const std::string& controllerName, int selectedIndex)
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	if (controller == nullptr)
		return false;

	controller->setSelectedIndex(selectedIndex);
	return true;
}

int FairyGuiSystem::GetObjectHandleControllerIndex(int objectHandle, const std::string& controllerName) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	return controller != nullptr ? controller->getSelectedIndex() : -1;
}

bool FairyGuiSystem::SetObjectHandleControllerPage(int objectHandle, const std::string& controllerName, const std::string& pageName)
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	if (controller == nullptr)
		return false;

	controller->setSelectedPage(pageName);
	return true;
}

std::string FairyGuiSystem::GetObjectHandleControllerPage(int objectHandle, const std::string& controllerName) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	return controller != nullptr ? controller->getSelectedPage() : std::string();
}

std::string FairyGuiSystem::GetObjectHandleControllerPageId(int objectHandle, const std::string& controllerName) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	return controller != nullptr ? controller->getSelectedPageId() : std::string();
}

int FairyGuiSystem::GetObjectHandleControllerPageCount(int objectHandle, const std::string& controllerName) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	return controller != nullptr ? controller->getPageCount() : 0;
}

std::string FairyGuiSystem::GetObjectHandleControllerPageNameAt(int objectHandle, const std::string& controllerName, int pageIndex) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	if (controller == nullptr || pageIndex < 0 || pageIndex >= controller->getPageCount())
		return std::string();

	const std::string pageId = controller->getPageId(pageIndex);
	return controller->getPageNameById(pageId);
}

std::string FairyGuiSystem::GetObjectHandleControllerPageIdAt(int objectHandle, const std::string& controllerName, int pageIndex) const
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	if (controller == nullptr || pageIndex < 0 || pageIndex >= controller->getPageCount())
		return std::string();

	return controller->getPageId(pageIndex);
}

bool FairyGuiSystem::SetObjectHandleValue(int objectHandle, float value)
{
	return SetRangeObjectValue(FindObjectHandle(objectHandle), value);
}

float FairyGuiSystem::GetObjectHandleValue(int objectHandle) const
{
	return GetRangeObjectValue(FindObjectHandle(objectHandle));
}

bool FairyGuiSystem::SetObjectHandleMin(int objectHandle, float minValue)
{
	return SetRangeObjectMin(FindObjectHandle(objectHandle), minValue);
}

float FairyGuiSystem::GetObjectHandleMin(int objectHandle) const
{
	return GetRangeObjectMin(FindObjectHandle(objectHandle));
}

bool FairyGuiSystem::SetObjectHandleMax(int objectHandle, float maxValue)
{
	return SetRangeObjectMax(FindObjectHandle(objectHandle), maxValue);
}

float FairyGuiSystem::GetObjectHandleMax(int objectHandle) const
{
	return GetRangeObjectMax(FindObjectHandle(objectHandle));
}

bool FairyGuiSystem::SetObjectHandleComboBoxSelectedIndex(int objectHandle, int selectedIndex)
{
	fairygui::GComboBox* comboBox = dynamic_cast<fairygui::GComboBox*>(FindObjectHandle(objectHandle));
	if (comboBox == nullptr)
		return false;

	comboBox->setSelectedIndex(selectedIndex);
	return true;
}

int FairyGuiSystem::GetObjectHandleComboBoxSelectedIndex(int objectHandle) const
{
	fairygui::GComboBox* comboBox = dynamic_cast<fairygui::GComboBox*>(FindObjectHandle(objectHandle));
	return comboBox != nullptr ? comboBox->getSelectedIndex() : -1;
}

bool FairyGuiSystem::SetObjectHandleComboBoxValue(int objectHandle, const std::string& value)
{
	fairygui::GComboBox* comboBox = dynamic_cast<fairygui::GComboBox*>(FindObjectHandle(objectHandle));
	if (comboBox == nullptr)
		return false;

	comboBox->setValue(value);
	return true;
}

std::string FairyGuiSystem::GetObjectHandleComboBoxValue(int objectHandle) const
{
	fairygui::GComboBox* comboBox = dynamic_cast<fairygui::GComboBox*>(FindObjectHandle(objectHandle));
	return comboBox != nullptr ? comboBox->getValue() : std::string();
}

bool FairyGuiSystem::PlayObjectHandleTransition(int objectHandle, const std::string& transitionName, int times, float delay, int callbackId)
{
	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(FindObjectHandle(objectHandle));
	fairygui::Transition* transition = FindTransition(component, transitionName);
	if (transition == nullptr)
		return false;

	if (callbackId > 0)
	{
		const std::string callbackTransitionName = transitionName.empty() ? transition->name : transitionName;
		transition->play(times, delay, [callbackId, objectHandle, callbackTransitionName]() {
			ScriptLuaVM* luaVM = GetScriptLuaVM();
			if (luaVM == nullptr)
				return;

			luaVM->callFunction(
				"FairyGuiManager_DispatchTransition",
				"iis",
				callbackId,
				objectHandle,
				const_cast<char*>(callbackTransitionName.c_str()));
		});
		return true;
	}

	transition->play(times, delay);
	return true;
}

bool FairyGuiSystem::StopObjectHandleTransition(int objectHandle, const std::string& transitionName, bool setToComplete, bool processCallback)
{
	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(FindObjectHandle(objectHandle));
	fairygui::Transition* transition = FindTransition(component, transitionName);
	if (transition == nullptr)
		return false;

	transition->stop(setToComplete, processCallback);
	return true;
}

bool FairyGuiSystem::SetObjectHandleFocus(int objectHandle)
{
	fairygui::GTextInput* input = FindTextInput(objectHandle);
	return FocusTextInput(input);
}

bool FairyGuiSystem::ClearObjectHandleFocus()
{
	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input != nullptr)
		input->dispatchEvent(fairygui::UIEventType::Exit);
	CancelNativeImeComposition();
	m_focusedObjectHandle = 0;
	EndImeComposition(true);
	return true;
}

bool FairyGuiSystem::SetObjectHandleListItemCount(int objectHandle, int itemCount)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr || itemCount < 0)
		return false;

	list->setNumItems(itemCount);
	return true;
}

bool FairyGuiSystem::SetObjectHandleListSelectedIndex(int objectHandle, int selectedIndex)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr)
		return false;

	list->setSelectedIndex(selectedIndex);
	return true;
}

int FairyGuiSystem::GetObjectHandleListSelectedIndex(int objectHandle)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr)
		return -1;

	return list->getSelectedIndex();
}

bool FairyGuiSystem::ScrollObjectHandleListToView(int objectHandle, int itemIndex)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr || itemIndex < 0 || itemIndex >= list->getNumItems())
		return false;

	list->scrollToView(itemIndex);
	return true;
}

bool FairyGuiSystem::SetObjectHandleListVirtual(int objectHandle, bool loop)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr)
		return false;
	if (list->getScrollPane() == nullptr || list->getDefaultItem().empty())
		return false;

	if (list->itemRenderer == nullptr)
	{
		list->itemRenderer = [this, objectHandle](int itemIndex, fairygui::GObject* item) {
			ScriptLuaVM* luaVM = GetScriptLuaVM();
			if (luaVM == nullptr || item == nullptr)
				return;

			const int itemHandle = GetOrCreateObjectAlias(GetObjectHandleOwner(objectHandle), item);
			luaVM->callFunction(
				"FairyGuiManager_RenderVirtualListItem",
				"iii",
				objectHandle,
				itemIndex + 1,
				itemHandle);
		};
	}
	if (!list->isVirtual())
	{
		if (loop)
			list->setVirtualAndLoop();
		else
			list->setVirtual();
	}
	return true;
}

bool FairyGuiSystem::RefreshObjectHandleList(int objectHandle)
{
	fairygui::GList* list = dynamic_cast<fairygui::GList*>(FindObjectHandle(objectHandle));
	if (list == nullptr)
		return false;

	if (list->isVirtual())
		list->refreshVirtualList();
	return true;
}

bool FairyGuiSystem::CenterObjectHandle(int objectHandle, bool restraint)
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	if (object == nullptr)
		return false;

	object->center(restraint);
	return true;
}

int FairyGuiSystem::AddObjectHandleEventListener(int objectHandle, const std::string& childPath, int eventType, int callbackId)
{
	fairygui::GObject* target = FindEventTarget(objectHandle, childPath);
	if (target == nullptr || callbackId <= 0)
		return 0;

	const int bindingId = m_nextListenerBindingId++;
	ListenerBinding binding;
	binding.rootHandle = objectHandle;
	binding.callbackId = callbackId;
	binding.eventType = eventType;
	binding.target = target;
	binding.targetObject = target;
	m_listenerBindings[bindingId] = binding;

	target->addEventListener(eventType, [this, callbackId, objectHandle, eventType, bindingId](fairygui::EventContext* context) {
		DispatchObjectHandleEvent(callbackId, objectHandle, eventType, bindingId, context);
	}, fairygui::EventTag(bindingId));
	return bindingId;
}

int FairyGuiSystem::AddObjectHandleClickListener(int objectHandle, const std::string& childPath, int callbackId)
{
	return AddObjectHandleEventListener(objectHandle, childPath, fairygui::UIEventType::Click, callbackId);
}

int FairyGuiSystem::AddObjectHandleControllerChangedListener(int objectHandle, const std::string& controllerName, int callbackId)
{
	fairygui::GController* controller = FindController(objectHandle, controllerName);
	if (controller == nullptr || callbackId <= 0)
		return 0;

	const int bindingId = m_nextListenerBindingId++;
	ListenerBinding binding;
	binding.rootHandle = objectHandle;
	binding.callbackId = callbackId;
	binding.eventType = fairygui::UIEventType::Changed;
	binding.target = controller;
	binding.targetObject = FindObjectHandle(objectHandle);
	binding.controllerName = controllerName;
	m_listenerBindings[bindingId] = binding;

	controller->addEventListener(fairygui::UIEventType::Changed, [this, callbackId, objectHandle, bindingId](fairygui::EventContext* context) {
		DispatchObjectHandleEvent(callbackId, objectHandle, fairygui::UIEventType::Changed, bindingId, context);
	}, fairygui::EventTag(bindingId));
	return bindingId;
}

bool FairyGuiSystem::RemoveObjectHandleListener(int bindingId)
{
	std::map<int, ListenerBinding>::iterator it = m_listenerBindings.find(bindingId);
	if (it == m_listenerBindings.end())
		return false;

	if (it->second.target != nullptr)
		it->second.target->removeEventListener(it->second.eventType, fairygui::EventTag(bindingId));
	m_listenerBindings.erase(it);
	return true;
}

bool FairyGuiSystem::RemoveObjectHandle(int objectHandle)
{
	std::map<int, ObjectHandleInfo>::iterator it = m_objectHandles.find(objectHandle);
	if (it == m_objectHandles.end() || it->second.object == nullptr)
		return false;

	if (objectHandle == m_focusedObjectHandle || GetObjectHandleOwner(m_focusedObjectHandle) == objectHandle)
		ClearObjectHandleFocus();
	RemoveObjectHandleAliases(objectHandle);
	RemoveObjectHandleListeners(objectHandle);
	m_textInputCarets.erase(objectHandle);
	if (it->second.retained)
	{
		it->second.object->removeFromParent();
		it->second.object->release();
	}
	m_objectHandles.erase(it);
	return true;
}

void FairyGuiSystem::ClearObjectHandles()
{
	m_listenerBindings.clear();

	std::vector<int> rootHandles;
	for (std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->second.ownerHandle == 0)
			rootHandles.push_back(it->first);
	}

	for (std::vector<int>::const_iterator it = rootHandles.begin(); it != rootHandles.end(); ++it)
		RemoveObjectHandle(*it);

	while (!m_objectHandles.empty())
		RemoveObjectHandle(m_objectHandles.begin()->first);
	m_textInputCarets.clear();
	m_focusedObjectHandle = 0;
}

bool FairyGuiSystem::CreateSmokeTestImage(const std::string& imagePath)
{
	if (!m_initialized || m_pRoot == nullptr || imagePath.empty())
		return false;

	fairygui::GLoader* loader = fairygui::GLoader::create();
	if (loader == nullptr)
		return false;

	loader->setPosition(16.0f, 16.0f);
	loader->setSize(256.0f, 64.0f);
	loader->setFill(fairygui::LoaderFillType::SCALE);
	loader->setURL(imagePath);
	m_pRoot->addChild(loader);
	return true;
}

void FairyGuiSystem::handleTrianglesCommand(const cocos2d::TrianglesCommand& command)
{
	if (m_pManualObject == nullptr || m_screenWidth == 0 || m_screenHeight == 0)
		return;

	const cocos2d::TrianglesCommand::Triangles& triangles = command.getTriangles();
	if (triangles.verts == nullptr || triangles.indices == nullptr || triangles.vertCount <= 0 || triangles.indexCount <= 0)
		return;

	if (m_stencilStage == cocos2d::STENCIL_STAGE_WRITE)
	{
		RecordStencilCommand(triangles.indexCount / 3);
		CollectStencilTriangle(command);
		return;
	}

	const std::string& materialName = GetMaterialName(command.getTexture());
	const int submittedTriangleCount = triangles.indexCount / 3;
	const bool hasScissorClip = m_scissorEnabled && m_scissorRect.size.width > 0.0f && m_scissorRect.size.height > 0.0f;
	const bool hasStencilClip = !m_stencilScopes.empty();
	if (hasScissorClip || hasStencilClip)
	{
		struct ActiveStencilScope
		{
			bool inverted;
			bool valid;
			std::vector<std::vector<ClipVertex> > polygons;
		};

		std::vector<ActiveStencilScope> activeScopes;
		activeScopes.reserve(m_stencilScopes.size());
		int stencilClipPolygonCount = 0;
		for (std::vector<StencilClipInfo>::const_iterator scopeIt = m_stencilScopes.begin(); scopeIt != m_stencilScopes.end(); ++scopeIt)
		{
			ActiveStencilScope activeScope;
			activeScope.inverted = scopeIt->inverted;
			activeScope.valid = scopeIt->valid;
			if (scopeIt->valid)
			{
				for (std::vector<StencilClipInfo::Polygon>::const_iterator polygonIt = scopeIt->polygons.begin(); polygonIt != scopeIt->polygons.end(); ++polygonIt)
				{
					std::vector<ClipVertex> polygon = BuildClipPolygon(polygonIt->points);
					if (IsValidClipPolygon(polygon))
						activeScope.polygons.push_back(polygon);
				}
				if (activeScope.polygons.empty())
				{
					std::vector<ClipVertex> rectPolygon = BuildRectClipPolygon(scopeIt->rect);
					if (IsValidClipPolygon(rectPolygon))
						activeScope.polygons.push_back(rectPolygon);
				}
				activeScope.valid = !activeScope.polygons.empty();
			}
			stencilClipPolygonCount += static_cast<int>(activeScope.polygons.size());
			activeScopes.push_back(activeScope);
		}

		const int stencilClipScopeCount = hasStencilClip ? static_cast<int>(activeScopes.size()) : 0;
		const std::vector<ClipVertex> scissorPolygon = hasScissorClip ? BuildRectClipPolygon(m_scissorRect) : std::vector<ClipVertex>();
		std::vector<ClipVertex> outputVertices;
		std::vector<std::vector<ClipVertex> > fragments;
		std::vector<std::vector<ClipVertex> > nextFragments;
		std::vector<ClipVertex> clippedPolygon;
		std::vector<ClipVertex> clipScratch;
		std::vector<ClipVertex> insideScratch;
		std::vector<ClipVertex> outsideScratch;
		outputVertices.reserve(static_cast<size_t>(triangles.indexCount));
		fragments.reserve(8);
		nextFragments.reserve(8);
		clippedPolygon.reserve(8);
		clipScratch.reserve(8);
		insideScratch.reserve(8);
		outsideScratch.reserve(8);
		int outputFragmentCount = 0;
		for (int index = 0; index + 2 < triangles.indexCount; index += 3)
		{
			std::vector<ClipVertex> triangle;
			triangle.resize(3);
			bool validTriangle = true;
			for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
			{
				const int sourceIndex = triangles.indices[index + vertexIndex];
				if (sourceIndex < 0 || sourceIndex >= triangles.vertCount)
				{
					validTriangle = false;
					break;
				}

				const cocos2d::V3F_C4B_T2F& vertex = triangles.verts[sourceIndex];
				triangle[vertexIndex].x = TransformX(command.getTransform(), vertex.vertices);
				triangle[vertexIndex].y = TransformY(command.getTransform(), vertex.vertices);
				triangle[vertexIndex].u = vertex.texCoords.u;
				triangle[vertexIndex].v = vertex.texCoords.v;
				triangle[vertexIndex].color = ToOgreColor(vertex.colors);
			}

			if (!validTriangle || !IsValidClipPolygon(triangle))
				continue;

			fragments.clear();
			fragments.push_back(triangle);
			if (hasScissorClip)
			{
				nextFragments.clear();
				for (std::vector<std::vector<ClipVertex> >::const_iterator fragmentIt = fragments.begin(); fragmentIt != fragments.end(); ++fragmentIt)
				{
					ClipPolygonToConvexPolygon(*fragmentIt, scissorPolygon, clippedPolygon, clipScratch);
					if (IsValidClipPolygon(clippedPolygon))
						nextFragments.push_back(clippedPolygon);
				}
				fragments.swap(nextFragments);
				if (fragments.empty())
					continue;
			}

			for (std::vector<ActiveStencilScope>::const_iterator scopeIt = activeScopes.begin(); scopeIt != activeScopes.end(); ++scopeIt)
			{
				if (!scopeIt->valid)
				{
					if (scopeIt->inverted)
						continue;
					fragments.clear();
					break;
				}

				if (scopeIt->inverted)
				{
					for (std::vector<std::vector<ClipVertex> >::const_iterator maskIt = scopeIt->polygons.begin(); maskIt != scopeIt->polygons.end(); ++maskIt)
					{
						nextFragments.clear();
						for (std::vector<std::vector<ClipVertex> >::const_iterator fragmentIt = fragments.begin(); fragmentIt != fragments.end(); ++fragmentIt)
							SubtractConvexPolygon(*fragmentIt, *maskIt, nextFragments, insideScratch, outsideScratch);
						fragments.swap(nextFragments);
						if (fragments.empty())
							break;
					}
				}
				else
				{
					nextFragments.clear();
					for (std::vector<std::vector<ClipVertex> >::const_iterator fragmentIt = fragments.begin(); fragmentIt != fragments.end(); ++fragmentIt)
					{
						for (std::vector<std::vector<ClipVertex> >::const_iterator maskIt = scopeIt->polygons.begin(); maskIt != scopeIt->polygons.end(); ++maskIt)
						{
							ClipPolygonToConvexPolygon(*fragmentIt, *maskIt, clippedPolygon, clipScratch);
							if (IsValidClipPolygon(clippedPolygon))
								nextFragments.push_back(clippedPolygon);
						}
					}
					fragments.swap(nextFragments);
				}

				if (fragments.empty())
					break;
			}

			outputFragmentCount += static_cast<int>(fragments.size());
			for (std::vector<std::vector<ClipVertex> >::const_iterator fragmentIt = fragments.begin(); fragmentIt != fragments.end(); ++fragmentIt)
				AppendPolygonTriangles(*fragmentIt, outputVertices);
		}

		if (outputVertices.empty())
		{
			RecordCpuClipWork(submittedTriangleCount, 0, outputFragmentCount, stencilClipScopeCount, stencilClipPolygonCount);
			RecordDrawCommand(command.getTexture(), materialName, triangles.vertCount, submittedTriangleCount, 0, true);
			return;
		}

		const int outputTriangleCount = static_cast<int>(outputVertices.size() / 3);
		RecordCpuClipWork(submittedTriangleCount, outputTriangleCount, outputFragmentCount, stencilClipScopeCount, stencilClipPolygonCount);
		RecordDrawCommand(command.getTexture(), materialName, static_cast<int>(outputVertices.size()), submittedTriangleCount, outputTriangleCount, true);
		m_pManualObject->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
		m_pManualObject->estimateVertexCount(outputVertices.size());
		m_pManualObject->estimateIndexCount(outputVertices.size());
		for (size_t index = 0; index < outputVertices.size(); ++index)
		{
			const ClipVertex& vertex = outputVertices[index];
			const float ndcX = vertex.x / static_cast<float>(m_screenWidth) * 2.0f - 1.0f;
			const float ndcY = vertex.y / static_cast<float>(m_screenHeight) * 2.0f - 1.0f;
			m_pManualObject->position(ndcX, ndcY, 0.0f);
			m_pManualObject->textureCoord(vertex.u, vertex.v);
			m_pManualObject->colour(vertex.color);
			m_pManualObject->index(static_cast<Ogre::uint32>(index));
		}
		m_pManualObject->end();
		return;
	}

	RecordDrawCommand(command.getTexture(), materialName, triangles.vertCount, submittedTriangleCount, submittedTriangleCount, false);
	m_pManualObject->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
	m_pManualObject->estimateVertexCount(static_cast<size_t>(triangles.vertCount));
	m_pManualObject->estimateIndexCount(static_cast<size_t>(triangles.indexCount));

	for (int index = 0; index < triangles.vertCount; ++index)
	{
		const cocos2d::V3F_C4B_T2F& vertex = triangles.verts[index];
		const float x = TransformX(command.getTransform(), vertex.vertices);
		const float y = TransformY(command.getTransform(), vertex.vertices);
		const float ndcX = x / static_cast<float>(m_screenWidth) * 2.0f - 1.0f;
		const float ndcY = y / static_cast<float>(m_screenHeight) * 2.0f - 1.0f;
		m_pManualObject->position(ndcX, ndcY, 0.0f);
		m_pManualObject->textureCoord(vertex.texCoords.u, vertex.texCoords.v);
		m_pManualObject->colour(ToOgreColor(vertex.colors));
	}

	for (int index = 0; index < triangles.indexCount; ++index)
		m_pManualObject->index(static_cast<Ogre::uint32>(triangles.indices[index]));

	m_pManualObject->end();
}

void FairyGuiSystem::handleCustomCommand(const cocos2d::CustomCommand& command)
{
	(void)command;
	++m_currentFrameStats.customCommandCount;
	SyncScissorState();
	SyncStencilState();
}

fairygui::GObject* FairyGuiSystem::FindObjectHandle(int objectHandle) const
{
	std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.find(objectHandle);
	return it != m_objectHandles.end() ? it->second.object : nullptr;
}

fairygui::GObject* FairyGuiSystem::FindEventTarget(int objectHandle, const std::string& childPath) const
{
	fairygui::GObject* root = FindObjectHandle(objectHandle);
	if (root == nullptr || childPath.empty())
		return root;

	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(root);
	if (component == nullptr)
		return nullptr;

	return component->getChildByPath(childPath);
}

fairygui::GController* FairyGuiSystem::FindController(int objectHandle, const std::string& controllerName) const
{
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(object);
	if (component == nullptr)
		return nullptr;

	if (!controllerName.empty())
		return component->getController(controllerName);
	if (!component->getControllers().empty())
		return component->getControllerAt(0);
	return nullptr;
}

fairygui::GTextInput* FairyGuiSystem::FindTextInput(int objectHandle) const
{
	return dynamic_cast<fairygui::GTextInput*>(FindObjectHandle(objectHandle));
}

fairygui::GTextInput* FairyGuiSystem::FindTextInputTarget(fairygui::GObject* target) const
{
	while (target != nullptr && target != m_pRoot)
	{
		fairygui::GTextInput* input = dynamic_cast<fairygui::GTextInput*>(target);
		if (input != nullptr)
			return input;
		target = target->findParent();
	}
	return nullptr;
}

int FairyGuiSystem::FindOwnerHandleForObject(fairygui::GObject* object) const
{
	if (object == nullptr)
		return 0;

	for (std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->second.object == object)
			return GetObjectHandleOwner(it->first);
	}

	for (std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->second.ownerHandle != 0)
			continue;
		fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(it->second.object);
		if (component != nullptr && component->isAncestorOf(object))
			return it->first;
	}
	return 0;
}

int FairyGuiSystem::GetOrCreateObjectAlias(int ownerHandle, fairygui::GObject* object)
{
	if (object == nullptr)
		return 0;

	ownerHandle = GetObjectHandleOwner(ownerHandle);
	for (std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->second.object == object && (it->first == ownerHandle || it->second.ownerHandle == ownerHandle))
			return it->first;
	}

	const int aliasHandle = m_nextObjectHandle++;
	ObjectHandleInfo handleInfo;
	handleInfo.object = object;
	handleInfo.ownerHandle = ownerHandle;
	handleInfo.retained = false;
	m_objectHandles[aliasHandle] = handleInfo;
	return aliasHandle;
}

int FairyGuiSystem::GetObjectHandleOwner(int objectHandle) const
{
	std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.find(objectHandle);
	if (it == m_objectHandles.end())
		return objectHandle;
	return it->second.ownerHandle != 0 ? it->second.ownerHandle : objectHandle;
}

void FairyGuiSystem::RemoveObjectHandleAliases(int objectHandle)
{
	std::vector<int> aliasHandles;
	for (std::map<int, ObjectHandleInfo>::const_iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->first != objectHandle && it->second.ownerHandle == objectHandle)
			aliasHandles.push_back(it->first);
	}

	for (std::vector<int>::const_iterator it = aliasHandles.begin(); it != aliasHandles.end(); ++it)
		RemoveObjectHandle(*it);
}

void FairyGuiSystem::RemoveObjectHandleListeners(int objectHandle)
{
	std::vector<int> bindingIds;
	for (std::map<int, ListenerBinding>::const_iterator it = m_listenerBindings.begin(); it != m_listenerBindings.end(); ++it)
	{
		if (it->second.rootHandle == objectHandle)
			bindingIds.push_back(it->first);
	}

	for (std::vector<int>::const_iterator it = bindingIds.begin(); it != bindingIds.end(); ++it)
		RemoveObjectHandleListener(*it);
}

bool FairyGuiSystem::FocusTextInput(fairygui::GTextInput* input)
{
	if (input == nullptr)
	{
		ClearObjectHandleFocus();
		return false;
	}

	const int ownerHandle = FindOwnerHandleForObject(input);
	const int inputHandle = GetOrCreateObjectAlias(ownerHandle, input);
	if (inputHandle <= 0)
		return false;

	if (m_focusedObjectHandle != inputHandle)
	{
		ClearObjectHandleFocus();
		m_focusedObjectHandle = inputHandle;
		input->dispatchEvent(fairygui::UIEventType::Enter);
	}
	if (m_textInputCarets.find(inputHandle) == m_textInputCarets.end())
		m_textInputCarets[inputHandle] = GetUtf8CodepointCount(input->getText());
	UpdateNativeImeCandidatePosition();
	return true;
}

bool FairyGuiSystem::ApplyTextInputKey(fairygui::GTextInput* input, int keyCode, int keyText)
{
	if (input == nullptr)
		return false;

	const int inputHandle = m_focusedObjectHandle;
	std::string text = input->getText();
	size_t caret = GetUtf8CodepointCount(text);
	std::map<int, size_t>::iterator caretIt = m_textInputCarets.find(inputHandle);
	if (caretIt != m_textInputCarets.end())
		caret = ClampUtf8Caret(text, caretIt->second);

	bool changed = false;
	if (keyCode == OIS_KC_HOME)
	{
		caret = 0;
	}
	else if (keyCode == OIS_KC_END)
	{
		caret = GetUtf8CodepointCount(text);
	}
	else if (keyCode == OIS_KC_LEFT)
	{
		if (caret > 0)
			--caret;
	}
	else if (keyCode == OIS_KC_RIGHT)
	{
		const size_t count = GetUtf8CodepointCount(text);
		if (caret < count)
			++caret;
	}
	else if (keyCode == OIS_KC_BACK)
	{
		changed = RemoveUtf8CodepointBeforeCaret(text, caret);
	}
	else if (keyCode == OIS_KC_DELETE)
	{
		changed = RemoveUtf8CodepointAtCaret(text, caret);
	}
	else if (keyCode == OIS_KC_RETURN || keyCode == OIS_KC_NUMPADENTER)
	{
		m_textInputCarets[inputHandle] = caret;
		input->dispatchEvent(fairygui::UIEventType::Submit);
		return true;
	}
	else if (IsPrintableKeyText(keyText))
	{
		changed = InsertUtf8CodepointAtCaret(text, caret, static_cast<unsigned int>(keyText));
	}

	if (changed)
	{
		input->setText(text);
		input->dispatchEvent(fairygui::UIEventType::Changed);
	}
	m_textInputCarets[inputHandle] = caret;
	return true;
}

bool FairyGuiSystem::ApplyTextInputUtf8Text(fairygui::GTextInput* input, const std::string& committedText)
{
	if (input == nullptr || committedText.empty())
		return false;

	const int inputHandle = m_focusedObjectHandle;
	std::string text = input->getText();
	size_t caret = GetUtf8CodepointCount(text);
	std::map<int, size_t>::iterator caretIt = m_textInputCarets.find(inputHandle);
	if (caretIt != m_textInputCarets.end())
		caret = ClampUtf8Caret(text, caretIt->second);

	if (!InsertUtf8TextAtCaret(text, caret, committedText))
		return false;

	input->setText(text);
	input->dispatchEvent(fairygui::UIEventType::Changed);
	m_textInputCarets[inputHandle] = caret;
	return true;
}

void FairyGuiSystem::EndImeComposition(bool countEnd)
{
	if (countEnd && (m_imeStats.compositionActive || !m_imeStats.compositionText.empty() || m_imeStats.candidateOpen))
		++m_imeStats.compositionEndCount;
	m_imeStats.compositionActive = false;
	m_imeStats.candidateOpen = false;
	m_imeStats.compositionText.clear();
	m_imeStats.candidateCount = 0;
	m_imeStats.candidateSelection = -1;
}

bool FairyGuiSystem::InjectImeCompositionText(const std::string& text)
{
	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input == nullptr)
	{
		m_focusedObjectHandle = 0;
		EndImeComposition(true);
		return false;
	}

	m_imeStats.focusedHandle = m_focusedObjectHandle;
	m_imeStats.compositionActive = !text.empty();
	m_imeStats.compositionText = text;
	++m_imeStats.compositionUpdateCount;
	UpdateNativeImeCandidatePosition();
	return true;
}

bool FairyGuiSystem::InjectImeCommitText(const std::string& text)
{
	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input == nullptr)
	{
		m_focusedObjectHandle = 0;
		EndImeComposition(true);
		return false;
	}

	m_imeStats.focusedHandle = m_focusedObjectHandle;
	m_imeStats.commitText = text;
	++m_imeStats.compositionCommitCount;
	EndImeComposition(false);
	return ApplyTextInputUtf8Text(input, text);
}

bool FairyGuiSystem::ClearImeCompositionText()
{
	CancelNativeImeComposition();
	EndImeComposition(true);
	return true;
}

std::string FairyGuiSystem::GetImeDebugString() const
{
	std::ostringstream stream;
	stream << "active=" << (m_imeStats.compositionActive ? 1 : 0)
		<< " candidate=" << (m_imeStats.candidateOpen ? 1 : 0)
		<< " focus=" << m_imeStats.focusedHandle
		<< " compUpdates=" << m_imeStats.compositionUpdateCount
		<< " commits=" << m_imeStats.compositionCommitCount
		<< " ends=" << m_imeStats.compositionEndCount
		<< " candOpen=" << m_imeStats.candidateOpenCount
		<< " candClose=" << m_imeStats.candidateCloseCount
		<< " candChange=" << m_imeStats.candidateChangeCount
		<< " candCount=" << m_imeStats.candidateCount
		<< " candSelection=" << m_imeStats.candidateSelection
		<< " pos=" << m_imeStats.compositionX << "," << m_imeStats.compositionY
		<< " comp=\"" << m_imeStats.compositionText << "\""
		<< " commit=\"" << m_imeStats.commitText << "\"";
	return stream.str();
}

void FairyGuiSystem::InstallNativeImeHook()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (m_nativeImeHookInstalled || m_pRenderWindow == nullptr)
		return;

	HWND hwnd = nullptr;
	m_pRenderWindow->getCustomAttribute("WINDOW", &hwnd);
	if (hwnd == nullptr)
		return;

	WNDPROC currentProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hwnd, GWLP_WNDPROC));
	if (currentProc == FairyGuiNativeImeWndProc)
	{
		std::map<HWND, WNDPROC>::iterator procIt = g_nativeImePreviousWndProcByWindow.find(hwnd);
		if (procIt == g_nativeImePreviousWndProcByWindow.end() || procIt->second == nullptr)
			return;
		m_nativeWindowHandle = hwnd;
		m_previousWindowProc = reinterpret_cast<void*>(procIt->second);
		g_nativeImeSystemsByWindow[hwnd] = this;
		m_nativeImeHookInstalled = true;
		return;
	}

	m_nativeWindowHandle = hwnd;
	m_previousWindowProc = reinterpret_cast<void*>(currentProc);
	g_nativeImePreviousWndProcByWindow[hwnd] = currentProc;
	g_nativeImeSystemsByWindow[hwnd] = this;
	SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(FairyGuiNativeImeWndProc));
	m_nativeImeHookInstalled = true;
#endif
}

void FairyGuiSystem::RemoveNativeImeHook()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (!m_nativeImeHookInstalled || m_nativeWindowHandle == nullptr)
		return;

	HWND hwnd = m_nativeWindowHandle;
	std::map<HWND, FairyGuiSystem*>::iterator systemIt = g_nativeImeSystemsByWindow.find(hwnd);
	if (systemIt != g_nativeImeSystemsByWindow.end() && systemIt->second != this)
	{
		m_nativeWindowHandle = nullptr;
		m_previousWindowProc = nullptr;
		m_nativeImeHookInstalled = false;
		return;
	}

	std::map<HWND, WNDPROC>::iterator procIt = g_nativeImePreviousWndProcByWindow.find(hwnd);
	const LONG_PTR currentProc = GetWindowLongPtr(hwnd, GWLP_WNDPROC);
	if (procIt != g_nativeImePreviousWndProcByWindow.end() && currentProc == reinterpret_cast<LONG_PTR>(FairyGuiNativeImeWndProc))
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(procIt->second));

	if (systemIt != g_nativeImeSystemsByWindow.end())
		g_nativeImeSystemsByWindow.erase(systemIt);
	g_nativeImePreviousWndProcByWindow.erase(hwnd);
	m_nativeWindowHandle = nullptr;
	m_previousWindowProc = nullptr;
	m_nativeImeHookInstalled = false;
#endif
}

bool FairyGuiSystem::HandleNativeImeMessage(unsigned int message, unsigned long long wParam, long long lParam, long long& result)
{
	result = 0;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (!m_initialized || m_nativeWindowHandle == nullptr)
		return false;

	if (message == WM_IME_STARTCOMPOSITION)
	{
		m_imeStats.compositionActive = true;
		m_imeStats.focusedHandle = m_focusedObjectHandle;
		UpdateNativeImeCandidatePosition();
		return false;
	}

	if (message == WM_IME_ENDCOMPOSITION)
	{
		EndImeComposition(true);
		return false;
	}

	if (message == WM_IME_NOTIFY)
	{
		HIMC context = ImmGetContext(m_nativeWindowHandle);
		const DWORD notify = static_cast<DWORD>(wParam);
		bool updateCandidatePosition = false;
		if (notify == IMN_OPENCANDIDATE)
		{
			m_imeStats.candidateOpen = true;
			++m_imeStats.candidateOpenCount;
			ReadImeCandidateInfo(context, m_imeStats.candidateCount, m_imeStats.candidateSelection);
			updateCandidatePosition = true;
		}
		else if (notify == IMN_CLOSECANDIDATE)
		{
			m_imeStats.candidateOpen = false;
			++m_imeStats.candidateCloseCount;
			m_imeStats.candidateCount = 0;
			m_imeStats.candidateSelection = -1;
		}
		else if (notify == IMN_CHANGECANDIDATE)
		{
			++m_imeStats.candidateChangeCount;
			ReadImeCandidateInfo(context, m_imeStats.candidateCount, m_imeStats.candidateSelection);
			updateCandidatePosition = true;
		}
		if (context != nullptr)
			ImmReleaseContext(m_nativeWindowHandle, context);
		if (updateCandidatePosition)
			UpdateNativeImeCandidatePosition();
		return false;
	}

	if (message != WM_IME_COMPOSITION)
		return false;

	HIMC context = ImmGetContext(m_nativeWindowHandle);
	if (context == nullptr)
		return false;

	const LPARAM flags = static_cast<LPARAM>(lParam);
	bool consumed = false;
	bool updateCandidatePosition = false;
	if ((flags & GCS_COMPSTR) != 0)
	{
		m_imeStats.focusedHandle = m_focusedObjectHandle;
		m_imeStats.compositionText = ReadImeCompositionString(context, GCS_COMPSTR);
		m_imeStats.compositionActive = !m_imeStats.compositionText.empty();
		++m_imeStats.compositionUpdateCount;
		updateCandidatePosition = true;
	}

	if ((flags & GCS_RESULTSTR) != 0)
	{
		const std::string resultText = ReadImeCompositionString(context, GCS_RESULTSTR);
		ImmReleaseContext(m_nativeWindowHandle, context);
		consumed = !resultText.empty() && InjectImeCommitText(resultText);
		result = 0;
		return consumed;
	}

	ImmReleaseContext(m_nativeWindowHandle, context);
	if (updateCandidatePosition)
		UpdateNativeImeCandidatePosition();
	return consumed;
#else
	(void)message;
	(void)wParam;
	(void)lParam;
	return false;
#endif
}

void FairyGuiSystem::CancelNativeImeComposition()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (m_nativeWindowHandle == nullptr)
		return;
	if (!m_imeStats.compositionActive && m_imeStats.compositionText.empty() && !m_imeStats.candidateOpen)
		return;

	HIMC context = ImmGetContext(m_nativeWindowHandle);
	if (context == nullptr)
		return;
	ImmNotifyIME(context, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
	ImmReleaseContext(m_nativeWindowHandle, context);
#endif
}

void FairyGuiSystem::UpdateNativeImeCandidatePosition()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (m_nativeWindowHandle == nullptr)
		return;

	fairygui::GTextInput* input = FindTextInput(m_focusedObjectHandle);
	if (input == nullptr)
		return;

	HIMC context = ImmGetContext(m_nativeWindowHandle);
	if (context == nullptr)
		return;

	const cocos2d::Rect bounds = input->localToGlobal(cocos2d::Rect(cocos2d::Vec2::ZERO, input->getSize()));
	const float candidateY = m_screenHeight > 0 ? static_cast<float>(m_screenHeight) - bounds.getMinY() : bounds.getMinY();
	const int rawX = ToRawInputX(ClampClientCoordinate(bounds.getMinX(), m_screenWidth));
	const int rawY = ToRawInputY(ClampClientCoordinate(candidateY, m_screenHeight));
	m_imeStats.focusedHandle = m_focusedObjectHandle;
	m_imeStats.compositionX = rawX;
	m_imeStats.compositionY = rawY;

	COMPOSITIONFORM composition = {};
	composition.dwStyle = CFS_POINT;
	composition.ptCurrentPos.x = rawX;
	composition.ptCurrentPos.y = rawY;
	ImmSetCompositionWindow(context, &composition);

	CANDIDATEFORM candidate = {};
	candidate.dwIndex = 0;
	candidate.dwStyle = CFS_CANDIDATEPOS;
	candidate.ptCurrentPos = composition.ptCurrentPos;
	ImmSetCandidateWindow(context, &candidate);
	ImmReleaseContext(m_nativeWindowHandle, context);
#endif
}

void FairyGuiSystem::DispatchObjectHandleEvent(int callbackId, int objectHandle, int eventType, int bindingId, fairygui::EventContext* context)
{
	H3D_PROFILE_SCOPE("FairyGuiSystem::DispatchEvent");
	ScriptLuaVM* luaVM = GetScriptLuaVM();
	if (luaVM == nullptr)
		return;

	int senderHandle = 0;
	int itemHandle = 0;
	int itemIndex = -1;
	int x = 0;
	int y = 0;
	int button = -1;
	int touchId = -1;
	int wheelDelta = 0;
	int dragDeltaX = 0;
	int dragDeltaY = 0;

	if (context != nullptr)
	{
		fairygui::GObject* sender = dynamic_cast<fairygui::GObject*>(context->getSender());
		if (sender != nullptr)
			senderHandle = GetOrCreateObjectAlias(objectHandle, sender);

		if (eventType == fairygui::UIEventType::ClickItem || eventType == fairygui::UIEventType::RightClickItem)
		{
			fairygui::GObject* item = static_cast<fairygui::GObject*>(context->getData());
			if (item != nullptr)
			{
				itemHandle = GetOrCreateObjectAlias(objectHandle, item);

				std::map<int, ListenerBinding>::const_iterator bindingIt = m_listenerBindings.find(bindingId);
				fairygui::GList* list = bindingIt != m_listenerBindings.end() ? dynamic_cast<fairygui::GList*>(bindingIt->second.targetObject) : nullptr;
				if (list != nullptr)
				{
					const int childIndex = list->getChildIndex(item);
					if (childIndex >= 0)
						itemIndex = list->childIndexToItemIndex(childIndex);
				}
			}
		}

		fairygui::InputEvent* input = context->getInput();
		if (input != nullptr)
		{
			x = input->getX();
			y = input->getY();
			button = static_cast<int>(input->getButton());
			touchId = input->getTouchId();
			wheelDelta = input->getMouseWheelDelta();
			cocos2d::Touch* touch = input->getTouch();
			if (touch != nullptr)
			{
				const cocos2d::Vec2& location = touch->getLocation();
				const cocos2d::Vec2& previousLocation = touch->getPreviousLocation();
				dragDeltaX = static_cast<int>(location.x - previousLocation.x);
				dragDeltaY = static_cast<int>(location.y - previousLocation.y);
			}
		}
	}

	luaVM->callFunction(
		"FairyGuiManager_DispatchEvent",
		"iiiiiiiiiiiiii",
		callbackId,
		objectHandle,
		eventType,
		bindingId,
		senderHandle,
		itemHandle,
		itemIndex,
		x,
		y,
		button,
		touchId,
		wheelDelta,
		dragDeltaX,
		dragDeltaY);
}

void FairyGuiSystem::ConvertMousePosition(int x, int y, float& outX, float& outY) const
{
	const float scaleX = GetInputScaleX();
	const float scaleY = GetInputScaleY();
	outX = static_cast<float>(x) / scaleX;
	const float logicalY = static_cast<float>(y) / scaleY;
	outY = m_screenHeight > 0 ? static_cast<float>(m_screenHeight) - logicalY : logicalY;
}

float FairyGuiSystem::GetInputScaleX() const
{
	return GetWindowInputScale(m_pRenderWindow, m_screenWidth, true);
}

float FairyGuiSystem::GetInputScaleY() const
{
	return GetWindowInputScale(m_pRenderWindow, m_screenHeight, false);
}

int FairyGuiSystem::ToRawInputX(int x) const
{
	return ToRawInputCoordinate(x, GetInputScaleX());
}

int FairyGuiSystem::ToRawInputY(int y) const
{
	return ToRawInputCoordinate(y, GetInputScaleY());
}

bool FairyGuiSystem::IsMouseOnUi(float x, float y) const
{
	if (m_pRoot == nullptr)
		return false;

	fairygui::GObject* target = m_pRoot->hitTest(cocos2d::Vec2(x, y), cocos2d::Camera::getVisitingCamera());
	return target != nullptr && target != m_pRoot;
}

void FairyGuiSystem::BeginOgreRender()
{
	m_stencilScopes.clear();
	m_pendingStencilValid = false;
	m_pendingStencilDepth = 0;
	m_pendingStencilPolygons.clear();
	SyncScissorState();
	SyncStencilState();
	if (m_pManualObject != nullptr)
		m_pManualObject->clear();
}

void FairyGuiSystem::EndOgreRender()
{
	if (m_pManualObject != nullptr && m_pManualNode != nullptr)
	{
		m_pManualObject->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
		m_pManualNode->_updateBounds();
	}
}

void FairyGuiSystem::SyncScissorState()
{
	cocos2d::GLView* view = cocos2d::Director::getInstance()->getOpenGLView();
	m_scissorEnabled = view != nullptr && view->isScissorEnabled();
	m_scissorRect = view != nullptr ? view->getScissorRect() : cocos2d::Rect::ZERO;
}

void FairyGuiSystem::SyncStencilState()
{
	cocos2d::Renderer* renderer = cocos2d::Director::getInstance()->getRenderer();
	if (renderer == nullptr)
		return;

	const unsigned int revision = renderer->getStencilRevision();
	if (revision == m_stencilRevision)
		return;

	const cocos2d::StencilStage previousStage = m_stencilStage;
	const int previousDepth = m_stencilDepth;
	const cocos2d::StencilStage nextStage = renderer->getStencilStage();
	const int nextDepth = renderer->getStencilDepth();
	const bool nextInverted = renderer->isStencilInverted();

	if (previousStage == cocos2d::STENCIL_STAGE_WRITE && nextStage == cocos2d::STENCIL_STAGE_TEST && m_pendingStencilDepth == nextDepth)
		FinalizeStencilScope(nextDepth);

	TrimStencilScopes(nextDepth);

	if (nextStage == cocos2d::STENCIL_STAGE_WRITE && (previousStage != cocos2d::STENCIL_STAGE_WRITE || previousDepth != nextDepth))
		BeginStencilWrite(nextDepth, nextInverted);

	m_stencilStage = nextStage;
	m_stencilDepth = nextDepth;
	m_stencilRevision = revision;
}

void FairyGuiSystem::BeginStencilWrite(int depth, bool inverted)
{
	m_pendingStencilDepth = depth;
	m_pendingStencilInverted = inverted;
	m_pendingStencilValid = false;
	m_pendingStencilRect = cocos2d::Rect::ZERO;
	m_pendingStencilPolygons.clear();
}

void FairyGuiSystem::CollectStencilTriangle(const cocos2d::TrianglesCommand& command)
{
	const cocos2d::TrianglesCommand::Triangles& triangles = command.getTriangles();
	for (int index = 0; index + 2 < triangles.indexCount; index += 3)
	{
		StencilClipInfo::Polygon polygon;
		polygon.points.reserve(3);
		bool validTriangle = true;
		for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
		{
			const int sourceIndex = triangles.indices[index + vertexIndex];
			if (sourceIndex < 0 || sourceIndex >= triangles.vertCount)
			{
				validTriangle = false;
				break;
			}

			const cocos2d::V3F_C4B_T2F& vertex = triangles.verts[sourceIndex];
			const float x = TransformX(command.getTransform(), vertex.vertices);
			const float y = TransformY(command.getTransform(), vertex.vertices);
			polygon.points.push_back(cocos2d::Vec2(x, y));
			if (!m_pendingStencilValid)
			{
				m_pendingStencilRect.setRect(x, y, 0.0f, 0.0f);
				m_pendingStencilValid = true;
				continue;
			}

			const float minX = std::min(m_pendingStencilRect.getMinX(), x);
			const float minY = std::min(m_pendingStencilRect.getMinY(), y);
			const float maxX = std::max(m_pendingStencilRect.getMaxX(), x);
			const float maxY = std::max(m_pendingStencilRect.getMaxY(), y);
			m_pendingStencilRect.setRect(minX, minY, maxX - minX, maxY - minY);
		}

		if (validTriangle && polygon.points.size() == 3)
			m_pendingStencilPolygons.push_back(polygon);
	}
}

void FairyGuiSystem::FinalizeStencilScope(int depth)
{
	TrimStencilScopes(depth - 1);

	StencilClipInfo clipInfo;
	clipInfo.rect = m_pendingStencilRect;
	clipInfo.polygons = m_pendingStencilPolygons;
	clipInfo.inverted = m_pendingStencilInverted;
	clipInfo.valid = m_pendingStencilValid && m_pendingStencilRect.size.width > 0.0f && m_pendingStencilRect.size.height > 0.0f;
	m_stencilScopes.push_back(clipInfo);
	m_pendingStencilValid = false;
	m_pendingStencilPolygons.clear();
}

void FairyGuiSystem::TrimStencilScopes(int depth)
{
	if (depth < 0)
		depth = 0;
	while (static_cast<int>(m_stencilScopes.size()) > depth)
		m_stencilScopes.pop_back();
}

void FairyGuiSystem::BuildActiveClipRects(std::vector<cocos2d::Rect>& clipRects) const
{
	if (m_stencilScopes.empty())
	{
		if (m_scissorEnabled && m_scissorRect.size.width > 0.0f && m_scissorRect.size.height > 0.0f)
			clipRects.push_back(m_scissorRect);
		return;
	}

	cocos2d::Rect baseRect;
	if (m_scissorEnabled && m_scissorRect.size.width > 0.0f && m_scissorRect.size.height > 0.0f)
		baseRect = m_scissorRect;
	else
		baseRect.setRect(0.0f, 0.0f, static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight));

	if (baseRect.size.width <= 0.0f || baseRect.size.height <= 0.0f)
		return;

	clipRects.push_back(baseRect);
	for (std::vector<StencilClipInfo>::const_iterator it = m_stencilScopes.begin(); it != m_stencilScopes.end(); ++it)
	{
		if (!it->valid)
		{
			if (it->inverted)
				continue;
			clipRects.clear();
			return;
		}

		std::vector<cocos2d::Rect> nextRects;
		for (std::vector<cocos2d::Rect>::const_iterator rectIt = clipRects.begin(); rectIt != clipRects.end(); ++rectIt)
		{
			if (it->inverted)
				SubtractRect(*rectIt, it->rect, nextRects);
			else
			{
				cocos2d::Rect intersection;
				if (IntersectRect(*rectIt, it->rect, intersection))
					nextRects.push_back(intersection);
			}
		}
		clipRects.swap(nextRects);
		if (clipRects.empty())
			return;
	}
}

bool FairyGuiSystem::CreateConfiguredPackageObject()
{
	if (!m_initialized || m_pRoot == nullptr)
		return false;

	const std::string packagePath = GetEnvironmentString("HELLO_FGUI_PACKAGE_PATH");
	const std::string objectName = GetEnvironmentString("HELLO_FGUI_OBJECT_NAME");
	if (packagePath.empty() || objectName.empty())
		return false;

	fairygui::UIPackage* package = AddPackage(packagePath);
	if (package == nullptr)
		return false;

	std::string packageName = GetEnvironmentString("HELLO_FGUI_PACKAGE_NAME");
	if (packageName.empty())
		packageName = package->getName();

	fairygui::GObject* object = fairygui::UIPackage::createObject(packageName, objectName);
	if (object == nullptr)
		return false;

	object->setPosition(
		GetEnvironmentFloat("HELLO_FGUI_OBJECT_X", 16.0f),
		GetEnvironmentFloat("HELLO_FGUI_OBJECT_Y", 16.0f));
	m_pRoot->addChild(object);
	return true;
}

void FairyGuiSystem::ResetFrameRenderStats()
{
	m_currentFrameStats = FrameRenderStats();
}

void FairyGuiSystem::RecordStencilCommand(int triangleCount)
{
	++m_currentFrameStats.stencilCommandCount;
	m_currentFrameStats.stencilTriangleCount += triangleCount > 0 ? triangleCount : 0;
}

void FairyGuiSystem::RecordCpuClipWork(int sourceTriangleCount, int outputTriangleCount, int fragmentCount, int stencilScopeCount, int stencilPolygonCount)
{
	FrameRenderStats& stats = m_currentFrameStats;
	stats.cpuClipSourceTriangleCount += sourceTriangleCount > 0 ? sourceTriangleCount : 0;
	stats.cpuClipOutputTriangleCount += outputTriangleCount > 0 ? outputTriangleCount : 0;
	stats.cpuClipFragmentCount += fragmentCount > 0 ? fragmentCount : 0;
	if (stencilScopeCount > stats.maxStencilClipScopeCount)
		stats.maxStencilClipScopeCount = stencilScopeCount;
	stats.stencilClipPolygonCount += stencilPolygonCount > 0 ? stencilPolygonCount : 0;
}

void FairyGuiSystem::RecordDrawCommand(cocos2d::Texture2D* texture, const std::string& materialName, int vertexCount, int submittedTriangleCount, int drawTriangleCount, bool clipped)
{
	FrameRenderStats& stats = m_currentFrameStats;
	submittedTriangleCount = submittedTriangleCount > 0 ? submittedTriangleCount : 0;
	drawTriangleCount = drawTriangleCount > 0 ? drawTriangleCount : 0;
	vertexCount = vertexCount > 0 ? vertexCount : 0;

	if (clipped)
	{
		++stats.clippedCommandCount;
		if (submittedTriangleCount > drawTriangleCount)
			stats.clippedTriangleCount += submittedTriangleCount - drawTriangleCount;
	}

	if (drawTriangleCount <= 0)
	{
		if (submittedTriangleCount > 0)
			++stats.culledCommandCount;
		return;
	}

	const std::string textureSource = texture != nullptr ? GetTextureSourceKey(texture) : "fallback";
	const std::string materialKey = !materialName.empty() ? materialName : "-";
	if (!stats.lastMaterialName.empty() && stats.lastMaterialName != materialKey)
		++stats.materialSwitchCount;
	if (!stats.lastTextureSource.empty() && stats.lastTextureSource != textureSource)
		++stats.textureSwitchCount;
	stats.lastMaterialName = materialKey;
	stats.lastTextureSource = textureSource;

	++stats.drawCommandCount;
	stats.drawTriangleCount += drawTriangleCount;
	if (drawTriangleCount > stats.maxBatchTriangles)
		stats.maxBatchTriangles = drawTriangleCount;
	if (vertexCount > stats.maxBatchVertices)
		stats.maxBatchVertices = vertexCount;
	stats.materialCommandCounts[materialKey] += 1;
	stats.textureCommandCounts[textureSource] += 1;
}

void FairyGuiSystem::FinalizeFrameRenderStats()
{
	m_lastFrameStats = m_currentFrameStats;
	m_lastFrameStats.detailString = BuildFrameRenderDetailString(m_lastFrameStats);
}

std::string FairyGuiSystem::BuildFrameRenderDetailString(const FrameRenderStats& stats) const
{
	std::ostringstream stream;
	stream << "draw=" << stats.drawCommandCount
		<< " tri=" << stats.drawTriangleCount
		<< " maxBatch=" << stats.maxBatchTriangles << "/" << stats.maxBatchVertices
		<< " switch=" << stats.materialSwitchCount << "/" << stats.textureSwitchCount
		<< " clip=" << stats.clippedCommandCount << "/" << stats.clippedTriangleCount
		<< " cull=" << stats.culledCommandCount
		<< " stencil=" << stats.stencilCommandCount << "/" << stats.stencilTriangleCount
		<< " custom=" << stats.customCommandCount
		<< " backend=" << GetStencilBackendString()
		<< " hwStencil=" << (IsHardwareStencilSupported() ? 1 : 0);

	if (stats.cpuClipSourceTriangleCount > 0)
		stream << " cpuClip=" << stats.cpuClipSourceTriangleCount << "/" << stats.cpuClipOutputTriangleCount << "/" << stats.cpuClipFragmentCount;
	if (stats.maxStencilClipScopeCount > 0 || stats.stencilClipPolygonCount > 0)
		stream << " stencilClip=" << stats.maxStencilClipScopeCount << "/" << stats.stencilClipPolygonCount;

	const std::string materialBrief = BuildCountBrief(stats.materialCommandCounts, 3);
	if (!materialBrief.empty())
		stream << " matTop=" << materialBrief;

	const std::string textureBrief = BuildCountBrief(stats.textureCommandCounts, 3);
	if (!textureBrief.empty())
		stream << " texTop=" << textureBrief;

	return stream.str();
}

bool FairyGuiSystem::IsHardwareStencilSupported() const
{
	Ogre::Root* root = Ogre::Root::getSingletonPtr();
	Ogre::RenderSystem* renderSystem = root != nullptr ? root->getRenderSystem() : nullptr;
	const Ogre::RenderSystemCapabilities* capabilities = renderSystem != nullptr ? renderSystem->getCapabilities() : nullptr;
	return capabilities != nullptr && capabilities->hasCapability(Ogre::RSC_HWSTENCIL);
}

std::string FairyGuiSystem::GetStencilBackendString() const
{
	return "shapeCpu";
}

std::string FairyGuiSystem::GetStencilBackendDetailString() const
{
	std::ostringstream stream;
	stream << "backend=" << GetStencilBackendString()
		<< " hwStencil=" << (IsHardwareStencilSupported() ? 1 : 0)
		<< " gpuPath=customRenderableRequired";
	return stream.str();
}

std::string FairyGuiSystem::GetMaterialDetailString() const
{
	std::ostringstream stream;
	bool first = true;
	for (std::map<std::string, std::string>::const_iterator it = m_materialNamesBySource.begin(); it != m_materialNamesBySource.end(); ++it)
	{
		if (!first)
			stream << "; ";
		stream << it->first << "=" << it->second;
		first = false;
	}
	return stream.str();
}

std::string FairyGuiSystem::GetTextureDetailString() const
{
	std::ostringstream stream;
	bool first = true;
	for (std::map<std::string, TextureDetail>::const_iterator it = m_textureDetailsBySource.begin(); it != m_textureDetailsBySource.end(); ++it)
	{
		if (!first)
			stream << "; ";
		stream << it->first << "=" << it->second.textureName << "(" << it->second.width << "x" << it->second.height << ")";
		first = false;
	}
	return stream.str();
}

const std::string& FairyGuiSystem::GetMaterialName(cocos2d::Texture2D* texture)
{
	if (texture == nullptr || texture->getImageData().empty())
	{
		if (!Ogre::MaterialManager::getSingleton().resourceExists(FAIRYGUI_FALLBACK_MATERIAL))
		{
			Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(FAIRYGUI_FALLBACK_MATERIAL, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			Ogre::Pass* pass = material->getTechnique(0)->getPass(0);
			pass->setLightingEnabled(false);
			pass->setDepthCheckEnabled(false);
			pass->setDepthWriteEnabled(false);
			pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
			pass->setCullingMode(Ogre::CULL_NONE);
			pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
			pass->setVertexColourTracking(Ogre::TVC_AMBIENT | Ogre::TVC_DIFFUSE);
		}
		return FAIRYGUI_FALLBACK_MATERIAL;
	}

	std::map<cocos2d::Texture2D*, std::string>::iterator it = m_materialNames.find(texture);
	if (it != m_materialNames.end())
		return it->second;

	const std::string sourceKey = GetTextureSourceKey(texture);
	std::map<std::string, std::string>::iterator sourceIt = m_materialNamesBySource.find(sourceKey);
	if (sourceIt != m_materialNamesBySource.end())
	{
		m_materialNames[texture] = sourceIt->second;
		return m_materialNames[texture];
	}

	const std::string textureName = CreateOgreTexture(texture);
	const std::string materialName = "Hello/FairyGUI/Material/" + std::to_string(++m_materialCounter);
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->setReceiveShadows(false);

	Ogre::Technique* technique = material->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	pass->setLightingEnabled(false);
	pass->setDepthCheckEnabled(false);
	pass->setDepthWriteEnabled(false);
	pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	pass->setCullingMode(Ogre::CULL_NONE);
	pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
	pass->setVertexColourTracking(Ogre::TVC_AMBIENT | Ogre::TVC_DIFFUSE);

	if (!textureName.empty())
	{
		Ogre::TextureUnitState* textureUnit = pass->createTextureUnitState(textureName);
		textureUnit->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
		textureUnit->setTextureFiltering(Ogre::TFO_BILINEAR);
	}

	m_materialNames[texture] = materialName;
	m_materialNamesBySource[sourceKey] = materialName;
	return m_materialNames[texture];
}

std::string FairyGuiSystem::GetTextureSourceKey(cocos2d::Texture2D* texture) const
{
	if (texture == nullptr)
		return std::string();

	if (!texture->getFilePath().empty())
		return "file:" + texture->getFilePath();

	std::ostringstream stream;
	stream << "ptr:" << texture;
	return stream.str();
}

std::string FairyGuiSystem::CreateOgreTexture(cocos2d::Texture2D* texture)
{
	std::map<cocos2d::Texture2D*, std::string>::iterator it = m_textureNames.find(texture);
	if (it != m_textureNames.end())
		return it->second;

	const std::vector<unsigned char>& data = texture->getImageData();
	if (data.empty())
		return std::string();

	const std::string sourceKey = GetTextureSourceKey(texture);
	std::map<std::string, std::string>::iterator sourceIt = m_textureNamesBySource.find(sourceKey);
	if (sourceIt != m_textureNamesBySource.end())
	{
		m_textureNames[texture] = sourceIt->second;
		return sourceIt->second;
	}

	const std::string textureName = "Hello/FairyGUI/Texture/" + std::to_string(m_textureNames.size() + 1);
	std::string extension = GetFileExtension(texture->getFilePath());
	if (extension.empty())
		extension = "png";
	try
	{
		Ogre::DataStreamPtr stream(new Ogre::MemoryDataStream((void*)&data[0], data.size(), false, true));
		Ogre::Image image;
		image.load(stream, extension);
		Ogre::TextureManager::getSingleton().loadImage(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, image);
		m_textureNames[texture] = textureName;
		m_textureNamesBySource[sourceKey] = textureName;
		TextureDetail detail;
		detail.textureName = textureName;
		detail.width = texture->getPixelsWide();
		detail.height = texture->getPixelsHigh();
		m_textureDetailsBySource[sourceKey] = detail;
		return textureName;
	}
	catch (const Ogre::Exception&)
	{
		return std::string();
	}
}

void FairyGuiSystem::DestroyOgreResources()
{
	if (m_pManualObject != nullptr && m_pSceneManager != nullptr)
	{
		if (m_pManualNode != nullptr)
			m_pManualNode->detachObject(m_pManualObject);
		m_pSceneManager->destroyManualObject(m_pManualObject);
	}

	if (m_pManualNode != nullptr && m_pSceneManager != nullptr)
		m_pSceneManager->destroySceneNode(m_pManualNode);

	for (std::map<std::string, std::string>::iterator it = m_materialNamesBySource.begin(); it != m_materialNamesBySource.end(); ++it)
	{
		if (!it->second.empty() && Ogre::MaterialManager::getSingleton().resourceExists(it->second))
			Ogre::MaterialManager::getSingleton().remove(it->second);
	}

	for (std::map<std::string, std::string>::iterator it = m_textureNamesBySource.begin(); it != m_textureNamesBySource.end(); ++it)
	{
		if (!it->second.empty() && Ogre::TextureManager::getSingleton().resourceExists(it->second))
			Ogre::TextureManager::getSingleton().remove(it->second);
	}

	if (Ogre::MaterialManager::getSingleton().resourceExists(FAIRYGUI_FALLBACK_MATERIAL))
		Ogre::MaterialManager::getSingleton().remove(FAIRYGUI_FALLBACK_MATERIAL);

	m_materialNames.clear();
	m_textureNames.clear();
	m_materialNamesBySource.clear();
	m_textureNamesBySource.clear();
	m_textureDetailsBySource.clear();
	m_materialCounter = 0;
}
