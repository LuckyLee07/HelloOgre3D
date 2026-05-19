#ifndef __HELLO_FAIRY_GUI_SYSTEM_H__
#define __HELLO_FAIRY_GUI_SYSTEM_H__

#include "cocos2d.h"

#include <map>
#include <string>

namespace Ogre
{
	class RenderWindow;
	class SceneManager;
	class SceneNode;
	class ManualObject;
}

namespace cocos2d
{
	class Scene;
}

namespace fairygui
{
	class GObject;
	class GRoot;
}

class FairyGuiSystem : public cocos2d::RenderCommandSink
{
public:
	FairyGuiSystem();
	~FairyGuiSystem();

	bool Initialize(Ogre::RenderWindow* renderWindow, Ogre::SceneManager* sceneManager);
	void Shutdown();

	void Update(float deltaSeconds);
	void Render();
	void HandleWindowResized(unsigned int width, unsigned int height);

	bool LoadPackage(const std::string& packagePath);
	std::string LoadPackageAndGetName(const std::string& packagePath);
	fairygui::GObject* CreateObject(const std::string& packageName, const std::string& objectName);
	bool AddToRoot(fairygui::GObject* object);
	int CreateObjectHandle(const std::string& packageName, const std::string& objectName);
	bool AddObjectHandleToRoot(int objectHandle);
	bool SetObjectHandlePosition(int objectHandle, float x, float y);
	bool SetObjectHandleSize(int objectHandle, float width, float height);
	bool SetObjectHandleVisible(int objectHandle, bool visible);
	bool CenterObjectHandle(int objectHandle, bool restraint);
	int AddObjectHandleClickListener(int objectHandle, const std::string& childPath, int callbackId);
	bool RemoveObjectHandleListener(int bindingId);
	bool RemoveObjectHandle(int objectHandle);
	void ClearObjectHandles();
	bool CreateSmokeTestImage(const std::string& imagePath);

	bool IsInitialized() const { return m_initialized; }
	fairygui::GRoot* GetRoot() const { return m_pRoot; }
	int GetLastRenderCommandCount() const { return m_lastRenderCommandCount; }
	int GetLastTriangleCount() const { return m_lastTriangleCount; }

private:
	struct ListenerBinding
	{
		int rootHandle;
		int callbackId;
		int eventType;
		fairygui::GObject* target;
	};

	virtual void handleTrianglesCommand(const cocos2d::TrianglesCommand& command) override;
	virtual void handleCustomCommand(const cocos2d::CustomCommand& command) override;
	fairygui::GObject* FindObjectHandle(int objectHandle) const;
	fairygui::GObject* FindEventTarget(int objectHandle, const std::string& childPath) const;
	void RemoveObjectHandleListeners(int objectHandle);
	void DispatchObjectHandleEvent(int callbackId, int objectHandle, int eventType, int bindingId);
	void BeginOgreRender();
	void EndOgreRender();
	bool CreateConfiguredPackageObject();
	const std::string& GetMaterialName(cocos2d::Texture2D* texture);
	std::string CreateOgreTexture(cocos2d::Texture2D* texture);
	void DestroyOgreResources();

	Ogre::RenderWindow* m_pRenderWindow;
	Ogre::SceneManager* m_pSceneManager;
	Ogre::SceneNode* m_pManualNode;
	Ogre::ManualObject* m_pManualObject;
	cocos2d::Scene* m_pScene;
	fairygui::GRoot* m_pRoot;
	bool m_initialized;
	unsigned int m_screenWidth;
	unsigned int m_screenHeight;
	int m_lastRenderCommandCount;
	int m_lastTriangleCount;
	unsigned int m_materialCounter;
	int m_nextObjectHandle;
	int m_nextListenerBindingId;
	std::map<int, fairygui::GObject*> m_objectHandles;
	std::map<int, ListenerBinding> m_listenerBindings;
	std::map<cocos2d::Texture2D*, std::string> m_materialNames;
	std::map<cocos2d::Texture2D*, std::string> m_textureNames;
};

#endif // __HELLO_FAIRY_GUI_SYSTEM_H__
