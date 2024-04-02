#ifndef __SANDBOX_DEF__
#define __SANDBOX_DEF__

#include "OgreMatrix3.h"
#include "OgreQuaternion.h"

#define DEFAULT_MATERIAL "White"

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
//tolua_end
#endif  // __SANDBOX_MANAGER__
