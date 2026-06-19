#include "SandboxMgr.h"
#include "objects/BlockObject.h"
#include "systems/service/RaycastService.h"
#include "systems/service/SceneService.h"
#include "systems/service/ScriptService.h"

SandboxMgr::SandboxMgr(SceneService* sceneService,
	ScriptService* scriptService,
	RaycastService* raycastService)
	: m_sceneService(sceneService),
	m_scriptService(scriptService),
	m_raycastService(raycastService)
{
}

SandboxMgr::~SandboxMgr()
{
	m_sceneService = nullptr;
	m_scriptService = nullptr;
	m_raycastService = nullptr;
}

Ogre::SceneManager* SandboxMgr::GetSceneCreator()
{
	return m_sceneService != nullptr ? m_sceneService->GetSceneManager() : nullptr;
}

void SandboxMgr::CallFile(const Ogre::String& filepath)
{
	if (m_scriptService != nullptr)
		m_scriptService->CallFile(filepath);
}

void SandboxMgr::SetUseCppFsmFlag(bool value)
{
	m_useCppFsmFlag = value;
}

void SandboxMgr::SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation)
{
	if (m_sceneService != nullptr)
		m_sceneService->SetSkyBox(materialName, rotation);
}

void SandboxMgr::SetAmbientLight(const Ogre::Vector3& colourValue)
{
	if (m_sceneService != nullptr)
		m_sceneService->SetAmbientLight(colourValue);
}

Ogre::Light* SandboxMgr::CreateDirectionalLight(const Ogre::Vector3& direction)
{
	return m_sceneService != nullptr ? m_sceneService->CreateDirectionalLight(direction) : nullptr;
}

void SandboxMgr::setMaterial(BlockObject* pObject, const Ogre::String& materialName)
{
	if (m_sceneService != nullptr)
		m_sceneService->setMaterial(pObject, materialName);
}

void SandboxMgr::setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName)
{
	if (m_sceneService != nullptr)
		m_sceneService->setMaterial(pNode, materialName);
}

void SandboxMgr::UpdateSceneGraph()
{
	if (m_sceneService != nullptr)
		m_sceneService->UpdateSceneGraph();
}

int SandboxMgr::RayCastObjectId(const Ogre::Vector3& from, const Ogre::Vector3& to) const
{
	return m_raycastService != nullptr ? m_raycastService->RayCastObjectId(from, to) : -1;
}
