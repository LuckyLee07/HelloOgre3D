#ifndef __INFLUENCE_MAP_SYSTEM_H__
#define __INFLUENCE_MAP_SYSTEM_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "OgreVector3.h"

// 战术影响图系统。
// 支持两种建图：
//   - Configure(): 2D 平面网格（yCount=1，所有 cell 都是 used）——保留旧行为/旧 sample。
//   - BuildFromNavMesh(): 3D 网格，把 navmesh 三角面体素化，只有贴在可走面上的 cell 标记为 used，
//     这样影响图天然贴合多层地形，并且不同楼层（同 X-Z 不同 Y）落在不同 cell（楼层分离）。
// 对齐 HelloOgre3DX Chapter 9 的 3D InfluenceMap 设计。
class InfluenceMapSystem
{
public:
	struct Stats
	{
		Stats();

		bool configured;
		int width;     // X 方向 cell 数
		int yCount;    // Y 方向 cell 数（2D flat 时为 1）
		int height;    // Z 方向 cell 数
		int totalCells;
		int usedCells; // 标记为 used（贴在可走面上）的 cell 数
		int layerCount;
		int activeCellCount;
		int sourceCount;
		int cellWriteCount;
		int queryCount;
		int spreadPassCount;
		int dirtyRegionCount;
		int dirtyCellCount;
		int lastSpreadRegionCellCount;
		bool spreadLimitedByDirtyRegion;
		int lastQueryCandidateCount;
		int maxQueryCandidates;
		int lastQueryCandidateLimit;
		bool lastQueryCapped;
		float lastBestScore;
		Ogre::Vector3 lastBestPosition;
	};

	InfluenceMapSystem();

	// 2D 平面网格（yCount=1，所有 cell used）。
	void Configure(float minX, float maxX, float minZ, float maxZ, float cellSize);

	// 3D 网格：从 navmesh 三角面（verts 为 xyz 连续数组，indices 每 3 个一个三角）体素化建图。
	// 网格包围盒 = 三角 AABB + 偏移；cellWidth 用于 X/Z，cellHeight 用于 Y。
	void BuildFromNavMesh(
		const std::vector<float>& verts,
		const std::vector<int>& indices,
		float cellWidth,
		float cellHeight,
		const Ogre::Vector3& boundaryMinOffset,
		const Ogre::Vector3& boundaryMaxOffset);

	void Clear();
	void ClearLayer(const std::string& layerName);
	void SetLayerOptions(const std::string& layerName, float falloff, float inertia);
	int AddPointSource(const std::string& layerName, const Ogre::Vector3& center, float strength);
	int AddRadialSource(const std::string& layerName, const Ogre::Vector3& center, float strength, float radius);
	int SpreadLayer(const std::string& layerName, int passCount);
	void SetMaxQueryCandidates(int maxCandidates);
	int GetMaxQueryCandidates() const { return m_maxQueryCandidates; }
	float SampleLayer(const std::string& layerName, const Ogre::Vector3& position) const;
	float ScorePosition(const Ogre::Vector3& position, float dangerWeight, float teamWeight, float objectiveWeight) const;
	Ogre::Vector3 FindBestPosition(const Ogre::Vector3& center, float radius, float step, float dangerWeight, float teamWeight, float objectiveWeight);
	float ScorePositionLayers(const Ogre::Vector3& position, float dangerWeight, float teamWeight, float objectiveWeight, float supportWeight, float coverWeight, float crowdWeight) const;
	Ogre::Vector3 FindBestPositionLayers(const Ogre::Vector3& center, float radius, float step, float dangerWeight, float teamWeight, float objectiveWeight, float supportWeight, float coverWeight, float crowdWeight);

	const Stats& GetStats() const { return m_stats; }
	int GetWidth() const { return m_width; }       // X 方向 cell 数
	int GetHeight() const { return m_height; }     // Z 方向 cell 数
	int GetYCount() const { return m_yCount; }     // Y 方向 cell 数
	float GetCellSize() const { return m_cellSize; }
	float GetCellHeight() const { return m_cellHeight; }
	bool Is3D() const { return m_yCount > 1; }

	// 取 cell 真实中心（含 Y）。
	Ogre::Vector3 GetCellCenter(int x, int y, int z) const;
	// 2D 兼容重载：默认 y 层 0（旧绘制路径在 flat 模式下使用）。
	Ogre::Vector3 GetCellCenter(int x, int z) const { return GetCellCenter(x, 0, z); }

	float GetLayerCellValue(const std::string& layerName, int x, int z) const; // 2D 兼容（y=0）
	int GetLayerActiveCellCount(const std::string& layerName) const;
	int GetLayerCellWriteCount(const std::string& layerName) const;

	// debug cell 遍历：遍历 used 且 |value|>=threshold 的 cell。
	int GetLayerDebugCellCount(const std::string& layerName, float threshold, int maxCells) const;
	Ogre::Vector3 GetLayerDebugCellPosition(const std::string& layerName, int luaIndex, float threshold) const;
	float GetLayerDebugCellValue(const std::string& layerName, int luaIndex, float threshold) const;

	// 一次性收集某层用于绘制的 cell：只含 used cell，位置为旧版 cell 底面中心，值为该 cell 的 layer 值。
	// drawNeutral=false 时跳过 |value|<threshold 的 cell；maxCells>0 时限制数量。
	void CollectDebugCells(const std::string& layerName, float threshold, int maxCells, bool drawNeutral,
		std::vector<Ogre::Vector3>& outPositions, std::vector<float>& outValues) const;

	std::string BuildDebugSummary() const;

private:
	struct CellRegion
	{
		CellRegion();

		bool valid;
		int minX;
		int maxX;
		int minY;
		int maxY;
		int minZ;
		int maxZ;
	};

	struct Layer
	{
		Layer();

		std::vector<float> values;
		int sourceCount;
		int cellWriteCount;
		int spreadPassCount;
		float falloff;
		float inertia;
		CellRegion dirtyRegion;
		int lastSpreadRegionCellCount;
		bool lastSpreadLimitedByDirtyRegion;
	};

	Layer& GetOrCreateLayer(const std::string& layerName);
	const Layer* FindLayer(const std::string& layerName) const;
	void EnsureConfigured();
	void ResizeLayer(Layer& layer);
	void AllocateGrid();
	bool WorldToCell(const Ogre::Vector3& position, int& outX, int& outY, int& outZ) const;
	int GetCellIndex(int x, int y, int z) const;
	bool IsUsed(int x, int y, int z) const;
	// 在 (x,z) 列中找出离 targetY 最近的 used cell 的 y 层；2D flat 时恒为 0。
	bool FindSurfaceCellY(int x, int z, float targetY, int& outY) const;
	float ReadCell(const Layer& layer, int x, int y, int z) const;
	void MarkDirtyCell(Layer& layer, int x, int y, int z);
	CellRegion ExpandRegion(const CellRegion& region, int radius) const;
	int CountUsedCellsInRegion(const CellRegion& region) const;
	void RecalculateStats();

	float m_minX;
	float m_maxX;
	float m_minZ;
	float m_maxZ;
	float m_minY;
	float m_maxY;
	float m_cellSize;    // X/Z cell 宽
	float m_cellHeight;  // Y cell 高
	int m_width;         // X
	int m_height;        // Z
	int m_yCount;        // Y
	int m_maxQueryCandidates;
	std::vector<unsigned char> m_used; // 每 cell 一个标记，1=贴在可走面
	std::unordered_map<std::string, Layer> m_layers;
	Stats m_stats;
};

#endif // __INFLUENCE_MAP_SYSTEM_H__
