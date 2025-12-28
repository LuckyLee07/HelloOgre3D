#ifndef __CAMERA_SERVICE_H__
#define __CAMERA_SERVICE_H__

#include "OgreVector3.h"
#include "OgreQuaternion.h"

namespace Ogre {
	class Camera;
	class SceneManager;
}
class ClientManager;

class CameraService
{
public:
	CameraService(ClientManager* pMananger);
	~CameraService() { m_clientManger = nullptr; }

	Ogre::Camera* GetCamera();
	Ogre::SceneManager* GetSceneManager();

	Ogre::Vector3 GetCameraUp();
	Ogre::Vector3 GetCameraLeft();
	Ogre::Vector3 GetCameraForward();
	Ogre::Vector3 GetCameraPosition();
	Ogre::Vector3 GetCameraRotation();
	Ogre::Quaternion GetCameraOrientation();

	long long GetRenderTime();
	long long GetSimulateTime();
	long long GetTotalSimulateTime();

private:
	ClientManager* m_clientManger;
};

#endif // __CAMERA_SERVICE_H__