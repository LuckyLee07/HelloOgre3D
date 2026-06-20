#ifndef __FAIRY_GUI_LUA_API_H__
#define __FAIRY_GUI_LUA_API_H__

#include <string>

class FairyGuiSystem;

class FairyGuiLuaApi //tolua_exports
{ //tolua_exports
public:
	explicit FairyGuiLuaApi(FairyGuiSystem* system);

	//tolua_begin
	bool IsAvailable() const;

	// Package and object creation.
	const char* LoadPackage(const char* packagePath);
	bool RemovePackage(const char* packageName) const;
	int CreateObject(const char* packageName, const char* objectName) const;
	int CreateContainer(const char* name) const;
	int CreateChildContainer(int ownerHandle, const char* name) const;
	int CreateLoader(int ownerHandle, const char* name, const char* url) const;
	int CreateText(int ownerHandle, const char* name, const char* text, float fontSize, float red, float green, float blue) const;
	int CreateTextInput(int ownerHandle, const char* name, const char* text, float fontSize, float red, float green, float blue) const;
	int CreateGraphRect(int ownerHandle, const char* name, float width, float height, float red, float green, float blue, float alpha) const;
	int CreateGraphRegularPolygon(int ownerHandle, const char* name, float width, float height, int sides, float red, float green, float blue, float alpha) const;
	int CreateModalMask(float red, float green, float blue, float alpha) const;

	// Render, resource, and service diagnostics.
	int GetLastRenderCommandCount() const;
	int GetLastTriangleCount() const;
	int GetScreenWidth() const;
	int GetScreenHeight() const;
	int GetRuntimeObjectHandleCount() const;
	int GetRuntimeListenerBindingCount() const;
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
	const char* GetStencilBackendString();
	const char* GetStencilBackendDetailString();
	const char* GetMaterialDetailString();
	const char* GetTextureDetailString();
	const char* GetFrameRenderDetailString();
	bool PlotServiceStats(int serviceOpenTotal, int serviceKindCount, int toastQueueCount, int loadingRefTotal, int serviceCreatedTotal, int serviceClosedTotal, int serviceFailedTotal, int servicePeakOpen) const;

	// Object handle access and properties.
	int GetChild(int objectHandle, const char* childPath) const;
	int GetListItem(int objectHandle, int itemIndex) const;
	int GetListItemCount(int objectHandle) const;
	bool AddObjectToRoot(int objectHandle) const;
	bool AddObjectToParent(int objectHandle, int parentHandle) const;
	bool SetObjectPosition(int objectHandle, float x, float y) const;
	bool SetObjectSize(int objectHandle, float width, float height) const;
	bool SetObjectVisible(int objectHandle, bool visible) const;
	bool SetObjectAlpha(int objectHandle, float alpha) const;
	bool SetObjectTouchable(int objectHandle, bool touchable) const;
	bool SetObjectMask(int objectHandle, int maskHandle, bool inverted) const;
	bool SetObjectSortingOrder(int objectHandle, int sortingOrder) const;
	bool SetObjectText(int objectHandle, const char* text) const;
	const char* GetObjectText(int objectHandle);
	bool SetObjectIcon(int objectHandle, const char* icon) const;
	bool SetObjectLoaderUrl(int objectHandle, const char* url) const;
	bool SetObjectControllerIndex(int objectHandle, const char* controllerName, int selectedIndex) const;
	int GetObjectControllerIndex(int objectHandle, const char* controllerName) const;
	bool SetObjectControllerPage(int objectHandle, const char* controllerName, const char* pageName) const;
	const char* GetObjectControllerPage(int objectHandle, const char* controllerName);
	const char* GetObjectControllerPageId(int objectHandle, const char* controllerName);
	int GetObjectControllerPageCount(int objectHandle, const char* controllerName) const;
	const char* GetObjectControllerPageNameAt(int objectHandle, const char* controllerName, int pageIndex);
	const char* GetObjectControllerPageIdAt(int objectHandle, const char* controllerName, int pageIndex);
	bool SetObjectValue(int objectHandle, float value) const;
	float GetObjectValue(int objectHandle) const;
	bool SetObjectMin(int objectHandle, float minValue) const;
	float GetObjectMin(int objectHandle) const;
	bool SetObjectMax(int objectHandle, float maxValue) const;
	float GetObjectMax(int objectHandle) const;
	bool SetComboBoxSelectedIndex(int objectHandle, int selectedIndex) const;
	int GetComboBoxSelectedIndex(int objectHandle) const;
	bool SetComboBoxValue(int objectHandle, const char* value) const;
	const char* GetComboBoxValue(int objectHandle);
	bool PlayTransition(int objectHandle, const char* transitionName, int times, float delay, int callbackId) const;
	bool StopTransition(int objectHandle, const char* transitionName, bool setToComplete, bool processCallback) const;
	bool FocusObject(int objectHandle) const;
	bool ClearFocus() const;
	int GetFocusedObject() const;
	bool SetListItemCount(int objectHandle, int itemCount) const;
	bool SetListSelectedIndex(int objectHandle, int selectedIndex) const;
	int GetListSelectedIndex(int objectHandle) const;
	bool SetListVirtual(int objectHandle, bool loop) const;
	bool RefreshList(int objectHandle) const;
	bool ScrollListToView(int objectHandle, int itemIndex) const;
	bool CenterObject(int objectHandle, bool restraint) const;

	// Input and IME bridge.
	bool InjectMouseMove(int x, int y) const;
	bool InjectMouseDown(int x, int y, int button) const;
	bool InjectMouseUp(int x, int y, int button) const;
	bool InjectMouseWheel(int x, int y, int wheelDelta) const;
	bool InjectLogicalMouseMove(int x, int y) const;
	bool InjectLogicalMouseDown(int x, int y, int button) const;
	bool InjectLogicalMouseUp(int x, int y, int button) const;
	bool InjectLogicalMouseWheel(int x, int y, int wheelDelta) const;
	bool InjectLogicalClick(int x, int y, int button) const;
	bool InjectKeyPressed(int keyCode, int keyText) const;
	bool InjectKeyReleased(int keyCode, int keyText) const;
	bool InjectImeCompositionText(const char* text) const;
	bool InjectImeCommitText(const char* text) const;
	bool ClearImeComposition() const;
	const char* GetImeDebugString();

	// Event bindings and object lifetime.
	int AddEventListener(int objectHandle, const char* childPath, int eventType, int callbackId) const;
	int AddClickListener(int objectHandle, const char* childPath, int callbackId) const;
	int AddControllerChangedListener(int objectHandle, const char* controllerName, int callbackId) const;
	bool RemoveListener(int bindingId) const;
	bool RemoveObject(int objectHandle) const;
	void ClearObjects() const;
	//tolua_end

private:
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* GetSystem() const;
#endif
	static const char* SafeString(const char* value);

	FairyGuiSystem* m_system; // non-owning; owned by GameManager/FairyGuiRuntime
	std::string m_lastPackageName;
	std::string m_lastStencilBackend;
	std::string m_lastStencilBackendDetail;
	std::string m_lastMaterialDetail;
	std::string m_lastTextureDetail;
	std::string m_lastFrameRenderDetail;
	std::string m_lastObjectText;
	std::string m_lastObjectValue;
	std::string m_lastControllerString;
	std::string m_lastImeDebug;
}; //tolua_exports

#endif // __FAIRY_GUI_LUA_API_H__
