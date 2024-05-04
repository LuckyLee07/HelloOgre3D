#ifndef __SANDBOX_DEF__
#define __SANDBOX_DEF__

#include "OgreVector3.h"
#include "OgreMatrix3.h"
#include "OgreQuaternion.h"
#include "LogSystem.h"

#define DEFAULT_MATERIAL "White"

#define DEFAULT_ATLAS "fonts/dejavu/dejavu"

#ifdef _DEBUG
#define APPLICATION_LOG         "Sandbox_d.log"
#define APPLICATION_CONFIG      "Sandbox_d.cfg"
#define APPLICATION_RESOURCES	"SandboxResources_d.cfg"
#else
#define APPLICATION_LOG         "Sandbox.log"
#define APPLICATION_CONFIG      "Sandbox.cfg"
#define APPLICATION_RESOURCES	"SandboxResources.cfg"
#endif

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

//tolua_end
#endif  // __SANDBOX_MANAGER__
