#include "ui/fairygui/FairyGuiSystem.h"

#include "Controller.h"
#include "cocos2d.h"
#include "GComponent.h"
#include "GGraph.h"
#include "GList.h"
#include "GLoader.h"
#include "GObject.h"
#include "GRoot.h"
#include "UIPackage.h"
#include "event/EventContext.h"
#include "event/InputEvent.h"
#include "event/UIEventType.h"

#include "Ogre.h"
#include "OgreRenderWindow.h"
#include "ScriptLuaVM.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

namespace
{
	const std::string FAIRYGUI_FALLBACK_MATERIAL = "Hello/FairyGUI/Fallback";

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
	HandleWindowResized(width, height);

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
	m_lastRenderCommandCount = 0;
	m_lastTriangleCount = 0;
	m_nextObjectHandle = 1;
	m_nextListenerBindingId = 1;
}

void FairyGuiSystem::Update(float deltaSeconds)
{
	if (!m_initialized)
		return;

	cocos2d::Director::getInstance()->mainLoop(deltaSeconds);
}

void FairyGuiSystem::Render()
{
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
	LogInputHit("up", x, y, mouseX, mouseY, m_pRoot->hitTest(cocos2d::Vec2(mouseX, mouseY), cocos2d::Camera::getVisitingCamera()));

	if (button != 0)
	{
		fairygui::GObject* target = m_pRoot->hitTest(cocos2d::Vec2(mouseX, mouseY), cocos2d::Camera::getVisitingCamera());
		if (consumed && target != nullptr && target != m_pRoot)
			target->bubbleEvent(button == 2 ? fairygui::UIEventType::MiddleClick : fairygui::UIEventType::RightClick);
		m_lastMouseX = mouseX;
		m_lastMouseY = mouseY;
		m_hasLastMousePosition = true;
		m_leftMouseDownOnUi = false;
		return consumed;
	}

	cocos2d::Touch touch;
	touch.setTouchInfo(0, mouseX, mouseY, previousX, previousY);
	m_pRoot->getInputProcessor()->touchUp(&touch, nullptr);

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;
	m_hasLastMousePosition = true;
	m_leftMouseDownOnUi = false;
	return consumed;
}

bool FairyGuiSystem::LoadPackage(const std::string& packagePath)
{
	if (!m_initialized || packagePath.empty())
		return false;

	return AddPackage(packagePath) != nullptr;
}

std::string FairyGuiSystem::LoadPackageAndGetName(const std::string& packagePath)
{
	if (!m_initialized || packagePath.empty())
		return std::string();

	fairygui::UIPackage* package = AddPackage(packagePath);
	return package != nullptr ? package->getName() : std::string();
}

bool FairyGuiSystem::RemovePackage(const std::string& packageName)
{
	if (!m_initialized || packageName.empty())
		return false;

	fairygui::UIPackage::removePackage(packageName);
	return true;
}

fairygui::GObject* FairyGuiSystem::CreateObject(const std::string& packageName, const std::string& objectName)
{
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
	if (component == nullptr || controllerName.empty())
		return false;

	fairygui::GController* controller = component->getController(controllerName);
	if (controller == nullptr)
		return false;

	controller->setSelectedIndex(selectedIndex);
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
	for (std::map<int, ObjectHandleInfo>::iterator it = m_objectHandles.begin(); it != m_objectHandles.end(); ++it)
	{
		if (it->second.object != nullptr && it->second.retained)
		{
			it->second.object->removeFromParent();
			it->second.object->release();
		}
	}
	m_objectHandles.clear();
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

	const std::string& materialName = GetMaterialName(command.getTexture());
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

void FairyGuiSystem::DispatchObjectHandleEvent(int callbackId, int objectHandle, int eventType, int bindingId, fairygui::EventContext* context)
{
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
		}
	}

	luaVM->callFunction(
		"FairyGuiManager_DispatchEvent",
		"iiiiiiiiiii",
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
		touchId);
}

void FairyGuiSystem::ConvertMousePosition(int x, int y, float& outX, float& outY) const
{
	float scale = 1.0f;
	if (m_pRenderWindow != nullptr)
	{
		const float pointToPixelScale = m_pRenderWindow->getViewPointToPixelScale();
		if (pointToPixelScale > 1.0f)
			scale = pointToPixelScale;
	}

	outX = static_cast<float>(x) / scale;
	const float logicalY = static_cast<float>(y) / scale;
	outY = m_screenHeight > 0 ? static_cast<float>(m_screenHeight) - logicalY : logicalY;
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
	return m_materialNames[texture];
}

std::string FairyGuiSystem::CreateOgreTexture(cocos2d::Texture2D* texture)
{
	std::map<cocos2d::Texture2D*, std::string>::iterator it = m_textureNames.find(texture);
	if (it != m_textureNames.end())
		return it->second;

	const std::vector<unsigned char>& data = texture->getImageData();
	if (data.empty())
		return std::string();

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

	for (std::map<cocos2d::Texture2D*, std::string>::iterator it = m_materialNames.begin(); it != m_materialNames.end(); ++it)
	{
		if (!it->second.empty() && Ogre::MaterialManager::getSingleton().resourceExists(it->second))
			Ogre::MaterialManager::getSingleton().remove(it->second);
	}

	for (std::map<cocos2d::Texture2D*, std::string>::iterator it = m_textureNames.begin(); it != m_textureNames.end(); ++it)
	{
		if (!it->second.empty() && Ogre::TextureManager::getSingleton().resourceExists(it->second))
			Ogre::TextureManager::getSingleton().remove(it->second);
	}

	if (Ogre::MaterialManager::getSingleton().resourceExists(FAIRYGUI_FALLBACK_MATERIAL))
		Ogre::MaterialManager::getSingleton().remove(FAIRYGUI_FALLBACK_MATERIAL);

	m_materialNames.clear();
	m_textureNames.clear();
	m_materialCounter = 0;
}
