#ifndef HELLO_RUNTIME_UI_SOUND_H
#define HELLO_RUNTIME_UI_SOUND_H

#include <string>
#include <unordered_map>
#include <vector>

// Local, single-channel UI feedback. Playback replaces the preceding short sound.
// PCM is owned here until StopAll; Lua owns no native audio pointers.
class RuntimeUiSound //tolua_exports
{ //tolua_exports
public:
	RuntimeUiSound();
	~RuntimeUiSound();

	//tolua_begin
	bool IsAvailable() const;
	bool Play(const char* path);
	void StopAll();
	void SetVolume(float volume);
	float GetVolume() const;
	//tolua_end

private:
	RuntimeUiSound(const RuntimeUiSound&) = delete;
	RuntimeUiSound& operator=(const RuntimeUiSound&) = delete;
	void* m_platformHandle;
	float m_volume;
	std::unordered_map<std::string, std::vector<unsigned char>> m_waveCache;
	std::vector<unsigned char> m_playback;
}; //tolua_exports

#endif
