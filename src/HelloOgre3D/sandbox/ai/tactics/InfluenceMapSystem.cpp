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

	// ===== AABB-triangle 重叠测试（Tomas Akenine-Moller，分离轴定理）=====
	// 直接端口自 HelloOgre3DX demo_framework/TriangleBoxIntersection.cpp，用于 navmesh 体素化。
	int planeBoxOverlap(float normal[3], float vert[3], float maxbox[3])
	{
		float vmin[3], vmax[3], v;
		for (int q = 0; q <= 2; q++)
		{
			v = vert[q];
			if (normal[q] > 0.0f) { vmin[q] = -maxbox[q] - v; vmax[q] = maxbox[q] - v; }
			else { vmin[q] = maxbox[q] - v; vmax[q] = -maxbox[q] - v; }
		}
		if ((normal[0] * vmin[0] + normal[1] * vmin[1] + normal[2] * vmin[2]) > 0.0f) return 0;
		if ((normal[0] * vmax[0] + normal[1] * vmax[1] + normal[2] * vmax[2]) >= 0.0f) return 1;
		return 0;
	}

#define IMS_X 0
#define IMS_Y 1
#define IMS_Z 2
#define IMS_FINDMINMAX(x0,x1,x2,vmin,vmax) \
	vmin = vmax = x0; \
	if (x1 < vmin) vmin = x1; if (x1 > vmax) vmax = x1; \
	if (x2 < vmin) vmin = x2; if (x2 > vmax) vmax = x2;
#define IMS_AXISTEST_X01(a,b,fa,fb) \
	p0 = a*v0[IMS_Y] - b*v0[IMS_Z]; p2 = a*v2[IMS_Y] - b*v2[IMS_Z]; \
	if (p0 < p2) { vmin = p0; vmax = p2; } else { vmin = p2; vmax = p0; } \
	rad = fa*boxhalfsize[IMS_Y] + fb*boxhalfsize[IMS_Z]; if (vmin > rad || vmax < -rad) return 0;
#define IMS_AXISTEST_X2(a,b,fa,fb) \
	p0 = a*v0[IMS_Y] - b*v0[IMS_Z]; p1 = a*v1[IMS_Y] - b*v1[IMS_Z]; \
	if (p0 < p1) { vmin = p0; vmax = p1; } else { vmin = p1; vmax = p0; } \
	rad = fa*boxhalfsize[IMS_Y] + fb*boxhalfsize[IMS_Z]; if (vmin > rad || vmax < -rad) return 0;
#define IMS_AXISTEST_Y02(a,b,fa,fb) \
	p0 = -a*v0[IMS_X] + b*v0[IMS_Z]; p2 = -a*v2[IMS_X] + b*v2[IMS_Z]; \
	if (p0 < p2) { vmin = p0; vmax = p2; } else { vmin = p2; vmax = p0; } \
	rad = fa*boxhalfsize[IMS_X] + fb*boxhalfsize[IMS_Z]; if (vmin > rad || vmax < -rad) return 0;
#define IMS_AXISTEST_Y1(a,b,fa,fb) \
	p0 = -a*v0[IMS_X] + b*v0[IMS_Z]; p1 = -a*v1[IMS_X] + b*v1[IMS_Z]; \
	if (p0 < p1) { vmin = p0; vmax = p1; } else { vmin = p1; vmax = p0; } \
	rad = fa*boxhalfsize[IMS_X] + fb*boxhalfsize[IMS_Z]; if (vmin > rad || vmax < -rad) return 0;
#define IMS_AXISTEST_Z12(a,b,fa,fb) \
	p1 = a*v1[IMS_X] - b*v1[IMS_Y]; p2 = a*v2[IMS_X] - b*v2[IMS_Y]; \
	if (p2 < p1) { vmin = p2; vmax = p1; } else { vmin = p1; vmax = p2; } \
	rad = fa*boxhalfsize[IMS_X] + fb*boxhalfsize[IMS_Y]; if (vmin > rad || vmax < -rad) return 0;
#define IMS_AXISTEST_Z0(a,b,fa,fb) \
	p0 = a*v0[IMS_X] - b*v0[IMS_Y]; p1 = a*v1[IMS_X] - b*v1[IMS_Y]; \
	if (p0 < p1) { vmin = p0; vmax = p1; } else { vmin = p1; vmax = p0; } \
	rad = fa*boxhalfsize[IMS_X] + fb*boxhalfsize[IMS_Y]; if (vmin > rad || vmax < -rad) return 0;

	int triBoxOverlap(float boxcenter[3], float boxhalfsize[3], float triverts[3][3])
	{
		float v0[3], v1[3], v2[3];
		float vmin, vmax, p0, p1, p2, rad, fex, fey, fez;
		float normal[3], e0[3], e1[3], e2[3];

		for (int i = 0; i < 3; ++i)
		{
			v0[i] = triverts[0][i] - boxcenter[i];
			v1[i] = triverts[1][i] - boxcenter[i];
			v2[i] = triverts[2][i] - boxcenter[i];
		}
		for (int i = 0; i < 3; ++i)
		{
			e0[i] = v1[i] - v0[i];
			e1[i] = v2[i] - v1[i];
			e2[i] = v0[i] - v2[i];
		}

		fex = fabsf(e0[IMS_X]); fey = fabsf(e0[IMS_Y]); fez = fabsf(e0[IMS_Z]);
		IMS_AXISTEST_X01(e0[IMS_Z], e0[IMS_Y], fez, fey);
		IMS_AXISTEST_Y02(e0[IMS_Z], e0[IMS_X], fez, fex);
		IMS_AXISTEST_Z12(e0[IMS_Y], e0[IMS_X], fey, fex);

		fex = fabsf(e1[IMS_X]); fey = fabsf(e1[IMS_Y]); fez = fabsf(e1[IMS_Z]);
		IMS_AXISTEST_X01(e1[IMS_Z], e1[IMS_Y], fez, fey);
		IMS_AXISTEST_Y02(e1[IMS_Z], e1[IMS_X], fez, fex);
		IMS_AXISTEST_Z0(e1[IMS_Y], e1[IMS_X], fey, fex);

		fex = fabsf(e2[IMS_X]); fey = fabsf(e2[IMS_Y]); fez = fabsf(e2[IMS_Z]);
		IMS_AXISTEST_X2(e2[IMS_Z], e2[IMS_Y], fez, fey);
		IMS_AXISTEST_Y1(e2[IMS_Z], e2[IMS_X], fez, fex);
		IMS_AXISTEST_Z12(e2[IMS_Y], e2[IMS_X], fey, fex);

		IMS_FINDMINMAX(v0[IMS_X], v1[IMS_X], v2[IMS_X], vmin, vmax);
		if (vmin > boxhalfsize[IMS_X] || vmax < -boxhalfsize[IMS_X]) return 0;
		IMS_FINDMINMAX(v0[IMS_Y], v1[IMS_Y], v2[IMS_Y], vmin, vmax);
		if (vmin > boxhalfsize[IMS_Y] || vmax < -boxhalfsize[IMS_Y]) return 0;
		IMS_FINDMINMAX(v0[IMS_Z], v1[IMS_Z], v2[IMS_Z], vmin, vmax);
		if (vmin > boxhalfsize[IMS_Z] || vmax < -boxhalfsize[IMS_Z]) return 0;

		normal[0] = e0[1] * e1[2] - e0[2] * e1[1];
		normal[1] = e0[2] * e1[0] - e0[0] * e1[2];
		normal[2] = e0[0] * e1[1] - e0[1] * e1[0];
		if (!planeBoxOverlap(normal, v0, boxhalfsize)) return 0;

		return 1;
	}

#undef IMS_X
#undef IMS_Y
#undef IMS_Z
#undef IMS_FINDMINMAX
#undef IMS_AXISTEST_X01
#undef IMS_AXISTEST_X2
#undef IMS_AXISTEST_Y02
#undef IMS_AXISTEST_Y1
#undef IMS_AXISTEST_Z12
#undef IMS_AXISTEST_Z0
}

InfluenceMapSystem::Stats::Stats()
	: configured(false)
	, width(0)
	, yCount(0)
	, height(0)
	, totalCells(0)
	, usedCells(0)
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
	, m_minY(0.0f)
	, m_maxY(0.0f)
	, m_cellSize(kDefaultCellSize)
	, m_cellHeight(0.0f)
	, m_width(0)
	, m_height(0)
	, m_yCount(1)
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
	m_minY = 0.0f;
	m_maxY = 0.0f;
	m_cellHeight = 0.0f;
	m_yCount = 1;
	m_cellSize = cellSize > 0.01f ? cellSize : kDefaultCellSize;
	m_width = std::max(1, static_cast<int>(std::floor((m_maxX - m_minX) / m_cellSize)) + 1);
	m_height = std::max(1, static_cast<int>(std::floor((m_maxZ - m_minZ) / m_cellSize)) + 1);

	AllocateGrid();
	// flat 模式：所有 cell 都视为 used（2D 行为）。
	std::fill(m_used.begin(), m_used.end(), static_cast<unsigned char>(1));

	RecalculateStats();
	m_stats.configured = true;
}

void InfluenceMapSystem::BuildFromNavMesh(
	const std::vector<float>& verts,
	const std::vector<int>& indices,
	float cellWidth,
	float cellHeight,
	const Ogre::Vector3& boundaryMinOffset,
	const Ogre::Vector3& boundaryMaxOffset)
{
	const int triCount = static_cast<int>(indices.size() / 3);
	if (verts.size() < 9 || triCount <= 0)
	{
		// 没有有效三角，退回默认 flat 网格。
		Configure(kDefaultMinX, kDefaultMaxX, kDefaultMinZ, kDefaultMaxZ, kDefaultCellSize);
		return;
	}

	const float cw = cellWidth > 0.01f ? cellWidth : kDefaultCellSize;
	const float ch = cellHeight > 0.01f ? cellHeight : cw;

	// 三角面 AABB。
	float aMinX = std::numeric_limits<float>::max();
	float aMinY = std::numeric_limits<float>::max();
	float aMinZ = std::numeric_limits<float>::max();
	float aMaxX = -std::numeric_limits<float>::max();
	float aMaxY = -std::numeric_limits<float>::max();
	float aMaxZ = -std::numeric_limits<float>::max();
	const int vertCount = static_cast<int>(verts.size() / 3);
	for (int i = 0; i < vertCount; ++i)
	{
		const float vx = verts[i * 3 + 0];
		const float vy = verts[i * 3 + 1];
		const float vz = verts[i * 3 + 2];
		aMinX = std::min(aMinX, vx); aMaxX = std::max(aMaxX, vx);
		aMinY = std::min(aMinY, vy); aMaxY = std::max(aMaxY, vy);
		aMinZ = std::min(aMinZ, vz); aMaxZ = std::max(aMaxZ, vz);
	}

	m_cellSize = cw;
	m_cellHeight = ch;
	m_minX = aMinX + boundaryMinOffset.x;
	m_minY = aMinY + boundaryMinOffset.y;
	m_minZ = aMinZ + boundaryMinOffset.z;
	m_maxX = aMaxX + boundaryMaxOffset.x + cw;
	m_maxY = aMaxY + boundaryMaxOffset.y + ch;
	m_maxZ = aMaxZ + boundaryMaxOffset.z + cw;

	m_width = std::max(1, static_cast<int>(std::ceil((m_maxX - m_minX) / m_cellSize)));
	m_yCount = std::max(1, static_cast<int>(std::ceil((m_maxY - m_minY) / m_cellHeight)));
	m_height = std::max(1, static_cast<int>(std::ceil((m_maxZ - m_minZ) / m_cellSize)));

	// 兜底：cell 数过大（多半是 navmesh AABB 含大平面导致），退回默认 flat，避免卡死。
	if (static_cast<long long>(m_width) * m_yCount * m_height > 3000000LL)
	{
		Configure(kDefaultMinX, kDefaultMaxX, kDefaultMinZ, kDefaultMaxZ, kDefaultCellSize);
		return;
	}

	AllocateGrid();

	// 体素化：逐三角，把与之重叠的 cell 标为 used（三角外层遍历，比逐 cell 测全部三角高效）。
	const float halfSize[3] = { m_cellSize * 0.5f, m_cellHeight * 0.5f, m_cellSize * 0.5f };
	for (int t = 0; t < triCount; ++t)
	{
		const int i0 = indices[t * 3 + 0];
		const int i1 = indices[t * 3 + 1];
		const int i2 = indices[t * 3 + 2];
		if (i0 < 0 || i1 < 0 || i2 < 0 || i0 >= vertCount || i1 >= vertCount || i2 >= vertCount)
			continue;

		float tri[3][3];
		const int ids[3] = { i0, i1, i2 };
		for (int c = 0; c < 3; ++c)
		{
			tri[c][0] = verts[ids[c] * 3 + 0];
			tri[c][1] = verts[ids[c] * 3 + 1];
			tri[c][2] = verts[ids[c] * 3 + 2];
		}
		const float triAvgY = (tri[0][1] + tri[1][1] + tri[2][1]) / 3.0f;

		// 三角 AABB → cell 范围。
		float tMinX = std::min(tri[0][0], std::min(tri[1][0], tri[2][0]));
		float tMaxX = std::max(tri[0][0], std::max(tri[1][0], tri[2][0]));
		float tMinY = std::min(tri[0][1], std::min(tri[1][1], tri[2][1]));
		float tMaxY = std::max(tri[0][1], std::max(tri[1][1], tri[2][1]));
		float tMinZ = std::min(tri[0][2], std::min(tri[1][2], tri[2][2]));
		float tMaxZ = std::max(tri[0][2], std::max(tri[1][2], tri[2][2]));

		const int cx0 = ClampInt(static_cast<int>(std::floor((tMinX - m_minX) / m_cellSize)), 0, m_width - 1);
		const int cx1 = ClampInt(static_cast<int>(std::floor((tMaxX - m_minX) / m_cellSize)), 0, m_width - 1);
		const int cy0 = ClampInt(static_cast<int>(std::floor((tMinY - m_minY) / m_cellHeight)), 0, m_yCount - 1);
		const int cy1 = ClampInt(static_cast<int>(std::floor((tMaxY - m_minY) / m_cellHeight)), 0, m_yCount - 1);
		const int cz0 = ClampInt(static_cast<int>(std::floor((tMinZ - m_minZ) / m_cellSize)), 0, m_height - 1);
		const int cz1 = ClampInt(static_cast<int>(std::floor((tMaxZ - m_minZ) / m_cellSize)), 0, m_height - 1);

		for (int cz = cz0; cz <= cz1; ++cz)
		{
			for (int cy = cy0; cy <= cy1; ++cy)
			{
				for (int cx = cx0; cx <= cx1; ++cx)
				{
					const int idx = GetCellIndex(cx, cy, cz);
					if (m_used[idx])
						continue;
					float boxCenter[3] = {
						m_minX + (static_cast<float>(cx) + 0.5f) * m_cellSize,
						m_minY + (static_cast<float>(cy) + 0.5f) * m_cellHeight,
						m_minZ + (static_cast<float>(cz) + 0.5f) * m_cellSize
					};
					if (triBoxOverlap(boxCenter, const_cast<float*>(halfSize), tri))
					{
						m_used[idx] = 1;
						// 记录该 cell 的真实表面高度（标记三角的平均 Y，clamp 到 cell 的 Y 范围内），用于贴地绘制。
						const float cellBottom = m_minY + static_cast<float>(cy) * m_cellHeight;
						m_surfaceY[idx] = ClampFloat(triAvgY, cellBottom, cellBottom + m_cellHeight);
					}
				}
			}
		}
	}

	RecalculateStats();
	m_stats.configured = true;
}

void InfluenceMapSystem::Clear()
{
	m_layers.clear();
	m_stats = Stats();
	RecalculateStats();
	m_stats.configured = true;
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
	int x = 0, y = 0, z = 0;
	if (!WorldToCell(center, x, y, z))
		return 0;

	int sy = 0;
	if (!FindSurfaceCellY(x, z, center.y, sy))
		return 0;

	layer.values[GetCellIndex(x, sy, z)] += strength;
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
			// 找该列贴在可走面、离源最近的 cell（保证落在源所在楼层、跟随地形）。
			int sy = 0;
			if (!FindSurfaceCellY(x, z, center.y, sy))
				continue;

			const Ogre::Vector3 cellCenter = GetCellCenter(x, sy, z);
			const float dx = cellCenter.x - center.x;
			const float dz = cellCenter.z - center.z;
			const float distSq = dx * dx + dz * dz;
			if (distSq > radiusSq)
				continue;

			const float distance = std::sqrt(distSq);
			const float contribution = strength * (1.0f - distance / sourceRadius);
			if (std::fabs(contribution) <= 0.0001f)
				continue;

			layer.values[GetCellIndex(x, sy, z)] += contribution;
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
			for (int y = 0; y < m_yCount; ++y)
			{
				for (int x = 0; x < m_width; ++x)
				{
					if (!IsUsed(x, y, z))
						continue;
					const int index = GetCellIndex(x, y, z);
					float strongest = previous[index];
					for (int dz = -1; dz <= 1; ++dz)
					{
						for (int dy = -1; dy <= 1; ++dy)
						{
							for (int dx = -1; dx <= 1; ++dx)
							{
								if (dx == 0 && dy == 0 && dz == 0)
									continue;
								const int nx = x + dx;
								const int ny = y + dy;
								const int nz = z + dz;
								if (!IsUsed(nx, ny, nz))
									continue;

								const float neighbor = previous[GetCellIndex(nx, ny, nz)];
								const float distanceScale = 1.0f / std::sqrt(static_cast<float>(dx * dx + dy * dy + dz * dz));
								const float propagated = neighbor * falloff * distanceScale;
								if (std::fabs(propagated) > std::fabs(strongest))
									strongest = propagated;
							}
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

	int x = 0, y = 0, z = 0;
	if (!WorldToCell(position, x, y, z))
		return 0.0f;
	int sy = 0;
	if (!FindSurfaceCellY(x, z, position.y, sy))
		return 0.0f;
	return layer->values[GetCellIndex(x, sy, z)];
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

float InfluenceMapSystem::GetLayerCellValue(const std::string& layerName, int x, int z) const
{
	const Layer* layer = FindLayer(layerName);
	if (layer == nullptr || x < 0 || z < 0 || x >= m_width || z >= m_height)
		return 0.0f;
	return layer->values[GetCellIndex(x, 0, z)];
}

int InfluenceMapSystem::GetLayerDebugCellCount(const std::string& layerName, float threshold, int maxCells) const
{
	const Layer* layer = FindLayer(layerName);
	if (layer == nullptr)
		return 0;

	const float safeThreshold = std::max(0.0f, threshold);
	const int limit = maxCells > 0 ? maxCells : std::numeric_limits<int>::max();
	int count = 0;
	for (int z = 0; z < m_height; ++z)
	{
		for (int y = 0; y < m_yCount; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				if (!IsUsed(x, y, z))
					continue;
				if (std::fabs(layer->values[GetCellIndex(x, y, z)]) < safeThreshold)
					continue;
				++count;
				if (count >= limit)
					return count;
			}
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
		for (int y = 0; y < m_yCount; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				if (!IsUsed(x, y, z))
					continue;
				if (std::fabs(layer->values[GetCellIndex(x, y, z)]) < safeThreshold)
					continue;
				++count;
				if (count == luaIndex)
					return GetCellCenter(x, y, z);
			}
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
		for (int y = 0; y < m_yCount; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				if (!IsUsed(x, y, z))
					continue;
				const float value = layer->values[GetCellIndex(x, y, z)];
				if (std::fabs(value) < safeThreshold)
					continue;
				++count;
				if (count == luaIndex)
					return value;
			}
		}
	}
	return 0.0f;
}

void InfluenceMapSystem::CollectDebugCells(const std::string& layerName, float threshold, int maxCells, bool drawNeutral,
	std::vector<Ogre::Vector3>& outPositions, std::vector<float>& outValues) const
{
	outPositions.clear();
	outValues.clear();
	const Layer* layer = FindLayer(layerName);
	if (layer == nullptr)
		return;

	const float safeThreshold = std::max(0.0f, threshold);
	const int limit = maxCells > 0 ? maxCells : std::numeric_limits<int>::max();
	for (int z = 0; z < m_height; ++z)
	{
		for (int y = 0; y < m_yCount; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				if (!IsUsed(x, y, z))
					continue;
				const int idx = GetCellIndex(x, y, z);
				const float value = layer->values[idx];
				if (!drawNeutral && std::fabs(value) < safeThreshold)
					continue;
				Ogre::Vector3 pos = GetCellCenter(x, y, z);
				// 对齐 chapter-9：画在 cell 的“底面”（minY + y*cellHeight），而非 cell 中心或 navmesh 表面。
				// 网格 Y 对齐到最低可走面 + cellHeight 与楼层间距匹配时，cell 底面正好落在该楼层地面上 → 贴地。
				pos.y = m_minY + static_cast<float>(y) * m_cellHeight;
				outPositions.push_back(pos);
				outValues.push_back(value);
				if (static_cast<int>(outPositions.size()) >= limit)
					return;
			}
		}
	}
}

std::string InfluenceMapSystem::BuildDebugSummary() const
{
	std::ostringstream stream;
	stream << "[TacticalInfluenceSystem] configured=" << (m_stats.configured ? "true" : "false")
		<< " grid=" << m_stats.width << "x" << m_stats.yCount << "x" << m_stats.height
		<< " used=" << m_stats.usedCells
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
	if (m_width <= 0 || m_height <= 0 || m_yCount <= 0)
		Configure(kDefaultMinX, kDefaultMaxX, kDefaultMinZ, kDefaultMaxZ, kDefaultCellSize);
}

void InfluenceMapSystem::AllocateGrid()
{
	const int totalCells = std::max(1, m_width * m_yCount * m_height);
	m_used.assign(totalCells, static_cast<unsigned char>(0));
	m_surfaceY.assign(totalCells, 0.0f);
	for (std::unordered_map<std::string, Layer>::iterator iter = m_layers.begin(); iter != m_layers.end(); ++iter)
		ResizeLayer(iter->second);
}

void InfluenceMapSystem::ResizeLayer(Layer& layer)
{
	const int totalCells = std::max(1, m_width * m_yCount * m_height);
	if (static_cast<int>(layer.values.size()) != totalCells)
		layer.values.assign(totalCells, 0.0f);
}

bool InfluenceMapSystem::WorldToCell(const Ogre::Vector3& position, int& outX, int& outY, int& outZ) const
{
	if (m_width <= 0 || m_height <= 0 || m_yCount <= 0 || m_cellSize <= 0.0f)
		return false;

	outX = ClampInt(static_cast<int>(std::floor((position.x - m_minX) / m_cellSize)), 0, m_width - 1);
	outZ = ClampInt(static_cast<int>(std::floor((position.z - m_minZ) / m_cellSize)), 0, m_height - 1);
	if (m_yCount <= 1 || m_cellHeight <= 0.0f)
		outY = 0;
	else
		outY = ClampInt(static_cast<int>(std::floor((position.y - m_minY) / m_cellHeight)), 0, m_yCount - 1);
	return true;
}

int InfluenceMapSystem::GetCellIndex(int x, int y, int z) const
{
	return x + y * m_width + z * m_width * m_yCount;
}

bool InfluenceMapSystem::IsUsed(int x, int y, int z) const
{
	if (x < 0 || x >= m_width || y < 0 || y >= m_yCount || z < 0 || z >= m_height)
		return false;
	const int idx = GetCellIndex(x, y, z);
	return idx >= 0 && idx < static_cast<int>(m_used.size()) && m_used[idx] != 0;
}

bool InfluenceMapSystem::FindSurfaceCellY(int x, int z, float targetY, int& outY) const
{
	if (x < 0 || x >= m_width || z < 0 || z >= m_height)
		return false;

	if (m_yCount <= 1)
	{
		if (IsUsed(x, 0, z)) { outY = 0; return true; }
		return false;
	}

	int best = -1;
	float bestDist = std::numeric_limits<float>::max();
	for (int y = 0; y < m_yCount; ++y)
	{
		if (!IsUsed(x, y, z))
			continue;
		const float cy = m_minY + (static_cast<float>(y) + 0.5f) * m_cellHeight;
		const float d = std::fabs(cy - targetY);
		if (d < bestDist)
		{
			bestDist = d;
			best = y;
		}
	}
	if (best >= 0)
	{
		outY = best;
		return true;
	}
	return false;
}

Ogre::Vector3 InfluenceMapSystem::GetCellCenter(int x, int y, int z) const
{
	return Ogre::Vector3(
		m_minX + (static_cast<float>(x) + 0.5f) * m_cellSize,
		m_minY + (static_cast<float>(y) + 0.5f) * m_cellHeight,
		m_minZ + (static_cast<float>(z) + 0.5f) * m_cellSize);
}

float InfluenceMapSystem::ReadCell(const Layer& layer, int x, int y, int z) const
{
	return layer.values[GetCellIndex(x, y, z)];
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

	int usedCells = 0;
	for (std::vector<unsigned char>::const_iterator usedIter = m_used.begin(); usedIter != m_used.end(); ++usedIter)
	{
		if (*usedIter != 0)
			++usedCells;
	}

	m_stats.configured = true;
	m_stats.width = m_width;
	m_stats.yCount = m_yCount;
	m_stats.height = m_height;
	m_stats.totalCells = m_width * m_yCount * m_height;
	m_stats.usedCells = usedCells;
	m_stats.layerCount = static_cast<int>(m_layers.size());
	m_stats.activeCellCount = activeCells;
	m_stats.sourceCount = sourceCount;
	m_stats.cellWriteCount = cellWriteCount;
	m_stats.spreadPassCount = spreadPassCount;
}
