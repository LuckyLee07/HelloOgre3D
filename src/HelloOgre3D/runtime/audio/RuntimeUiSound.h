#ifndef HELLO_RUNTIME_UI_SOUND_H
#define HELLO_RUNTIME_UI_SOUND_H

#include <string>
#include <unordered_map>
#include <vector>

// Local short sounds. Play replaces all voices; PlayLayer mixes up to eight.
// PCM and platform voices are owned here; Lua owns no native audio pointers.
class RuntimeUiSound //tolua_exports
{ //tolua_exports
public:
	RuntimeUiSound();
	~RuntimeUiSound();

	//tolua_begin
	bool IsAvailable() const;
	bool Play(const char* path);
	bool PlayLayer(const char* path, float gain, float pan, int priority);
	void StopAll();
	void SetVolume(float volume);
	float GetVolume() const;
	//tolua_end

private:
	RuntimeUiSound(const RuntimeUiSound&) = delete;
	RuntimeUiSound& operator=(const RuntimeUiSound&) = delete;
	bool PlayInternal(const char* path, float gain, float pan, int priority, bool replace);
	void* m_platformHandle;
	float m_volume;
	std::unordered_map<std::string, std::vector<unsigned char>> m_waveCache;
	std::vector<unsigned char> m_playback;
}; //tolua_exports

#endif
