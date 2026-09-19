#include "RuntimeUiSound.h"

#include "OgrePlatform.h"
#include "OgreLogManager.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <memory>
#include <utility>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
bool RuntimeUiSoundMacPlay(void*& handle, const unsigned char* bytes, unsigned long size, float volume);
bool RuntimeUiSoundMacIsPlaying(void* handle);
void RuntimeUiSoundMacSetVolume(void* handle, float volume);
void RuntimeUiSoundMacStop(void*& handle);
#endif

namespace
{
	const size_t MaxVoices = 8;

	struct PcmInfo
	{
		size_t offset = 0;
		size_t length = 0;
		unsigned int channels = 0;
		unsigned int rate = 0;
	};

	unsigned int Read16(const std::vector<unsigned char>& bytes, size_t offset)
	{
		return bytes[offset] | (static_cast<unsigned int>(bytes[offset + 1]) << 8);
	}

	unsigned int Read32(const std::vector<unsigned char>& bytes, size_t offset)
	{
		return Read16(bytes, offset) | (Read16(bytes, offset + 2) << 16);
	}

	void Write16(std::vector<unsigned char>& bytes, size_t offset, unsigned int value)
	{
		bytes[offset] = static_cast<unsigned char>(value & 255);
		bytes[offset + 1] = static_cast<unsigned char>((value >> 8) & 255);
	}

	void Write32(std::vector<unsigned char>& bytes, size_t offset, unsigned int value)
	{
		Write16(bytes, offset, value);
		Write16(bytes, offset + 2, value >> 16);
	}

	bool FindPcmData(const std::vector<unsigned char>& bytes, PcmInfo& info)
	{
		if (bytes.size() < 44 || std::memcmp(bytes.data(), "RIFF", 4) != 0
			|| std::memcmp(bytes.data() + 8, "WAVE", 4) != 0)
			return false;
		const size_t riffLength = Read32(bytes, 4);
		if (riffLength < 36 || riffLength > bytes.size() - 8) return false;
		const size_t end = riffLength + 8;
		bool foundFormat = false, foundData = false;
		for (size_t offset = 12; offset < end;)
		{
			if (end - offset < 8) return false;
			const size_t size = Read32(bytes, offset + 4);
			const size_t start = offset + 8;
			if (size > end - start || (size & 1) > end - start - size) return false;
			if (std::memcmp(bytes.data() + offset, "fmt ", 4) == 0)
			{
				if (foundFormat || size < 16) return false;
				info.channels = Read16(bytes, start + 2);
				info.rate = Read32(bytes, start + 4);
				if (Read16(bytes, start) != 1 || (info.channels != 1 && info.channels != 2)
					|| Read16(bytes, start + 14) != 16 || info.rate < 8000 || info.rate > 96000
					|| Read16(bytes, start + 12) != info.channels * 2
					|| Read32(bytes, start + 8) != info.rate * info.channels * 2)
					return false;
				foundFormat = true;
			}
			else if (std::memcmp(bytes.data() + offset, "data", 4) == 0)
			{
				if (foundData) return false;
				info.offset = start;
				info.length = size;
				foundData = true;
			}
			offset = start + size + (size & 1);
		}
		return foundFormat && foundData && info.length > 0 && info.length % (info.channels * 2) == 0;
	}

	void LogSoundFailure(const std::string& reason, const char* path)
	{
		if (Ogre::LogManager::getSingletonPtr() != nullptr)
			Ogre::LogManager::getSingleton().logMessage("[RuntimeUiSound] " + reason + ": " + path);
	}

	struct SoundVoice
	{
		std::vector<unsigned char> wave;
		int priority = 0;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		HWAVEOUT handle = nullptr;
		WAVEHDR header = {};
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		void* handle = nullptr;
#endif
	};

	struct SoundBackend
	{
		// Stable allocations: Windows retains pointers to both WAVEHDR and PCM.
		std::vector<std::unique_ptr<SoundVoice>> voices;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		HMODULE module = nullptr;
		decltype(&waveOutOpen) open = nullptr;
		decltype(&waveOutPrepareHeader) prepare = nullptr;
		decltype(&waveOutWrite) write = nullptr;
		decltype(&waveOutReset) reset = nullptr;
		decltype(&waveOutUnprepareHeader) unprepare = nullptr;
		decltype(&waveOutClose) close = nullptr;
		decltype(&waveOutSetVolume) setVolume = nullptr;
		decltype(&waveOutGetNumDevs) getNumDevices = nullptr;
#endif
	};

	bool IsPlaying(const SoundVoice& voice)
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		return voice.handle != nullptr && (voice.header.dwFlags & WHDR_DONE) == 0;
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		return RuntimeUiSoundMacIsPlaying(voice.handle);
#else
		(void)voice;
		return false;
#endif
	}

	bool StopVoice(SoundBackend& backend, SoundVoice& voice)
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		if (voice.handle == nullptr) return true;
		// Reset returns queued PCM to us before unprepare/free. No audio callback
		// refers to C++ or Lua state, including while a sample is being unloaded.
		if (backend.reset(voice.handle) != MMSYSERR_NOERROR) return false;
		if ((voice.header.dwFlags & WHDR_PREPARED) != 0
			&& backend.unprepare(voice.handle, &voice.header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			return false;
		if (backend.close(voice.handle) != MMSYSERR_NOERROR) return false;
		voice.handle = nullptr;
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		(void)backend;
		RuntimeUiSoundMacStop(voice.handle);
#else
		(void)backend;
		(void)voice;
#endif
		return true;
	}

	void ReapVoices(SoundBackend& backend, bool stopAll)
	{
		for (auto it = backend.voices.begin(); it != backend.voices.end();)
		{
			if ((stopAll || !IsPlaying(**it)) && StopVoice(backend, **it))
				it = backend.voices.erase(it);
			else
				++it;
		}
	}

	bool SetVoiceVolume(SoundBackend& backend, SoundVoice& voice, float volume)
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		const DWORD level = static_cast<DWORD>(volume * 65535.0f);
		// A handle changes only this instance, never the system device volume.
		return backend.setVolume(voice.handle, level | (level << 16)) == MMSYSERR_NOERROR;
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		(void)backend;
		RuntimeUiSoundMacSetVolume(voice.handle, volume);
		return true;
#else
		(void)backend;
		(void)voice;
		(void)volume;
		return false;
#endif
	}

	void MakeStereoWave(const std::vector<unsigned char>& source, const PcmInfo& info,
		float gain, float pan, std::vector<unsigned char>& wave)
	{
		const size_t frames = info.length / (info.channels * 2);
		wave.assign(44 + frames * 4, 0);
		std::memcpy(wave.data(), "RIFF", 4);
		Write32(wave, 4, static_cast<unsigned int>(wave.size() - 8));
		std::memcpy(wave.data() + 8, "WAVEfmt ", 8);
		Write32(wave, 16, 16);
		Write16(wave, 20, 1);
		Write16(wave, 22, 2);
		Write32(wave, 24, info.rate);
		Write32(wave, 28, info.rate * 4);
		Write16(wave, 32, 4);
		Write16(wave, 34, 16);
		std::memcpy(wave.data() + 36, "data", 4);
		Write32(wave, 40, static_cast<unsigned int>(frames * 4));
		// Balance preserves legacy centre loudness and existing stereo content.
		const float gains[2] = { gain * std::min(1.0f, 1.0f - pan), gain * std::min(1.0f, 1.0f + pan) };
		for (size_t frame = 0; frame < frames; ++frame)
		{
			for (size_t channel = 0; channel < 2; ++channel)
			{
				const size_t input = info.offset + (frame * info.channels + (info.channels == 1 ? 0 : channel)) * 2;
				const unsigned int raw = Read16(source, input);
				const int sample = raw >= 32768 ? static_cast<int>(raw) - 65536 : static_cast<int>(raw);
				const int scaled = static_cast<int>(sample * gains[channel]);
				Write16(wave, 44 + frame * 4 + channel * 2, static_cast<unsigned int>(scaled) & 65535);
			}
		}
	}
}

RuntimeUiSound::RuntimeUiSound()
	: m_platformHandle(new SoundBackend()), m_volume(0.65f)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	SoundBackend& backend = *static_cast<SoundBackend*>(m_platformHandle);
	backend.module = LoadLibraryW(L"winmm.dll");
	if (backend.module != nullptr)
	{
#define HELLO_LOAD_WAVE(member, name) backend.member = reinterpret_cast<decltype(backend.member)>(GetProcAddress(backend.module, name))
		HELLO_LOAD_WAVE(open, "waveOutOpen");
		HELLO_LOAD_WAVE(prepare, "waveOutPrepareHeader");
		HELLO_LOAD_WAVE(write, "waveOutWrite");
		HELLO_LOAD_WAVE(reset, "waveOutReset");
		HELLO_LOAD_WAVE(unprepare, "waveOutUnprepareHeader");
		HELLO_LOAD_WAVE(close, "waveOutClose");
		HELLO_LOAD_WAVE(setVolume, "waveOutSetVolume");
		HELLO_LOAD_WAVE(getNumDevices, "waveOutGetNumDevs");
#undef HELLO_LOAD_WAVE
	}
#endif
}

RuntimeUiSound::~RuntimeUiSound()
{
	StopAll();
	SoundBackend* backend = static_cast<SoundBackend*>(m_platformHandle);
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	if (!backend->voices.empty())
	{
		// A faulty driver that refuses reset still owns these buffers. Keep the
		// bounded allocation alive rather than freeing memory beneath the OS.
		LogSoundFailure("driver refused to release audio buffers", "shutdown");
		return;
	}
	if (backend->module != nullptr) FreeLibrary(backend->module);
#endif
	delete backend;
}

bool RuntimeUiSound::IsAvailable() const
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	const SoundBackend& backend = *static_cast<SoundBackend*>(m_platformHandle);
	return backend.open && backend.prepare && backend.write && backend.reset
		&& backend.unprepare && backend.close && backend.setVolume
		&& backend.getNumDevices && backend.getNumDevices() > 0;
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	return true;
#else
	return false;
#endif
}

bool RuntimeUiSound::Play(const char* path)
{
	return PlayInternal(path, 1.0f, 0.0f, 0, true);
}

bool RuntimeUiSound::PlayLayer(const char* path, float gain, float pan, int priority)
{
	return PlayInternal(path, gain, pan, priority, false);
}

bool RuntimeUiSound::PlayInternal(const char* path, float gain, float pan, int priority, bool replace)
{
	if (path == nullptr || path[0] == 0 || !IsAvailable() || !std::isfinite(gain) || !std::isfinite(pan))
		return false;
	gain = std::max(0.0f, std::min(1.0f, gain));
	pan = std::max(-1.0f, std::min(1.0f, pan));
	priority = std::max(0, std::min(100, priority));
	if (m_volume <= 0.0f || gain <= 0.0f) return true;
	SoundBackend& backend = *static_cast<SoundBackend*>(m_platformHandle);
	ReapVoices(backend, false);
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
		PcmInfo info;
		if (!FindPcmData(bytes, info))
		{
			LogSoundFailure("requires complete PCM 16-bit WAV frames", path);
			return false;
		}
		if (m_waveCache.size() >= 32) m_waveCache.clear();
		cached = m_waveCache.emplace(path, std::move(bytes)).first;
	}

	if (replace) StopAll();
	if (backend.voices.size() >= MaxVoices)
	{
		const auto victim = std::min_element(backend.voices.begin(), backend.voices.end(),
			[](const std::unique_ptr<SoundVoice>& left, const std::unique_ptr<SoundVoice>& right)
			{ return left->priority < right->priority; });
		// Among equal priorities, replace the oldest. Never evict a more
		// important sound for a shot/impact during a dense burst.
		if ((*victim)->priority > priority || !StopVoice(backend, **victim)) return false;
		backend.voices.erase(victim);
	}
	PcmInfo info;
	if (!FindPcmData(cached->second, info)) return false;
	MakeStereoWave(cached->second, info, gain, pan, m_playback);
	std::unique_ptr<SoundVoice> voice(new SoundVoice());
	voice->wave = std::move(m_playback);
	voice->priority = priority;
	bool played = false;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	WAVEFORMATEX format = {};
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 2;
	format.nSamplesPerSec = info.rate;
	format.nAvgBytesPerSec = info.rate * 4;
	format.nBlockAlign = 4;
	format.wBitsPerSample = 16;
	if (backend.open(&voice->handle, WAVE_MAPPER, &format, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR)
	{
		voice->header.lpData = reinterpret_cast<LPSTR>(voice->wave.data() + 44);
		voice->header.dwBufferLength = static_cast<DWORD>(voice->wave.size() - 44);
		played = SetVoiceVolume(backend, *voice, m_volume)
			&& backend.prepare(voice->handle, &voice->header, sizeof(WAVEHDR)) == MMSYSERR_NOERROR
			&& backend.write(voice->handle, &voice->header, sizeof(WAVEHDR)) == MMSYSERR_NOERROR;
	}
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
	played = RuntimeUiSoundMacPlay(voice->handle, voice->wave.data(), static_cast<unsigned long>(voice->wave.size()), m_volume);
#endif
	if (played || !StopVoice(backend, *voice)) backend.voices.push_back(std::move(voice));
	if (!played) LogSoundFailure("backend playback failed", path);
	return played;
}

void RuntimeUiSound::StopAll()
{
	ReapVoices(*static_cast<SoundBackend*>(m_platformHandle), true);
	m_playback.clear();
}

void RuntimeUiSound::SetVolume(float volume)
{
	if (!std::isfinite(volume)) return;
	m_volume = std::max(0.0f, std::min(1.0f, volume));
	if (m_volume == 0.0f)
	{
		StopAll();
		return;
	}
	SoundBackend& backend = *static_cast<SoundBackend*>(m_platformHandle);
	ReapVoices(backend, false);
	for (auto& voice : backend.voices)
	{
		if (!SetVoiceVolume(backend, *voice, m_volume))
		{
			LogSoundFailure("master volume unavailable; stopping voices", "volume");
			StopAll();
			break;
		}
	}
}

float RuntimeUiSound::GetVolume() const
{
	return m_volume;
}
