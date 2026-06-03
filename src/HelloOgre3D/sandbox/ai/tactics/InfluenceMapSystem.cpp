#include "ai/tactics/InfluenceMapSystem.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>

namespace
{
	const float kDefaultMinX = -40.0f;
	const float kDefaultMaxX = 60.0f;
	const float kDefaultMinZ = -20.0f;
	const float kDefaultMaxZ = 70.0f;
	const float kDefaultCellSize = 5.0f;
	const float kActiveCellThreshold = 0.01f;
	const float kDefaultFalloff = 0.2f;
	const float kDefaultInertia = 0.5f;

	float ClampFloat(float value, float minValue, float maxValue)
	{
		return std::max(minValue, std::min(maxValue, value));
	}

	int ClampInt(int value, int minValue, int maxValue)
	{
		return std::max(minValue, std::min(maxValue, value));
	}

	std::string FormatVec3(const Ogre::Vector3& value)
	{
		std::ostringstream stream;
		stream << std::fixed << std::setprecision(2)
			<< value.x << "," << value.y << "," << value.z;
		return stream.str();
	}
}

InfluenceMapSystem::Stats::Stats()
	: configured(false)
	, width(0)
	, height(0)
	, totalCells(0)
	, layerCount(0)
	, activeCellCount(0)
	, sourceCount(0)
	, cellWriteCount(0)
	, queryCount(0)
	, spreadPassCount(0)
	, lastQueryCandidateCount(0)
	, lastBestScore(0.0f)
	, lastBestPosition(Ogre::Vector3::ZERO)
{
}

InfluenceMapSystem::Layer::Layer()
	: sourceCount(0)
	, cellWriteCount(0)
	, spreadPassCount(0)
	, falloff(kDefaultFalloff)
	, inertia(kDefaultInertia)
{
}

InfluenceMapSystem::InfluenceMapSystem()
	: m_minX(kDefaultMinX)
	, m_maxX(kDefaultMaxX)
	, m_minZ(kDefaultMinZ)
	, m_maxZ(kDefaultMaxZ)
	, m_cellSize(kDefaultCellSize)
	, m_width(0)
	, m_height(0)
{
	Configure(m_minX, m_maxX, m_minZ, m_maxZ, m_cellSize);
}

void InfluenceMapSystem::Configure(float minX, float maxX, float minZ, float maxZ, float cellSize)
{
	if (maxX < minX)
		std::swap(minX, maxX);
	if (maxZ < minZ)
		std::swap(minZ, maxZ);

	m_minX = minX;
	m_maxX = maxX;
	m_minZ = minZ;
	m_maxZ = maxZ;
	m_cellSize = cellSize > 0.01f ? cellSize : kDefaultCellSize;
	m_width = std::max(1, static_cast<int>(std::ceil((m_maxX - m_minX) / m_cellSize)));
	m_height = std::max(1, static_cast<int>(std::ceil((m_maxZ - m_minZ) / m_cellSize)));

	for (std::unordered_map<std::string, Layer>::iterator iter = m_layers.begin(); iter != m_layers.end(); ++iter)
		ResizeLayer(iter->second);

	RecalculateStats();
	m_stats.configured = true;
}

void InfluenceMapSystem::Clear()
{
	m_layers.clear();
	m_stats = Stats();
	m_stats.configured = true;
	m_stats.width = m_width;
	m_stats.height = m_height;
	m_stats.totalCells = m_width * m_height;
}

void InfluenceMapSystem::ClearLayer(const std::string& layerName)
{
	Layer& layer = GetOrCreateLayer(layerName);
	std::fill(layer.values.begin(), layer.values.end(), 0.0f);
	layer.sourceCount = 0;
	layer.cellWriteCount = 0;
	layer.spreadPassCount = 0;
	RecalculateStats();
}

void InfluenceMapSystem::SetLayerOptions(const std::string& layerName, float falloff, float inertia)
{
	Layer& layer = GetOrCreateLayer(layerName);
	layer.falloff = ClampFloat(falloff, 0.0f, 1.0f);
	layer.inertia = ClampFloat(inertia, 0.0f, 1.0f);
}

int InfluenceMapSystem::AddPointSource(const std::string& layerName, const Ogre::Vector3& center, float strength)
{
	EnsureConfigured();
	if (std::fabs(strength) <= 0.0001f)
		return 0;

	Layer& layer = GetOrCreateLayer(layerName);
	int x = 0;
	int z = 0;
	if (!WorldToCell(center, x, z))
		return 0;

	layer.values[GetCellIndex(x, z)] += strength;
	++layer.sourceCount;
	++layer.cellWriteCount;
	RecalculateStats();
	return 1;
}

int InfluenceMapSystem::AddRadialSource(const std::string& layerName, const Ogre::Vector3& center, float strength, float radius)
{
	EnsureConfigured();
	if (std::fabs(strength) <= 0.0001f)
		return 0;

	Layer& layer = GetOrCreateLayer(layerName);
	const float sourceRadius = radius > 0.01f ? radius : m_cellSize;
	const float radiusSq = sourceRadius * sourceRadius;
	const int minX = ClampInt(static_cast<int>(std::floor((center.x - sourceRadius - m_minX) / m_cellSize)), 0, m_width - 1);
	const int maxX = ClampInt(static_cast<int>(std::floor((center.x + sourceRadius - m_minX) / m_cellSize)), 0, m_width - 1);
	const int minZ = ClampInt(static_cast<int>(std::floor((center.z - sourceRadius - m_minZ) / m_cellSize)), 0, m_height - 1);
	const int maxZ = ClampInt(static_cast<int>(std::floor((center.z + sourceRadius - m_minZ) / m_cellSize)), 0, m_height - 1);
	int written = 0;

	for (int z = minZ; z <= maxZ; ++z)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			const Ogre::Vector3 cellCenter = GetCellCenter(x, z);
			const float dx = cellCenter.x - center.x;
			const float dz = cellCenter.z - center.z;
			const float distSq = dx * dx + dz * dz;
			if (distSq > radiusSq)
				continue;

			const float distance = std::sqrt(distSq);
			const float contribution = strength * (1.0f - distance / sourceRadius);
			if (std::fabs(contribution) <= 0.0001f)
				continue;

			layer.values[GetCellIndex(x, z)] += contribution;
			++written;
		}
	}

	if (written > 0)
	{
		++layer.sourceCount;
		layer.cellWriteCount += written;
		RecalculateStats();
	}
	return written;
}

int InfluenceMapSystem::SpreadLayer(const std::string& layerName, int passCount)
{
	EnsureConfigured();
	Layer* layer = const_cast<Layer*>(FindLayer(layerName));
	if (layer == nullptr || layer->values.empty())
		return 0;

	const int passes = ClampInt(passCount, 0, 16);
	if (passes <= 0)
		return 0;

	int changedCells = 0;
	const float falloff = ClampFloat(layer->falloff, 0.0f, 1.0f);
	const float inertia = ClampFloat(layer->inertia, 0.0f, 1.0f);
	for (int pass = 0; pass < passes; ++pass)
	{
		const std::vector<float> previous = layer->values;
		std::vector<float> next = previous;
		for (int z = 0; z < m_height; ++z)
		{
			for (int x = 0; x < m_width; ++x)
			{
				const int index = GetCellIndex(x, z);
				float strongest = previous[index];
				for (int dz = -1; dz <= 1; ++dz)
				{
					for (int dx = -1; dx <= 1; ++dx)
					{
						if (dx == 0 && dz == 0)
							continue;
						const int nx = x + dx;
						const int nz = z + dz;
						if (nx < 0 || nx >= m_width || nz < 0 || nz >= m_height)
							continue;

						const float neighbor = ReadCell(*layer, nx, nz);
						const float distanceScale = (dx != 0 && dz != 0) ? 0.7071067f : 1.0f;
						const float propagated = neighbor * falloff * distanceScale;
						if (std::fabs(propagated) > std::fabs(strongest))
							strongest = propagated;
					}
				}

				const float value = previous[index] * inertia + strongest * (1.0f - inertia);
				if (std::fabs(value - previous[index]) > 0.0001f)
				{
					next[index] = value;
					++changedCells;
				}
			}
		}
		layer->values.swap(next);
	}

	if (changedCells > 0)
	{
		layer->cellWriteCount += changedCells;
		layer->spreadPassCount += passes;
		RecalculateStats();
	}
	return changedCells;
}

float InfluenceMapSystem::SampleLayer(const std::string& layerName, const Ogre::Vector3& position) const
{
	const Layer* layer = FindLayer(layerName);
	if (layer == nullptr)
		return 0.0f;

	int x = 0;
	int z = 0;
	if (!WorldToCell(position, x, z))
		return 0.0f;
	return layer->values[GetCellIndex(x, z)];
}

float InfluenceMapSystem::ScorePosition(const Ogre::Vector3& position, float dangerWeight, float teamWeight, float objectiveWeight) const
{
	const float danger = SampleLayer("danger", position);
	const float team = SampleLayer("team", position);
	const float objective = SampleLayer("objective", position);
	return danger * dangerWeight + team * teamWeight + objective * objectiveWeight;
}

Ogre::Vector3 InfluenceMapSystem::FindBestPosition(const Ogre::Vector3& center, float radius, float step, float dangerWeight, float teamWeight, float objectiveWeight)
{
	EnsureConfigured();
	const float queryRadius = radius > 0.01f ? radius : m_cellSize;
	const float queryStep = step > 0.01f ? step : m_cellSize;
	const float radiusSq = queryRadius * queryRadius;
	const float minX = ClampFloat(center.x - queryRadius, m_minX, m_maxX);
	const float maxX = ClampFloat(center.x + queryRadius, m_minX, m_maxX);
	const float minZ = ClampFloat(center.z - queryRadius, m_minZ, m_maxZ);
	const float maxZ = ClampFloat(center.z + queryRadius, m_minZ, m_maxZ);

	float bestScore = -std::numeric_limits<float>::max();
	Ogre::Vector3 bestPosition = center;
	int candidates = 0;

	for (float z = minZ; z <= maxZ + 0.001f; z += queryStep)
	{
		for (float x = minX; x <= maxX + 0.001f; x += queryStep)
		{
			const float dx = x - center.x;
			const float dz = z - center.z;
			if (dx * dx + dz * dz > radiusSq)
				continue;

			const Ogre::Vector3 position(x, center.y, z);
			const float score = ScorePosition(position, dangerWeight, teamWeight, objectiveWeight);
			++candidates;
			if (score > bestScore)
			{
				bestScore = score;
				bestPosition = position;
			}
		}
	}

	++m_stats.queryCount;
	m_stats.lastQueryCandidateCount = candidates;
	m_stats.lastBestScore = candidates > 0 ? bestScore : 0.0f;
	m_stats.lastBestPosition = bestPosition;
	return bestPosition;
}

int InfluenceMapSystem::GetLayerActiveCellCount(const std::string& layerName) const
{
	const Layer* layer = FindLayer(layerName);
	if (layer == nullptr)
		return 0;

	int activeCells = 0;
	for (std::vector<float>::const_iterator iter = layer->values.begin(); iter != layer->values.end(); ++iter)
	{
		if (std::fabs(*iter) >= kActiveCellThreshold)
			++activeCells;
	}
	return activeCells;
}

int InfluenceMapSystem::GetLayerCellWriteCount(const std::string& layerName) const
{
	const Layer* layer = FindLayer(layerName);
	return layer != nullptr ? layer->cellWriteCount : 0;
}

int InfluenceMapSystem::GetLayerDebugCellCount(const std::string& layerName, float threshold, int maxCells) const
{
	const Layer* layer = FindLayer(layerName);
	if (layer == nullptr)
		return 0;

	const float safeThreshold = std::max(0.0f, threshold);
	const int limit = maxCells > 0 ? maxCells : std::numeric_limits<int>::max();
	int count = 0;
	for (std::vector<float>::const_iterator iter = layer->values.begin(); iter != layer->values.end(); ++iter)
	{
		if (std::fabs(*iter) >= safeThreshold)
		{
			++count;
			if (count >= limit)
				return count;
		}
	}
	return count;
}

Ogre::Vector3 InfluenceMapSystem::GetLayerDebugCellPosition(const std::string& layerName, int luaIndex, float threshold) const
{
	const Layer* layer = FindLayer(layerName);
	if (layer == nullptr || luaIndex <= 0)
		return Ogre::Vector3::ZERO;

	const float safeThreshold = std::max(0.0f, threshold);
	int count = 0;
	for (int z = 0; z < m_height; ++z)
	{
		for (int x = 0; x < m_width; ++x)
		{
			if (std::fabs(layer->values[GetCellIndex(x, z)]) < safeThreshold)
				continue;
			++count;
			if (count == luaIndex)
				return GetCellCenter(x, z);
		}
	}
	return Ogre::Vector3::ZERO;
}

float InfluenceMapSystem::GetLayerDebugCellValue(const std::string& layerName, int luaIndex, float threshold) const
{
	const Layer* layer = FindLayer(layerName);
	if (layer == nullptr || luaIndex <= 0)
		return 0.0f;

	const float safeThreshold = std::max(0.0f, threshold);
	int count = 0;
	for (int z = 0; z < m_height; ++z)
	{
		for (int x = 0; x < m_width; ++x)
		{
			const float value = layer->values[GetCellIndex(x, z)];
			if (std::fabs(value) < safeThreshold)
				continue;
			++count;
			if (count == luaIndex)
				return value;
		}
	}
	return 0.0f;
}

std::string InfluenceMapSystem::BuildDebugSummary() const
{
	std::ostringstream stream;
	stream << "[TacticalInfluenceSystem] configured=" << (m_stats.configured ? "true" : "false")
		<< " grid=" << m_stats.width << "x" << m_stats.height
		<< " layers=" << m_stats.layerCount
		<< " activeCells=" << m_stats.activeCellCount
		<< " sources=" << m_stats.sourceCount
		<< " writes=" << m_stats.cellWriteCount
		<< " spreadPasses=" << m_stats.spreadPassCount
		<< " queries=" << m_stats.queryCount
		<< " candidates=" << m_stats.lastQueryCandidateCount
		<< " best=" << FormatVec3(m_stats.lastBestPosition)
		<< " bestScore=" << std::fixed << std::setprecision(2) << m_stats.lastBestScore;
	return stream.str();
}

InfluenceMapSystem::Layer& InfluenceMapSystem::GetOrCreateLayer(const std::string& layerName)
{
	const std::string safeName = layerName.empty() ? "default" : layerName;
	Layer& layer = m_layers[safeName];
	ResizeLayer(layer);
	return layer;
}

const InfluenceMapSystem::Layer* InfluenceMapSystem::FindLayer(const std::string& layerName) const
{
	const std::string safeName = layerName.empty() ? "default" : layerName;
	std::unordered_map<std::string, Layer>::const_iterator found = m_layers.find(safeName);
	return found != m_layers.end() ? &found->second : nullptr;
}

void InfluenceMapSystem::EnsureConfigured()
{
	if (m_width <= 0 || m_height <= 0)
		Configure(kDefaultMinX, kDefaultMaxX, kDefaultMinZ, kDefaultMaxZ, kDefaultCellSize);
}

void InfluenceMapSystem::ResizeLayer(Layer& layer)
{
	const int totalCells = std::max(1, m_width * m_height);
	if (static_cast<int>(layer.values.size()) != totalCells)
		layer.values.assign(totalCells, 0.0f);
}

bool InfluenceMapSystem::WorldToCell(const Ogre::Vector3& position, int& outX, int& outZ) const
{
	if (m_width <= 0 || m_height <= 0 || m_cellSize <= 0.0f)
		return false;

	outX = ClampInt(static_cast<int>(std::floor((position.x - m_minX) / m_cellSize)), 0, m_width - 1);
	outZ = ClampInt(static_cast<int>(std::floor((position.z - m_minZ) / m_cellSize)), 0, m_height - 1);
	return true;
}

int InfluenceMapSystem::GetCellIndex(int x, int z) const
{
	return z * m_width + x;
}

Ogre::Vector3 InfluenceMapSystem::GetCellCenter(int x, int z) const
{
	return Ogre::Vector3(m_minX + (static_cast<float>(x) + 0.5f) * m_cellSize, 0.0f, m_minZ + (static_cast<float>(z) + 0.5f) * m_cellSize);
}

float InfluenceMapSystem::ReadCell(const Layer& layer, int x, int z) const
{
	return layer.values[GetCellIndex(x, z)];
}

void InfluenceMapSystem::RecalculateStats()
{
	int activeCells = 0;
	int sourceCount = 0;
	int cellWriteCount = 0;
	int spreadPassCount = 0;
	for (std::unordered_map<std::string, Layer>::const_iterator layerIter = m_layers.begin(); layerIter != m_layers.end(); ++layerIter)
	{
		const Layer& layer = layerIter->second;
		sourceCount += layer.sourceCount;
		cellWriteCount += layer.cellWriteCount;
		spreadPassCount += layer.spreadPassCount;
		for (std::vector<float>::const_iterator valueIter = layer.values.begin(); valueIter != layer.values.end(); ++valueIter)
		{
			if (std::fabs(*valueIter) >= kActiveCellThreshold)
				++activeCells;
		}
	}

	m_stats.configured = true;
	m_stats.width = m_width;
	m_stats.height = m_height;
	m_stats.totalCells = m_width * m_height;
	m_stats.layerCount = static_cast<int>(m_layers.size());
	m_stats.activeCellCount = activeCells;
	m_stats.sourceCount = sourceCount;
	m_stats.cellWriteCount = cellWriteCount;
	m_stats.spreadPassCount = spreadPassCount;
}
