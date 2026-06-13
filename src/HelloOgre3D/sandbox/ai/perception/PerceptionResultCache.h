#ifndef __PERCEPTION_RESULT_CACHE_H__
#define __PERCEPTION_RESULT_CACHE_H__

#include <string>

#include "OgreVector3.h"

// PerceptionResultCache: consolidates one perception tick's "current visible target +
// last-known position + meta" into one explicit struct, as the single source of truth
// for perception results. First slice = additive/observational: filled by
// AIController::TickPerception after the scan, from data already written to the
// blackboard / memory snapshot this frame (behavior unchanged). Later slices route
// HasEnemy / CanShootEnemy / Lua / BT to read this instead of recomputing, while
// keeping the existing blackboard keys compatible.
struct PerceptionResultCache
{
	PerceptionResultCache() { Reset(); }

	void Reset()
	{
		hasCurrentTarget = false;
		currentTargetId = -1;
		currentTargetPos = Ogre::Vector3::ZERO;
		currentDistance = 0.0f;
		currentConfidence = 0.0f;

		hasLastKnown = false;
		lastKnownTargetId = -1;
		lastKnownPos = Ogre::Vector3::ZERO;
		lastKnownConfidence = 0.0f;
		lastKnownAgeMs = 0;

		scannedThisTick = false;
		lastScanTimeMs = 0;
		scanCostMs = 0.0;
		candidateCount = 0;
		updateCount = 0;
		source.clear();
	}

	// Current visible target (from this frame's vision scan; hasCurrentTarget=false when none).
	bool hasCurrentTarget;
	int currentTargetId;
	Ogre::Vector3 currentTargetPos;
	float currentDistance;
	float currentConfidence;

	// Last-known position (from memory snapshot; retained and decayed after target is lost).
	bool hasLastKnown;
	int lastKnownTargetId;
	Ogre::Vector3 lastKnownPos;
	float lastKnownConfidence;
	int lastKnownAgeMs;

	// Meta.
	bool scannedThisTick;     // whether a scan actually ran this tick (gated by scanIntervalMs)
	long long lastScanTimeMs; // local timestamp of the most recent scan
	double scanCostMs;        // most recent scan cost (only valid when perf profiler is on, else 0)
	int candidateCount;       // candidate count (per-agent granularity TBD via spatial query; 0 this slice)
	int updateCount;          // cumulative update count
	std::string source;       // result source (e.g. "VisionSensor")
};

#endif // __PERCEPTION_RESULT_CACHE_H__
