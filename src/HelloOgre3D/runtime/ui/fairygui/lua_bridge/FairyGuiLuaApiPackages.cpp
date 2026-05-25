#include "FairyGuiLuaApiInternal.h"

const char* FairyGuiLuaApi::LoadPackage(const char* packagePath)
{
	m_lastPackageName.clear();
#if defined(HELLO_ENABLE_FGUI)
	if (packagePath == nullptr)
		return "";

	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return "";

	m_lastPackageName = system->LoadPackageAndGetName(packagePath);
#endif
	return m_lastPackageName.c_str();
}

bool FairyGuiLuaApi::RemovePackage(const char* packageName) const
{
#if defined(HELLO_ENABLE_FGUI)
	if (packageName == nullptr)
		return false;

	FairyGuiSystem* system = GetSystem();
	return system != nullptr && system->RemovePackage(packageName);
#else
	return false;
#endif
}

int FairyGuiLuaApi::CreateObject(const char* packageName, const char* objectName) const
{
#if defined(HELLO_ENABLE_FGUI)
	if (packageName == nullptr || objectName == nullptr)
		return 0;

	FairyGuiSystem* system = GetSystem();
	if (system == nullptr)
		return 0;

	return system->CreateObjectHandle(packageName, objectName);
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateContainer(const char* name) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->CreateContainerHandle(SafeString(name)) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateChildContainer(int ownerHandle, const char* name) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->CreateChildContainerHandle(ownerHandle, SafeString(name)) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateLoader(int ownerHandle, const char* name, const char* url) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->CreateLoaderHandle(ownerHandle, SafeString(name), SafeString(url)) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateText(int ownerHandle, const char* name, const char* text, float fontSize, float red, float green, float blue) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->CreateTextHandle(ownerHandle, SafeString(name), SafeString(text), fontSize, red, green, blue) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateTextInput(int ownerHandle, const char* name, const char* text, float fontSize, float red, float green, float blue) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->CreateTextInputHandle(ownerHandle, SafeString(name), SafeString(text), fontSize, red, green, blue) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateGraphRect(int ownerHandle, const char* name, float width, float height, float red, float green, float blue, float alpha) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->CreateGraphRectHandle(ownerHandle, SafeString(name), width, height, red, green, blue, alpha) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateGraphRegularPolygon(int ownerHandle, const char* name, float width, float height, int sides, float red, float green, float blue, float alpha) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->CreateGraphRegularPolygonHandle(ownerHandle, SafeString(name), width, height, sides, red, green, blue, alpha) : 0;
#else
	return 0;
#endif
}

int FairyGuiLuaApi::CreateModalMask(float red, float green, float blue, float alpha) const
{
#if defined(HELLO_ENABLE_FGUI)
	FairyGuiSystem* system = GetSystem();
	return system != nullptr ? system->CreateModalMaskHandle(red, green, blue, alpha) : 0;
#else
	return 0;
#endif
}
