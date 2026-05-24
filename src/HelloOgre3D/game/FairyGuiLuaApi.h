#ifndef __FAIRY_GUI_LUA_API_H__
#define __FAIRY_GUI_LUA_API_H__

#include <string>

class ClientManager;

#if defined(HELLO_ENABLE_FGUI)
class FairyGuiSystem;
#endif

class FairyGuiLuaApi
{
public:
	explicit FairyGuiLuaApi(ClientManager* clientManager);

	bool IsAvailable() const;
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

private:
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* GetSystem() const;
#endif

	ClientManager* m_clientManager;
	std::string m_lastPackageName;
	std::string m_lastStencilBackend;
	std::string m_lastStencilBackendDetail;
	std::string m_lastMaterialDetail;
	std::string m_lastTextureDetail;
	std::string m_lastFrameRenderDetail;
};

#endif // __FAIRY_GUI_LUA_API_H__
