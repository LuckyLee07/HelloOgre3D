#ifndef __HELLO_FAIRY_GUI_SYSTEM_INTERNAL_H__
#define __HELLO_FAIRY_GUI_SYSTEM_INTERNAL_H__

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
	const std::string FAIRYGUI_FALLBACK_TEXTURE = "textures/base/baseWhite.dds";
	const std::string FAIRYGUI_VERTEX_PROGRAM = "gorilla2DVP";
	const std::string FAIRYGUI_FRAGMENT_PROGRAM = "gorilla2DFP";
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

	bool HasFairyGuiGpuProgram(const std::string& programName)
	{
		Ogre::HighLevelGpuProgramManager* manager = Ogre::HighLevelGpuProgramManager::getSingletonPtr();
		return manager != nullptr && manager->resourceExists(programName);
	}

	void ConfigureFairyGuiPass(Ogre::Pass* pass, const std::string& textureName)
	{
		if (pass == nullptr)
			return;

		pass->setLightingEnabled(false);
		pass->setDepthCheckEnabled(false);
		pass->setDepthWriteEnabled(false);
		pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		pass->setCullingMode(Ogre::CULL_NONE);
		pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
		pass->setVertexColourTracking(Ogre::TVC_AMBIENT | Ogre::TVC_DIFFUSE);
		if (HasFairyGuiGpuProgram(FAIRYGUI_VERTEX_PROGRAM))
			pass->setVertexProgram(FAIRYGUI_VERTEX_PROGRAM);
		if (HasFairyGuiGpuProgram(FAIRYGUI_FRAGMENT_PROGRAM))
			pass->setFragmentProgram(FAIRYGUI_FRAGMENT_PROGRAM);

		pass->removeAllTextureUnitStates();
		Ogre::TextureUnitState* textureUnit = pass->createTextureUnitState(textureName.empty() ? FAIRYGUI_FALLBACK_TEXTURE : textureName);
		textureUnit->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
		textureUnit->setTextureFiltering(Ogre::TFO_BILINEAR);
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

#endif // __HELLO_FAIRY_GUI_SYSTEM_INTERNAL_H__
