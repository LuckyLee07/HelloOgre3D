#ifndef __INFLUENCE_MAP_SYSTEM_H__
#define __INFLUENCE_MAP_SYSTEM_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "OgreVector3.h"

class InfluenceMapSystem
{
public:
	struct Stats
	{
		Stats();

		bool configured;
		int width;
		int height;
		int totalCells;
		int layerCount;
		int activeCellCount;
		int sourceCount;
		int cellWriteCount;
		int queryCount;
		int spreadPassCount;
		int lastQueryCandidateCount;
		float lastBestScore;
		Ogre::Vector3 lastBestPosition;
	};

	InfluenceMapSystem();

	void Configure(float minX, float maxX, float minZ, float maxZ, float cellSize);
	void Clear();
	void ClearLayer(const std::string& layerName);
	void SetLayerOptions(const std::string& layerName, float falloff, float inertia);
	int AddPointSource(const std::string& layerName, const Ogre::Vector3& center, float strength);
	int AddRadialSource(const std::string& layerName, const Ogre::Vector3& center, float strength, float radius);
	int SpreadLayer(const std::string& layerName, int passCount);
	float SampleLayer(const std::string& layerName, const Ogre::Vector3& position) const;
	float ScorePosition(const Ogre::Vector3& position, float dangerWeight, float teamWeight, float objectiveWeight) const;
	Ogre::Vector3 FindBestPosition(const Ogre::Vector3& center, float radius, float step, float dangerWeight, float teamWeight, float objectiveWeight);

	const Stats& GetStats() const { return m_stats; }
	int GetLayerActiveCellCount(const std::string& layerName) const;
	int GetLayerCellWriteCount(const std::string& layerName) const;
	int GetLayerDebugCellCount(const std::string& layerName, float threshold, int maxCells) const;
	Ogre::Vector3 GetLayerDebugCellPosition(const std::string& layerName, int luaIndex, float threshold) const;
	float GetLayerDebugCellValue(const std::string& layerName, int luaIndex, float threshold) const;
	std::string BuildDebugSummary() const;

private:
	struct Layer
	{
		Layer();

		std::vector<float> values;
		int sourceCount;
		int cellWriteCount;
		int spreadPassCount;
		float falloff;
		float inertia;
	};

	Layer& GetOrCreateLayer(const std::string& layerName);
	const Layer* FindLayer(const std::string& layerName) const;
	void EnsureConfigured();
	void ResizeLayer(Layer& layer);
	bool WorldToCell(const Ogre::Vector3& position, int& outX, int& outZ) const;
	int GetCellIndex(int x, int z) const;
	Ogre::Vector3 GetCellCenter(int x, int z) const;
	float ReadCell(const Layer& layer, int x, int z) const;
	void RecalculateStats();

	float m_minX;
	float m_maxX;
	float m_minZ;
	float m_maxZ;
	float m_cellSize;
	int m_width;
	int m_height;
	std::unordered_map<std::string, Layer> m_layers;
	Stats m_stats;
};

#endif // __INFLUENCE_MAP_SYSTEM_H__
