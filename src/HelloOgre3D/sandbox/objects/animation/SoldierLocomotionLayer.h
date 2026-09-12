#ifndef HELLO_SOLDIER_LOCOMOTION_LAYER_H
#define HELLO_SOLDIER_LOCOMOTION_LAYER_H

#include "OgreAnimationState.h"
#include "OgreBone.h"
#include "OgreEntity.h"
#include "OgreSkeletonInstance.h"
#include "OgreVector3.h"
#include "LogSystem.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>

inline bool SoldierAnimationTraceEnabled()
{
	static const bool enabled = []() {
		const char* value = std::getenv("HELLO_ANIMATION_TRACE");
		return value != nullptr && std::strcmp(value, "1") == 0;
	}();
	return enabled;
}

// A presentation-only layer over the existing ASM. Restore its state before the
// next simulation tick so gait sampling never changes action clocks or notifies.
// All Ogre pointers are borrowed. Release() is called while the entity is alive;
// destruction itself never dereferences them (the body may have been replaced).
class SoldierLocomotionLayer
{
public:
	explicit SoldierLocomotionLayer(Ogre::Entity* entity) : m_entity(entity)
	{
		m_saved.reserve(8);
		m_managedMasks.reserve(12);
		if (entity == nullptr || !entity->hasSkeleton()) return;
		Ogre::SkeletonInstance* skeleton = entity->getSkeleton();
		if (!skeleton->hasBone("b_Hips") || !skeleton->hasBone("b_Spine") ||
			!skeleton->hasBone("b_LeftFoot") || !skeleton->hasBone("b_RightFoot")) return;
		const char* names[] = {"stand_run_forward_aim", "stand_run_backward_aim",
			"crouch_left_aim", "crouch_right_aim", "crouch_forward_aim",
			"crouch_backward_aim", "stand_idle_aim", "crouch_idle_aim"};
		for (const char* name : names)
			if (!entity->hasAnimationState(name)) return;
		m_upper.resize(skeleton->getNumBones(), false);
		m_identityMask.resize(skeleton->getNumBones(), 1.0f);
		m_workMask.resize(skeleton->getNumBones(), 0.0f);
		Ogre::Bone* spine = skeleton->getBone("b_Spine");
		m_spine = spine;
		for (unsigned short index = 0; index < skeleton->getNumBones(); ++index)
		{
			Ogre::Bone* bone = skeleton->getBone(index);
			for (Ogre::Node* node = bone; node != nullptr; node = node->getParent())
			{
				if (node == spine) { m_upper[bone->getHandle()] = true; break; }
			}
		}
		m_valid = true;
	}

	void Restore()
	{
		if (!m_active) return;
		// Return the attachment seam to animation ownership before sampling the next action.
		m_spine->setManuallyControlled(m_spineWasManual);
		m_spine->setOrientation(m_originalSpineOrientation);
		for (SavedState& saved : m_saved)
		{
			saved.state->_setBlendMaskData(saved.originalMask.empty() ?
				m_identityMask.data() : saved.originalMask.data());
			saved.state->setWeight(saved.weight);
			saved.state->setLoop(saved.loop);
			saved.state->setTimePosition(saved.time);
			saved.state->setEnabled(saved.enabled);
		}
		m_entity->getSkeleton()->setBlendMode(m_previousBlendMode);
		m_saved.clear();
		m_active = false;
	}

	void Release()
	{
		Restore();
		for (Ogre::AnimationState* state : m_managedMasks) state->destroyBlendMask();
		m_managedMasks.clear();
		m_entity = nullptr;
		m_spine = nullptr;
		m_valid = false;
	}

	void Apply(const Ogre::Vector3& velocity, Ogre::Vector3 forward, bool crouched,
		bool allowed, int deltaMs, unsigned int objectId, const std::string& upperState)
	{
		const float speed = std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
		m_traceMs += std::max(0, deltaMs);
		if (!m_valid || !allowed)
		{
			Trace(objectId, speed, upperState, false, "none", "none", 0, 0);
			return;
		}
		forward.y = 0;
		if (forward.squaredLength() < 0.0001f) forward = Ogre::Vector3::UNIT_Z;
		forward.normalise();
		const Ogre::Vector3 right = forward.crossProduct(Ogre::Vector3::UNIT_Y);
		const float along = velocity.dotProduct(forward);
		const float sideways = velocity.dotProduct(right);
		const float sum = std::abs(along) + std::abs(sideways);
		const float dt = std::max(0, deltaMs) * 0.001f;
		const float response = 1.0f - std::exp(-dt / 0.10f);
		const float targetMoving = speed > 0.03f ? std::min(1.0f, speed / 0.65f) : 0.0f;
		m_movingWeight += (targetMoving - m_movingWeight) * response;
		m_crouchWeight += ((crouched ? 1.0f : 0.0f) - m_crouchWeight) * response;
		if (sum > 0.03f)
		{
			const float target[4] = {std::max(0.0f, along) / sum, std::max(0.0f, -along) / sum,
				std::max(0.0f, -sideways) / sum, std::max(0.0f, sideways) / sum};
			for (int i = 0; i < 4; ++i) m_direction[i] += (target[i] - m_direction[i]) * response;
		}
		const float longitudinalWeight = m_direction[0] + m_direction[1];
		const float lateralWeight = m_direction[2] + m_direction[3];
		const float movingWeight = m_movingWeight;
		const char* longitudinal = crouched ? (along < 0 ? "crouch_backward_aim" : "crouch_forward_aim") :
			(along < 0 ? "stand_run_backward_aim" : "stand_run_forward_aim");
		const char* lateral = sideways < 0 ? "crouch_left_aim" : "crouch_right_aim";
		// Common phase survives starts, stops and sign changes. Cycle distances
		// remain 2.25 m for standing run and 1.375 m for the authored tactical gait.
		const float stride = longitudinalWeight * (2.25f * (1.0f - m_crouchWeight) + 1.375f * m_crouchWeight) + lateralWeight * 1.375f;
		if (speed > 0.03f) m_phase = std::fmod(m_phase + dt * speed / stride, 1.0f);

		Ogre::SkeletonInstance* skeleton = m_entity->getSkeleton();
		// Preserve the action's model-space upper-body facing. A local spine mask
		// alone would inherit the sidestep clip's hip yaw and turn the gun sideways.
		skeleton->setAnimationState(*m_entity->getAllAnimationStates());
		skeleton->_updateTransforms();
		const Ogre::Quaternion actionSpineOrientation = m_spine->_getDerivedOrientation();
		m_originalSpineOrientation = m_spine->getOrientation();
		m_spineWasManual = m_spine->isManuallyControlled();
		m_previousBlendMode = skeleton->getBlendMode();
		m_active = true;
		Ogre::AnimationStateIterator states = m_entity->getAllAnimationStates()->getAnimationStateIterator();
		while (states.hasMoreElements())
		{
			Ogre::AnimationState* state = states.getNext();
			if (state->getEnabled()) Capture(state);
		}
		const float stand = 1.0f - m_crouchWeight;
		AddLower("stand_idle_aim", (1.0f - movingWeight) * stand, 0.0f);
		AddLower("crouch_idle_aim", (1.0f - movingWeight) * m_crouchWeight, 0.0f);
		AddLower("stand_run_forward_aim", movingWeight * m_direction[0] * stand, m_phase);
		AddLower("stand_run_backward_aim", movingWeight * m_direction[1] * stand, m_phase);
		AddLower("crouch_forward_aim", movingWeight * m_direction[0] * m_crouchWeight, m_phase);
		AddLower("crouch_backward_aim", movingWeight * m_direction[1] * m_crouchWeight, m_phase);
		AddLower("crouch_left_aim", movingWeight * m_direction[2], m_phase);
		AddLower("crouch_right_aim", movingWeight * m_direction[3], m_phase);

		// Ogre's AVERAGE mode normalises the sum of whole-state weights, ignoring
		// masks. CUMULATIVE is required; each bone still has a total weight of one.
		m_entity->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);
		for (SavedState& saved : m_saved)
		{
			for (size_t bone = 0; bone < m_upper.size(); ++bone)
				m_workMask[bone] = m_upper[bone] ? saved.upperWeight : saved.lowerWeight;
			saved.state->_setBlendMaskData(m_workMask.data());
			saved.state->setWeight(1.0f);
			saved.state->setEnabled(saved.upperWeight + saved.lowerWeight > 0.0001f);
			if (saved.lowerWeight > 0)
			{
				saved.state->setLoop(true);
				saved.state->setTimePosition(saved.lowerTime);
			}
		}
		// Keep the authored legs and pelvis, compensating only their inherited
		// rotation at the upper-body seam. Pelvis translation still supplies gait bob.
		skeleton->setAnimationState(*m_entity->getAllAnimationStates());
		skeleton->_updateTransforms();
		const Ogre::Node* parent = m_spine->getParent();
		// Ogre manual bones skip reset but still receive animation tracks: mask
		// this sampled seam out as well, so repeated pose evaluation is idempotent.
		for (SavedState& saved : m_saved)
			saved.state->setBlendMaskEntry(m_spine->getHandle(), 0.0f);
		m_spine->setManuallyControlled(true);
		m_spine->setOrientation(parent != nullptr ?
			parent->_getDerivedOrientation().Inverse() * actionSpineOrientation : actionSpineOrientation);
		skeleton->_updateTransforms();
		// Manual ownership lasts until Restore, including repeated attachment-pose
		// and renderer evaluations of this same frame's masked animation states.
		Trace(objectId, speed, upperState, true, longitudinal, lateral,
			movingWeight * longitudinalWeight, movingWeight * lateralWeight);
	}

private:
	struct SavedState
	{
		Ogre::AnimationState* state;
		float time, weight;
		bool enabled, loop;
		float upperWeight, lowerWeight, lowerTime;
		Ogre::AnimationState::BoneBlendMask originalMask;
	};

	SavedState& Capture(Ogre::AnimationState* state)
	{
		for (SavedState& saved : m_saved) if (saved.state == state) return saved;
		SavedState saved;
		saved.state = state;
		saved.time = state->getTimePosition();
		saved.weight = state->getWeight();
		saved.enabled = state->getEnabled();
		saved.loop = state->getLoop();
		saved.upperWeight = saved.enabled ? saved.weight : 0.0f;
		saved.lowerWeight = saved.lowerTime = 0;
		if (!state->hasBlendMask())
		{
			state->createBlendMask(m_upper.size(), 1.0f);
			m_managedMasks.push_back(state);
		}
		else if (std::find(m_managedMasks.begin(), m_managedMasks.end(), state) == m_managedMasks.end())
			saved.originalMask = *state->getBlendMask();
		m_saved.push_back(std::move(saved));
		return m_saved.back();
	}

	void AddLower(const char* name, float weight, float phase)
	{
		if (weight <= 0.0001f) return;
		Ogre::AnimationState* state = m_entity->getAnimationState(name);
		SavedState& saved = Capture(state);
		saved.lowerWeight += weight;
		saved.lowerTime = phase * state->getLength();
	}

	void Trace(unsigned int objectId, float speed, const std::string& upperState,
		bool active, const char* longitudinal, const char* lateral, float forwardWeight, float lateralWeight)
	{
		if (!SoldierAnimationTraceEnabled() || m_traceMs < 500) return;
		m_traceMs = 0;
		CCLOG_INFO("[AnimMotion] id=%u speed=%.3f layer=%d upper=%s lower=%s:%.2f,%s:%.2f phase=%.3f",
			objectId, speed, active ? 1 : 0, upperState.c_str(), longitudinal,
			forwardWeight, lateral, lateralWeight, m_phase);
	}

	Ogre::Entity* m_entity = nullptr;
	Ogre::Bone* m_spine = nullptr;
	Ogre::Quaternion m_originalSpineOrientation;
	bool m_spineWasManual = false;
	bool m_valid = false;
	bool m_active = false;
	float m_phase = 0;
	float m_movingWeight = 0, m_crouchWeight = 0;
	float m_direction[4] = {1, 0, 0, 0};
	int m_traceMs = 0;
	Ogre::SkeletonAnimationBlendMode m_previousBlendMode = Ogre::ANIMBLEND_AVERAGE;
	std::vector<bool> m_upper;
	std::vector<float> m_identityMask, m_workMask;
	std::vector<SavedState> m_saved;
	std::vector<Ogre::AnimationState*> m_managedMasks;
};

#endif
