#include "CameraService.h"
#include "OgreSceneManager.h"
#include "GameFunction.h"
#include "manager/ClientManager.h"

CameraService::CameraService(ClientManager* pMananger)
	: m_clientManger(pMananger)
{
}

Ogre::Camera* CameraService::GetCamera()
{
	return m_clientManger->getCamera();
}

Ogre::SceneManager* CameraService::GetSceneManager()
{
	auto* pRootSceneNode = m_clientManger->getRootSceneNode();
	return pRootSceneNode->getCreator();
}

Ogre::Vector3 CameraService::GetCameraUp()
{
	Ogre::Camera* pCamera = GetCamera();
	return -1.0f * pCamera->getDerivedUp();
}

Ogre::Vector3 CameraService::GetCameraLeft()
{
	Ogre::Camera* pCamera = GetCamera();
	return -1.0f * pCamera->getDerivedRight();
}

Ogre::Vector3 CameraService::GetCameraForward()
{
	Ogre::Camera* pCamera = GetCamera();
	return pCamera->getDerivedDirection();
}

Ogre::Vector3 CameraService::GetCameraPosition()
{
	Ogre::Camera* pCamera = GetCamera();
	return pCamera->getDerivedPosition();
}

Ogre::Vector3 CameraService::GetCameraRotation()
{
	return QuaternionToRotationDegrees(GetCameraOrientation());
}

Ogre::Quaternion CameraService::GetCameraOrientation()
{
	Ogre::Camera* pCamera = GetCamera();
	return pCamera->getDerivedOrientation();
}

long long CameraService::GetRenderTime()
{
	ClientManager::ProfileTimeType profileType = ClientManager::ProfileTimeType::P_RENDER_TIME;
	return m_clientManger->GetProfileTime(profileType);
}

long long CameraService::GetSimulateTime()
{
	ClientManager::ProfileTimeType profileType = ClientManager::ProfileTimeType::P_SIMULATE_TIME;
	return m_clientManger->GetProfileTime(profileType);
}

long long CameraService::GetTotalSimulateTime()
{
	ClientManager::ProfileTimeType profileType = ClientManager::ProfileTimeType::P_TOTAL_SIMULATE_TIME;
	return m_clientManger->GetProfileTime(profileType);
}