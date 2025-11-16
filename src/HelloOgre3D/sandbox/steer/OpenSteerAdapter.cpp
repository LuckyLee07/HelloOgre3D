#include "OpenSteerAdapter.h"
#include "components/AgentLocomotion.h"
#include "GameFunction.h"

OpenSteerAdapter::OpenSteerAdapter(AgentLocomotion* owner) : m_owner(owner)
{

}

/**
* implementation of OpenSteer AbstractVehicle.
* @see opensteer/include/AbstractVehicle.h
*/
float OpenSteerAdapter::mass(void) const
{
	return static_cast<float>(m_owner->GetMass());
}

float OpenSteerAdapter::setMass(float mass)
{
	m_owner->SetMass(Ogre::Real(mass));
	return this->mass();
}

float OpenSteerAdapter::radius(void) const
{
	return static_cast<float>(m_owner->GetRadius());
}

float OpenSteerAdapter::setRadius(float radius)
{
	m_owner->SetRadius(Ogre::Real(radius));
	return this->radius();
}

OpenSteer::Vec3 OpenSteerAdapter::velocity(void) const
{
	return Vector3ToVec3(m_owner->GetVelocity());
}

float OpenSteerAdapter::speed(void) const
{
	return static_cast<float>(m_owner->GetSpeed());
}

float OpenSteerAdapter::setSpeed(float speed)
{
	m_owner->SetSpeed(Ogre::Real(speed));
	return this->speed();
}

OpenSteer::Vec3 OpenSteerAdapter::predictFuturePosition(const float predictionTime) const
{
	Ogre::Real upredictionTime = std::max(Ogre::Real(0), predictionTime);
	return Vector3ToVec3(m_owner->GetVelocity() * upredictionTime + m_owner->GetPosition());
}

float OpenSteerAdapter::maxForce(void) const
{
	return static_cast<float>(m_owner->GetMaxForce());
}

float OpenSteerAdapter::setMaxForce(float maxForce)
{
	m_owner->SetMaxForce(Ogre::Real(maxForce));
	return this->maxForce();
}

float OpenSteerAdapter::maxSpeed(void) const
{
	return static_cast<float>(m_owner->GetMaxSpeed());
}

float OpenSteerAdapter::setMaxSpeed(float maxSpeed)
{
	m_owner->SetMaxSpeed(Ogre::Real(maxSpeed));
	return this->maxSpeed();
}

/**
* implementation of OpenSteer AbstractLocalSpace.
* @see opensteer/include/LocalSpace.h
*/
OpenSteer::Vec3 OpenSteerAdapter::side(void) const
{
	return Vector3ToVec3(m_owner->GetLeft());
}

OpenSteer::Vec3 OpenSteerAdapter::setSide(OpenSteer::Vec3 s)
{
	assert(false);
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 OpenSteerAdapter::up(void) const
{
	return Vector3ToVec3(m_owner->GetUp());
}

OpenSteer::Vec3 OpenSteerAdapter::setUp(OpenSteer::Vec3 u)
{
	assert(false);
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 OpenSteerAdapter::forward(void) const
{
	return Vector3ToVec3(m_owner->GetForward());
}

OpenSteer::Vec3 OpenSteerAdapter::setForward(OpenSteer::Vec3 f)
{
	assert(false);
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 OpenSteerAdapter::position(void) const
{
	return Vector3ToVec3(m_owner->GetPosition());
}

OpenSteer::Vec3 OpenSteerAdapter::setPosition(OpenSteer::Vec3 pos)
{
	m_owner->SetPosition(Vec3ToVector3(pos));
	return this->position();
}

bool OpenSteerAdapter::rightHanded(void) const
{
	return true;
}

void OpenSteerAdapter::resetLocalSpace(void)
{
	assert(false);
}

void OpenSteerAdapter::setUnitSideFromForwardAndUp(void)
{
	assert(false);
}

OpenSteer::Vec3 OpenSteerAdapter::localizeDirection(const OpenSteer::Vec3& globalDirection) const
{
	float dirX = globalDirection.dot(Vector3ToVec3(m_owner->GetLeft()));
	float dirY = globalDirection.dot(Vector3ToVec3(m_owner->GetUp()));
	float dirZ = globalDirection.dot(Vector3ToVec3(m_owner->GetForward()));
	return OpenSteer::Vec3(dirX, dirY, dirZ);
}

OpenSteer::Vec3 OpenSteerAdapter::localizePosition(const OpenSteer::Vec3& globalPosition) const
{
	const OpenSteer::Vec3& position = Vector3ToVec3(m_owner->GetPosition());
	OpenSteer::Vec3 globalOffset = globalPosition - position;
	return localizeDirection(globalOffset);
}

OpenSteer::Vec3 OpenSteerAdapter::globalizePosition(const OpenSteer::Vec3& localPosition) const
{
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 OpenSteerAdapter::globalizeDirection(const OpenSteer::Vec3& localDirection) const
{
	return OpenSteer::Vec3();
}

void OpenSteerAdapter::regenerateOrthonormalBasisUF(const OpenSteer::Vec3& newUnitForward)
{
	(void)newUnitForward;
}

void OpenSteerAdapter::regenerateOrthonormalBasis(const OpenSteer::Vec3& newForward)
{
	(void)newForward;
}

void OpenSteerAdapter::regenerateOrthonormalBasis(const OpenSteer::Vec3& newForward, const OpenSteer::Vec3& newUp)
{
	(void)newForward;
	(void)newUp;
}

OpenSteer::Vec3 OpenSteerAdapter::localRotateForwardToSide(const OpenSteer::Vec3& v) const
{
	return OpenSteer::Vec3();
}

OpenSteer::Vec3 OpenSteerAdapter::globalRotateForwardToSide(const OpenSteer::Vec3& globalForward) const
{
	return OpenSteer::Vec3();
}
