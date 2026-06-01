#ifndef __VISION_SENSOR_H__
#define __VISION_SENSOR_H__

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "ai/perception/AgentPerceptionQuery.h"

struct VisionSensorConfig
{
	VisionSensorConfig()
		: scanIntervalMs(200)
		, initialDelayMs(0)
	{
	}

	AgentPerceptionOptions perception;
	int scanIntervalMs;
	int initialDelayMs;
};

class VisionSensor
{
public:
	VisionSensor()
		: m_timeSinceScanMs(0)
		, m_lastScanIntervalMs(200)
		, m_lastInitialDelayMs(0)
		, m_scanCount(0)
		, m_hasScanned(false)
		, m_hasVisibleTarget(false)
	{
	}

	bool Tick(AgentObject* owner, AgentObject* preferredEnemy, IAgentPerceptionQuery* query, const VisionSensorConfig& config, int deltaMs, bool forceScan)
	{
		const int elapsedMs = std::max(0, deltaMs);
		m_timeSinceScanMs += elapsedMs;
		m_lastScanIntervalMs = std::max(0, config.scanIntervalMs);
		m_lastInitialDelayMs = std::max(0, config.initialDelayMs);
		m_lastConfig = config;

		const int firstScanDelayMs = m_lastScanIntervalMs > 0 ? std::min(m_lastInitialDelayMs, m_lastScanIntervalMs) : 0;
		if (!forceScan && !m_hasScanned && firstScanDelayMs > 0 && m_timeSinceScanMs < firstScanDelayMs)
			return false;

		if (!forceScan && m_hasScanned && m_lastScanIntervalMs > 0 && m_timeSinceScanMs < m_lastScanIntervalMs)
			return false;

		AgentPerceptionResult result;
		bool found = false;
		if (owner != nullptr && query != nullptr)
		{
			if (preferredEnemy != nullptr)
			{
				found = query->TryGetEnemy(owner, preferredEnemy, config.perception, result);
			}
			if (!found)
			{
				found = query->FindNearestEnemy(owner, config.perception, result);
			}
		}

		m_hasScanned = true;
		m_hasVisibleTarget = found;
		m_lastResult = found ? result : AgentPerceptionResult();
		m_timeSinceScanMs = 0;
		++m_scanCount;
		return true;
	}

	void Clear()
	{
		m_timeSinceScanMs = 0;
		m_scanCount = 0;
		m_hasScanned = false;
		m_hasVisibleTarget = false;
		m_lastResult = AgentPerceptionResult();
	}

	bool HasScanned() const { return m_hasScanned; }
	bool HasVisibleTarget() const { return m_hasVisibleTarget; }
	const AgentPerceptionResult& GetLastResult() const { return m_lastResult; }

	std::string BuildDebugString() const
	{
		std::ostringstream stream;
		stream << "vision={scans:" << m_scanCount
			<< ",interval:" << m_lastScanIntervalMs
			<< ",delay:" << m_lastInitialDelayMs
			<< ",age:" << m_timeSinceScanMs;
		if (m_hasVisibleTarget)
		{
			stream << ",target:" << m_lastResult.targetId
				<< ",dist:" << static_cast<int>(std::sqrt(std::max(0.0f, m_lastResult.distanceSquared)))
				<< ",conf:" << std::fixed << std::setprecision(2) << m_lastResult.confidence;
		}
		else
		{
			stream << ",target:none";
		}
		stream << "}";
		return stream.str();
	}

private:
	int m_timeSinceScanMs;
	int m_lastScanIntervalMs;
	int m_lastInitialDelayMs;
	int m_scanCount;
	bool m_hasScanned;
	bool m_hasVisibleTarget;
	AgentPerceptionResult m_lastResult;
	VisionSensorConfig m_lastConfig;
};

#endif // __VISION_SENSOR_H__
