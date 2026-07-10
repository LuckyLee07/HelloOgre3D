#include "ai/perception/HearingDangerSense.h"

#include <algorithm>
#include <cmath>

#include "ai/common/Blackboard.h"
#include "ai/team/TeamBlackboardService.h"
#include "components/ai/AIController.h"
#include "core/SandboxServices.h"
#include "objects/AgentObject.h"
#include "systems/manager/ObjectManager.h"
#include "systems/service/NavigationService.h"

namespace
{
	float Clamp01(float value)
	{
		return std::max(0.0f, std::min(1.0f, value));
	}

	Ogre::Vector3 Normalize2D(const Ogre::Vector3& value)
	{
		Ogre::Vector3 result(value.x, 0.0f, value.z);
		if (result.squaredLength() <= 0.0001f)
			return Ogre::Vector3::UNIT_Z;
		result.normalise();
		return result;
	}

	float DistanceToSegmentSq2D(const Ogre::Vector3& point, const Ogre::Vector3& start, const Ogre::Vector3& end)
	{
		const float vx = end.x - start.x;
		const float vz = end.z - start.z;
		const float wx = point.x - start.x;
		const float wz = point.z - start.z;
		const float lengthSq = vx * vx + vz * vz;
		float t = 0.0f;
		if (lengthSq > 0.0001f)
			t = Clamp01((wx * vx + wz * vz) / lengthSq);
		const float cx = start.x + vx * t;
		const float cz = start.z + vz * t;
		const float dx = point.x - cx;
		const float dz = point.z - cz;
		return dx * dx + dz * dz;
	}

	std::string MakeResponseKey(int agentId, int sourceId)
	{
		return std::to_string(agentId) + ":" + std::to_string(sourceId);
	}
}

HearingDangerSense::Config::Config()
	: enabled(false)
	, scanIntervalMs(100)
	, agentsPerTick(3)
	, responseCooldownMs(450)
	, dangerCooldownMs(350)
	, investigateStopDistance(5.0f)
	, escapeDistance(9.0f)
{
}

HearingDangerSense::Event::Event()
	: sourceId(-1)
	, sourceTeamId(-1)
	, targetId(-1)
	, position(Ogre::Vector3::ZERO)
	, impactPosition(Ogre::Vector3::ZERO)
	, timeMs(0)
	, ttlMs(1800)
	, hearingRadius(44.0f)
	, dangerRadius(13.0f)
{
}

HearingDangerSense::Stats::Stats()
	: enabled(0)
	, eventCount(0)
	, publishedEventCount(0)
	, prunedEventCount(0)
	, runCount(0)
	, skipCount(0)
	, agentCheckCount(0)
	, heardResponseCount(0)
	, dangerResponseCount(0)
	, investigationCount(0)
	, retreatFactApplyCount(0)
{
}

HearingDangerSense::HearingDangerSense()
	: m_currentTimeMs(0)
	, m_elapsedMs(0)
	, m_cursor(0)
	, m_publishedEventCount(0)
	, m_prunedEventCount(0)
	, m_runCount(0)
	, m_skipCount(0)
	, m_agentCheckCount(0)
	, m_heardResponseCount(0)
	, m_dangerResponseCount(0)
	, m_investigationCount(0)
	, m_retreatFactApplyCount(0)
{
	RefreshStats();
}

void HearingDangerSense::Configure(bool enabled, int scanIntervalMs, int agentsPerTick, int responseCooldownMs, int dangerCooldownMs, float investigateStopDistance, float escapeDistance)
{
	m_config.enabled = enabled;
	m_config.scanIntervalMs = std::max(1, scanIntervalMs);
	m_config.agentsPerTick = std::max(1, agentsPerTick);
	m_config.responseCooldownMs = std::max(0, responseCooldownMs);
	m_config.dangerCooldownMs = std::max(0, dangerCooldownMs);
	m_config.investigateStopDistance = std::max(0.0f, investigateStopDistance);
	m_config.escapeDistance = std::max(0.0f, escapeDistance);
	RefreshStats();
}

void HearingDangerSense::Clear()
{
	m_events.clear();
	m_lastHeardAt.clear();
	m_lastDangerAt.clear();
	m_currentTimeMs = 0;
	m_elapsedMs = 0;
	m_cursor = 0;
	m_publishedEventCount = 0;
	m_prunedEventCount = 0;
	m_runCount = 0;
	m_skipCount = 0;
	m_agentCheckCount = 0;
	m_heardResponseCount = 0;
	m_dangerResponseCount = 0;
	m_investigationCount = 0;
	m_retreatFactApplyCount = 0;
	RefreshStats();
}

bool HearingDangerSense::PublishEvent(int sourceId, int sourceTeamId, int targetId, const Ogre::Vector3& position, const Ogre::Vector3& impactPosition, int timeMs, int ttlMs, float hearingRadius, float dangerRadius)
{
	if (!m_config.enabled || sourceId < 0)
		return false;

	Event event;
	event.sourceId = sourceId;
	event.sourceTeamId = sourceTeamId;
	event.targetId = targetId;
	event.position = position;
	event.impactPosition = impactPosition;
	event.timeMs = std::max(0, timeMs);
	event.ttlMs = std::max(1, ttlMs);
	event.hearingRadius = std::max(0.0f, hearingRadius);
	event.dangerRadius = std::max(0.0f, dangerRadius);
	m_events.push_back(event);
	++m_publishedEventCount;
	RefreshStats();
	return true;
}

void HearingDangerSense::Update(const std::vector<AgentObject*>& agents, int deltaMs, ObjectManager* objectManager)
{
	m_currentTimeMs += std::max(0, deltaMs);
	m_prunedEventCount += PruneEvents();
	PruneCooldowns();

	if (!m_config.enabled)
	{
		RefreshStats();
		return;
	}

	m_elapsedMs += std::max(0, deltaMs);
	const int agentCount = static_cast<int>(agents.size());
	if (agentCount <= 0)
	{
		++m_skipCount;
		RefreshStats();
		return;
	}

	const bool forceFirstRun = m_runCount == 0;
	if (!forceFirstRun && m_elapsedMs < m_config.scanIntervalMs)
	{
		++m_skipCount;
		RefreshStats();
		return;
	}

	m_elapsedMs = 0;
	++m_runCount;
	const int agentsPerTick = std::max(1, std::min(agentCount, m_config.agentsPerTick));
	for (int index = 0; index < agentsPerTick; ++index)
	{
		const int agentIndex = (m_cursor + index) % agentCount;
		ApplyToAgent(agents[agentIndex], objectManager);
	}
	m_cursor = (m_cursor + agentsPerTick) % agentCount;
	m_agentCheckCount += agentsPerTick;
	RefreshStats();
}

void HearingDangerSense::ApplyToAgent(AgentObject* agent, ObjectManager* objectManager)
{
	if (agent == nullptr || agent->GetHealth() <= 0.0f)
		return;

	AIController* ai = agent->GetAIComponent();
	Blackboard* blackboard = ai != nullptr ? ai->GetBlackboard() : nullptr;
	if (blackboard == nullptr || blackboard->GetBool("perception.hasTarget", false))
		return;

	const int agentId = static_cast<int>(agent->GetObjId());
	const int agentTeamId = static_cast<int>(agent->GetTeamId());
	const Ogre::Vector3 agentPosition = agent->GetPosition();
	for (const Event& event : m_events)
	{
		if (event.sourceTeamId == agentTeamId)
			continue;

		if (event.hearingRadius > 0.0f)
		{
			const float distance = agentPosition.distance(event.position);
			if (distance <= event.hearingRadius)
			{
				const std::string key = MakeResponseKey(agentId, event.sourceId);
				std::unordered_map<std::string, long long>::iterator found = m_lastHeardAt.find(key);
				if (found == m_lastHeardAt.end() || m_currentTimeMs - found->second >= m_config.responseCooldownMs)
				{
					m_lastHeardAt[key] = m_currentTimeMs;
					const float confidence = std::max(0.05f, 1.0f - (distance / event.hearingRadius));
					const Ogre::Vector3 fromSound = Normalize2D(agentPosition - event.position);
					const Ogre::Vector3 investigatePosition = ProjectToNavigation(objectManager, event.position + Ogre::Vector3(fromSound.x * m_config.investigateStopDistance, 0.0f, fromSound.z * m_config.investigateStopDistance));
					blackboard->SetString("chapter8.sensoryIntent", "investigate_sound");
					blackboard->SetVec3("sense.heardSoundPos", event.position);
					blackboard->SetVec3("sense.heardInvestigatePos", investigatePosition);
					blackboard->SetObjectId("sense.heardSoundSourceId", event.sourceId);
					blackboard->SetInt("sense.heardSoundAgeMs", static_cast<int>(std::max<long long>(0, m_currentTimeMs - event.timeMs)));
					blackboard->SetFloat("sense.heardSoundConfidence", confidence);
					++m_heardResponseCount;
					++m_investigationCount;
				}
			}
		}

		if (event.dangerRadius > 0.0f)
		{
			const float dangerRadiusSq = event.dangerRadius * event.dangerRadius;
			const float distanceSq = DistanceToSegmentSq2D(agentPosition, event.position, event.impactPosition);
			if (distanceSq <= dangerRadiusSq)
			{
				const std::string key = MakeResponseKey(agentId, event.sourceId);
				std::unordered_map<std::string, long long>::iterator found = m_lastDangerAt.find(key);
				if (found == m_lastDangerAt.end() || m_currentTimeMs - found->second >= m_config.dangerCooldownMs)
				{
					m_lastDangerAt[key] = m_currentTimeMs;
					const float distance = std::sqrt(std::max(0.0f, distanceSq));
					const float dangerLevel = std::max(0.1f, 1.0f - (distance / event.dangerRadius));
					const Ogre::Vector3 away = Normalize2D(agentPosition - event.impactPosition);
					const Ogre::Vector3 escapePosition = ProjectToNavigation(objectManager, agentPosition + Ogre::Vector3(away.x * m_config.escapeDistance, 0.0f, away.z * m_config.escapeDistance));
					blackboard->SetString("chapter8.sensoryIntent", "evade_danger");
					blackboard->SetFloat("sense.dangerLevel", dangerLevel);
					blackboard->SetVec3("sense.dangerPos", event.impactPosition);
					blackboard->SetVec3("sense.dangerEscapePos", escapePosition);
					blackboard->SetObjectId("sense.dangerSourceId", event.sourceId);

					TeamBlackboardService* teamBlackboard = objectManager != nullptr ? objectManager->GetTeamBlackboardService() : nullptr;
					if (teamBlackboard != nullptr)
					{
						const std::string factKey = "retreat:" + std::to_string(agentId);
						teamBlackboard->RememberFact("RetreatPoint", agentTeamId, agentId, event.sourceId, escapePosition, m_currentTimeMs, dangerLevel, 0, event.ttlMs, factKey);
						if (teamBlackboard->writeBestTeamFactToBlackboard(agent, "RetreatPoint", "team.retreat.cpp", true))
							++m_retreatFactApplyCount;
					}
					++m_dangerResponseCount;
				}
			}
		}
	}
}

int HearingDangerSense::PruneEvents()
{
	int pruned = 0;
	for (std::vector<Event>::iterator iter = m_events.begin(); iter != m_events.end();)
	{
		if (m_currentTimeMs - iter->timeMs > iter->ttlMs)
		{
			iter = m_events.erase(iter);
			++pruned;
		}
		else
		{
			++iter;
		}
	}
	return pruned;
}

int HearingDangerSense::PruneCooldowns()
{
	int pruned = 0;
	const long long heardCooldownMs = std::max(0, m_config.responseCooldownMs);
	for (std::unordered_map<std::string, long long>::iterator iter = m_lastHeardAt.begin(); iter != m_lastHeardAt.end();)
	{
		if (m_currentTimeMs - iter->second > heardCooldownMs)
		{
			iter = m_lastHeardAt.erase(iter);
			++pruned;
		}
		else
		{
			++iter;
		}
	}

	const long long dangerCooldownMs = std::max(0, m_config.dangerCooldownMs);
	for (std::unordered_map<std::string, long long>::iterator iter = m_lastDangerAt.begin(); iter != m_lastDangerAt.end();)
	{
		if (m_currentTimeMs - iter->second > dangerCooldownMs)
		{
			iter = m_lastDangerAt.erase(iter);
			++pruned;
		}
		else
		{
			++iter;
		}
	}
	return pruned;
}

void HearingDangerSense::RefreshStats()
{
	m_stats.enabled = m_config.enabled ? 1 : 0;
	m_stats.eventCount = static_cast<int>(m_events.size());
	m_stats.publishedEventCount = m_publishedEventCount;
	m_stats.prunedEventCount = m_prunedEventCount;
	m_stats.runCount = m_runCount;
	m_stats.skipCount = m_skipCount;
	m_stats.agentCheckCount = m_agentCheckCount;
	m_stats.heardResponseCount = m_heardResponseCount;
	m_stats.dangerResponseCount = m_dangerResponseCount;
	m_stats.investigationCount = m_investigationCount;
	m_stats.retreatFactApplyCount = m_retreatFactApplyCount;
}

Ogre::Vector3 HearingDangerSense::ProjectToNavigation(ObjectManager* objectManager, const Ogre::Vector3& position) const
{
	if (objectManager == nullptr)
		return position;

	NavigationService* navigation = objectManager->GetSandboxServices().navigation;
	if (navigation == nullptr || navigation->GetNavigationMeshCount() <= 0)
		return position;
	return navigation->FindClosestPoint("default", position);
}
