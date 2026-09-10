#include "RuntimeUiSound.h"

#include "OgrePlatform.h"
#include "OgreLogManager.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <utility>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
bool RuntimeUiSoundMacPlay(void*& handle, const unsigned char* bytes, unsigned long size);
void RuntimeUiSoundMacStop(void*& handle);
#endif

namespace
{
	unsigned int Read16(const std::vector<unsigned char>& bytes, size_t offset)
	{
		return bytes[offset] | (static_cast<unsigned int>(bytes[offset + 1]) << 8);
	}

	unsigned int Read32(const std::vector<unsigned char>& bytes, size_t offset)
	{
		return Read16(bytes, offset) | (Read16(bytes, offset + 2) << 16);
	}

	bool FindPcmData(const std::vector<unsigned char>& bytes, size_t& dataOffset, size_t& dataLength)
	{
		if (bytes.size() < 44 || std::memcmp(bytes.data(), "RIFF", 4) != 0
			|| std::memcmp(bytes.data() + 8, "WAVE", 4) != 0)
			return false;
		bool validFormat = false;
		dataLength = 0;
		for (size_t offset = 12; offset + 8 <= bytes.size();)
		{
			const size_t size = Read32(bytes, offset + 4);
			const size_t start = offset + 8;
			if (size > bytes.size() - start) return false;
			if (std::memcmp(bytes.data() + offset, "fmt ", 4) == 0)
			{
				if (size < 16) return false;
				const unsigned int channels = Read16(bytes, start + 2);
				const unsigned int rate = Read32(bytes, start + 4);
				validFormat = Read16(bytes, start) == 1 && (channels == 1 || channels == 2)
					&& Read16(bytes, start + 14) == 16 && rate >= 8000 && rate <= 96000
					&& Read16(bytes, start + 12) == channels * 2
					&& Read32(bytes, start + 8) == rate * channels * 2;
			}
			else if (std::memcmp(bytes.data() + offset, "data", 4) == 0)
			{
				dataOffset = start;
				dataLength = size;
			}
			offset = start + size + (size & 1);
		}
		return validFormat && dataLength > 0 && dataLength % 2 == 0;
	}

	void LogSoundFailure(const std::string& reason, const char* path)
	{
		if (Ogre::LogManager::getSingletonPtr() != nullptr)
			Ogre::LogManager::getSingleton().logMessage("[RuntimeUiSound] " + reason + ": " + path);
	}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	typedef BOOL (WINAPI* PlaySoundFunction)(LPCSTR, HMODULE, DWORD);
	PlaySoundFunction GetPlaybackFunction(void* module)
	{
		return module != nullptr ? reinterpret_cast<PlaySoundFunction>(GetProcAddress(static_cast<HMODULE>(module), "PlaySoundA")) : nullptr;
	}
#endif
}

RuntimeUiSound::RuntimeUiSound()
	: m_platformHandle(nullptr), m_volume(0.65f)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	m_platformHandle = LoadLibraryW(L"winmm.dll");
#endif
}

RuntimeUiSound::~RuntimeUiSound()
{
	StopAll();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (m_platformHandle != nullptr)
		FreeLibrary(static_cast<HMODULE>(m_platformHandle));
#endif
}

bool RuntimeUiSound::IsAvailable() const
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	return GetPlaybackFunction(m_platformHandle) != nullptr;
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	return true;
#else
	return false;
#endif
}

bool RuntimeUiSound::Play(const char* path)
{
	if (path == nullptr || path[0] == 0 || !IsAvailable()) return false;
	if (m_volume <= 0.0f) return true;
	auto cached = m_waveCache.find(path);
	if (cached == m_waveCache.end())
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		const std::streamoff size = file ? static_cast<std::streamoff>(file.tellg()) : -1;
		if (size < 44 || size > 2 * 1024 * 1024)
		{
			LogSoundFailure("missing or oversized WAV", path);
			return false;
		}
		std::vector<unsigned char> bytes(static_cast<size_t>(size));
		file.seekg(0);
		if (!file.read(reinterpret_cast<char*>(bytes.data()), size)) return false;
		size_t dataOffset = 0, dataLength = 0;
		if (!FindPcmData(bytes, dataOffset, dataLength))
		{
			LogSoundFailure("requires PCM 16-bit WAV", path);
			return false;
		}
		// UI feedback uses a handful of files; cap caching for arbitrary callers.
		if (m_waveCache.size() >= 32) m_waveCache.clear();
		cached = m_waveCache.emplace(path, std::move(bytes)).first;
	}

	StopAll();
	m_playback = cached->second;
	size_t dataOffset = 0, dataLength = 0;
	if (!FindPcmData(m_playback, dataOffset, dataLength)) return false;
	for (size_t offset = dataOffset; offset < dataOffset + dataLength; offset += 2)
	{
		const unsigned int raw = Read16(m_playback, offset);
		const int sample = raw >= 32768 ? static_cast<int>(raw) - 65536 : static_cast<int>(raw);
		const int scaled = static_cast<int>(sample * m_volume);
		const unsigned int encoded = scaled < 0 ? static_cast<unsigned int>(scaled + 65536) : static_cast<unsigned int>(scaled);
		m_playback[offset] = static_cast<unsigned char>(encoded & 255);
		m_playback[offset + 1] = static_cast<unsigned char>((encoded >> 8) & 255);
	}

	bool played = false;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	played = GetPlaybackFunction(m_platformHandle)(reinterpret_cast<LPCSTR>(m_playback.data()), nullptr, SND_MEMORY | SND_ASYNC | SND_NODEFAULT) != FALSE;
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	played = RuntimeUiSoundMacPlay(m_platformHandle, m_playback.data(), static_cast<unsigned long>(m_playback.size()));
#endif
	if (!played) LogSoundFailure("backend playback failed", path);
	return played;
}

void RuntimeUiSound::StopAll()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	PlaySoundFunction play = GetPlaybackFunction(m_platformHandle);
	if (play != nullptr) play(nullptr, nullptr, 0);
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	RuntimeUiSoundMacStop(m_platformHandle);
#endif
	m_playback.clear();
}

void RuntimeUiSound::SetVolume(float volume)
{
	if (!std::isfinite(volume)) return;
	m_volume = std::max(0.0f, std::min(1.0f, volume));
	if (m_volume == 0.0f) StopAll();
}

float RuntimeUiSound::GetVolume() const
{
	return m_volume;
}
