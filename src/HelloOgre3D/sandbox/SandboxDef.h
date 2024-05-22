#ifndef __SANDBOX_DEF__
#define __SANDBOX_DEF__

#include "OgreVector3.h"
#include "OgreMatrix3.h"
#include "OgreQuaternion.h"
#include "LogSystem.h"

#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
#include "opensteer/include/Vec3.h"

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
// 给定的三个角度（分别沿X、Y、Z轴的旋转），创建表示这个旋转的Ogre::Quaternion四元数
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

//tolua_end

inline OpenSteer::Vec3 Vector3ToVec3(const Ogre::Vector3& vector)
{
	return OpenSteer::Vec3((float)vector.x, (float)vector.y, (float)vector.z);
}

inline Ogre::Vector3 Vec3ToVector3(const OpenSteer::Vec3& vector)
{
	return Ogre::Vector3(Ogre::Real(vector.x), Ogre::Real(vector.y), Ogre::Real(vector.z));
}

#endif  // __SANDBOX_MANAGER__
