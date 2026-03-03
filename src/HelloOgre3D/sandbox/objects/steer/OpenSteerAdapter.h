#ifndef __OPENSTEER_ADAPTER_H__
#define __OPENSTEER_ADAPTER_H__

#include "opensteer/include/Vec3.h"
#include "opensteer/include/SteerLibrary.h"

class AgentLocomotion;
class OpenSteerAdapter : public OpenSteer::SteerLibraryMixin<OpenSteer::AbstractVehicle>
{
public:
	explicit OpenSteerAdapter(AgentLocomotion* owner);
	~OpenSteerAdapter() = default;

	/**
	 * implementation of OpenSteer AbstractVehicle.
	 * @see opensteer/include/AbstractVehicle.h
	 */
	virtual float mass(void) const override;
	virtual float setMass(float) override;

	virtual float radius(void) const override;
	virtual float setRadius(float) override;

	virtual OpenSteer::Vec3 velocity(void) const override;

	virtual float speed(void) const override;
	virtual float setSpeed(float) override;

	virtual OpenSteer::Vec3 predictFuturePosition(const float predictionTime) const override;

	virtual float maxForce(void) const override;
	virtual float setMaxForce(float) override;

	virtual float maxSpeed(void) const override;
	virtual float setMaxSpeed(float) override;

	/**
	 * implementation of OpenSteer AbstractLocalSpace.
	 * @see opensteer/include/LocalSpace.h
	 */
	virtual OpenSteer::Vec3 side(void) const;
	virtual OpenSteer::Vec3 setSide(OpenSteer::Vec3 s);

	virtual OpenSteer::Vec3 up(void) const;
	virtual OpenSteer::Vec3 setUp(OpenSteer::Vec3 u);
	
	virtual OpenSteer::Vec3 forward(void) const;
	virtual OpenSteer::Vec3 setForward(OpenSteer::Vec3 f);
	
	virtual OpenSteer::Vec3 position(void) const;
	virtual OpenSteer::Vec3 setPosition(OpenSteer::Vec3 p);

	virtual bool rightHanded(void) const;

	virtual void resetLocalSpace(void);
	virtual void setUnitSideFromForwardAndUp(void);

	virtual OpenSteer::Vec3 localizeDirection(const OpenSteer::Vec3& globalDirection) const;
	virtual OpenSteer::Vec3 localizePosition(const OpenSteer::Vec3& globalPosition) const;
	virtual OpenSteer::Vec3 globalizePosition(const OpenSteer::Vec3& localPosition) const;
	virtual OpenSteer::Vec3 globalizeDirection(const OpenSteer::Vec3& localDirection) const;

	virtual void regenerateOrthonormalBasisUF(const OpenSteer::Vec3& newUnitForward);
	virtual void regenerateOrthonormalBasis(const OpenSteer::Vec3& newForward);
	virtual void regenerateOrthonormalBasis(const OpenSteer::Vec3& newForward, const OpenSteer::Vec3& newUp);

	virtual OpenSteer::Vec3 localRotateForwardToSide(const OpenSteer::Vec3& v) const;
	virtual OpenSteer::Vec3 globalRotateForwardToSide(const OpenSteer::Vec3& globalForward) const;

protected:
	AgentLocomotion* m_owner = nullptr;
};

#endif // __OPENSTEER_ADAPTER_H__