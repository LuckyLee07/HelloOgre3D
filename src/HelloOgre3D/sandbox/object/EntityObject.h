#ifndef __Entity_OBJECT__
#define __Entity_OBJECT__

#include "BaseObject.h"
#include "OgreString.h"
#include "OgreMath.h"
#include "OgreVector3.h"
#include <unordered_map>

namespace Ogre {
	class Entity;
	class SceneNode;
}
class AgentAnim;
class AgentAnimStateMachine;

class EntityObject : public BaseObject //tolua_exports
{ //tolua_exports
public:
	EntityObject(const Ogre::String& meshFile);
	EntityObject(const Ogre::MeshPtr& meshPtr);
	EntityObject(Ogre::SceneNode* pSceneNode);
	virtual ~EntityObject();

	void update(int deltaMsec) override;

	BaseObject* getOwner() { return m_owner; }
	//tolua_begin
	void InitWithOwner(BaseObject* owner=nullptr, bool canFireEvent=true);

	const Ogre::Vector3& getOriginPos() const;
	void setOriginPos(const Ogre::Vector3& position);
	void setMaterial(const Ogre::String& materialName);
	
	virtual void setPosition(const Ogre::Vector3& position);
	virtual void setRotation(const Ogre::Vector3& position);
	virtual void setOrientation(const Ogre::Quaternion& quaternion);

	virtual Ogre::Vector3 GetPosition() const;
	virtual Ogre::Quaternion GetOrientation() const;

	void AttachToBone(const Ogre::String& boneName, EntityObject* entityObj, const Ogre::Vector3& positionOffset, const Ogre::Vector3& rotationOffset);

	AgentAnim* GetAnimation(const char* animationName);
	AgentAnimStateMachine* GetObjectASM();
	//tolua_end

	void SetDerivedPosition(const Ogre::Vector3& position);
	void SetDerivedRotation(const Ogre::Vector3& position);
	void SetDerivedOrientation(const Ogre::Quaternion& quaternion);

	Ogre::Vector3 GetDerivedPosition() const;
	Ogre::Quaternion GetDerivedOrientation() const;

	Ogre::Entity* getEntity() { return m_pEntity; }
	Ogre::Entity* getDetachEntity();
	Ogre::SceneNode* getSceneNode() { return m_pSceneNode; }

protected:
	Ogre::SceneNode* m_pSceneNode;
	Ogre::Entity* m_pEntity;
	Ogre::Vector3 m_originPos;

	BaseObject* m_owner; //≥÷”–’ﬂ

	AgentAnimStateMachine* m_pAnimateStateMachine;
	std::unordered_map<std::string, AgentAnim*> m_animations;
}; //tolua_exports

#endif  // __Entity_OBJECT__
