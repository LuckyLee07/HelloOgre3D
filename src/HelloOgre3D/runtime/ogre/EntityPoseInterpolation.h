#ifndef HELLO_ENTITY_POSE_INTERPOLATION_H
#define HELLO_ENTITY_POSE_INTERPOLATION_H

#include "OgreAnimationState.h"
#include "OgreBone.h"
#include "OgreEntity.h"
#include "OgreSkeletonInstance.h"
#include <algorithm>
#include <vector>

// Borrows one entity. The owner calls Restore/Release before replacing it.
// Destruction itself never dereferences borrowed pointers. Animation clocks,
// weights and notifies remain owned by the simulation; rendering samples poses only.
class EntityPoseInterpolation
{
public:
	explicit EntityPoseInterpolation(Ogre::Entity* entity) : m_entity(entity) {}

	void Restore()
	{
		if (!m_displayed) return;
		Ogre::SkeletonInstance* skeleton = m_entity->getSkeleton();
		for (unsigned short index = 0; index < m_current.size(); ++index)
		{
			Ogre::Bone* bone = skeleton->getBone(index);
			SetPose(bone, m_current[index]);
			bone->setManuallyControlled(m_current[index].manual);
		}
		for (Ogre::AnimationState* state : m_enabledStates) state->setEnabled(true);
		m_enabledStates.clear();
		m_displayed = false;
		skeleton->_updateTransforms();
	}

	void Capture()
	{
		Restore();
		if (m_entity == nullptr || !m_entity->hasSkeleton()) return;
		Ogre::SkeletonInstance* skeleton = m_entity->getSkeleton();
		skeleton->setAnimationState(*m_entity->getAllAnimationStates());
		skeleton->_updateTransforms();
		m_previous = m_current;
		m_current.resize(skeleton->getNumBones());
		for (unsigned short index = 0; index < m_current.size(); ++index)
		{
			Ogre::Bone* bone = skeleton->getBone(index);
			m_current[index] = {bone->getPosition(), bone->getScale(), bone->getOrientation(), bone->isManuallyControlled()};
		}
		if (m_previous.size() != m_current.size()) m_previous = m_current;
	}

	void Display(float alpha)
	{
		if (m_current.empty()) return;
		alpha = std::max(0.0f, std::min(1.0f, alpha));
		if (!m_displayed)
		{
			Ogre::AnimationStateIterator states = m_entity->getAllAnimationStates()->getAnimationStateIterator();
			while (states.hasMoreElements())
			{
				Ogre::AnimationState* state = states.getNext();
				if (state->getEnabled()) { m_enabledStates.push_back(state); state->setEnabled(false); }
			}
			m_displayed = true;
		}
		Ogre::SkeletonInstance* skeleton = m_entity->getSkeleton();
		for (unsigned short index = 0; index < m_current.size(); ++index)
		{
			const Pose& a = m_previous[index];
			const Pose& b = m_current[index];
			Ogre::Bone* bone = skeleton->getBone(index);
			bone->setManuallyControlled(true);
			bone->setPosition(a.position + (b.position - a.position) * alpha);
			bone->setScale(a.scale + (b.scale - a.scale) * alpha);
			bone->setOrientation(Ogre::Quaternion::Slerp(alpha, a.orientation, b.orientation, true));
		}
		skeleton->_updateTransforms();
	}

	void Freeze() { m_previous = m_current; }

	void Release()
	{
		Restore();
		m_previous.clear();
		m_current.clear();
		m_entity = nullptr;
	}

private:
	struct Pose { Ogre::Vector3 position, scale; Ogre::Quaternion orientation; bool manual; };
	static void SetPose(Ogre::Bone* bone, const Pose& pose)
	{
		bone->setPosition(pose.position);
		bone->setScale(pose.scale);
		bone->setOrientation(pose.orientation);
	}
	Ogre::Entity* m_entity = nullptr;
	bool m_displayed = false;
	std::vector<Pose> m_previous, m_current;
	std::vector<Ogre::AnimationState*> m_enabledStates;
};

#endif
