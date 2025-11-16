#ifndef __PHYSICS_COMPONENT_H__
#define __PHYSICS_COMPONENT_H__

#include "OgreVector3.h"

class btRigidBody;
class VehicleObject;

class PhysicsComponent
{
public:
	PhysicsComponent(VehicleObject* owner, btRigidBody* body);
	virtual ~PhysicsComponent();
	
	// RigidBody ops
	btRigidBody* GetRigidBody() const { return m_body; }
	void ResetRigidBody(btRigidBody* newBody);
	void DeleteRigidBody();

	// Transforms/velocity/force
	void SetPosition(const Ogre::Vector3& position);
	void SetOrientation(const Ogre::Quaternion& quaternion);
	void SetRotation(const Ogre::Vector3& rotation);
	Ogre::Vector3 GetPosition() const;
	Ogre::Quaternion GetOrientation() const;

	void SetForward(const Ogre::Vector3& forward);
	void SetVelocity(const Ogre::Vector3& velocity);
	Ogre::Vector3 GetVelocity() const;

	Ogre::Vector3 GetUp() const;
	Ogre::Vector3 GetLeft() const;
	Ogre::Vector3 GetForward() const;

	void ApplyForce(const Ogre::Vector3& force);

	// Parameters
	void SetMass(Ogre::Real mass);
	Ogre::Real GetMass() const;
	Ogre::Real GetHorizontalSpeed() const;

	void RebuildCapsule(Ogre::Real height, Ogre::Real radius);

private:
	void AddToWorld();
	void RemoveFromWorld();

private:
	btRigidBody* m_body = nullptr;

	VehicleObject* m_owner = nullptr;
};

#endif // __PHYSICS_COMPONENT_H__