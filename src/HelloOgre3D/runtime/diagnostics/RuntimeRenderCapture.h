#ifndef __RUNTIME_RENDER_CAPTURE_H__
#define __RUNTIME_RENDER_CAPTURE_H__

#include "OgreException.h"
#include "OgreFileSystemLayer.h"
#include "OgreLogManager.h"
#include "OgreRenderWindow.h"
#include "OgreStringConverter.h"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace RuntimeRenderCapture
{
namespace
{
	struct CaptureState
	{
		CaptureState()
			: initialized(false)
			, enabled(false)
			, timingStarted(false)
			, useSimulationClock(false)
			, elapsedMs(0.0)
			, maxDeltaMs(250.0)
			, nextIndex(0)
			, skippedLargeDeltaCount(0)
		{
		}

		bool initialized;
		bool enabled;
		bool timingStarted;
		bool useSimulationClock;
		double elapsedMs;
		double maxDeltaMs;
		size_t nextIndex;
		int skippedLargeDeltaCount;
		std::string outputDir;
		std::string prefix;
		std::vector<int> captureMs;
	};

	CaptureState& GetState()
	{
		static CaptureState state;
		return state;
	}

	bool IsFalseEnvValue(const char* value)
	{
		if (value == nullptr || value[0] == '\0')
			return false;
		const std::string text(value);
		return text == "0" || text == "false" || text == "FALSE" || text == "False" || text == "off" || text == "OFF";
	}

	bool IsTrueEnvValue(const char* value)
	{
		if (value == nullptr || value[0] == '\0')
			return false;
		return !IsFalseEnvValue(value);
	}

	bool IsSimulationClockValue(const char* value)
	{
		if (value == nullptr || value[0] == '\0')
			return false;
		const std::string text(value);
		return text == "simulation" || text == "sim" || text == "SIMULATION" || text == "Sim";
	}

	const char* GetEnvAlias(const char* primary, const char* legacy)
	{
		const char* value = std::getenv(primary);
		if (value != nullptr && value[0] != '\0')
			return value;
		return std::getenv(legacy);
	}

	std::string NormalizeDirectory(std::string path)
	{
		std::replace(path.begin(), path.end(), '\\', '/');
		while (path.size() > 1 && path[path.size() - 1] == '/')
			path.erase(path.size() - 1);
		return path;
	}

	bool EnsureDirectoryRecursive(const std::string& directory)
	{
		if (directory.empty())
			return false;

		const std::string normalized = NormalizeDirectory(directory);
		size_t start = 0;
		if (normalized.size() >= 2 && normalized[1] == ':')
			start = 2;
		while (start < normalized.size() && normalized[start] == '/')
			++start;

		size_t pos = start;
		while (pos <= normalized.size())
		{
			size_t slash = normalized.find('/', pos);
			const size_t end = slash == std::string::npos ? normalized.size() : slash;
			const std::string part = normalized.substr(0, end);
			if (!part.empty() && !(part.size() == 2 && part[1] == ':'))
			{
				if (!Ogre::FileSystemLayer::createDirectory(part))
					return false;
			}
			if (slash == std::string::npos)
				break;
			pos = slash + 1;
		}
		return true;
	}

	std::vector<int> ParseCaptureTimes(const char* value)
	{
		std::vector<int> result;
		const std::string text = value != nullptr && value[0] != '\0' ? value : "1000,2500";
		std::stringstream stream(text);
		std::string token;
		while (std::getline(stream, token, ','))
		{
			const int parsed = std::atoi(token.c_str());
			if (parsed >= 0)
				result.push_back(parsed);
		}

		std::sort(result.begin(), result.end());
		result.erase(std::unique(result.begin(), result.end()), result.end());
		return result;
	}

	std::string JoinCaptureTimes(const std::vector<int>& captureMs)
	{
		std::ostringstream stream;
		for (size_t i = 0; i < captureMs.size(); ++i)
		{
			if (i > 0)
				stream << ",";
			stream << captureMs[i];
		}
		return stream.str();
	}

	double ParsePositiveDoubleEnv(const char* name, double fallback)
	{
		const char* value = std::getenv(name);
		if (value == nullptr || value[0] == '\0')
			return fallback;
		const double parsed = std::atof(value);
		return parsed > 0.0 ? parsed : fallback;
	}

	void LogMessage(const std::string& message)
	{
		Ogre::LogManager* logManager = Ogre::LogManager::getSingletonPtr();
		if (logManager != nullptr)
			logManager->logMessage(message);
	}

	void Initialize()
	{
		CaptureState& state = GetState();
		if (state.initialized)
			return;

		state.initialized = true;
		state.enabled = IsTrueEnvValue(GetEnvAlias("HELLO_RENDER_CAPTURE", "HELLO_VISUAL_CAPTURE"));
		if (!state.enabled)
			return;

		const char* outputDir = GetEnvAlias("HELLO_RENDER_CAPTURE_DIR", "HELLO_VISUAL_CAPTURE_DIR");
		if (outputDir == nullptr || outputDir[0] == '\0')
		{
			LogMessage("[RenderCapture] disabled missing output dir");
			state.enabled = false;
			return;
		}

		state.outputDir = NormalizeDirectory(outputDir);
		const char* prefix = GetEnvAlias("HELLO_RENDER_CAPTURE_PREFIX", "HELLO_VISUAL_CAPTURE_PREFIX");
		state.prefix = prefix != nullptr && prefix[0] != '\0' ? prefix : "capture";
		state.captureMs = ParseCaptureTimes(GetEnvAlias("HELLO_RENDER_CAPTURE_MS", "HELLO_VISUAL_CAPTURE_MS"));
		state.maxDeltaMs = ParsePositiveDoubleEnv("HELLO_RENDER_CAPTURE_MAX_DELTA_MS", 250.0);
		state.useSimulationClock = IsSimulationClockValue(std::getenv("HELLO_RENDER_CAPTURE_CLOCK"))
			|| IsTrueEnvValue(GetEnvAlias("HELLO_RENDER_CAPTURE_SIMULATION_CLOCK", "HELLO_VISUAL_CAPTURE_SIMULATION_CLOCK"));
		if (state.captureMs.empty())
		{
			LogMessage("[RenderCapture] disabled missing capture times");
			state.enabled = false;
			return;
		}

		if (!EnsureDirectoryRecursive(state.outputDir))
		{
			LogMessage("[RenderCapture] disabled cannot create output dir: " + state.outputDir);
			state.enabled = false;
			return;
		}

		LogMessage("[RenderCapture] enabled dir=" + state.outputDir + " prefix=" + state.prefix + " clock="
			+ (state.useSimulationClock ? "simulation" : "render") + " capturesMs=" + JoinCaptureTimes(state.captureMs));
	}

	std::string BuildCapturePath(const CaptureState& state, int targetMs)
	{
		std::ostringstream stream;
		stream << state.outputDir << "/" << state.prefix << "_" << std::setw(5) << std::setfill('0') << targetMs << "ms.png";
		return stream.str();
	}
}

	static void Update(Ogre::RenderWindow* renderWindow, float deltaSeconds, double simulationElapsedMs = -1.0)
	{
		Initialize();

		CaptureState& state = GetState();
		if (!state.enabled || renderWindow == nullptr || state.nextIndex >= state.captureMs.size())
			return;

		if (!state.timingStarted)
		{
			state.timingStarted = true;
			LogMessage("[RenderCapture] armed");
			return;
		}

		if (state.useSimulationClock && simulationElapsedMs >= 0.0)
		{
			state.elapsedMs = std::max(0.0, simulationElapsedMs);
		}
		else if (deltaSeconds > 0.0f)
		{
			const double deltaMs = static_cast<double>(deltaSeconds) * 1000.0;
			if (deltaMs > state.maxDeltaMs)
			{
				++state.skippedLargeDeltaCount;
				if (state.skippedLargeDeltaCount <= 3)
				{
					std::ostringstream stream;
					stream << std::fixed << std::setprecision(2);
					stream << "[RenderCapture] skipLargeDelta deltaMs=" << deltaMs << " maxDeltaMs=" << state.maxDeltaMs;
					LogMessage(stream.str());
				}
				return;
			}
			state.elapsedMs += deltaMs;
		}

		while (state.nextIndex < state.captureMs.size() && state.elapsedMs + 0.5 >= state.captureMs[state.nextIndex])
		{
			const int targetMs = state.captureMs[state.nextIndex];
			const std::string path = BuildCapturePath(state, targetMs);
			try
			{
				renderWindow->writeContentsToFile(path);
				std::ostringstream stream;
				stream << std::fixed << std::setprecision(2);
				stream << "[RenderCapture] captured path=" << path << " targetMs=" << targetMs << " elapsedMs=" << state.elapsedMs;
				LogMessage(stream.str());
			}
			catch (const Ogre::Exception& e)
			{
				LogMessage(std::string("[RenderCapture] failed path=") + path + " error=" + e.getFullDescription());
			}
			catch (const std::exception& e)
			{
				LogMessage(std::string("[RenderCapture] failed path=") + path + " error=" + e.what());
			}
			catch (...)
			{
				LogMessage(std::string("[RenderCapture] failed path=") + path + " error=unknown");
			}
			++state.nextIndex;
		}
	}
}

#endif  // __RUNTIME_RENDER_CAPTURE_H__
