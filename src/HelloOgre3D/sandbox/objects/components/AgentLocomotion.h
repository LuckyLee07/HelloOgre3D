#ifndef __AGENT_LOCOMOTION_H__
#define __AGENT_LOCOMOTION_H__

#include <vector>
#include "OgreVector3.h"
#include "component/IComponent.h"

class AgentObject;
class VehicleObject;
class OpenSteerAdapter;

class AgentLocomotion : public IComponent
{
public:
	AgentLocomotion();
	virtual ~AgentLocomotion();

	virtual void onAttach(GameObject* owner);
	virtual void onDetach();

	// 位置设置
	void SetPosition(const Ogre::Vector3& position);
	Ogre::Vector3 GetPosition() const;

	// 物理参数与状态
	void SetMass(Ogre::Real mass);
	void SetHeight(Ogre::Real height);
	void SetRadius(Ogre::Real radius);
	void SetSpeed(Ogre::Real speed);
	void SetHealth(Ogre::Real health);
	void SetMaxForce(Ogre::Real maxForce);
	void SetMaxSpeed(Ogre::Real maxSpeed);

	Ogre::Real GetMass() const;
	Ogre::Real GetSpeed() const;
	Ogre::Real GetHeight() const;
	Ogre::Real GetRadius() const;
	Ogre::Real GetHealth() const;
	Ogre::Real GetMaxForce() const;
	Ogre::Real GetMaxSpeed() const;

	// 姿态与速度
	void SetForward(const Ogre::Vector3& forward);
	void SetVelocity(const Ogre::Vector3& velocity);

	void SetTarget(const Ogre::Vector3& targetPos);
	void SetTargetRadius(Ogre::Real radius);

	Ogre::Real GetTargetRadius() const;
	Ogre::Vector3 GetTarget() const;
	Ogre::Vector3 GetVelocity() const;

	virtual Ogre::Vector3 GetUp() const;
	virtual Ogre::Vector3 GetLeft() const;
	virtual Ogre::Vector3 GetForward() const;

	// 路径
	void SetPath(const std::vector<Ogre::Vector3>& points, bool cyclic);
	const std::vector<Ogre::Vector3>& GetPath() const { return m_points; }
	bool HasPath() const { return !m_points.empty(); }
	bool IsPathCyclic() const { return m_pathCyclic; }
	
	// 距离/采样（仍转发；可后续迁移）
	Ogre::Real GetDistanceAlongPath(const Ogre::Vector3& position) const;
	Ogre::Vector3 GetNearestPointOnPath(const Ogre::Vector3& position) const;
	Ogre::Vector3 GetPointOnPath(const Ogre::Real distance) const;

	// Steering forces
	Ogre::Vector3 ForceToPosition(const Ogre::Vector3& position);
	Ogre::Vector3 ForceToFollowPath(Ogre::Real predictionTime);
	Ogre::Vector3 ForceToStayOnPath(Ogre::Real predictionTime);
	Ogre::Vector3 ForceToWander(Ogre::Real deltaMilliSeconds);
	Ogre::Vector3 ForceToTargetSpeed(Ogre::Real targetSpeed);

	// 规避之力 智能体或其他障碍物
	Ogre::Vector3 ForceToAvoidAgents(Ogre::Real predictionTime);
	Ogre::Vector3 ForceToAvoidObjects(Ogre::Real predictionTime);

	Ogre::Vector3 ForceToCombine(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle);
	Ogre::Vector3 ForceToSeparate(const std::vector<AgentObject*>& agents, Ogre::Real distance, Ogre::Real angle);

	// 力之应用
	void ApplyForce(const Ogre::Vector3& force);

	OpenSteerAdapter* GetAdapter() const { return m_adapter; }

public:
	static const float DEFAULT_AGENT_MAX_FORCE;
	static const float DEFAULT_AGENT_MAX_SPEED;
	static const float DEFAULT_AGENT_TARGET_RADIUS;

private:
	VehicleObject* m_owner = nullptr;

	// Path 路径
	std::vector<Ogre::Vector3> m_points;
	bool m_pathCyclic = false;

	// 运动/策略参数
	Ogre::Real m_maxForce = 0.0f;
	Ogre::Real m_maxSpeed = 0.0f;
	Ogre::Real m_targetRadius = 0.0f;
	Ogre::Vector3 m_targetPos = Ogre::Vector3::ZERO;

	VehicleObject* m_targetAgent = nullptr; // 非拥有
	OpenSteerAdapter* m_adapter = nullptr;  // OpenSteer 适配
};

#endif // __AGENT_LOCOMOTION_H__