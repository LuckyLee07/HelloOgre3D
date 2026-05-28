#ifndef __HELLO_FAIRY_GUI_SYSTEM_IMPL_H__
#define __HELLO_FAIRY_GUI_SYSTEM_IMPL_H__

#include "ui/fairygui/FairyGuiSystemFairyIncludes.h"

class FairyGuiSystemImpl : public cocos2d::RenderCommandSink
{
public:
	FairyGuiSystemImpl();
	~FairyGuiSystemImpl();

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
	int CreateGraphRectHandle(int ownerHandle, const std::string& name, float width, float height, float red, float green, float blue, float alpha);
	int CreateGraphRegularPolygonHandle(int ownerHandle, const std::string& name, float width, float height, int sides, float red, float green, float blue, float alpha);
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
	int GetObjectHandleControllerIndex(int objectHandle, const std::string& controllerName) const;
	bool SetObjectHandleControllerPage(int objectHandle, const std::string& controllerName, const std::string& pageName);
	std::string GetObjectHandleControllerPage(int objectHandle, const std::string& controllerName) const;
	std::string GetObjectHandleControllerPageId(int objectHandle, const std::string& controllerName) const;
	int GetObjectHandleControllerPageCount(int objectHandle, const std::string& controllerName) const;
	std::string GetObjectHandleControllerPageNameAt(int objectHandle, const std::string& controllerName, int pageIndex) const;
	std::string GetObjectHandleControllerPageIdAt(int objectHandle, const std::string& controllerName, int pageIndex) const;
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
	bool SetObjectHandleListVirtual(int objectHandle, bool loop);
	bool RefreshObjectHandleList(int objectHandle);
	bool CenterObjectHandle(int objectHandle, bool restraint);
	int AddObjectHandleEventListener(int objectHandle, const std::string& childPath, int eventType, int callbackId);
	int AddObjectHandleClickListener(int objectHandle, const std::string& childPath, int callbackId);
	int AddObjectHandleControllerChangedListener(int objectHandle, const std::string& controllerName, int callbackId);
	bool RemoveObjectHandleListener(int bindingId);
	bool RemoveObjectHandle(int objectHandle);
	void ClearObjectHandles();
	bool CreateSmokeTestImage(const std::string& imagePath);
	bool InjectImeCompositionText(const std::string& text);
	bool InjectImeCommitText(const std::string& text);
	bool ClearImeCompositionText();
	bool HandleNativeImeMessage(unsigned int message, unsigned long long wParam, long long lParam, long long& result);

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
	int GetLastDrawCommandCount() const { return m_lastFrameStats.drawCommandCount; }
	int GetLastDrawTriangleCount() const { return m_lastFrameStats.drawTriangleCount; }
	int GetLastMaterialSwitchCount() const { return m_lastFrameStats.materialSwitchCount; }
	int GetLastTextureSwitchCount() const { return m_lastFrameStats.textureSwitchCount; }
	int GetLastClippedCommandCount() const { return m_lastFrameStats.clippedCommandCount; }
	int GetLastClippedTriangleCount() const { return m_lastFrameStats.clippedTriangleCount; }
	int GetLastCulledCommandCount() const { return m_lastFrameStats.culledCommandCount; }
	int GetLastStencilCommandCount() const { return m_lastFrameStats.stencilCommandCount; }
	int GetLastStencilTriangleCount() const { return m_lastFrameStats.stencilTriangleCount; }
	int GetLastCpuClipSourceTriangleCount() const { return m_lastFrameStats.cpuClipSourceTriangleCount; }
	int GetLastCpuClipOutputTriangleCount() const { return m_lastFrameStats.cpuClipOutputTriangleCount; }
	int GetLastCpuClipFragmentCount() const { return m_lastFrameStats.cpuClipFragmentCount; }
	int GetLastStencilClipScopeCount() const { return m_lastFrameStats.maxStencilClipScopeCount; }
	int GetLastStencilClipPolygonCount() const { return m_lastFrameStats.stencilClipPolygonCount; }
	int GetLastCustomCommandCount() const { return m_lastFrameStats.customCommandCount; }
	int GetLastMaxBatchTriangles() const { return m_lastFrameStats.maxBatchTriangles; }
	int GetLastMaxBatchVertices() const { return m_lastFrameStats.maxBatchVertices; }
	bool IsHardwareStencilSupported() const;
	std::string GetStencilBackendString() const;
	std::string GetStencilBackendDetailString() const;
	std::string GetMaterialDetailString() const;
	std::string GetTextureDetailString() const;
	std::string GetFrameRenderDetailString() const { return m_lastFrameStats.detailString; }
	std::string GetImeDebugString() const;
	bool IsImeCompositionActive() const { return m_imeStats.compositionActive; }
	bool IsImeCandidateOpen() const { return m_imeStats.candidateOpen; }
	int GetImeCompositionUpdateCount() const { return m_imeStats.compositionUpdateCount; }
	int GetImeCompositionCommitCount() const { return m_imeStats.compositionCommitCount; }
	int GetImeCompositionEndCount() const { return m_imeStats.compositionEndCount; }
	int GetImeCandidateOpenCount() const { return m_imeStats.candidateOpenCount; }
	int GetImeCandidateCloseCount() const { return m_imeStats.candidateCloseCount; }
	int GetImeCandidateCount() const { return m_imeStats.candidateCount; }
	int GetImeCandidateSelection() const { return m_imeStats.candidateSelection; }

private:
	struct ListenerBinding
	{
		int rootHandle;
		int callbackId;
		int eventType;
		fairygui::UIEventDispatcher* target;
		fairygui::GObject* targetObject;
		std::string controllerName;
	};

	struct TextureDetail
	{
		std::string textureName;
		int width;
		int height;
	};

	struct FrameRenderStats
	{
		FrameRenderStats()
			: drawCommandCount(0), drawTriangleCount(0), materialSwitchCount(0), textureSwitchCount(0),
			clippedCommandCount(0), clippedTriangleCount(0), culledCommandCount(0),
			stencilCommandCount(0), stencilTriangleCount(0),
			cpuClipSourceTriangleCount(0), cpuClipOutputTriangleCount(0), cpuClipFragmentCount(0),
			maxStencilClipScopeCount(0), stencilClipPolygonCount(0),
			customCommandCount(0),
			maxBatchTriangles(0), maxBatchVertices(0),
			materialCommandCounts(), textureCommandCounts(), lastMaterialName(), lastTextureSource(), detailString()
		{
		}

		int drawCommandCount;
		int drawTriangleCount;
		int materialSwitchCount;
		int textureSwitchCount;
		int clippedCommandCount;
		int clippedTriangleCount;
		int culledCommandCount;
		int stencilCommandCount;
		int stencilTriangleCount;
		int cpuClipSourceTriangleCount;
		int cpuClipOutputTriangleCount;
		int cpuClipFragmentCount;
		int maxStencilClipScopeCount;
		int stencilClipPolygonCount;
		int customCommandCount;
		int maxBatchTriangles;
		int maxBatchVertices;
		std::map<std::string, int> materialCommandCounts;
		std::map<std::string, int> textureCommandCounts;
		std::string lastMaterialName;
		std::string lastTextureSource;
		std::string detailString;
	};

	struct ObjectHandleInfo
	{
		fairygui::GObject* object;
		int ownerHandle;
		bool retained;
	};

	struct ImeStats
	{
		ImeStats()
			: compositionActive(false), candidateOpen(false), compositionUpdateCount(0),
			compositionCommitCount(0), compositionEndCount(0), candidateOpenCount(0),
			candidateCloseCount(0), candidateChangeCount(0), candidateCount(0),
			candidateSelection(-1), focusedHandle(0), compositionX(0), compositionY(0),
			compositionText(), commitText()
		{
		}

		bool compositionActive;
		bool candidateOpen;
		int compositionUpdateCount;
		int compositionCommitCount;
		int compositionEndCount;
		int candidateOpenCount;
		int candidateCloseCount;
		int candidateChangeCount;
		int candidateCount;
		int candidateSelection;
		int focusedHandle;
		int compositionX;
		int compositionY;
		std::string compositionText;
		std::string commitText;
	};

	struct StencilClipInfo
	{
		struct Polygon
		{
			std::vector<cocos2d::Vec2> points;
		};

		cocos2d::Rect rect;
		std::vector<Polygon> polygons;
		bool inverted;
		bool valid;
	};

	virtual void handleTrianglesCommand(const cocos2d::TrianglesCommand& command) override;
	virtual void handleCustomCommand(const cocos2d::CustomCommand& command) override;
	fairygui::GObject* FindObjectHandle(int objectHandle) const;
	fairygui::GObject* FindEventTarget(int objectHandle, const std::string& childPath) const;
	fairygui::GController* FindController(int objectHandle, const std::string& controllerName) const;
	fairygui::GTextInput* FindTextInput(int objectHandle) const;
	fairygui::GTextInput* FindTextInputTarget(fairygui::GObject* target) const;
	int FindOwnerHandleForObject(fairygui::GObject* object) const;
	int GetOrCreateObjectAlias(int ownerHandle, fairygui::GObject* object);
	int GetObjectHandleOwner(int objectHandle) const;
	void RemoveObjectHandleAliases(int objectHandle);
	void RemoveObjectHandleListeners(int objectHandle);
	bool FocusTextInput(fairygui::GTextInput* input);
	bool ApplyTextInputKey(fairygui::GTextInput* input, int keyCode, int keyText);
	bool ApplyTextInputUtf8Text(fairygui::GTextInput* input, const std::string& committedText);
	void InstallNativeImeHook();
	void RemoveNativeImeHook();
	void UpdateNativeImeCandidatePosition();
	void CancelNativeImeComposition();
	void EndImeComposition(bool countEnd);
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
	void ResetFrameRenderStats();
	void RecordStencilCommand(int triangleCount);
	void RecordCpuClipWork(int sourceTriangleCount, int outputTriangleCount, int fragmentCount, int stencilScopeCount, int stencilPolygonCount);
	void RecordDrawCommand(cocos2d::Texture2D* texture, const std::string& materialName, int vertexCount, int submittedTriangleCount, int drawTriangleCount, bool clipped);
	void FinalizeFrameRenderStats();
	std::string BuildFrameRenderDetailString(const FrameRenderStats& stats) const;
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
	std::vector<StencilClipInfo::Polygon> m_pendingStencilPolygons;
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
	std::map<std::string, TextureDetail> m_textureDetailsBySource;
	FrameRenderStats m_currentFrameStats;
	FrameRenderStats m_lastFrameStats;
	ImeStats m_imeStats;
#if defined(_WIN32)
	HWND m_nativeWindowHandle;
	void* m_previousWindowProc;
	bool m_nativeImeHookInstalled;
#endif
};

#endif // __HELLO_FAIRY_GUI_SYSTEM_IMPL_H__