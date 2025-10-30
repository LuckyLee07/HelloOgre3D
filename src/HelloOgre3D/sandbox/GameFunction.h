#ifndef __GAME_FUNCTION__
#define __GAME_FUNCTION__

#include "GameDefine.h"
#include "OgreVector3.h"
#include "OgreMatrix3.h"
#include "OgreQuaternion.h"
#include "LogSystem.h"

#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "opensteer/include/Vec3.h"

inline int ConvertAnimID(int animId, int stype)
{
	if (stype == SOLDIER_STAND)
	{
		if (animId == CROUCH_SSTATE_DEAD || animId == CROUCH_SSTATE_FIRE ||
			animId == CROUCH_SSTATE_IDLE_AIM || animId == CROUCH_SSTATE_FORWARD)
		{
			return (animId - SSTATE_MAXCOUNT + 4);
		}
	}
	else if (stype == SOLDIER_CROUCH)
	{
		if (animId == SSTATE_DEAD || animId == SSTATE_FIRE ||
			animId == SSTATE_IDLE_AIM || animId == SSTATE_RUN_FORWARD)
		{
			return (SSTATE_MAXCOUNT - 4 + animId);
		}
	}
	return animId; // ����״̬������
}

//tolua_begin
// �����������Ƕȣ��ֱ���X��Y��Z�����ת����������ʾ�����ת��Ogre::Quaternion��Ԫ��
inline Ogre::Quaternion QuaternionFromRotationDegrees(Ogre::Real xRotation, Ogre::Real yRotation, Ogre::Real zRotation)
{
	Ogre::Matrix3 matrix;
	matrix.FromEulerAnglesXYZ(Ogre::Degree(xRotation), Ogre::Degree(yRotation), Ogre::Degree(zRotation));

	return Ogre::Quaternion(matrix);
}

inline Ogre::Vector3 QuaternionToRotationDegrees(const Ogre::Quaternion& quaternion)
{
	Ogre::Vector3 angles;

	Ogre::Radian xAngle;
	Ogre::Radian yAngle;
	Ogre::Radian zAngle;

	Ogre::Matrix3 rotation;
	quaternion.ToRotationMatrix(rotation);
	rotation.ToEulerAnglesXYZ(xAngle, yAngle, zAngle);

	angles.x = xAngle.valueDegrees();
	angles.y = yAngle.valueDegrees();
	angles.z = zAngle.valueDegrees();

	return angles;
}

inline Ogre::Vector3 BtVector3ToVector3(const btVector3& vector)
{
	return Ogre::Vector3(vector.m_floats[0], vector.m_floats[1], vector.m_floats[2]);
}

inline Ogre::Quaternion BtQuaternionToQuaternion(const btQuaternion& orientation)
{
	return Ogre::Quaternion(orientation.w(), orientation.x(),
		orientation.y(), orientation.z());
}

inline Ogre::Quaternion GetForward(const Ogre::Vector3& forward)
{
	return Ogre::Vector3::UNIT_Z.getRotationTo(forward);
}

//tolua_end

inline OpenSteer::Vec3 Vector3ToVec3(const Ogre::Vector3& vector)
{
	return OpenSteer::Vec3((float)vector.x, (float)vector.y, (float)vector.z);
}

inline Ogre::Vector3 Vec3ToVector3(const OpenSteer::Vec3& vector)
{
	return Ogre::Vector3(Ogre::Real(vector.x), Ogre::Real(vector.y), Ogre::Real(vector.z));
}

#endif  // __GAME_FUNCTION__
