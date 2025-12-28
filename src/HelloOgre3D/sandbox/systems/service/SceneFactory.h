#ifndef __SCENE_FACTORY_H__
#define __SCENE_FACTORY_H__

#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreNameGenerator.h"

namespace Ogre {
	class Camera;
	class SceneNode;
	class SceneManager;
}

class SceneFactory
{
public:
	static Ogre::SceneNode* CreateChildSceneNode();
	static Ogre::SceneNode* CreateNodePlane(Ogre::Real length, Ogre::Real width);

	static Ogre::SceneNode* CreateNodeCapsule(Ogre::Real height, Ogre::Real radius);

	static void RemParticleBySceneNode(Ogre::SceneNode* particleNode);
	static Ogre::SceneNode* CreateParticle(Ogre::SceneNode* parentNode, const Ogre::String& particleName);

	static void GetMeshInfo(const Ogre::Mesh* mesh, size_t& vertex_count, Ogre::Vector3*& vertices, size_t& index_count, unsigned long*& indices);

	static bool GetBonePosition(Ogre::SceneNode& node, const Ogre::String& boneName, Ogre::Vector3& outPosition);
	static bool GetBonePosition(Ogre::MovableObject& object, const Ogre::String& boneName, Ogre::Vector3& outPosition);

	static bool GetBoneOrientation(Ogre::SceneNode& node, const Ogre::String& boneName, Ogre::Quaternion& outOrientation);
	static bool GetBoneOrientation(Ogre::MovableObject& object, const Ogre::String& boneName, Ogre::Quaternion& outOrientation);

private:
	static Ogre::NameGenerator s_nameGenerator;
};

#endif // __SCENE_FACTORY_H__