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
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

namespace
{
	const std::string FAIRYGUI_FALLBACK_MATERIAL = "Hello/FairyGUI/Fallback";
	const int OIS_KC_ESCAPE = 0x01;
	const int OIS_KC_TAB = 0x0F;
	const int OIS_KC_BACK = 0x0E;
	const int OIS_KC_RETURN = 0x1C;
	const int OIS_KC_NUMPADENTER = 0x9C;
	const int OIS_KC_DELETE = 0xD3;

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
		return keyText >= 32 && keyText != 127;
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

	void RemoveLastUtf8Codepoint(std::string& text)
	{
		if (text.empty())
			return;

		size_t erasePos = text.size() - 1;
		while (erasePos > 0 && (static_cast<unsigned char>(text[erasePos]) & 0xC0) == 0x80)
			--erasePos;
		text.erase(erasePos);
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
}

FairyGuiSystem::FairyGuiSystem()
	: m_pRenderWindow(nullptr), m_pSceneManager(nullptr), m_pManualNode(nullptr), m_pManualObject(nullptr),
	m_pScene(nullptr), m_pRoot(nullptr), m_initialized(false), m_screenWidth(0), m_screenHeight(0),
	m_lastMouseX(0.0f), m_lastMouseY(0.0f), m_hasLastMousePosition(false), m_leftMouseDownOnUi(false),
	m_focusedObjectHandle(0),
	m_scissorEnabled(false), m_scissorRect(cocos2d::Rect::ZERO),
	m_stencilStage(cocos2d::STENCIL_STAGE_DISABLED), m_stencilDepth(0), m_stencilRevision(0),
	m_pendingStencilDepth(0), m_pendingStencilInverted(false), m_pendingStencilValid(false), m_pendingStencilRect(cocos2d::Rect::ZERO), m_stencilScopes(),
	m_lastRenderCommandCount(0), m_lastTriangleCount(0), m_materialCounter(0), m_nextObjectHandle(1), m_nextListenerBindingId(1),
	m_objectHandles(), m_listenerBindings(), m_materialNames(), m_textureNames()
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
		if (!CreateConfiguredPackageObject() && IsEnvironmentEnabled("HELLO_FGUI_SMOKE_TEST"))
			CreateSmokeTestImage("media/fonts/dejavu/dejavu.png");
	}
	return m_initialized;
}

void FairyGuiSystem::Shutdown()
{
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
	m_nextObjectHandle = 1;
	m_nextListenerBindingId = 1;
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
	renderer->beginFrame();
	renderer->setCommandSink(this);
	BeginOgreRender();
	if (m_pScene != nullptr)
		m_pScene->visit(renderer, cocos2d::Mat4::IDENTITY, 0);
	EndOgreRender();
	renderer->setCommandSink(nullptr);
	m_lastRenderCommandCount = renderer->getTriangleCommandCount();
	m_lastTriangleCount = renderer->getSubmittedTriangleCount();
	H3D_PROFILE_PLOT("FGUIRenderCommands", static_cast<double>(m_lastRenderCommandCount));
	H3D_PROFILE_PLOT("FGUITriangles", static_cast<double>(m_lastTriangleCount));
}

void FairyGuiSystem::HandleWindowResized(unsigned int width, unsigned int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
	cocos2d::Director::getInstance()->getOpenGLView()->setFrameSize((float)width, (float)height);
	if (m_pRoot != nullptr)
		m_pRoot->setSize((float)width, (float)height);
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
	fairygui::GObject* object = FindObjectHandle(objectHandle);
	fairygui::GComponent* component = dynamic_cast<fairygui::GComponent*>(object);
	if (component == nullptr)
		return false;

	fairygui::GController* controller = nullptr;
	if (!controllerName.empty())
		controller = component->getController(controllerName);
	else if (!component->getControllers().empty())
		controller = component->getControllerAt(0);
	if (controller == nullptr)
		return false;

	controller->setSelectedIndex(selectedIndex);
	return true;
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
	m_focusedObjectHandle = 0;
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
		m_focusedObjectHandle = 0;
	RemoveObjectHandleAliases(objectHandle);
	RemoveObjectHandleListeners(objectHandle);
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
		CollectStencilTriangle(command);
		return;
	}

	const std::string& materialName = GetMaterialName(command.getTexture());
	std::vector<cocos2d::Rect> clipRects;
	BuildActiveClipRects(clipRects);
	if (!clipRects.empty())
	{
		std::vector<ClipVertex> outputVertices;
		std::vector<ClipVertex> clippedPolygon;
		std::vector<ClipVertex> clipScratch;
		outputVertices.reserve(static_cast<size_t>(triangles.indexCount) * clipRects.size());
		clippedPolygon.reserve(8);
		clipScratch.reserve(8);
		for (int index = 0; index + 2 < triangles.indexCount; index += 3)
		{
			ClipVertex triangle[3];
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

			if (!validTriangle)
				continue;

			for (std::vector<cocos2d::Rect>::const_iterator clipIt = clipRects.begin(); clipIt != clipRects.end(); ++clipIt)
			{
				ClipTriangleToRect(triangle, *clipIt, clippedPolygon, clipScratch);
				if (clippedPolygon.size() < 3)
					continue;

				for (size_t polygonIndex = 1; polygonIndex + 1 < clippedPolygon.size(); ++polygonIndex)
				{
					outputVertices.push_back(clippedPolygon[0]);
					outputVertices.push_back(clippedPolygon[polygonIndex]);
					outputVertices.push_back(clippedPolygon[polygonIndex + 1]);
				}
			}
		}

		if (outputVertices.empty())
			return;

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
	return true;
}

bool FairyGuiSystem::ApplyTextInputKey(fairygui::GTextInput* input, int keyCode, int keyText)
{
	if (input == nullptr)
		return false;

	std::string text = input->getText();
	bool changed = false;
	if (keyCode == OIS_KC_BACK || keyCode == OIS_KC_DELETE)
	{
		if (!text.empty())
		{
			RemoveLastUtf8Codepoint(text);
			changed = true;
		}
	}
	else if (keyCode == OIS_KC_RETURN || keyCode == OIS_KC_NUMPADENTER)
	{
		input->dispatchEvent(fairygui::UIEventType::Submit);
		return true;
	}
	else if (IsPrintableKeyText(keyText))
	{
		AppendUtf8Codepoint(text, static_cast<unsigned int>(keyText));
		changed = true;
	}

	if (changed)
	{
		input->setText(text);
		input->dispatchEvent(fairygui::UIEventType::Changed);
	}
	return true;
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
				fairygui::GList* list = bindingIt != m_listenerBindings.end() ? dynamic_cast<fairygui::GList*>(bindingIt->second.target) : nullptr;
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
}

void FairyGuiSystem::CollectStencilTriangle(const cocos2d::TrianglesCommand& command)
{
	const cocos2d::TrianglesCommand::Triangles& triangles = command.getTriangles();
	for (int index = 0; index < triangles.indexCount; ++index)
	{
		const int sourceIndex = triangles.indices[index];
		if (sourceIndex < 0 || sourceIndex >= triangles.vertCount)
			continue;

		const cocos2d::V3F_C4B_T2F& vertex = triangles.verts[sourceIndex];
		const float x = TransformX(command.getTransform(), vertex.vertices);
		const float y = TransformY(command.getTransform(), vertex.vertices);
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
}

void FairyGuiSystem::FinalizeStencilScope(int depth)
{
	TrimStencilScopes(depth - 1);

	StencilClipInfo clipInfo;
	clipInfo.rect = m_pendingStencilRect;
	clipInfo.inverted = m_pendingStencilInverted;
	clipInfo.valid = m_pendingStencilValid && m_pendingStencilRect.size.width > 0.0f && m_pendingStencilRect.size.height > 0.0f;
	m_stencilScopes.push_back(clipInfo);
	m_pendingStencilValid = false;
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
	m_materialCounter = 0;
}
