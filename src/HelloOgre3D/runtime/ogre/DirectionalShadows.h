#ifndef HELLO_RUNTIME_DIRECTIONAL_SHADOWS_H
#define HELLO_RUNTIME_DIRECTIONAL_SHADOWS_H

#include <cmath>
#include "OgreException.h"
#include "OgreStringConverter.h"
#include "OgreLight.h"
#include "OgreLogManager.h"
#include "OgreMaterial.h"
#include "OgreMaterialManager.h"
#include "OgreSceneManager.h"

namespace RuntimeOgre
{
// The light and shadow render targets remain owned by SceneManager.
inline bool ConfigureDirectionalShadows(Ogre::SceneManager* scene, Ogre::Light* light, bool enabled, float farDistance = 32.0f)
{
	if (scene == nullptr || !std::isfinite(farDistance) || farDistance < 1.0f || farDistance > 200.0f)
		return false;
	if (!enabled)
	{
		scene->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
		scene->setShadowTextureCasterMaterial(Ogre::String());
		scene->setShadowTextureReceiverMaterial(Ogre::String());
		scene->setShadowTextureSelfShadow(false);
		scene->setShadowTextureCount(0);
		Ogre::LogManager::getSingleton().logMessage("[DirectionalShadows] disabled");
		return true;
	}
	if (light == nullptr || light->getType() != Ogre::Light::LT_DIRECTIONAL)
		return false;
	try
	{
		Ogre::MaterialPtr caster = Ogre::MaterialManager::getSingleton().getByName("Relay/ShadowCaster");
		Ogre::MaterialPtr receiver = Ogre::MaterialManager::getSingleton().getByName("Relay/ShadowReceiver");
		if (caster.isNull() || receiver.isNull())
			return false;
		caster->load();
		receiver->load();
		if (caster->getBestTechnique() == nullptr || receiver->getBestTechnique() == nullptr)
			return false;
		scene->setShadowTextureCount(1);
		scene->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 1);
		scene->setShadowTextureConfig(0, 1536, 1536, Ogre::PF_FLOAT32_R, 0);
		scene->setShadowTextureCasterMaterial("Relay/ShadowCaster");
		scene->setShadowTextureReceiverMaterial("Relay/ShadowReceiver");
		scene->setShadowTextureSelfShadow(true);
		scene->setShadowCasterRenderBackFaces(false);
		scene->setShadowFarDistance(farDistance);
		scene->setShadowDirectionalLightExtrusionDistance(100.0f);
		light->setCastShadows(true);
		light->setShadowNearClipDistance(0.1f);
		light->setShadowFarClipDistance(220.0f);
		scene->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
		Ogre::LogManager::getSingleton().logMessage(
			"[DirectionalShadows] texture modulative 1536 R32F PCF4 far=" + Ogre::StringConverter::toString(farDistance) + " clip=0.1:220");
		return true;
	}
	catch (const Ogre::Exception& error)
	{
		ConfigureDirectionalShadows(scene, nullptr, false);
		Ogre::LogManager::getSingleton().logMessage(
			"[DirectionalShadows] configuration failed: " + error.getFullDescription());
		return false;
	}
}
}

#endif
