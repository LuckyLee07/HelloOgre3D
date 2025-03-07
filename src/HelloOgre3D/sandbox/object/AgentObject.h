#ifndef __AGENT_OBJECT__
#define __AGENT_OBJECT__

#include "OgreMath.h"
#include "OgreVector3.h"
#include "SandboxDef.h"
#include "VehicleObject.h"
#include "OISKeyboard.h"

namespace Ogre {
	class SceneNode;
}

struct lua_State;
class ScriptLuaVM;
class BlockObject;
class EntityObject;

class AgentObject : public VehicleObject //tolua_exports
{ //tolua_exports
public:
	AgentObject(EntityObject* pAgentBody, btRigidBody* pRigidBody = nullptr);
	virtual ~AgentObject();

	void Initialize();

	virtual void update(int deltaMilisec) override;
	virtual void updateWorldTransform() override;

	//tolua_begin
	void initAgentBody(const Ogre::String& meshFile);
	void initAgentWeapon(const Ogre::String& meshFile);

	EntityObject* getAgentBody() { return m_pAgentBody; }
	EntityObject* getAgentWeapon() { return m_pAgentWeapon; }

	AGENT_OBJ_TYPE getAgentType() { return m_agentType; }
	void setAgentType(AGENT_OBJ_TYPE agentType) { m_agentType = agentType; }
	
	Ogre::Vector3 GetPosition() const override;
	Ogre::Quaternion GetOrientation() const override;

	virtual Ogre::Vector3 GetUp() const;
	virtual Ogre::Vector3 GetLeft() const;
	virtual Ogre::Vector3 GetForward() const;

	void ShootBullet();
	//tolua_end
	
	void DoShootBullet(const Ogre::Vector3& position, const Ogre::Vector3& rotation);

	void setPluginEnv(lua_State* L); // 手动tolua
	void BindLuaPluginByFile(const std::string& fileName);
	void HandleKeyEvent(OIS::KeyCode keycode, unsigned int key);

private:
	void CreateEventDispatcher();
	void RemoveEventDispatcher();
	
private:
	int m_luaRef = 0; // 绑定的Lua表
	ScriptLuaVM* m_pScriptVM;

	AGENT_OBJ_TYPE m_agentType;
	EntityObject* m_pAgentBody;
	EntityObject* m_pAgentWeapon;
}; //tolua_exports

#endif  // __AGENT_OBJECT__
