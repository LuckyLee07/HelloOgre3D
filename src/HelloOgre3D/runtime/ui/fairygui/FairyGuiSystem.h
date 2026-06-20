#ifndef __HELLO_FAIRY_GUI_SYSTEM_H__
#define __HELLO_FAIRY_GUI_SYSTEM_H__

#include <string>

class FairyGuiSystemImpl;

struct FairyGuiSystemStartupContext
{
	FairyGuiSystemStartupContext()
		: renderWindow(nullptr)
		, sceneManager(nullptr)
	{
	}

	void* renderWindow;
	void* sceneManager;
};

class FairyGuiSystem
{
public:
	FairyGuiSystem();
	~FairyGuiSystem();

	bool Initialize(const FairyGuiSystemStartupContext& context);
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
	bool InjectImeCompositionText(const std::string& text);
	bool InjectImeCommitText(const std::string& text);
	bool ClearImeCompositionText();
	bool HandleNativeImeMessage(unsigned int message, unsigned long long wParam, long long lParam, long long& result);

	bool LoadPackage(const std::string& packagePath);
	std::string LoadPackageAndGetName(const std::string& packagePath);
	bool RemovePackage(const std::string& packageName);
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
	int GetFocusedObjectHandle() const;
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

	bool IsInitialized() const;
	int GetLastRenderCommandCount() const;
	int GetLastTriangleCount() const;
	int GetScreenWidth() const;
	int GetScreenHeight() const;
	int GetObjectHandleCount() const;
	int GetListenerBindingCount() const;
	int GetMaterialCount() const;
	int GetTextureCount() const;
	int GetMaterialAliasCount() const;
	int GetTextureAliasCount() const;
	int GetLastDrawCommandCount() const;
	int GetLastDrawTriangleCount() const;
	int GetLastMaterialSwitchCount() const;
	int GetLastTextureSwitchCount() const;
	int GetLastClippedCommandCount() const;
	int GetLastClippedTriangleCount() const;
	int GetLastCulledCommandCount() const;
	int GetLastStencilCommandCount() const;
	int GetLastStencilTriangleCount() const;
	int GetLastCpuClipSourceTriangleCount() const;
	int GetLastCpuClipOutputTriangleCount() const;
	int GetLastCpuClipFragmentCount() const;
	int GetLastStencilClipScopeCount() const;
	int GetLastStencilClipPolygonCount() const;
	int GetLastCustomCommandCount() const;
	int GetLastMaxBatchTriangles() const;
	int GetLastMaxBatchVertices() const;
	bool IsHardwareStencilSupported() const;
	std::string GetStencilBackendString() const;
	std::string GetStencilBackendDetailString() const;
	std::string GetMaterialDetailString() const;
	std::string GetTextureDetailString() const;
	std::string GetFrameRenderDetailString() const;
	std::string GetImeDebugString() const;
	bool IsImeCompositionActive() const;
	bool IsImeCandidateOpen() const;
	int GetImeCompositionUpdateCount() const;
	int GetImeCompositionCommitCount() const;
	int GetImeCompositionEndCount() const;
	int GetImeCandidateOpenCount() const;
	int GetImeCandidateCloseCount() const;
	int GetImeCandidateCount() const;
	int GetImeCandidateSelection() const;

private:
	FairyGuiSystem(const FairyGuiSystem&);
	FairyGuiSystem& operator=(const FairyGuiSystem&);

	FairyGuiSystemImpl* m_impl; // owning pimpl; created/destroyed by FairyGuiSystem
};

#endif // __HELLO_FAIRY_GUI_SYSTEM_H__
