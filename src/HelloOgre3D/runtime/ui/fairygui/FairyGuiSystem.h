#ifndef __HELLO_FAIRY_GUI_SYSTEM_H__
#define __HELLO_FAIRY_GUI_SYSTEM_H__

#include "cocos2d.h"

#include <cstddef>
#include <map>
#include <string>
#include <vector>

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
	class EventContext;
	class GObject;
	class GRoot;
	class GTextInput;
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
	bool InjectMouseMove(int x, int y);
	bool InjectMouseDown(int x, int y, int button);
	bool InjectMouseUp(int x, int y, int button);
	bool InjectMouseWheel(int x, int y, int wheelDelta);
	bool InjectLogicalMouseMove(int x, int y);
	bool InjectLogicalMouseDown(int x, int y, int button);
	bool InjectLogicalMouseUp(int x, int y, int button);
	bool InjectLogicalMouseWheel(int x, int y, int wheelDelta);
	bool InjectKeyPressed(int keyCode, int keyText);
	bool InjectKeyReleased(int keyCode, int keyText);

	bool LoadPackage(const std::string& packagePath);
	std::string LoadPackageAndGetName(const std::string& packagePath);
	bool RemovePackage(const std::string& packageName);
	fairygui::GObject* CreateObject(const std::string& packageName, const std::string& objectName);
	bool AddToRoot(fairygui::GObject* object);
	int CreateObjectHandle(const std::string& packageName, const std::string& objectName);
	int CreateContainerHandle(const std::string& name);
	int CreateChildContainerHandle(int ownerHandle, const std::string& name);
	int CreateLoaderHandle(int ownerHandle, const std::string& name, const std::string& url);
	int CreateTextHandle(int ownerHandle, const std::string& name, const std::string& text, float fontSize, float red, float green, float blue);
	int CreateTextInputHandle(int ownerHandle, const std::string& name, const std::string& text, float fontSize, float red, float green, float blue);
	int CreateModalMaskHandle(float red, float green, float blue, float alpha);
	int GetObjectHandleChild(int objectHandle, const std::string& childPath);
	int GetObjectHandleListItem(int objectHandle, int itemIndex);
	int GetObjectHandleListItemCount(int objectHandle);
	bool AddObjectHandleToRoot(int objectHandle);
	bool AddObjectHandleToParent(int objectHandle, int parentHandle);
	bool SetObjectHandlePosition(int objectHandle, float x, float y);
	bool SetObjectHandleSize(int objectHandle, float width, float height);
	bool SetObjectHandleVisible(int objectHandle, bool visible);
	bool SetObjectHandleAlpha(int objectHandle, float alpha);
	bool SetObjectHandleTouchable(int objectHandle, bool touchable);
	bool SetObjectHandleMask(int objectHandle, int maskHandle, bool inverted);
	bool SetObjectHandleSortingOrder(int objectHandle, int sortingOrder);
	bool SetObjectHandleText(int objectHandle, const std::string& text);
	std::string GetObjectHandleText(int objectHandle) const;
	bool SetObjectHandleIcon(int objectHandle, const std::string& icon);
	bool SetObjectHandleLoaderUrl(int objectHandle, const std::string& url);
	bool SetObjectHandleControllerIndex(int objectHandle, const std::string& controllerName, int selectedIndex);
	bool SetObjectHandleValue(int objectHandle, float value);
	float GetObjectHandleValue(int objectHandle) const;
	bool SetObjectHandleMin(int objectHandle, float minValue);
	float GetObjectHandleMin(int objectHandle) const;
	bool SetObjectHandleMax(int objectHandle, float maxValue);
	float GetObjectHandleMax(int objectHandle) const;
	bool SetObjectHandleComboBoxSelectedIndex(int objectHandle, int selectedIndex);
	int GetObjectHandleComboBoxSelectedIndex(int objectHandle) const;
	bool SetObjectHandleComboBoxValue(int objectHandle, const std::string& value);
	std::string GetObjectHandleComboBoxValue(int objectHandle) const;
	bool PlayObjectHandleTransition(int objectHandle, const std::string& transitionName, int times, float delay, int callbackId);
	bool StopObjectHandleTransition(int objectHandle, const std::string& transitionName, bool setToComplete, bool processCallback);
	bool SetObjectHandleFocus(int objectHandle);
	bool ClearObjectHandleFocus();
	int GetFocusedObjectHandle() const { return m_focusedObjectHandle; }
	bool SetObjectHandleListItemCount(int objectHandle, int itemCount);
	bool SetObjectHandleListSelectedIndex(int objectHandle, int selectedIndex);
	int GetObjectHandleListSelectedIndex(int objectHandle);
	bool ScrollObjectHandleListToView(int objectHandle, int itemIndex);
	bool CenterObjectHandle(int objectHandle, bool restraint);
	int AddObjectHandleEventListener(int objectHandle, const std::string& childPath, int eventType, int callbackId);
	int AddObjectHandleClickListener(int objectHandle, const std::string& childPath, int callbackId);
	bool RemoveObjectHandleListener(int bindingId);
	bool RemoveObjectHandle(int objectHandle);
	void ClearObjectHandles();
	bool CreateSmokeTestImage(const std::string& imagePath);

	bool IsInitialized() const { return m_initialized; }
	fairygui::GRoot* GetRoot() const { return m_pRoot; }
	int GetLastRenderCommandCount() const { return m_lastRenderCommandCount; }
	int GetLastTriangleCount() const { return m_lastTriangleCount; }
	int GetScreenWidth() const { return static_cast<int>(m_screenWidth); }
	int GetScreenHeight() const { return static_cast<int>(m_screenHeight); }
	int GetObjectHandleCount() const { return static_cast<int>(m_objectHandles.size()); }
	int GetListenerBindingCount() const { return static_cast<int>(m_listenerBindings.size()); }
	int GetMaterialCount() const { return static_cast<int>(m_materialNamesBySource.size()); }
	int GetTextureCount() const { return static_cast<int>(m_textureNamesBySource.size()); }
	int GetMaterialAliasCount() const { return static_cast<int>(m_materialNames.size()); }
	int GetTextureAliasCount() const { return static_cast<int>(m_textureNames.size()); }

private:
	struct ListenerBinding
	{
		int rootHandle;
		int callbackId;
		int eventType;
		fairygui::GObject* target;
	};

	struct ObjectHandleInfo
	{
		fairygui::GObject* object;
		int ownerHandle;
		bool retained;
	};

	struct StencilClipInfo
	{
		cocos2d::Rect rect;
		bool inverted;
		bool valid;
	};

	virtual void handleTrianglesCommand(const cocos2d::TrianglesCommand& command) override;
	virtual void handleCustomCommand(const cocos2d::CustomCommand& command) override;
	fairygui::GObject* FindObjectHandle(int objectHandle) const;
	fairygui::GObject* FindEventTarget(int objectHandle, const std::string& childPath) const;
	fairygui::GTextInput* FindTextInput(int objectHandle) const;
	fairygui::GTextInput* FindTextInputTarget(fairygui::GObject* target) const;
	int FindOwnerHandleForObject(fairygui::GObject* object) const;
	int GetOrCreateObjectAlias(int ownerHandle, fairygui::GObject* object);
	int GetObjectHandleOwner(int objectHandle) const;
	void RemoveObjectHandleAliases(int objectHandle);
	void RemoveObjectHandleListeners(int objectHandle);
	bool FocusTextInput(fairygui::GTextInput* input);
	bool ApplyTextInputKey(fairygui::GTextInput* input, int keyCode, int keyText);
	void DispatchObjectHandleEvent(int callbackId, int objectHandle, int eventType, int bindingId, fairygui::EventContext* context);
	void ConvertMousePosition(int x, int y, float& outX, float& outY) const;
	float GetInputScaleX() const;
	float GetInputScaleY() const;
	int ToRawInputX(int x) const;
	int ToRawInputY(int y) const;
	bool IsMouseOnUi(float x, float y) const;
	void BeginOgreRender();
	void EndOgreRender();
	void SyncScissorState();
	void SyncStencilState();
	void BeginStencilWrite(int depth, bool inverted);
	void CollectStencilTriangle(const cocos2d::TrianglesCommand& command);
	void FinalizeStencilScope(int depth);
	void TrimStencilScopes(int depth);
	void BuildActiveClipRects(std::vector<cocos2d::Rect>& clipRects) const;
	bool CreateConfiguredPackageObject();
	std::string GetTextureSourceKey(cocos2d::Texture2D* texture) const;
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
	float m_lastMouseX;
	float m_lastMouseY;
	bool m_hasLastMousePosition;
	bool m_leftMouseDownOnUi;
	int m_focusedObjectHandle;
	bool m_scissorEnabled;
	cocos2d::Rect m_scissorRect;
	cocos2d::StencilStage m_stencilStage;
	int m_stencilDepth;
	unsigned int m_stencilRevision;
	int m_pendingStencilDepth;
	bool m_pendingStencilInverted;
	bool m_pendingStencilValid;
	cocos2d::Rect m_pendingStencilRect;
	std::vector<StencilClipInfo> m_stencilScopes;
	int m_lastRenderCommandCount;
	int m_lastTriangleCount;
	unsigned int m_materialCounter;
	int m_nextObjectHandle;
	int m_nextListenerBindingId;
	std::map<int, ObjectHandleInfo> m_objectHandles;
	std::map<int, ListenerBinding> m_listenerBindings;
	std::map<int, std::size_t> m_textInputCarets;
	std::map<cocos2d::Texture2D*, std::string> m_materialNames;
	std::map<cocos2d::Texture2D*, std::string> m_textureNames;
	std::map<std::string, std::string> m_materialNamesBySource;
	std::map<std::string, std::string> m_textureNamesBySource;
};

#endif // __HELLO_FAIRY_GUI_SYSTEM_H__
