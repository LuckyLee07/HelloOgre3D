/*
** Lua binding: SandboxToLua
** Generated automatically by tolua++-1.0.92 on Thu Jun 18 22:19:30 2026.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_SandboxToLua_open (lua_State* tolua_S);

#include <string>
#include <vector>
#include "Ogre.h"
#include "ois/includes/OISKeyboard.h"
#include "recast/include/Recast.h"
#include "../GameDefine.h"
#include "../GameFunction.h"
#include "../systems/ui/UIFrame.h"
#include "../systems/ui/UIManager.h"
#include "../systems/input/IPlayerInput.h"
#include "../systems/service/CameraService.h"
#include "../objects/animation/AgentAnim.h"
#include "../objects/animation/AgentAnimStateMachine.h"
#include "../components/agent/AgentAttrib.h"
#include "../components/anim/AnimComponent.h"
#include "../core/object/BaseObject.h"
#include "../objects/BlockObject.h"
#include "../objects/AgentObject.h"
#include "../components/combat/WeaponComponent.h"
#include "../objects/SoldierObject.h"
#include "../components/ai/AIController.h"
#include "../systems/service/ObjectFactory.h"
#include "../systems/manager/SandboxMgr.h"
#include "../systems/manager/ObjectManager.h"
#include "../ai/navigation/NavigationMesh.h"
#include "../ai/common/Blackboard.h"
#include "../ai/decision/DecisionNode.h"
#include "../ai/decision/DecisionAction.h"
#include "../ai/decision/DecisionBranch.h"
#include "../ai/decision/DecisionTree.h"
#include "../ai/decision/LuaDecisionAction.h"
#include "../ai/decision/DecisionTreeDriver.h"
#include "../ai/behavior/BehaviorNode.h"
#include "../ai/behavior/BehaviorAction.h"
#include "../ai/behavior/BehaviorComposite.h"
#include "../ai/behavior/BehaviorTree.h"
#include "../ai/behavior/LuaBehaviorAction.h"
#include "../ai/behavior/LuaCondition.h"
#include "../ai/behavior/BehaviorTreeDriver.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_Ogre__Quaternion (lua_State* tolua_S)
{
 Ogre::Quaternion* self = (Ogre::Quaternion*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_Ogre__Vector3 (lua_State* tolua_S)
{
 Ogre::Vector3* self = (Ogre::Vector3*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_std__vector_AgentObject__ (lua_State* tolua_S)
{
 std::vector<AgentObject*>* self = (std::vector<AgentObject*>*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_Ogre__Vector2 (lua_State* tolua_S)
{
 Ogre::Vector2* self = (Ogre::Vector2*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"Ogre::Vector2");
 tolua_usertype(tolua_S,"LuaDecisionAction");
 tolua_usertype(tolua_S,"BehaviorSequence");
 tolua_usertype(tolua_S,"AgentAnimStateMachine");
 tolua_usertype(tolua_S,"BaseObject");
 tolua_usertype(tolua_S,"btVector3");
 tolua_usertype(tolua_S,"AnimComponent");
 tolua_usertype(tolua_S,"BehaviorTreeDriver");
 tolua_usertype(tolua_S,"std::vector<EntityObject*>");
 tolua_usertype(tolua_S,"LuaCondition");
 tolua_usertype(tolua_S,"BehaviorTree");
 tolua_usertype(tolua_S,"std::vector<AgentObject*>");
 tolua_usertype(tolua_S,"CameraService");
 tolua_usertype(tolua_S,"BehaviorParallel");
 tolua_usertype(tolua_S,"AgentAnim");
 tolua_usertype(tolua_S,"ObjectManager");
 tolua_usertype(tolua_S,"SoldierAnimController");
 tolua_usertype(tolua_S,"AIController");
 tolua_usertype(tolua_S,"SoldierObject");
 tolua_usertype(tolua_S,"BehaviorComposite");
 tolua_usertype(tolua_S,"WeaponComponent");
 tolua_usertype(tolua_S,"IPlayerInput");
 tolua_usertype(tolua_S,"Ogre::Node");
 tolua_usertype(tolua_S,"BehaviorAction");
 tolua_usertype(tolua_S,"BehaviorSelector");
 tolua_usertype(tolua_S,"IComponent");
 tolua_usertype(tolua_S,"IDecisionDriver");
 tolua_usertype(tolua_S,"AgentObject");
 tolua_usertype(tolua_S,"Ogre::AnimationState");
 tolua_usertype(tolua_S,"std::vector<Ogre::Vector3>");
 tolua_usertype(tolua_S,"DecisionTreeDriver");
 tolua_usertype(tolua_S,"DecisionTree");
 tolua_usertype(tolua_S,"btQuaternion");
 tolua_usertype(tolua_S,"DecisionBranch");
 tolua_usertype(tolua_S,"DecisionAction");
 tolua_usertype(tolua_S,"Ogre::Light");
 tolua_usertype(tolua_S,"NavigationMesh");
 tolua_usertype(tolua_S,"std::vector<BlockObject*>");
 tolua_usertype(tolua_S,"Blackboard");
 tolua_usertype(tolua_S,"BlockObject");
 tolua_usertype(tolua_S,"Ogre::SceneNode");
 tolua_usertype(tolua_S,"DecisionNode");
 tolua_usertype(tolua_S,"SandboxMgr");
 tolua_usertype(tolua_S,"AgentAttrib");
 tolua_usertype(tolua_S,"ObjectFactory");
 tolua_usertype(tolua_S,"Ogre::ColourValue");
 tolua_usertype(tolua_S,"BehaviorRandomSelector");
 tolua_usertype(tolua_S,"IAnimContextProvider");
 tolua_usertype(tolua_S,"BehaviorNode");
 tolua_usertype(tolua_S,"LuaBehaviorAction");
 tolua_usertype(tolua_S,"Ogre::Vector3");
 tolua_usertype(tolua_S,"SandboxObject");
 tolua_usertype(tolua_S,"UIFrame");
 tolua_usertype(tolua_S,"rcConfig");
 tolua_usertype(tolua_S,"Ogre::Quaternion");
 tolua_usertype(tolua_S,"UIManager");
 tolua_usertype(tolua_S,"Ogre::Camera");
}

/* function: QuaternionFromRotationDegrees */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_QuaternionFromRotationDegrees00
static int tolua_SandboxToLua_QuaternionFromRotationDegrees00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
   float xRotation = ((  float)  tolua_tonumber(tolua_S,1,0));
   float yRotation = ((  float)  tolua_tonumber(tolua_S,2,0));
   float zRotation = ((  float)  tolua_tonumber(tolua_S,3,0));
  {
   Ogre::Quaternion tolua_ret = (Ogre::Quaternion)  QuaternionFromRotationDegrees(xRotation,yRotation,zRotation);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Quaternion)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Quaternion));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'QuaternionFromRotationDegrees'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: QuaternionToRotationDegrees */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_QuaternionToRotationDegrees00
static int tolua_SandboxToLua_QuaternionToRotationDegrees00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     (tolua_isvaluenil(tolua_S,1,&tolua_err) || !tolua_isusertype(tolua_S,1,"const Ogre::Quaternion",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Quaternion* quaternion = ((const Ogre::Quaternion*)  tolua_tousertype(tolua_S,1,0));
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  QuaternionToRotationDegrees(*quaternion);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'QuaternionToRotationDegrees'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: BtVector3ToVector3 */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BtVector3ToVector300
static int tolua_SandboxToLua_BtVector3ToVector300(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     (tolua_isvaluenil(tolua_S,1,&tolua_err) || !tolua_isusertype(tolua_S,1,"const btVector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const btVector3* vector = ((const btVector3*)  tolua_tousertype(tolua_S,1,0));
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  BtVector3ToVector3(*vector);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BtVector3ToVector3'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: BtQuaternionToQuaternion */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BtQuaternionToQuaternion00
static int tolua_SandboxToLua_BtQuaternionToQuaternion00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     (tolua_isvaluenil(tolua_S,1,&tolua_err) || !tolua_isusertype(tolua_S,1,"const btQuaternion",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const btQuaternion* orientation = ((const btQuaternion*)  tolua_tousertype(tolua_S,1,0));
  {
   Ogre::Quaternion tolua_ret = (Ogre::Quaternion)  BtQuaternionToQuaternion(*orientation);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Quaternion)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Quaternion));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BtQuaternionToQuaternion'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: GetForward */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_GetForward00
static int tolua_SandboxToLua_GetForward00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     (tolua_isvaluenil(tolua_S,1,&tolua_err) || !tolua_isusertype(tolua_S,1,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Vector3* forward = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0));
  {
   Ogre::Quaternion tolua_ret = (Ogre::Quaternion)  GetForward(*forward);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Quaternion)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Quaternion));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetForward'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addChild of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_addChild00
static int tolua_SandboxToLua_UIFrame_addChild00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"UIFrame",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
  UIFrame* pChild = ((UIFrame*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'addChild'", NULL);
#endif
  {
   self->addChild(pChild);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addChild'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setFontType of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_setFontType00
static int tolua_SandboxToLua_UIFrame_setFontType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
  UIFrame::FontType fontType = ((UIFrame::FontType) (int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setFontType'", NULL);
#endif
  {
   self->setFontType(fontType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setFontType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setText of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_setText00
static int tolua_SandboxToLua_UIFrame_setText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
  const std::string pText = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setText'", NULL);
#endif
  {
   self->setText(pText);
   tolua_pushcppstring(tolua_S,(const char*)pText);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setText'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMarkupText of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_setMarkupText00
static int tolua_SandboxToLua_UIFrame_setMarkupText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
  const std::string pText = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setMarkupText'", NULL);
#endif
  {
   self->setMarkupText(pText);
   tolua_pushcppstring(tolua_S,(const char*)pText);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setMarkupText'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setPosition of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_setPosition00
static int tolua_SandboxToLua_UIFrame_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector2",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector2* position = ((const Ogre::Vector2*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setPosition'", NULL);
#endif
  {
   self->setPosition(*position);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDimension of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_setDimension00
static int tolua_SandboxToLua_UIFrame_setDimension00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector2",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector2* dimension = ((const Ogre::Vector2*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setDimension'", NULL);
#endif
  {
   self->setDimension(*dimension);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setDimension'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setTextMargin of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_setTextMargin00
static int tolua_SandboxToLua_UIFrame_setTextMargin00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
   float top = ((  float)  tolua_tonumber(tolua_S,2,0));
   float left = ((  float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setTextMargin'", NULL);
#endif
  {
   self->setTextMargin(top,left);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setTextMargin'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setBackgroundColor of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_setBackgroundColor00
static int tolua_SandboxToLua_UIFrame_setBackgroundColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::ColourValue* colorValue = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setBackgroundColor'", NULL);
#endif
  {
   self->setBackgroundColor(*colorValue);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setBackgroundColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setGradientColor of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_setGradientColor00
static int tolua_SandboxToLua_UIFrame_setGradientColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::ColourValue",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
  Gorilla::Gradient direction = ((Gorilla::Gradient) (int)  tolua_tonumber(tolua_S,2,0));
  const Ogre::ColourValue* startColor = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,3,0));
  const Ogre::ColourValue* endColor = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setGradientColor'", NULL);
#endif
  {
   self->setGradientColor(direction,*startColor,*endColor);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setGradientColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setVisible of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_setVisible00
static int tolua_SandboxToLua_UIFrame_setVisible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
  bool visible = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setVisible'", NULL);
#endif
  {
   self->setVisible(visible);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setVisible'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: isVisible of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_isVisible00
static int tolua_SandboxToLua_UIFrame_isVisible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'isVisible'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->isVisible();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'isVisible'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDimension of class  UIFrame */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIFrame_GetDimension00
static int tolua_SandboxToLua_UIFrame_GetDimension00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIFrame",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIFrame* self = (UIFrame*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDimension'", NULL);
#endif
  {
   Ogre::Vector2 tolua_ret = (Ogre::Vector2)  self->GetDimension();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector2)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector2));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDimension'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateUIFrame of class  UIManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIManager_CreateUIFrame00
static int tolua_SandboxToLua_UIManager_CreateUIFrame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIManager* self = (UIManager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateUIFrame'", NULL);
#endif
  {
   UIFrame* tolua_ret = (UIFrame*)  self->CreateUIFrame(index);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"UIFrame");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateUIFrame'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMarkupColor of class  UIManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIManager_SetMarkupColor00
static int tolua_SandboxToLua_UIManager_SetMarkupColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIManager* self = (UIManager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  const Ogre::ColourValue* color = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMarkupColor'", NULL);
#endif
  {
   self->SetMarkupColor(index,*color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMarkupColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: isKeyDown of class  IPlayerInput */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_IPlayerInput_isKeyDown00
static int tolua_SandboxToLua_IPlayerInput_isKeyDown00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const IPlayerInput",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const IPlayerInput* self = (const IPlayerInput*)  tolua_tousertype(tolua_S,1,0);
  OIS::KeyCode key = ((OIS::KeyCode) (int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'isKeyDown'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->isKeyDown(key);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'isKeyDown'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: isKeyPressed of class  IPlayerInput */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_IPlayerInput_isKeyPressed00
static int tolua_SandboxToLua_IPlayerInput_isKeyPressed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const IPlayerInput",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const IPlayerInput* self = (const IPlayerInput*)  tolua_tousertype(tolua_S,1,0);
  OIS::KeyCode key = ((OIS::KeyCode) (int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'isKeyPressed'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->isKeyPressed(key);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'isKeyPressed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: isKeyReleased of class  IPlayerInput */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_IPlayerInput_isKeyReleased00
static int tolua_SandboxToLua_IPlayerInput_isKeyReleased00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const IPlayerInput",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const IPlayerInput* self = (const IPlayerInput*)  tolua_tousertype(tolua_S,1,0);
  OIS::KeyCode key = ((OIS::KeyCode) (int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'isKeyReleased'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->isKeyReleased(key);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'isKeyReleased'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCamera of class  CameraService */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_GetCamera00
static int tolua_SandboxToLua_CameraService_GetCamera00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCamera'", NULL);
#endif
  {
   Ogre::Camera* tolua_ret = (Ogre::Camera*)  self->GetCamera();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Camera");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCamera'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCameraUp of class  CameraService */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_GetCameraUp00
static int tolua_SandboxToLua_CameraService_GetCameraUp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCameraUp'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetCameraUp();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCameraUp'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCameraLeft of class  CameraService */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_GetCameraLeft00
static int tolua_SandboxToLua_CameraService_GetCameraLeft00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCameraLeft'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetCameraLeft();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCameraLeft'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCameraForward of class  CameraService */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_GetCameraForward00
static int tolua_SandboxToLua_CameraService_GetCameraForward00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCameraForward'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetCameraForward();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCameraForward'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCameraPosition of class  CameraService */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_GetCameraPosition00
static int tolua_SandboxToLua_CameraService_GetCameraPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCameraPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetCameraPosition();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCameraPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCameraRotation of class  CameraService */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_GetCameraRotation00
static int tolua_SandboxToLua_CameraService_GetCameraRotation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCameraRotation'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetCameraRotation();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCameraRotation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCameraOrientation of class  CameraService */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_GetCameraOrientation00
static int tolua_SandboxToLua_CameraService_GetCameraOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCameraOrientation'", NULL);
#endif
  {
   Ogre::Quaternion tolua_ret = (Ogre::Quaternion)  self->GetCameraOrientation();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Quaternion)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Quaternion));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCameraOrientation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRenderTime of class  CameraService */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_GetRenderTime00
static int tolua_SandboxToLua_CameraService_GetRenderTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRenderTime'", NULL);
#endif
  {
   long long tolua_ret = (long long)  self->GetRenderTime();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRenderTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSimulateTime of class  CameraService */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_GetSimulateTime00
static int tolua_SandboxToLua_CameraService_GetSimulateTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSimulateTime'", NULL);
#endif
  {
   long long tolua_ret = (long long)  self->GetSimulateTime();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSimulateTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTotalSimulateTime of class  CameraService */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_CameraService_GetTotalSimulateTime00
static int tolua_SandboxToLua_CameraService_GetTotalSimulateTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"CameraService",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  CameraService* self = (CameraService*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTotalSimulateTime'", NULL);
#endif
  {
   long long tolua_ret = (long long)  self->GetTotalSimulateTime();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTotalSimulateTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Init of class  AgentAnim */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnim_Init00
static int tolua_SandboxToLua_AgentAnim_Init00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnim",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnim* self = (AgentAnim*)  tolua_tousertype(tolua_S,1,0);
  float startTime = ((float)  tolua_tonumber(tolua_S,2,0.0f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Init'", NULL);
#endif
  {
   self->Init(startTime);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Init'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Clear of class  AgentAnim */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnim_Clear00
static int tolua_SandboxToLua_AgentAnim_Clear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnim",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnim* self = (AgentAnim*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Clear'", NULL);
#endif
  {
   self->Clear();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Clear'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetEnabled of class  AgentAnim */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnim_SetEnabled00
static int tolua_SandboxToLua_AgentAnim_SetEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnim",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnim* self = (AgentAnim*)  tolua_tousertype(tolua_S,1,0);
  bool enable = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetEnabled'", NULL);
#endif
  {
   self->SetEnabled(enable);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetLooping of class  AgentAnim */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnim_SetLooping00
static int tolua_SandboxToLua_AgentAnim_SetLooping00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnim",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnim* self = (AgentAnim*)  tolua_tousertype(tolua_S,1,0);
  bool looping = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetLooping'", NULL);
#endif
  {
   self->SetLooping(looping);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetLooping'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddTime of class  AgentAnim */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnim_AddTime00
static int tolua_SandboxToLua_AgentAnim_AddTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnim",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnim* self = (AgentAnim*)  tolua_tousertype(tolua_S,1,0);
  float deltaTime = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddTime'", NULL);
#endif
  {
   self->AddTime(deltaTime);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTime of class  AgentAnim */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnim_GetTime00
static int tolua_SandboxToLua_AgentAnim_GetTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnim",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnim* self = (AgentAnim*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTime'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetTime();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLength of class  AgentAnim */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnim_GetLength00
static int tolua_SandboxToLua_AgentAnim_GetLength00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnim",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnim* self = (AgentAnim*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLength'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetLength();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLength'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetWeight of class  AgentAnim */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnim_GetWeight00
static int tolua_SandboxToLua_AgentAnim_GetWeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnim",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnim* self = (AgentAnim*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetWeight'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetWeight();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetWeight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetWeight of class  AgentAnim */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnim_SetWeight00
static int tolua_SandboxToLua_AgentAnim_SetWeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnim",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnim* self = (AgentAnim*)  tolua_tousertype(tolua_S,1,0);
  float weight = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetWeight'", NULL);
#endif
  {
   self->SetWeight(weight);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetWeight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RequestState of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_RequestState00
static int tolua_SandboxToLua_AgentAnimStateMachine_RequestState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
  int stateId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RequestState'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RequestState(stateId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RequestState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RequestState of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_RequestState01
static int tolua_SandboxToLua_AgentAnimStateMachine_RequestState01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
  const std::string stateName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RequestState'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RequestState(stateName);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)stateName);
  }
 }
 return 2;
tolua_lerror:
 return tolua_SandboxToLua_AgentAnimStateMachine_RequestState00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurrStateID of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateID00
static int tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurrStateID'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetCurrStateID();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurrStateID'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurrStateName of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateName00
static int tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurrStateName'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->GetCurrStateName();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurrStateName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNextStateName of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_GetNextStateName00
static int tolua_SandboxToLua_AgentAnimStateMachine_GetNextStateName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNextStateName'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->GetNextStateName();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNextStateName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDesiredStateName of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_GetDesiredStateName00
static int tolua_SandboxToLua_AgentAnimStateMachine_GetDesiredStateName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDesiredStateName'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->GetDesiredStateName();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDesiredStateName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasNextState of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_HasNextState00
static int tolua_SandboxToLua_AgentAnimStateMachine_HasNextState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasNextState'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasNextState();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasNextState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsTransitioning of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_IsTransitioning00
static int tolua_SandboxToLua_AgentAnimStateMachine_IsTransitioning00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsTransitioning'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsTransitioning();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsTransitioning'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsCurrentState of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_IsCurrentState00
static int tolua_SandboxToLua_AgentAnimStateMachine_IsCurrentState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
  const std::string stateName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsCurrentState'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsCurrentState(stateName);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)stateName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsCurrentState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsNextState of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_IsNextState00
static int tolua_SandboxToLua_AgentAnimStateMachine_IsNextState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
  const std::string stateName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsNextState'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsNextState(stateName);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)stateName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsNextState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurrStateTime of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateTime00
static int tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateTime00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurrStateTime'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetCurrStateTime();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurrStateTime'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurrStateLength of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateLength00
static int tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateLength00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurrStateLength'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetCurrStateLength();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurrStateLength'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurrStateProgress of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateProgress00
static int tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateProgress00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurrStateProgress'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetCurrStateProgress();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurrStateProgress'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddState of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_AddState00
static int tolua_SandboxToLua_AgentAnimStateMachine_AddState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"AgentAnim",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
  const std::string name = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  AgentAnim* animation = ((AgentAnim*)  tolua_tousertype(tolua_S,3,0));
  bool looping = ((bool)  tolua_toboolean(tolua_S,4,false));
  float rate = ((float)  tolua_tonumber(tolua_S,5,1.0f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddState'", NULL);
#endif
  {
   self->AddState(name,animation,looping,rate);
   tolua_pushcppstring(tolua_S,(const char*)name);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddTransition of class  AgentAnimStateMachine */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAnimStateMachine_AddTransition00
static int tolua_SandboxToLua_AgentAnimStateMachine_AddTransition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAnimStateMachine",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAnimStateMachine* self = (AgentAnimStateMachine*)  tolua_tousertype(tolua_S,1,0);
  const std::string fromState = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const std::string toState = ((const std::string)  tolua_tocppstring(tolua_S,3,0));
  float blendOutWindow = ((float)  tolua_tonumber(tolua_S,4,0.0f));
  float duration = ((float)  tolua_tonumber(tolua_S,5,0.2f));
  float blendInWindow = ((float)  tolua_tonumber(tolua_S,6,0.0f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddTransition'", NULL);
#endif
  {
   self->AddTransition(fromState,toState,blendOutWindow,duration,blendInWindow);
   tolua_pushcppstring(tolua_S,(const char*)fromState);
   tolua_pushcppstring(tolua_S,(const char*)toState);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddTransition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetHealth of class  AgentAttrib */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAttrib_SetHealth00
static int tolua_SandboxToLua_AgentAttrib_SetHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAttrib",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAttrib* self = (AgentAttrib*)  tolua_tousertype(tolua_S,1,0);
   float health = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetHealth'", NULL);
#endif
  {
   self->SetHealth(health);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetHealth of class  AgentAttrib */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAttrib_GetHealth00
static int tolua_SandboxToLua_AgentAttrib_GetHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentAttrib",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentAttrib* self = (const AgentAttrib*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetHealth'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetHealth();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMaxHealth of class  AgentAttrib */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAttrib_SetMaxHealth00
static int tolua_SandboxToLua_AgentAttrib_SetMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAttrib",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAttrib* self = (AgentAttrib*)  tolua_tousertype(tolua_S,1,0);
   float maxHealth = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMaxHealth'", NULL);
#endif
  {
   self->SetMaxHealth(maxHealth);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMaxHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMaxHealth of class  AgentAttrib */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAttrib_GetMaxHealth00
static int tolua_SandboxToLua_AgentAttrib_GetMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentAttrib",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentAttrib* self = (const AgentAttrib*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMaxHealth'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetMaxHealth();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaxHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetStanceType of class  AgentAttrib */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAttrib_SetStanceType00
static int tolua_SandboxToLua_AgentAttrib_SetStanceType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentAttrib",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentAttrib* self = (AgentAttrib*)  tolua_tousertype(tolua_S,1,0);
  int stanceType = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetStanceType'", NULL);
#endif
  {
   self->SetStanceType(stanceType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetStanceType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStanceType of class  AgentAttrib */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentAttrib_GetStanceType00
static int tolua_SandboxToLua_AgentAttrib_GetStanceType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentAttrib",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentAttrib* self = (const AgentAttrib*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStanceType'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetStanceType();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStanceType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBodyAnimation of class  AnimComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AnimComponent_GetBodyAnimation00
static int tolua_SandboxToLua_AnimComponent_GetBodyAnimation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AnimComponent",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AnimComponent* self = (AnimComponent*)  tolua_tousertype(tolua_S,1,0);
  const char* animationName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBodyAnimation'", NULL);
#endif
  {
   AgentAnim* tolua_ret = (AgentAnim*)  self->GetBodyAnimation(animationName);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentAnim");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBodyAnimation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBodyAsm of class  AnimComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AnimComponent_GetBodyAsm00
static int tolua_SandboxToLua_AnimComponent_GetBodyAsm00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AnimComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AnimComponent* self = (const AnimComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBodyAsm'", NULL);
#endif
  {
   AgentAnimStateMachine* tolua_ret = (AgentAnimStateMachine*)  self->GetBodyAsm();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentAnimStateMachine");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBodyAsm'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetWeaponAnimation of class  AnimComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AnimComponent_GetWeaponAnimation00
static int tolua_SandboxToLua_AnimComponent_GetWeaponAnimation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AnimComponent",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AnimComponent* self = (AnimComponent*)  tolua_tousertype(tolua_S,1,0);
  const char* animationName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetWeaponAnimation'", NULL);
#endif
  {
   AgentAnim* tolua_ret = (AgentAnim*)  self->GetWeaponAnimation(animationName);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentAnim");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetWeaponAnimation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetWeaponAsm of class  AnimComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AnimComponent_GetWeaponAsm00
static int tolua_SandboxToLua_AnimComponent_GetWeaponAsm00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AnimComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AnimComponent* self = (const AnimComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetWeaponAsm'", NULL);
#endif
  {
   AgentAnimStateMachine* tolua_ret = (AgentAnimStateMachine*)  self->GetWeaponAsm();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentAnimStateMachine");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetWeaponAsm'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasNextAnim of class  AnimComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AnimComponent_HasNextAnim00
static int tolua_SandboxToLua_AnimComponent_HasNextAnim00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AnimComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AnimComponent* self = (const AnimComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasNextAnim'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasNextAnim();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasNextAnim'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsAnimReadyForMove of class  AnimComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AnimComponent_IsAnimReadyForMove00
static int tolua_SandboxToLua_AnimComponent_IsAnimReadyForMove00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AnimComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AnimComponent* self = (const AnimComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsAnimReadyForMove'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsAnimReadyForMove();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsAnimReadyForMove'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsAnimReadyForShoot of class  AnimComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AnimComponent_IsAnimReadyForShoot00
static int tolua_SandboxToLua_AnimComponent_IsAnimReadyForShoot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AnimComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AnimComponent* self = (const AnimComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsAnimReadyForShoot'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsAnimReadyForShoot();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsAnimReadyForShoot'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjId of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_GetObjId00
static int tolua_SandboxToLua_BaseObject_GetObjId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BaseObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BaseObject* self = (const BaseObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjId'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->GetObjId();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTeamId of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_GetTeamId00
static int tolua_SandboxToLua_BaseObject_GetTeamId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BaseObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BaseObject* self = (const BaseObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTeamId'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->GetTeamId();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTeamId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetTeamId of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_SetTeamId00
static int tolua_SandboxToLua_BaseObject_SetTeamId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BaseObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BaseObject* self = (BaseObject*)  tolua_tousertype(tolua_S,1,0);
  unsigned int teamId = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetTeamId'", NULL);
#endif
  {
   self->SetTeamId(teamId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetTeamId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasComponent of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_HasComponent00
static int tolua_SandboxToLua_BaseObject_HasComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BaseObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BaseObject* self = (const BaseObject*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasComponent'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasComponent(key);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasComponent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetComponentCount of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_GetComponentCount00
static int tolua_SandboxToLua_BaseObject_GetComponentCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BaseObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BaseObject* self = (const BaseObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetComponentCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetComponentCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetComponentCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: BuildComponentDebugString of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_BuildComponentDebugString00
static int tolua_SandboxToLua_BaseObject_BuildComponentDebugString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BaseObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BaseObject* self = (const BaseObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'BuildComponentDebugString'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->BuildComponentDebugString();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BuildComponentDebugString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAIComponent of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_GetAIComponent00
static int tolua_SandboxToLua_BaseObject_GetAIComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BaseObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BaseObject* self = (BaseObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAIComponent'", NULL);
#endif
  {
   AIController* tolua_ret = (AIController*)  self->GetAIComponent();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AIController");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAIComponent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetWeaponComponent of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_GetWeaponComponent00
static int tolua_SandboxToLua_BaseObject_GetWeaponComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BaseObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BaseObject* self = (BaseObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetWeaponComponent'", NULL);
#endif
  {
   WeaponComponent* tolua_ret = (WeaponComponent*)  self->GetWeaponComponent();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"WeaponComponent");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetWeaponComponent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAnimComponent of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_GetAnimComponent00
static int tolua_SandboxToLua_BaseObject_GetAnimComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BaseObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BaseObject* self = (BaseObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAnimComponent'", NULL);
#endif
  {
   AnimComponent* tolua_ret = (AnimComponent*)  self->GetAnimComponent();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AnimComponent");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAnimComponent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAttribComponent of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_GetAttribComponent00
static int tolua_SandboxToLua_BaseObject_GetAttribComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BaseObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BaseObject* self = (BaseObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAttribComponent'", NULL);
#endif
  {
   AgentAttrib* tolua_ret = (AgentAttrib*)  self->GetAttribComponent();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentAttrib");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAttribComponent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setPosition of class  BlockObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BlockObject_setPosition00
static int tolua_SandboxToLua_BlockObject_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BlockObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BlockObject* self = (BlockObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setPosition'", NULL);
#endif
  {
   self->setPosition(*position);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setRotation of class  BlockObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BlockObject_setRotation00
static int tolua_SandboxToLua_BlockObject_setRotation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BlockObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BlockObject* self = (BlockObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setRotation'", NULL);
#endif
  {
   self->setRotation(*position);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setRotation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setOrientation of class  BlockObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BlockObject_setOrientation00
static int tolua_SandboxToLua_BlockObject_setOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BlockObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Quaternion",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BlockObject* self = (BlockObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Quaternion* quaternion = ((const Ogre::Quaternion*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setOrientation'", NULL);
#endif
  {
   self->setOrientation(*quaternion);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setOrientation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMass of class  BlockObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BlockObject_GetMass00
static int tolua_SandboxToLua_BlockObject_GetMass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BlockObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BlockObject* self = (const BlockObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMass'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetMass();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMass'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMass of class  BlockObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BlockObject_SetMass00
static int tolua_SandboxToLua_BlockObject_SetMass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BlockObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BlockObject* self = (BlockObject*)  tolua_tousertype(tolua_S,1,0);
  const float mass = ((const float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMass'", NULL);
#endif
  {
   self->SetMass(mass);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMass'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRadius of class  BlockObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BlockObject_GetRadius00
static int tolua_SandboxToLua_BlockObject_GetRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BlockObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BlockObject* self = (const BlockObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRadius'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetRadius();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRadius'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPosition of class  BlockObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BlockObject_GetPosition00
static int tolua_SandboxToLua_BlockObject_GetPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BlockObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BlockObject* self = (const BlockObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetPosition();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaterial of class  BlockObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BlockObject_setMaterial00
static int tolua_SandboxToLua_BlockObject_setMaterial00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BlockObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BlockObject* self = (BlockObject*)  tolua_tousertype(tolua_S,1,0);
  const std::string materialName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setMaterial'", NULL);
#endif
  {
   self->setMaterial(materialName);
   tolua_pushcppstring(tolua_S,(const char*)materialName);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setMaterial'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: applyImpulse of class  BlockObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BlockObject_applyImpulse00
static int tolua_SandboxToLua_BlockObject_applyImpulse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BlockObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BlockObject* self = (BlockObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* impulse = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'applyImpulse'", NULL);
#endif
  {
   self->applyImpulse(*impulse);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'applyImpulse'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: applyAngularImpulse of class  BlockObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BlockObject_applyAngularImpulse00
static int tolua_SandboxToLua_BlockObject_applyAngularImpulse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BlockObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BlockObject* self = (BlockObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* aImpulse = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'applyAngularImpulse'", NULL);
#endif
  {
   self->applyAngularImpulse(*aImpulse);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'applyAngularImpulse'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: initBody of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_initBody00
static int tolua_SandboxToLua_AgentObject_initBody00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const std::string meshFile = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'initBody'", NULL);
#endif
  {
   self->initBody(meshFile);
   tolua_pushcppstring(tolua_S,(const char*)meshFile);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'initBody'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getBody of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_getBody00
static int tolua_SandboxToLua_AgentObject_getBody00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getBody'", NULL);
#endif
  {
   AgentObject* tolua_ret = (AgentObject*)  self->getBody();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getBody'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAnimation of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetAnimation00
static int tolua_SandboxToLua_AgentObject_GetAnimation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const char* animationName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAnimation'", NULL);
#endif
  {
   AgentAnim* tolua_ret = (AgentAnim*)  self->GetAnimation(animationName);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentAnim");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAnimation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectASM of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetObjectASM00
static int tolua_SandboxToLua_AgentObject_GetObjectASM00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectASM'", NULL);
#endif
  {
   AgentAnimStateMachine* tolua_ret = (AgentAnimStateMachine*)  self->GetObjectASM();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentAnimStateMachine");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectASM'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetRenderVisible of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetRenderVisible00
static int tolua_SandboxToLua_AgentObject_SetRenderVisible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  bool visible = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRenderVisible'", NULL);
#endif
  {
   self->SetRenderVisible(visible);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetRenderVisible'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getAgentType of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_getAgentType00
static int tolua_SandboxToLua_AgentObject_getAgentType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getAgentType'", NULL);
#endif
  {
   AGENT_OBJ_TYPE tolua_ret = (AGENT_OBJ_TYPE)  self->getAgentType();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getAgentType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setAgentType of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_setAgentType00
static int tolua_SandboxToLua_AgentObject_setAgentType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  AGENT_OBJ_TYPE agentType = ((AGENT_OBJ_TYPE) (int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setAgentType'", NULL);
#endif
  {
   self->setAgentType(agentType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setAgentType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPosition of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetPosition00
static int tolua_SandboxToLua_AgentObject_GetPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetPosition();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetOrientation of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetOrientation00
static int tolua_SandboxToLua_AgentObject_GetOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetOrientation'", NULL);
#endif
  {
   Ogre::Quaternion tolua_ret = (Ogre::Quaternion)  self->GetOrientation();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Quaternion)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Quaternion));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetOrientation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetUp of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetUp00
static int tolua_SandboxToLua_AgentObject_GetUp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetUp'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetUp();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetUp'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLeft of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetLeft00
static int tolua_SandboxToLua_AgentObject_GetLeft00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLeft'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetLeft();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLeft'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetForward of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetForward00
static int tolua_SandboxToLua_AgentObject_GetForward00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetForward'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetForward();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetForward'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setPosition of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_setPosition00
static int tolua_SandboxToLua_AgentObject_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setPosition'", NULL);
#endif
  {
   self->setPosition(*position);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setRotation of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_setRotation00
static int tolua_SandboxToLua_AgentObject_setRotation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* rotation = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setRotation'", NULL);
#endif
  {
   self->setRotation(*rotation);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setRotation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setOrientation of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_setOrientation00
static int tolua_SandboxToLua_AgentObject_setOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Quaternion",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Quaternion* quaternion = ((const Ogre::Quaternion*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setOrientation'", NULL);
#endif
  {
   self->setOrientation(*quaternion);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setOrientation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetForward of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetForward00
static int tolua_SandboxToLua_AgentObject_SetForward00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* forward = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetForward'", NULL);
#endif
  {
   self->SetForward(*forward);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetForward'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetVelocity of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetVelocity00
static int tolua_SandboxToLua_AgentObject_SetVelocity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* velocity = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetVelocity'", NULL);
#endif
  {
   self->SetVelocity(*velocity);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetVelocity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetTarget of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetTarget00
static int tolua_SandboxToLua_AgentObject_SetTarget00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* targetPos = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetTarget'", NULL);
#endif
  {
   self->SetTarget(*targetPos);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetTarget'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetTargetRadius of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetTargetRadius00
static int tolua_SandboxToLua_AgentObject_SetTargetRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float radius = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetTargetRadius'", NULL);
#endif
  {
   self->SetTargetRadius(radius);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetTargetRadius'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTarget of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetTarget00
static int tolua_SandboxToLua_AgentObject_GetTarget00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTarget'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetTarget();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTarget'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetVelocity of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetVelocity00
static int tolua_SandboxToLua_AgentObject_GetVelocity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetVelocity'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetVelocity();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetVelocity'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMass of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetMass00
static int tolua_SandboxToLua_AgentObject_SetMass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float mass = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMass'", NULL);
#endif
  {
   self->SetMass(mass);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMass'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetHeight of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetHeight00
static int tolua_SandboxToLua_AgentObject_SetHeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float height = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetHeight'", NULL);
#endif
  {
   self->SetHeight(height);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetHeight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetRadius of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetRadius00
static int tolua_SandboxToLua_AgentObject_SetRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float radius = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRadius'", NULL);
#endif
  {
   self->SetRadius(radius);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetRadius'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetSpeed of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetSpeed00
static int tolua_SandboxToLua_AgentObject_SetSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float speed = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetSpeed'", NULL);
#endif
  {
   self->SetSpeed(speed);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetHealth of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetHealth00
static int tolua_SandboxToLua_AgentObject_SetHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float health = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetHealth'", NULL);
#endif
  {
   self->SetHealth(health);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMass of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetMass00
static int tolua_SandboxToLua_AgentObject_GetMass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMass'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetMass();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMass'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSpeed of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetSpeed00
static int tolua_SandboxToLua_AgentObject_GetSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSpeed'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetSpeed();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetHeight of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetHeight00
static int tolua_SandboxToLua_AgentObject_GetHeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetHeight'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetHeight();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetHeight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRadius of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetRadius00
static int tolua_SandboxToLua_AgentObject_GetRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRadius'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetRadius();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRadius'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetHealth of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetHealth00
static int tolua_SandboxToLua_AgentObject_GetHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetHealth'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetHealth();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMaxForce of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetMaxForce00
static int tolua_SandboxToLua_AgentObject_SetMaxForce00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float maxForce = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMaxForce'", NULL);
#endif
  {
   self->SetMaxForce(maxForce);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMaxForce'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMaxSpeed of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetMaxSpeed00
static int tolua_SandboxToLua_AgentObject_SetMaxSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float maxSpeed = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMaxSpeed'", NULL);
#endif
  {
   self->SetMaxSpeed(maxSpeed);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMaxSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMaxForce of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetMaxForce00
static int tolua_SandboxToLua_AgentObject_GetMaxForce00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMaxForce'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetMaxForce();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaxForce'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMaxSpeed of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetMaxSpeed00
static int tolua_SandboxToLua_AgentObject_GetMaxSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMaxSpeed'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetMaxSpeed();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaxSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTargetRadius of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetTargetRadius00
static int tolua_SandboxToLua_AgentObject_GetTargetRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTargetRadius'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetTargetRadius();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTargetRadius'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PredictFuturePosition of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_PredictFuturePosition00
static int tolua_SandboxToLua_AgentObject_PredictFuturePosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float predictionTime = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PredictFuturePosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->PredictFuturePosition(predictionTime);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PredictFuturePosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToPosition of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_ForceToPosition00
static int tolua_SandboxToLua_AgentObject_ForceToPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToPosition(*position);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceToPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToFollowPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_ForceToFollowPath00
static int tolua_SandboxToLua_AgentObject_ForceToFollowPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float predictionTime = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToFollowPath'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToFollowPath(predictionTime);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceToFollowPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToStayOnPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_ForceToStayOnPath00
static int tolua_SandboxToLua_AgentObject_ForceToStayOnPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float predictionTime = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToStayOnPath'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToStayOnPath(predictionTime);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceToStayOnPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToWander of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_ForceToWander00
static int tolua_SandboxToLua_AgentObject_ForceToWander00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float deltaMilliSeconds = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToWander'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToWander(deltaMilliSeconds);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceToWander'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToTargetSpeed of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_ForceToTargetSpeed00
static int tolua_SandboxToLua_AgentObject_ForceToTargetSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float targetSpeed = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToTargetSpeed'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToTargetSpeed(targetSpeed);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceToTargetSpeed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToAvoidAgents of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_ForceToAvoidAgents00
static int tolua_SandboxToLua_AgentObject_ForceToAvoidAgents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float predictionTime = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToAvoidAgents'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToAvoidAgents(predictionTime);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceToAvoidAgents'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToAvoidObjects of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_ForceToAvoidObjects00
static int tolua_SandboxToLua_AgentObject_ForceToAvoidObjects00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
   float predictionTime = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToAvoidObjects'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToAvoidObjects(predictionTime);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceToAvoidObjects'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToCombine of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_ForceToCombine00
static int tolua_SandboxToLua_AgentObject_ForceToCombine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const std::vector<AgentObject*>",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const std::vector<AgentObject*>* agents = ((const std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,2,0));
   float distance = ((  float)  tolua_tonumber(tolua_S,3,0));
   float angle = ((  float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToCombine'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToCombine(*agents,distance,angle);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceToCombine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToSeparate of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_ForceToSeparate00
static int tolua_SandboxToLua_AgentObject_ForceToSeparate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const std::vector<AgentObject*>",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const std::vector<AgentObject*>* agents = ((const std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,2,0));
   float distance = ((  float)  tolua_tonumber(tolua_S,3,0));
   float angle = ((  float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToSeparate'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToSeparate(*agents,distance,angle);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ForceToSeparate'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ApplyForce of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_ApplyForce00
static int tolua_SandboxToLua_AgentObject_ApplyForce00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* force = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ApplyForce'", NULL);
#endif
  {
   self->ApplyForce(*force);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ApplyForce'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_SetPath00
static int tolua_SandboxToLua_AgentObject_SetPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const std::vector<Ogre::Vector3>",0,&tolua_err)) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const std::vector<Ogre::Vector3>* points = ((const std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,2,0));
  bool cyclic = ((bool)  tolua_toboolean(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPath'", NULL);
#endif
  {
   self->SetPath(*points,cyclic);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetPath00
static int tolua_SandboxToLua_AgentObject_GetPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPath'", NULL);
#endif
  {
   const std::vector<Ogre::Vector3>& tolua_ret = (const std::vector<Ogre::Vector3>&)  self->GetPath();
    tolua_pushusertype(tolua_S,(void*)&tolua_ret,"const std::vector<Ogre::Vector3>");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_HasPath00
static int tolua_SandboxToLua_AgentObject_HasPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasPath'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasPath();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDistanceAlongPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetDistanceAlongPath00
static int tolua_SandboxToLua_AgentObject_GetDistanceAlongPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDistanceAlongPath'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetDistanceAlongPath(*position);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDistanceAlongPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetNearestPointOnPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetNearestPointOnPath00
static int tolua_SandboxToLua_AgentObject_GetNearestPointOnPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetNearestPointOnPath'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetNearestPointOnPath(*position);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetNearestPointOnPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetPointOnPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetPointOnPath00
static int tolua_SandboxToLua_AgentObject_GetPointOnPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AgentObject* self = (const AgentObject*)  tolua_tousertype(tolua_S,1,0);
  const float distance = ((const float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetPointOnPath'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetPointOnPath(distance);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetPointOnPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsMoving of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_IsMoving00
static int tolua_SandboxToLua_AgentObject_IsMoving00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsMoving'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsMoving();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsMoving'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsFalling of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_IsFalling00
static int tolua_SandboxToLua_AgentObject_IsFalling00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsFalling'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsFalling();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsFalling'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: OnGround of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_OnGround00
static int tolua_SandboxToLua_AgentObject_OnGround00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'OnGround'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->OnGround();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'OnGround'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetUseCppFSM of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetUseCppFSM00
static int tolua_SandboxToLua_AgentObject_GetUseCppFSM00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetUseCppFSM'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetUseCppFSM();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetUseCppFSM'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurStateId of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetCurStateId00
static int tolua_SandboxToLua_AgentObject_GetCurStateId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurStateId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetCurStateId();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurStateId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCurStateName of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetCurStateName00
static int tolua_SandboxToLua_AgentObject_GetCurStateName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetCurStateName'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->GetCurStateName();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetCurStateName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInput of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_GetInput00
static int tolua_SandboxToLua_AgentObject_GetInput00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetInput'", NULL);
#endif
  {
   IPlayerInput* tolua_ret = (IPlayerInput*)  self->GetInput();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"IPlayerInput");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInput'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RequestState of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_RequestState00
static int tolua_SandboxToLua_AgentObject_RequestState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  int soldierState = ((int)  tolua_tonumber(tolua_S,2,0));
  bool forceUpdate = ((bool)  tolua_toboolean(tolua_S,3,false));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RequestState'", NULL);
#endif
  {
   self->RequestState(soldierState,forceUpdate);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RequestState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasNextAnim of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_HasNextAnim00
static int tolua_SandboxToLua_AgentObject_HasNextAnim00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasNextAnim'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasNextAnim();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasNextAnim'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsAnimReadyForMove of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_IsAnimReadyForMove00
static int tolua_SandboxToLua_AgentObject_IsAnimReadyForMove00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsAnimReadyForMove'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsAnimReadyForMove();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsAnimReadyForMove'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsAnimReadyForShoot of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_IsAnimReadyForShoot00
static int tolua_SandboxToLua_AgentObject_IsAnimReadyForShoot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsAnimReadyForShoot'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsAnimReadyForShoot();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsAnimReadyForShoot'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAnimation of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_GetAnimation00
static int tolua_SandboxToLua_WeaponComponent_GetAnimation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"WeaponComponent",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  WeaponComponent* self = (WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
  const char* animationName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAnimation'", NULL);
#endif
  {
   AgentAnim* tolua_ret = (AgentAnim*)  self->GetAnimation(animationName);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentAnim");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAnimation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectASM of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_GetObjectASM00
static int tolua_SandboxToLua_WeaponComponent_GetObjectASM00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const WeaponComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const WeaponComponent* self = (const WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectASM'", NULL);
#endif
  {
   AgentAnimStateMachine* tolua_ret = (AgentAnimStateMachine*)  self->GetObjectASM();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentAnimStateMachine");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectASM'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SyncToHandBone of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_SyncToHandBone00
static int tolua_SandboxToLua_WeaponComponent_SyncToHandBone00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"WeaponComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  WeaponComponent* self = (WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SyncToHandBone'", NULL);
#endif
  {
   self->SyncToHandBone();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SyncToHandBone'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ShootBullet of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_ShootBullet00
static int tolua_SandboxToLua_WeaponComponent_ShootBullet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"WeaponComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  WeaponComponent* self = (WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ShootBullet'", NULL);
#endif
  {
   self->ShootBullet();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ShootBullet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetAmmo of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_SetAmmo00
static int tolua_SandboxToLua_WeaponComponent_SetAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"WeaponComponent",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  WeaponComponent* self = (WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
  int ammo = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetAmmo'", NULL);
#endif
  {
   self->SetAmmo(ammo);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAmmo of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_GetAmmo00
static int tolua_SandboxToLua_WeaponComponent_GetAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const WeaponComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const WeaponComponent* self = (const WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAmmo'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetAmmo();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMaxAmmo of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_SetMaxAmmo00
static int tolua_SandboxToLua_WeaponComponent_SetMaxAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"WeaponComponent",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  WeaponComponent* self = (WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
  int maxAmmo = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMaxAmmo'", NULL);
#endif
  {
   self->SetMaxAmmo(maxAmmo);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMaxAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMaxAmmo of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_GetMaxAmmo00
static int tolua_SandboxToLua_WeaponComponent_GetMaxAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const WeaponComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const WeaponComponent* self = (const WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMaxAmmo'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetMaxAmmo();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaxAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasAmmo of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_HasAmmo00
static int tolua_SandboxToLua_WeaponComponent_HasAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const WeaponComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const WeaponComponent* self = (const WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasAmmo'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasAmmo();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ConsumeAmmo of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_ConsumeAmmo00
static int tolua_SandboxToLua_WeaponComponent_ConsumeAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"WeaponComponent",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  WeaponComponent* self = (WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
  int amount = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ConsumeAmmo'", NULL);
#endif
  {
   self->ConsumeAmmo(amount);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ConsumeAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RestoreAmmo of class  WeaponComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_WeaponComponent_RestoreAmmo00
static int tolua_SandboxToLua_WeaponComponent_RestoreAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"WeaponComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  WeaponComponent* self = (WeaponComponent*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RestoreAmmo'", NULL);
#endif
  {
   self->RestoreAmmo();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RestoreAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: initWeapon of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_initWeapon00
static int tolua_SandboxToLua_SoldierObject_initWeapon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  const std::string meshFile = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'initWeapon'", NULL);
#endif
  {
   self->initWeapon(meshFile);
   tolua_pushcppstring(tolua_S,(const char*)meshFile);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'initWeapon'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getWeapon of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_getWeapon00
static int tolua_SandboxToLua_SoldierObject_getWeapon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getWeapon'", NULL);
#endif
  {
   WeaponComponent* tolua_ret = (WeaponComponent*)  self->getWeapon();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"WeaponComponent");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getWeapon'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetRenderVisible of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_SetRenderVisible00
static int tolua_SandboxToLua_SoldierObject_SetRenderVisible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  bool visible = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRenderVisible'", NULL);
#endif
  {
   self->SetRenderVisible(visible);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetRenderVisible'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: changeStanceType of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_changeStanceType00
static int tolua_SandboxToLua_SoldierObject_changeStanceType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  int stanceType = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'changeStanceType'", NULL);
#endif
  {
   self->changeStanceType(stanceType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'changeStanceType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getStanceType of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_getStanceType00
static int tolua_SandboxToLua_SoldierObject_getStanceType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getStanceType'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getStanceType();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getStanceType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ShootBullet of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_ShootBullet00
static int tolua_SandboxToLua_SoldierObject_ShootBullet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ShootBullet'", NULL);
#endif
  {
   self->ShootBullet();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ShootBullet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInput of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_GetInput00
static int tolua_SandboxToLua_SoldierObject_GetInput00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetInput'", NULL);
#endif
  {
   IPlayerInput* tolua_ret = (IPlayerInput*)  self->GetInput();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"IPlayerInput");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInput'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RequestState of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_RequestState00
static int tolua_SandboxToLua_SoldierObject_RequestState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  int soldierState = ((int)  tolua_tonumber(tolua_S,2,0));
  bool forceUpdate = ((bool)  tolua_toboolean(tolua_S,3,false));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RequestState'", NULL);
#endif
  {
   self->RequestState(soldierState,forceUpdate);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RequestState'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasNextAnim of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_HasNextAnim00
static int tolua_SandboxToLua_SoldierObject_HasNextAnim00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasNextAnim'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasNextAnim();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasNextAnim'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsAnimReadyForMove of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_IsAnimReadyForMove00
static int tolua_SandboxToLua_SoldierObject_IsAnimReadyForMove00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsAnimReadyForMove'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsAnimReadyForMove();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsAnimReadyForMove'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsAnimReadyForShoot of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_IsAnimReadyForShoot00
static int tolua_SandboxToLua_SoldierObject_IsAnimReadyForShoot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsAnimReadyForShoot'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsAnimReadyForShoot();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsAnimReadyForShoot'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAnimController of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_GetAnimController00
static int tolua_SandboxToLua_SoldierObject_GetAnimController00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAnimController'", NULL);
#endif
  {
   SoldierAnimController* tolua_ret = (SoldierAnimController*)  self->GetAnimController();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SoldierAnimController");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAnimController'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAIController of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_GetAIController00
static int tolua_SandboxToLua_SoldierObject_GetAIController00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAIController'", NULL);
#endif
  {
   AIController* tolua_ret = (AIController*)  self->GetAIController();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AIController");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAIController'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAI of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_GetAI00
static int tolua_SandboxToLua_SoldierObject_GetAI00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAI'", NULL);
#endif
  {
   AIController* tolua_ret = (AIController*)  self->GetAI();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AIController");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAI'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMaxHealth of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_SetMaxHealth00
static int tolua_SandboxToLua_SoldierObject_SetMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
   float maxHealth = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMaxHealth'", NULL);
#endif
  {
   self->SetMaxHealth(maxHealth);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMaxHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMaxHealth of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_GetMaxHealth00
static int tolua_SandboxToLua_SoldierObject_GetMaxHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMaxHealth'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetMaxHealth();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaxHealth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetAmmo of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_SetAmmo00
static int tolua_SandboxToLua_SoldierObject_SetAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  int ammo = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetAmmo'", NULL);
#endif
  {
   self->SetAmmo(ammo);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAmmo of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_GetAmmo00
static int tolua_SandboxToLua_SoldierObject_GetAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAmmo'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetAmmo();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMaxAmmo of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_SetMaxAmmo00
static int tolua_SandboxToLua_SoldierObject_SetMaxAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  int maxAmmo = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMaxAmmo'", NULL);
#endif
  {
   self->SetMaxAmmo(maxAmmo);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMaxAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMaxAmmo of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_GetMaxAmmo00
static int tolua_SandboxToLua_SoldierObject_GetMaxAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMaxAmmo'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetMaxAmmo();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaxAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasAmmo of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_HasAmmo00
static int tolua_SandboxToLua_SoldierObject_HasAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasAmmo'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasAmmo();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ConsumeAmmo of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_ConsumeAmmo00
static int tolua_SandboxToLua_SoldierObject_ConsumeAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  int amount = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ConsumeAmmo'", NULL);
#endif
  {
   self->ConsumeAmmo(amount);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ConsumeAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RestoreAmmo of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_RestoreAmmo00
static int tolua_SandboxToLua_SoldierObject_RestoreAmmo00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RestoreAmmo'", NULL);
#endif
  {
   self->RestoreAmmo();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RestoreAmmo'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasEnemy of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_HasEnemy00
static int tolua_SandboxToLua_SoldierObject_HasEnemy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasEnemy'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasEnemy();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasEnemy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasEnemy of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_HasEnemy01
static int tolua_SandboxToLua_SoldierObject_HasEnemy01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasEnemy'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasEnemy(navMeshName);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 2;
tolua_lerror:
 return tolua_SandboxToLua_SoldierObject_HasEnemy00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: CanShootEnemy of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_CanShootEnemy00
static int tolua_SandboxToLua_SoldierObject_CanShootEnemy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CanShootEnemy'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CanShootEnemy();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CanShootEnemy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CanShootEnemy of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_CanShootEnemy01
static int tolua_SandboxToLua_SoldierObject_CanShootEnemy01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  float shootDistance = ((float)  tolua_tonumber(tolua_S,3,3.0f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CanShootEnemy'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CanShootEnemy(navMeshName,shootDistance);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 2;
tolua_lerror:
 return tolua_SandboxToLua_SoldierObject_CanShootEnemy00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetEnemy of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_GetEnemy00
static int tolua_SandboxToLua_SoldierObject_GetEnemy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetEnemy'", NULL);
#endif
  {
   AgentObject* tolua_ret = (AgentObject*)  self->GetEnemy();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetEnemy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBonePosition of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_GetBonePosition00
static int tolua_SandboxToLua_SoldierObject_GetBonePosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  const std::string boneName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBonePosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetBonePosition(boneName);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushcppstring(tolua_S,(const char*)boneName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBonePosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBoneForward of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_GetBoneForward00
static int tolua_SandboxToLua_SoldierObject_GetBoneForward00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  const std::string boneName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBoneForward'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetBoneForward(boneName);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushcppstring(tolua_S,(const char*)boneName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBoneForward'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasMovePosition of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_HasMovePosition00
static int tolua_SandboxToLua_SoldierObject_HasMovePosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  float reachDistance = ((float)  tolua_tonumber(tolua_S,2,1.5f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasMovePosition'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasMovePosition(reachDistance);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasMovePosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMovePosition of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_SetMovePosition00
static int tolua_SandboxToLua_SoldierObject_SetMovePosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* movePos = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMovePosition'", NULL);
#endif
  {
   self->SetMovePosition(*movePos);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMovePosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearMovePosition of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_ClearMovePosition00
static int tolua_SandboxToLua_SoldierObject_ClearMovePosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearMovePosition'", NULL);
#endif
  {
   self->ClearMovePosition();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearMovePosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsTargetReached of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_IsTargetReached00
static int tolua_SandboxToLua_SoldierObject_IsTargetReached00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SoldierObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SoldierObject* self = (const SoldierObject*)  tolua_tousertype(tolua_S,1,0);
  float threshold = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsTargetReached'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsTargetReached(threshold);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsTargetReached'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: EnterIdleAnim of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_EnterIdleAnim00
static int tolua_SandboxToLua_SoldierObject_EnterIdleAnim00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'EnterIdleAnim'", NULL);
#endif
  {
   self->EnterIdleAnim();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnterIdleAnim'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: EnterMoveAnim of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_EnterMoveAnim00
static int tolua_SandboxToLua_SoldierObject_EnterMoveAnim00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'EnterMoveAnim'", NULL);
#endif
  {
   self->EnterMoveAnim();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnterMoveAnim'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: EnterShootAnim of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_EnterShootAnim00
static int tolua_SandboxToLua_SoldierObject_EnterShootAnim00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'EnterShootAnim'", NULL);
#endif
  {
   self->EnterShootAnim();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnterShootAnim'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: EnterReloadAnim of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_EnterReloadAnim00
static int tolua_SandboxToLua_SoldierObject_EnterReloadAnim00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'EnterReloadAnim'", NULL);
#endif
  {
   self->EnterReloadAnim();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnterReloadAnim'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: EnterDeathAnim of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_EnterDeathAnim00
static int tolua_SandboxToLua_SoldierObject_EnterDeathAnim00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'EnterDeathAnim'", NULL);
#endif
  {
   self->EnterDeathAnim();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'EnterDeathAnim'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: __IAnimContextProvider__ of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_get_SoldierObject___IAnimContextProvider__
static int tolua_get_SoldierObject___IAnimContextProvider__(lua_State* tolua_S)
{
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable '__IAnimContextProvider__'",NULL);
#endif
#ifdef __cplusplus
   tolua_pushusertype(tolua_S,(void*)static_cast<IAnimContextProvider*>(self), "IAnimContextProvider");
#else
   tolua_pushusertype(tolua_S,(void*)((IAnimContextProvider*)self), "IAnimContextProvider");
#endif
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetOwner of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_GetOwner00
static int tolua_SandboxToLua_AIController_GetOwner00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AIController",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AIController* self = (const AIController*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetOwner'", NULL);
#endif
  {
   SoldierObject* tolua_ret = (SoldierObject*)  self->GetOwner();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SoldierObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetOwner'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBlackboard of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_GetBlackboard00
static int tolua_SandboxToLua_AIController_GetBlackboard00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AIController",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AIController* self = (const AIController*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBlackboard'", NULL);
#endif
  {
   Blackboard* tolua_ret = (Blackboard*)  self->GetBlackboard();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Blackboard");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBlackboard'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetEnemy of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_GetEnemy00
static int tolua_SandboxToLua_AIController_GetEnemy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AIController",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AIController* self = (const AIController*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetEnemy'", NULL);
#endif
  {
   AgentObject* tolua_ret = (AgentObject*)  self->GetEnemy();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetEnemy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasEnemy of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_HasEnemy00
static int tolua_SandboxToLua_AIController_HasEnemy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AIController",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AIController* self = (AIController*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasEnemy'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasEnemy();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasEnemy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasEnemy of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_HasEnemy01
static int tolua_SandboxToLua_AIController_HasEnemy01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AIController",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  AIController* self = (AIController*)  tolua_tousertype(tolua_S,1,0);
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasEnemy'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasEnemy(navMeshName);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 2;
tolua_lerror:
 return tolua_SandboxToLua_AIController_HasEnemy00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: CanShootEnemy of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_CanShootEnemy00
static int tolua_SandboxToLua_AIController_CanShootEnemy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AIController",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AIController* self = (AIController*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CanShootEnemy'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CanShootEnemy();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CanShootEnemy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CanShootEnemy of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_CanShootEnemy01
static int tolua_SandboxToLua_AIController_CanShootEnemy01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AIController",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  AIController* self = (AIController*)  tolua_tousertype(tolua_S,1,0);
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  float shootDistance = ((float)  tolua_tonumber(tolua_S,3,3.0f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CanShootEnemy'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CanShootEnemy(navMeshName,shootDistance);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 2;
tolua_lerror:
 return tolua_SandboxToLua_AIController_CanShootEnemy00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasMovePosition of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_HasMovePosition00
static int tolua_SandboxToLua_AIController_HasMovePosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AIController",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AIController* self = (const AIController*)  tolua_tousertype(tolua_S,1,0);
  float reachDistance = ((float)  tolua_tonumber(tolua_S,2,1.5f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasMovePosition'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasMovePosition(reachDistance);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasMovePosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMovePosition of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_SetMovePosition00
static int tolua_SandboxToLua_AIController_SetMovePosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AIController",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AIController* self = (AIController*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* movePos = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMovePosition'", NULL);
#endif
  {
   self->SetMovePosition(*movePos);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetMovePosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearMovePosition of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_ClearMovePosition00
static int tolua_SandboxToLua_AIController_ClearMovePosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AIController",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AIController* self = (AIController*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearMovePosition'", NULL);
#endif
  {
   self->ClearMovePosition();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearMovePosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsTargetReached of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_IsTargetReached00
static int tolua_SandboxToLua_AIController_IsTargetReached00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AIController",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AIController* self = (const AIController*)  tolua_tousertype(tolua_S,1,0);
  float threshold = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsTargetReached'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsTargetReached(threshold);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsTargetReached'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetDriverByType of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_SetDriverByType00
static int tolua_SandboxToLua_AIController_SetDriverByType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AIController",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AIController* self = (AIController*)  tolua_tousertype(tolua_S,1,0);
  const char* type = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetDriverByType'", NULL);
#endif
  {
   self->SetDriverByType(type);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetDriverByType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDecisionTreeDriver of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_GetDecisionTreeDriver00
static int tolua_SandboxToLua_AIController_GetDecisionTreeDriver00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AIController",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AIController* self = (const AIController*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDecisionTreeDriver'", NULL);
#endif
  {
   DecisionTreeDriver* tolua_ret = (DecisionTreeDriver*)  self->GetDecisionTreeDriver();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"DecisionTreeDriver");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDecisionTreeDriver'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBehaviorTreeDriver of class  AIController */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AIController_GetBehaviorTreeDriver00
static int tolua_SandboxToLua_AIController_GetBehaviorTreeDriver00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const AIController",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const AIController* self = (const AIController*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBehaviorTreeDriver'", NULL);
#endif
  {
   BehaviorTreeDriver* tolua_ret = (BehaviorTreeDriver*)  self->GetBehaviorTreeDriver();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorTreeDriver");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBehaviorTreeDriver'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreatePlane of class  ObjectFactory */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectFactory_CreatePlane00
static int tolua_SandboxToLua_ObjectFactory_CreatePlane00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectFactory",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectFactory* self = (ObjectFactory*)  tolua_tousertype(tolua_S,1,0);
  float length = ((float)  tolua_tonumber(tolua_S,2,0));
  float width = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreatePlane'", NULL);
#endif
  {
   BlockObject* tolua_ret = (BlockObject*)  self->CreatePlane(length,width);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BlockObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreatePlane'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateBlockObject of class  ObjectFactory */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectFactory_CreateBlockObject00
static int tolua_SandboxToLua_ObjectFactory_CreateBlockObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectFactory",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectFactory* self = (ObjectFactory*)  tolua_tousertype(tolua_S,1,0);
  const std::string meshfilePath = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateBlockObject'", NULL);
#endif
  {
   BlockObject* tolua_ret = (BlockObject*)  self->CreateBlockObject(meshfilePath);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BlockObject");
   tolua_pushcppstring(tolua_S,(const char*)meshfilePath);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateBlockObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateBlockBox of class  ObjectFactory */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectFactory_CreateBlockBox00
static int tolua_SandboxToLua_ObjectFactory_CreateBlockBox00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectFactory",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectFactory* self = (ObjectFactory*)  tolua_tousertype(tolua_S,1,0);
  float width = ((float)  tolua_tonumber(tolua_S,2,0));
  float height = ((float)  tolua_tonumber(tolua_S,3,0));
  float length = ((float)  tolua_tonumber(tolua_S,4,0));
  float uTile = ((float)  tolua_tonumber(tolua_S,5,0));
  float vTile = ((float)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateBlockBox'", NULL);
#endif
  {
   BlockObject* tolua_ret = (BlockObject*)  self->CreateBlockBox(width,height,length,uTile,vTile);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BlockObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateBlockBox'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateBullet of class  ObjectFactory */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectFactory_CreateBullet00
static int tolua_SandboxToLua_ObjectFactory_CreateBullet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectFactory",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectFactory* self = (ObjectFactory*)  tolua_tousertype(tolua_S,1,0);
   float height = ((  float)  tolua_tonumber(tolua_S,2,0));
   float radius = ((  float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateBullet'", NULL);
#endif
  {
   BlockObject* tolua_ret = (BlockObject*)  self->CreateBullet(height,radius);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BlockObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateBullet'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateAgent of class  ObjectFactory */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectFactory_CreateAgent00
static int tolua_SandboxToLua_ObjectFactory_CreateAgent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectFactory",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectFactory* self = (ObjectFactory*)  tolua_tousertype(tolua_S,1,0);
  AGENT_OBJ_TYPE agentType = ((AGENT_OBJ_TYPE) (int)  tolua_tonumber(tolua_S,2,0));
  const char* filepath = ((const char*)  tolua_tostring(tolua_S,3,nullptr));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateAgent'", NULL);
#endif
  {
   AgentObject* tolua_ret = (AgentObject*)  self->CreateAgent(agentType,filepath);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateAgent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateSoldier of class  ObjectFactory */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectFactory_CreateSoldier00
static int tolua_SandboxToLua_ObjectFactory_CreateSoldier00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectFactory",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectFactory* self = (ObjectFactory*)  tolua_tousertype(tolua_S,1,0);
  const std::string meshFile = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const char* filepath = ((const char*)  tolua_tostring(tolua_S,3,nullptr));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateSoldier'", NULL);
#endif
  {
   SoldierObject* tolua_ret = (SoldierObject*)  self->CreateSoldier(meshFile,filepath);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SoldierObject");
   tolua_pushcppstring(tolua_S,(const char*)meshFile);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateSoldier'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CallFile of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_CallFile00
static int tolua_SandboxToLua_SandboxMgr_CallFile00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  const std::string filepath = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CallFile'", NULL);
#endif
  {
   self->CallFile(filepath);
   tolua_pushcppstring(tolua_S,(const char*)filepath);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CallFile'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetSkyBox of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_SetSkyBox00
static int tolua_SandboxToLua_SandboxMgr_SetSkyBox00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  const std::string materialName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const Ogre::Vector3* rotation = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetSkyBox'", NULL);
#endif
  {
   self->SetSkyBox(materialName,*rotation);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetSkyBox'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetAmbientLight of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_SetAmbientLight00
static int tolua_SandboxToLua_SandboxMgr_SetAmbientLight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* colourValue = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetAmbientLight'", NULL);
#endif
  {
   self->SetAmbientLight(*colourValue);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetAmbientLight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateDirectionalLight of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_CreateDirectionalLight00
static int tolua_SandboxToLua_SandboxMgr_CreateDirectionalLight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* rotation = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateDirectionalLight'", NULL);
#endif
  {
   Ogre::Light* tolua_ret = (Ogre::Light*)  self->CreateDirectionalLight(*rotation);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Light");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateDirectionalLight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaterial of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_setMaterial00
static int tolua_SandboxToLua_SandboxMgr_setMaterial00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"Ogre::SceneNode",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  Ogre::SceneNode* pNode = ((Ogre::SceneNode*)  tolua_tousertype(tolua_S,2,0));
  const std::string materialName = ((const std::string)  tolua_tocppstring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setMaterial'", NULL);
#endif
  {
   self->setMaterial(pNode,materialName);
   tolua_pushcppstring(tolua_S,(const char*)materialName);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setMaterial'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMaterial of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_setMaterial01
static int tolua_SandboxToLua_SandboxMgr_setMaterial01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"BlockObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  BlockObject* pObject = ((BlockObject*)  tolua_tousertype(tolua_S,2,0));
  const std::string materialName = ((const std::string)  tolua_tocppstring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setMaterial'", NULL);
#endif
  {
   self->setMaterial(pObject,materialName);
   tolua_pushcppstring(tolua_S,(const char*)materialName);
  }
 }
 return 1;
tolua_lerror:
 return tolua_SandboxToLua_SandboxMgr_setMaterial00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetUseCppFsmFlag of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_SetUseCppFsmFlag00
static int tolua_SandboxToLua_SandboxMgr_SetUseCppFsmFlag00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  bool value = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetUseCppFsmFlag'", NULL);
#endif
  {
   self->SetUseCppFsmFlag(value);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetUseCppFsmFlag'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: UpdateSceneGraph of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_UpdateSceneGraph00
static int tolua_SandboxToLua_SandboxMgr_UpdateSceneGraph00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'UpdateSceneGraph'", NULL);
#endif
  {
   self->UpdateSceneGraph();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'UpdateSceneGraph'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: DefaultConfig of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_DefaultConfig00
static int tolua_SandboxToLua_SandboxMgr_DefaultConfig00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"rcConfig",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  rcConfig* config = ((rcConfig*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'DefaultConfig'", NULL);
#endif
  {
   self->DefaultConfig(*config);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'DefaultConfig'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ApplySettingConfig of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_ApplySettingConfig00
static int tolua_SandboxToLua_SandboxMgr_ApplySettingConfig00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"rcConfig",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  rcConfig* config = ((rcConfig*)  tolua_tousertype(tolua_S,2,0));
  float height = ((float)  tolua_tonumber(tolua_S,3,0));
  float radius = ((float)  tolua_tonumber(tolua_S,4,0));
  float climb = ((float)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ApplySettingConfig'", NULL);
#endif
  {
   self->ApplySettingConfig(*config,height,radius,climb);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ApplySettingConfig'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateNavigationMesh of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_CreateNavigationMesh00
static int tolua_SandboxToLua_SandboxMgr_CreateNavigationMesh00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const rcConfig",0,&tolua_err)) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  const rcConfig* config = ((const rcConfig*)  tolua_tousertype(tolua_S,2,0));
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateNavigationMesh'", NULL);
#endif
  {
   NavigationMesh* tolua_ret = (NavigationMesh*)  self->CreateNavigationMesh(*config,navMeshName);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"NavigationMesh");
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateNavigationMesh'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RandomPoint of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_RandomPoint00
static int tolua_SandboxToLua_SandboxMgr_RandomPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SandboxMgr",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SandboxMgr* self = (const SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RandomPoint'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->RandomPoint(navMeshName);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RandomPoint'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: FindClosestPoint of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_FindClosestPoint00
static int tolua_SandboxToLua_SandboxMgr_FindClosestPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SandboxMgr",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SandboxMgr* self = (const SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const Ogre::Vector3* point = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'FindClosestPoint'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->FindClosestPoint(navMeshName,*point);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FindClosestPoint'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: FindPath of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_FindPath00
static int tolua_SandboxToLua_SandboxMgr_FindPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SandboxMgr",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const Ogre::Vector3",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,5,&tolua_err) || !tolua_isusertype(tolua_S,5,"std::vector<Ogre::Vector3>",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SandboxMgr* self = (const SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const Ogre::Vector3* start = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
  const Ogre::Vector3* end = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,4,0));
  std::vector<Ogre::Vector3>* outPath = ((std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'FindPath'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->FindPath(navMeshName,*start,*end,*outPath);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FindPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RayCastObjectId of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_RayCastObjectId00
static int tolua_SandboxToLua_SandboxMgr_RayCastObjectId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const SandboxMgr",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const SandboxMgr* self = (const SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* from = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  const Ogre::Vector3* to = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RayCastObjectId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->RayCastObjectId(*from,*to);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RayCastObjectId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clearAllObjects of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_clearAllObjects00
static int tolua_SandboxToLua_ObjectManager_clearAllObjects00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int objType = ((int)  tolua_tonumber(tolua_S,2,0));
  bool forceAll = ((bool)  tolua_toboolean(tolua_S,3,true));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clearAllObjects'", NULL);
#endif
  {
   self->clearAllObjects(objType,forceAll);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clearAllObjects'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getAllAgents of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getAllAgents00
static int tolua_SandboxToLua_ObjectManager_getAllAgents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getAllAgents'", NULL);
#endif
  {
   const std::vector<AgentObject*>& tolua_ret = (const std::vector<AgentObject*>&)  self->getAllAgents();
    tolua_pushusertype(tolua_S,(void*)&tolua_ret,"const std::vector<AgentObject*>");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getAllAgents'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getAllBlocks of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getAllBlocks00
static int tolua_SandboxToLua_ObjectManager_getAllBlocks00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getAllBlocks'", NULL);
#endif
  {
   const std::vector<BlockObject*>& tolua_ret = (const std::vector<BlockObject*>&)  self->getAllBlocks();
    tolua_pushusertype(tolua_S,(void*)&tolua_ret,"const std::vector<BlockObject*>");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getAllBlocks'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getSpecifyAgents of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getSpecifyAgents00
static int tolua_SandboxToLua_ObjectManager_getSpecifyAgents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  AGENT_OBJ_TYPE agentType = ((AGENT_OBJ_TYPE) (int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getSpecifyAgents'", NULL);
#endif
  {
   std::vector<AgentObject*> tolua_ret = (std::vector<AgentObject*>)  self->getSpecifyAgents(agentType);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<AgentObject*>)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<AgentObject*>));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getSpecifyAgents'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getObjectCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getObjectCount00
static int tolua_SandboxToLua_ObjectManager_getObjectCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getObjectCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getObjectCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getObjectCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getAiAgentCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getAiAgentCount00
static int tolua_SandboxToLua_ObjectManager_getAiAgentCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getAiAgentCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getAiAgentCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getAiAgentCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getAiSoldierCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getAiSoldierCount00
static int tolua_SandboxToLua_ObjectManager_getAiSoldierCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getAiSoldierCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getAiSoldierCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getAiSoldierCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: buildAiDebugSummary of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_buildAiDebugSummary00
static int tolua_SandboxToLua_ObjectManager_buildAiDebugSummary00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int maxAgents = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'buildAiDebugSummary'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->buildAiDebugSummary(maxAgents);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'buildAiDebugSummary'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: configureAiScheduler of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_configureAiScheduler00
static int tolua_SandboxToLua_ObjectManager_configureAiScheduler00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  bool enabled = ((bool)  tolua_toboolean(tolua_S,2,0));
  int tickIntervalMs = ((int)  tolua_tonumber(tolua_S,3,0));
  int maxTicksPerFrame = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'configureAiScheduler'", NULL);
#endif
  {
   self->configureAiScheduler(enabled,tickIntervalMs,maxTicksPerFrame);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'configureAiScheduler'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: buildAiSchedulerDebugSummary of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_buildAiSchedulerDebugSummary00
static int tolua_SandboxToLua_ObjectManager_buildAiSchedulerDebugSummary00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'buildAiSchedulerDebugSummary'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->buildAiSchedulerDebugSummary();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'buildAiSchedulerDebugSummary'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clearTeamBlackboardFacts of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_clearTeamBlackboardFacts00
static int tolua_SandboxToLua_ObjectManager_clearTeamBlackboardFacts00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clearTeamBlackboardFacts'", NULL);
#endif
  {
   self->clearTeamBlackboardFacts();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clearTeamBlackboardFacts'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: configureTeamBlackboard of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_configureTeamBlackboard00
static int tolua_SandboxToLua_ObjectManager_configureTeamBlackboard00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int ttlMs = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'configureTeamBlackboard'", NULL);
#endif
  {
   self->configureTeamBlackboard(ttlMs);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'configureTeamBlackboard'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: rememberTeamEnemyFact of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_rememberTeamEnemyFact00
static int tolua_SandboxToLua_ObjectManager_rememberTeamEnemyFact00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,5,&tolua_err) || !tolua_isusertype(tolua_S,5,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int teamId = ((int)  tolua_tonumber(tolua_S,2,0));
  int reporterId = ((int)  tolua_tonumber(tolua_S,3,0));
  int targetId = ((int)  tolua_tonumber(tolua_S,4,0));
  const Ogre::Vector3* targetPosition = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,5,0));
  int lastSeenMs = ((int)  tolua_tonumber(tolua_S,6,0));
  float confidence = ((float)  tolua_tonumber(tolua_S,7,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'rememberTeamEnemyFact'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->rememberTeamEnemyFact(teamId,reporterId,targetId,*targetPosition,lastSeenMs,confidence);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'rememberTeamEnemyFact'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: writeBestTeamEnemyFactToBlackboard of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_writeBestTeamEnemyFactToBlackboard00
static int tolua_SandboxToLua_ObjectManager_writeBestTeamEnemyFactToBlackboard00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"AgentObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  AgentObject* agent = ((AgentObject*)  tolua_tousertype(tolua_S,2,0));
  const std::string keyPrefix = ((const std::string)  tolua_tocppstring(tolua_S,3,0));
  bool allowOwnReport = ((bool)  tolua_toboolean(tolua_S,4,false));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'writeBestTeamEnemyFactToBlackboard'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->writeBestTeamEnemyFactToBlackboard(agent,keyPrefix,allowOwnReport);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)keyPrefix);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'writeBestTeamEnemyFactToBlackboard'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTeamBlackboardFactCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTeamBlackboardFactCount00
static int tolua_SandboxToLua_ObjectManager_getTeamBlackboardFactCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTeamBlackboardFactCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTeamBlackboardFactCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTeamBlackboardFactCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTeamBlackboardReportCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTeamBlackboardReportCount00
static int tolua_SandboxToLua_ObjectManager_getTeamBlackboardReportCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTeamBlackboardReportCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTeamBlackboardReportCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTeamBlackboardReportCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: buildTeamBlackboardDebugSummary of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_buildTeamBlackboardDebugSummary00
static int tolua_SandboxToLua_ObjectManager_buildTeamBlackboardDebugSummary00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'buildTeamBlackboardDebugSummary'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->buildTeamBlackboardDebugSummary();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'buildTeamBlackboardDebugSummary'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clearTacticalInfluence of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_clearTacticalInfluence00
static int tolua_SandboxToLua_ObjectManager_clearTacticalInfluence00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clearTacticalInfluence'", NULL);
#endif
  {
   self->clearTacticalInfluence();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clearTacticalInfluence'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: configureTacticalInfluence of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_configureTacticalInfluence00
static int tolua_SandboxToLua_ObjectManager_configureTacticalInfluence00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  float minX = ((float)  tolua_tonumber(tolua_S,2,0));
  float maxX = ((float)  tolua_tonumber(tolua_S,3,0));
  float minZ = ((float)  tolua_tonumber(tolua_S,4,0));
  float maxZ = ((float)  tolua_tonumber(tolua_S,5,0));
  float cellSize = ((float)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'configureTacticalInfluence'", NULL);
#endif
  {
   self->configureTacticalInfluence(minX,maxX,minZ,maxZ,cellSize);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'configureTacticalInfluence'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: configureTacticalInfluenceFromNavMesh of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_configureTacticalInfluenceFromNavMesh00
static int tolua_SandboxToLua_ObjectManager_configureTacticalInfluenceFromNavMesh00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,5,&tolua_err) || !tolua_isusertype(tolua_S,5,"const Ogre::Vector3",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,6,&tolua_err) || !tolua_isusertype(tolua_S,6,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  float cellWidth = ((float)  tolua_tonumber(tolua_S,3,0));
  float cellHeight = ((float)  tolua_tonumber(tolua_S,4,0));
  const Ogre::Vector3* boundaryMinOffset = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,5,0));
  const Ogre::Vector3* boundaryMaxOffset = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'configureTacticalInfluenceFromNavMesh'", NULL);
#endif
  {
   self->configureTacticalInfluenceFromNavMesh(navMeshName,cellWidth,cellHeight,*boundaryMinOffset,*boundaryMaxOffset);
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'configureTacticalInfluenceFromNavMesh'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clearTacticalInfluenceLayer of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_clearTacticalInfluenceLayer00
static int tolua_SandboxToLua_ObjectManager_clearTacticalInfluenceLayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clearTacticalInfluenceLayer'", NULL);
#endif
  {
   self->clearTacticalInfluenceLayer(layerName);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clearTacticalInfluenceLayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setTacticalInfluenceLayerOptions of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_setTacticalInfluenceLayerOptions00
static int tolua_SandboxToLua_ObjectManager_setTacticalInfluenceLayerOptions00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  float falloff = ((float)  tolua_tonumber(tolua_S,3,0));
  float inertia = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setTacticalInfluenceLayerOptions'", NULL);
#endif
  {
   self->setTacticalInfluenceLayerOptions(layerName,falloff,inertia);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setTacticalInfluenceLayerOptions'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addTacticalInfluenceSource of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_addTacticalInfluenceSource00
static int tolua_SandboxToLua_ObjectManager_addTacticalInfluenceSource00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const Ogre::Vector3* center = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
  float strength = ((float)  tolua_tonumber(tolua_S,4,0));
  float radius = ((float)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'addTacticalInfluenceSource'", NULL);
#endif
  {
   int tolua_ret = (int)  self->addTacticalInfluenceSource(layerName,*center,strength,radius);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addTacticalInfluenceSource'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addTacticalInfluencePoint of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_addTacticalInfluencePoint00
static int tolua_SandboxToLua_ObjectManager_addTacticalInfluencePoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const Ogre::Vector3* center = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
  float strength = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'addTacticalInfluencePoint'", NULL);
#endif
  {
   int tolua_ret = (int)  self->addTacticalInfluencePoint(layerName,*center,strength);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addTacticalInfluencePoint'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: spreadTacticalInfluenceLayer of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_spreadTacticalInfluenceLayer00
static int tolua_SandboxToLua_ObjectManager_spreadTacticalInfluenceLayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int passCount = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'spreadTacticalInfluenceLayer'", NULL);
#endif
  {
   int tolua_ret = (int)  self->spreadTacticalInfluenceLayer(layerName,passCount);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'spreadTacticalInfluenceLayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: sampleTacticalInfluence of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_sampleTacticalInfluence00
static int tolua_SandboxToLua_ObjectManager_sampleTacticalInfluence00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sampleTacticalInfluence'", NULL);
#endif
  {
   float tolua_ret = (float)  self->sampleTacticalInfluence(layerName,*position);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sampleTacticalInfluence'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: scoreTacticalPosition of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_scoreTacticalPosition00
static int tolua_SandboxToLua_ObjectManager_scoreTacticalPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  float dangerWeight = ((float)  tolua_tonumber(tolua_S,3,0));
  float teamWeight = ((float)  tolua_tonumber(tolua_S,4,0));
  float objectiveWeight = ((float)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'scoreTacticalPosition'", NULL);
#endif
  {
   float tolua_ret = (float)  self->scoreTacticalPosition(*position,dangerWeight,teamWeight,objectiveWeight);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'scoreTacticalPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: findBestTacticalPosition of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_findBestTacticalPosition00
static int tolua_SandboxToLua_ObjectManager_findBestTacticalPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,8,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* center = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  float radius = ((float)  tolua_tonumber(tolua_S,3,0));
  float step = ((float)  tolua_tonumber(tolua_S,4,0));
  float dangerWeight = ((float)  tolua_tonumber(tolua_S,5,0));
  float teamWeight = ((float)  tolua_tonumber(tolua_S,6,0));
  float objectiveWeight = ((float)  tolua_tonumber(tolua_S,7,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'findBestTacticalPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->findBestTacticalPosition(*center,radius,step,dangerWeight,teamWeight,objectiveWeight);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'findBestTacticalPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: scoreTacticalQueryPosition of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_scoreTacticalQueryPosition00
static int tolua_SandboxToLua_ObjectManager_scoreTacticalQueryPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string queryType = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'scoreTacticalQueryPosition'", NULL);
#endif
  {
   float tolua_ret = (float)  self->scoreTacticalQueryPosition(queryType,*position);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)queryType);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'scoreTacticalQueryPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: findBestTacticalQueryPosition of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_findBestTacticalQueryPosition00
static int tolua_SandboxToLua_ObjectManager_findBestTacticalQueryPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string queryType = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const Ogre::Vector3* center = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
  float radius = ((float)  tolua_tonumber(tolua_S,4,0));
  float step = ((float)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'findBestTacticalQueryPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->findBestTacticalQueryPosition(queryType,*center,radius,step);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushcppstring(tolua_S,(const char*)queryType);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'findBestTacticalQueryPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: findBestSupportPosition of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_findBestSupportPosition00
static int tolua_SandboxToLua_ObjectManager_findBestSupportPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* center = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  float radius = ((float)  tolua_tonumber(tolua_S,3,0));
  float step = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'findBestSupportPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->findBestSupportPosition(*center,radius,step);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'findBestSupportPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: findLowThreatPosition of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_findLowThreatPosition00
static int tolua_SandboxToLua_ObjectManager_findLowThreatPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* center = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  float radius = ((float)  tolua_tonumber(tolua_S,3,0));
  float step = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'findLowThreatPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->findLowThreatPosition(*center,radius,step);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'findLowThreatPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: configureTacticalEvents of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_configureTacticalEvents00
static int tolua_SandboxToLua_ObjectManager_configureTacticalEvents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int eventTtlMs = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'configureTacticalEvents'", NULL);
#endif
  {
   self->configureTacticalEvents(eventTtlMs);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'configureTacticalEvents'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clearTacticalEvents of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_clearTacticalEvents00
static int tolua_SandboxToLua_ObjectManager_clearTacticalEvents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clearTacticalEvents'", NULL);
#endif
  {
   self->clearTacticalEvents();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clearTacticalEvents'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: publishTacticalEvent of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_publishTacticalEvent00
static int tolua_SandboxToLua_ObjectManager_publishTacticalEvent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,7,&tolua_err) || !tolua_isusertype(tolua_S,7,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,9,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,10,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,11,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string eventType = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int senderId = ((int)  tolua_tonumber(tolua_S,3,0));
  int targetId = ((int)  tolua_tonumber(tolua_S,4,0));
  int teamId = ((int)  tolua_tonumber(tolua_S,5,0));
  int targetTeamId = ((int)  tolua_tonumber(tolua_S,6,0));
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,7,0));
  int timeMs = ((int)  tolua_tonumber(tolua_S,8,0));
  const std::string scopeName = ((const std::string)  tolua_tocppstring(tolua_S,9,0));
  bool queueEvent = ((bool)  tolua_toboolean(tolua_S,10,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'publishTacticalEvent'", NULL);
#endif
  {
   self->publishTacticalEvent(eventType,senderId,targetId,teamId,targetTeamId,*position,timeMs,scopeName,queueEvent);
   tolua_pushcppstring(tolua_S,(const char*)eventType);
   tolua_pushcppstring(tolua_S,(const char*)scopeName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'publishTacticalEvent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventCount00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalEventCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventTypeCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventTypeCount00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventTypeCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string eventType = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventTypeCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalEventTypeCount(eventType);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)eventType);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventTypeCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getLastTacticalEventPosition of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getLastTacticalEventPosition00
static int tolua_SandboxToLua_ObjectManager_getLastTacticalEventPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string eventType = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const Ogre::Vector3* fallback = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getLastTacticalEventPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->getLastTacticalEventPosition(eventType,*fallback);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushcppstring(tolua_S,(const char*)eventType);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getLastTacticalEventPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventDebugRecordCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventDebugRecordCount00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventDebugRecordCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventDebugRecordCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalEventDebugRecordCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventDebugRecordCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventDebugType of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventDebugType00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventDebugType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int luaIndex = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventDebugType'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->getTacticalEventDebugType(luaIndex);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventDebugType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventDebugSenderId of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventDebugSenderId00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventDebugSenderId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int luaIndex = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventDebugSenderId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalEventDebugSenderId(luaIndex);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventDebugSenderId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventDebugTargetId of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTargetId00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTargetId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int luaIndex = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventDebugTargetId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalEventDebugTargetId(luaIndex);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventDebugTargetId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventDebugTeamId of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTeamId00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTeamId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int luaIndex = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventDebugTeamId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalEventDebugTeamId(luaIndex);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventDebugTeamId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventDebugTargetTeamId of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTargetTeamId00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTargetTeamId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int luaIndex = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventDebugTargetTeamId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalEventDebugTargetTeamId(luaIndex);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventDebugTargetTeamId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventDebugPosition of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventDebugPosition00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventDebugPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int luaIndex = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventDebugPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->getTacticalEventDebugPosition(luaIndex);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventDebugPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventDebugTimeMs of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTimeMs00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTimeMs00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int luaIndex = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventDebugTimeMs'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalEventDebugTimeMs(luaIndex);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventDebugTimeMs'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalEventDebugRemainingTtlMs of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalEventDebugRemainingTtlMs00
static int tolua_SandboxToLua_ObjectManager_getTacticalEventDebugRemainingTtlMs00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int luaIndex = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalEventDebugRemainingTtlMs'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalEventDebugRemainingTtlMs(luaIndex);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalEventDebugRemainingTtlMs'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: rebuildTacticalDangerLayer of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_rebuildTacticalDangerLayer00
static int tolua_SandboxToLua_ObjectManager_rebuildTacticalDangerLayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,9,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int perspectiveTeamId = ((int)  tolua_tonumber(tolua_S,2,0));
  float dangerStrength = ((float)  tolua_tonumber(tolua_S,3,0));
  float bulletShotRadius = ((float)  tolua_tonumber(tolua_S,4,0));
  float bulletImpactRadius = ((float)  tolua_tonumber(tolua_S,5,0));
  float deadFriendlyRadius = ((float)  tolua_tonumber(tolua_S,6,0));
  float enemySightingRadius = ((float)  tolua_tonumber(tolua_S,7,0));
  int spreadPasses = ((int)  tolua_tonumber(tolua_S,8,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'rebuildTacticalDangerLayer'", NULL);
#endif
  {
   int tolua_ret = (int)  self->rebuildTacticalDangerLayer(perspectiveTeamId,dangerStrength,bulletShotRadius,bulletImpactRadius,deadFriendlyRadius,enemySightingRadius,spreadPasses);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'rebuildTacticalDangerLayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: rebuildTacticalTeamLayer of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_rebuildTacticalTeamLayer00
static int tolua_SandboxToLua_ObjectManager_rebuildTacticalTeamLayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int positiveTeamId = ((int)  tolua_tonumber(tolua_S,2,0));
  float teamStrength = ((float)  tolua_tonumber(tolua_S,3,0));
  float radius = ((float)  tolua_tonumber(tolua_S,4,0));
  int spreadPasses = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'rebuildTacticalTeamLayer'", NULL);
#endif
  {
   int tolua_ret = (int)  self->rebuildTacticalTeamLayer(positiveTeamId,teamStrength,radius,spreadPasses);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'rebuildTacticalTeamLayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: rebuildTacticalObjectiveLayer of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_rebuildTacticalObjectiveLayer00
static int tolua_SandboxToLua_ObjectManager_rebuildTacticalObjectiveLayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* center = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  float strength = ((float)  tolua_tonumber(tolua_S,3,0));
  float radius = ((float)  tolua_tonumber(tolua_S,4,0));
  int spreadPasses = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'rebuildTacticalObjectiveLayer'", NULL);
#endif
  {
   int tolua_ret = (int)  self->rebuildTacticalObjectiveLayer(*center,strength,radius,spreadPasses);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'rebuildTacticalObjectiveLayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalInfluenceLayerActiveCellCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerActiveCellCount00
static int tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerActiveCellCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalInfluenceLayerActiveCellCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalInfluenceLayerActiveCellCount(layerName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalInfluenceLayerActiveCellCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalInfluenceLayerCellWriteCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerCellWriteCount00
static int tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerCellWriteCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalInfluenceLayerCellWriteCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalInfluenceLayerCellWriteCount(layerName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalInfluenceLayerCellWriteCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalInfluenceLayerDebugCellCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerDebugCellCount00
static int tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerDebugCellCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  float threshold = ((float)  tolua_tonumber(tolua_S,3,0));
  int maxCells = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalInfluenceLayerDebugCellCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalInfluenceLayerDebugCellCount(layerName,threshold,maxCells);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalInfluenceLayerDebugCellCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalInfluenceLayerDebugCellPosition of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerDebugCellPosition00
static int tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerDebugCellPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int luaIndex = ((int)  tolua_tonumber(tolua_S,3,0));
  float threshold = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalInfluenceLayerDebugCellPosition'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->getTacticalInfluenceLayerDebugCellPosition(layerName,luaIndex,threshold);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalInfluenceLayerDebugCellPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalInfluenceLayerDebugCellValue of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerDebugCellValue00
static int tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerDebugCellValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int luaIndex = ((int)  tolua_tonumber(tolua_S,3,0));
  float threshold = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalInfluenceLayerDebugCellValue'", NULL);
#endif
  {
   float tolua_ret = (float)  self->getTacticalInfluenceLayerDebugCellValue(layerName,luaIndex,threshold);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalInfluenceLayerDebugCellValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: drawTacticalInfluenceLayer of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_drawTacticalInfluenceLayer00
static int tolua_SandboxToLua_ObjectManager_drawTacticalInfluenceLayer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const Ogre::ColourValue",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,5,&tolua_err) || !tolua_isusertype(tolua_S,5,"const Ogre::ColourValue",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,6,&tolua_err) || !tolua_isusertype(tolua_S,6,"const Ogre::ColourValue",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,7,&tolua_err) || !tolua_isusertype(tolua_S,7,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,10,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,11,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,12,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,13,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,14,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  float yOffset = ((float)  tolua_tonumber(tolua_S,3,0));
  const Ogre::ColourValue* positiveValue = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,4,0));
  const Ogre::ColourValue* zeroValue = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,5,0));
  const Ogre::ColourValue* negativeValue = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,6,0));
  const Ogre::ColourValue* gridColor = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,7,0));
  float threshold = ((float)  tolua_tonumber(tolua_S,8,0));
  int maxCells = ((int)  tolua_tonumber(tolua_S,9,0));
  bool drawNeutralCells = ((bool)  tolua_toboolean(tolua_S,10,0));
  bool projectToNav = ((bool)  tolua_toboolean(tolua_S,11,0));
  float maxProjectionDistance = ((float)  tolua_tonumber(tolua_S,12,0));
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,13,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'drawTacticalInfluenceLayer'", NULL);
#endif
  {
   int tolua_ret = (int)  self->drawTacticalInfluenceLayer(layerName,yOffset,*positiveValue,*zeroValue,*negativeValue,*gridColor,threshold,maxCells,drawNeutralCells,projectToNav,maxProjectionDistance,navMeshName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'drawTacticalInfluenceLayer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: rebuildTacticalInfluenceLayerDebugVisual of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_rebuildTacticalInfluenceLayerDebugVisual00
static int tolua_SandboxToLua_ObjectManager_rebuildTacticalInfluenceLayerDebugVisual00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const Ogre::ColourValue",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,5,&tolua_err) || !tolua_isusertype(tolua_S,5,"const Ogre::ColourValue",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,6,&tolua_err) || !tolua_isusertype(tolua_S,6,"const Ogre::ColourValue",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,7,&tolua_err) || !tolua_isusertype(tolua_S,7,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,10,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,11,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,12,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,13,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,14,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  const std::string layerName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  float yOffset = ((float)  tolua_tonumber(tolua_S,3,0));
  const Ogre::ColourValue* positiveValue = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,4,0));
  const Ogre::ColourValue* zeroValue = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,5,0));
  const Ogre::ColourValue* negativeValue = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,6,0));
  const Ogre::ColourValue* gridColor = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,7,0));
  float threshold = ((float)  tolua_tonumber(tolua_S,8,0));
  int maxCells = ((int)  tolua_tonumber(tolua_S,9,0));
  bool drawNeutralCells = ((bool)  tolua_toboolean(tolua_S,10,0));
  bool projectToNav = ((bool)  tolua_toboolean(tolua_S,11,0));
  float maxProjectionDistance = ((float)  tolua_tonumber(tolua_S,12,0));
  const std::string navMeshName = ((const std::string)  tolua_tocppstring(tolua_S,13,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'rebuildTacticalInfluenceLayerDebugVisual'", NULL);
#endif
  {
   int tolua_ret = (int)  self->rebuildTacticalInfluenceLayerDebugVisual(layerName,yOffset,*positiveValue,*zeroValue,*negativeValue,*gridColor,threshold,maxCells,drawNeutralCells,projectToNav,maxProjectionDistance,navMeshName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)layerName);
   tolua_pushcppstring(tolua_S,(const char*)navMeshName);
  }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'rebuildTacticalInfluenceLayerDebugVisual'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setTacticalInfluenceDebugVisible of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_setTacticalInfluenceDebugVisible00
static int tolua_SandboxToLua_ObjectManager_setTacticalInfluenceDebugVisible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  bool visible = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setTacticalInfluenceDebugVisible'", NULL);
#endif
  {
   self->setTacticalInfluenceDebugVisible(visible);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setTacticalInfluenceDebugVisible'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clearTacticalInfluenceDebugVisuals of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_clearTacticalInfluenceDebugVisuals00
static int tolua_SandboxToLua_ObjectManager_clearTacticalInfluenceDebugVisuals00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clearTacticalInfluenceDebugVisuals'", NULL);
#endif
  {
   self->clearTacticalInfluenceDebugVisuals();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clearTacticalInfluenceDebugVisuals'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalInfluenceActiveCellCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalInfluenceActiveCellCount00
static int tolua_SandboxToLua_ObjectManager_getTacticalInfluenceActiveCellCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalInfluenceActiveCellCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalInfluenceActiveCellCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalInfluenceActiveCellCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalInfluenceCellWriteCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalInfluenceCellWriteCount00
static int tolua_SandboxToLua_ObjectManager_getTacticalInfluenceCellWriteCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalInfluenceCellWriteCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalInfluenceCellWriteCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalInfluenceCellWriteCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getTacticalInfluenceQueryCount of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_getTacticalInfluenceQueryCount00
static int tolua_SandboxToLua_ObjectManager_getTacticalInfluenceQueryCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getTacticalInfluenceQueryCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->getTacticalInfluenceQueryCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getTacticalInfluenceQueryCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: buildTacticalInfluenceDebugSummary of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_buildTacticalInfluenceDebugSummary00
static int tolua_SandboxToLua_ObjectManager_buildTacticalInfluenceDebugSummary00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const ObjectManager* self = (const ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'buildTacticalInfluenceDebugSummary'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->buildTacticalInfluenceDebugSummary();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'buildTacticalInfluenceDebugSummary'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: buildAiEventDebugSummary of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_buildAiEventDebugSummary00
static int tolua_SandboxToLua_ObjectManager_buildAiEventDebugSummary00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int maxAgents = ((int)  tolua_tonumber(tolua_S,2,0));
  int maxEvents = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'buildAiEventDebugSummary'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->buildAiEventDebugSummary(maxAgents,maxEvents);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'buildAiEventDebugSummary'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: runAiEventScopeSelfTest of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_runAiEventScopeSelfTest00
static int tolua_SandboxToLua_ObjectManager_runAiEventScopeSelfTest00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'runAiEventScopeSelfTest'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->runAiEventScopeSelfTest();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'runAiEventScopeSelfTest'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: buildObjectDebugSummary of class  ObjectManager */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_ObjectManager_buildObjectDebugSummary00
static int tolua_SandboxToLua_ObjectManager_buildObjectDebugSummary00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ObjectManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ObjectManager* self = (ObjectManager*)  tolua_tousertype(tolua_S,1,0);
  int maxObjects = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'buildObjectDebugSummary'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->buildObjectDebugSummary(maxObjects);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'buildObjectDebugSummary'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsValid of class  NavigationMesh */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_NavigationMesh_IsValid00
static int tolua_SandboxToLua_NavigationMesh_IsValid00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const NavigationMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const NavigationMesh* self = (const NavigationMesh*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsValid'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsValid();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsValid'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: FindClosestPoint of class  NavigationMesh */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_NavigationMesh_FindClosestPoint00
static int tolua_SandboxToLua_NavigationMesh_FindClosestPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const NavigationMesh",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const NavigationMesh* self = (const NavigationMesh*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* point = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'FindClosestPoint'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->FindClosestPoint(*point);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FindClosestPoint'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: FindPath of class  NavigationMesh */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_NavigationMesh_FindPath00
static int tolua_SandboxToLua_NavigationMesh_FindPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const NavigationMesh",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"std::vector<Ogre::Vector3>",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const NavigationMesh* self = (const NavigationMesh*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* start = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  const Ogre::Vector3* end = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
  std::vector<Ogre::Vector3>* outPath = ((std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'FindPath'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->FindPath(*start,*end,*outPath);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FindPath'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RandomPoint of class  NavigationMesh */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_NavigationMesh_RandomPoint00
static int tolua_SandboxToLua_NavigationMesh_RandomPoint00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const NavigationMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const NavigationMesh* self = (const NavigationMesh*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RandomPoint'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->RandomPoint();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RandomPoint'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: HasDebugVisual of class  NavigationMesh */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_NavigationMesh_HasDebugVisual00
static int tolua_SandboxToLua_NavigationMesh_HasDebugVisual00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const NavigationMesh",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const NavigationMesh* self = (const NavigationMesh*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'HasDebugVisual'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->HasDebugVisual();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'HasDebugVisual'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetDebugVisible of class  NavigationMesh */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_NavigationMesh_SetDebugVisible00
static int tolua_SandboxToLua_NavigationMesh_SetDebugVisible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"NavigationMesh",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  NavigationMesh* self = (NavigationMesh*)  tolua_tousertype(tolua_S,1,0);
  bool visible = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetDebugVisible'", NULL);
#endif
  {
   self->SetDebugVisible(visible);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetDebugVisible'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAgentOwner of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetAgentOwner00
static int tolua_SandboxToLua_Blackboard_GetAgentOwner00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAgentOwner'", NULL);
#endif
  {
   AgentObject* tolua_ret = (AgentObject*)  self->GetAgentOwner();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAgentOwner'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetOwner of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetOwner00
static int tolua_SandboxToLua_Blackboard_GetOwner00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetOwner'", NULL);
#endif
  {
   SoldierObject* tolua_ret = (SoldierObject*)  self->GetOwner();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SoldierObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetOwner'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetAgent of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetAgent00
static int tolua_SandboxToLua_Blackboard_SetAgent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  AgentObject* value = ((AgentObject*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetAgent'", NULL);
#endif
  {
   self->SetAgent(key,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetAgent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetAgent of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetAgent00
static int tolua_SandboxToLua_Blackboard_GetAgent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetAgent'", NULL);
#endif
  {
   AgentObject* tolua_ret = (AgentObject*)  self->GetAgent(key);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentObject");
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetAgent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetFloat of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetFloat00
static int tolua_SandboxToLua_Blackboard_SetFloat00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetFloat'", NULL);
#endif
  {
   self->SetFloat(key,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetFloat'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFloat of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetFloat00
static int tolua_SandboxToLua_Blackboard_GetFloat00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  float defaultValue = ((float)  tolua_tonumber(tolua_S,3,0.0f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFloat'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetFloat(key,defaultValue);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFloat'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetInt of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetInt00
static int tolua_SandboxToLua_Blackboard_SetInt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetInt'", NULL);
#endif
  {
   self->SetInt(key,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetInt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetInt of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetInt00
static int tolua_SandboxToLua_Blackboard_GetInt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int defaultValue = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetInt'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetInt(key,defaultValue);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetInt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectId of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetObjectId00
static int tolua_SandboxToLua_Blackboard_SetObjectId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectId'", NULL);
#endif
  {
   self->SetObjectId(key,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectId of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetObjectId00
static int tolua_SandboxToLua_Blackboard_GetObjectId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int defaultValue = ((int)  tolua_tonumber(tolua_S,3,-1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectId'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetObjectId(key,defaultValue);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetBool of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetBool00
static int tolua_SandboxToLua_Blackboard_SetBool00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  bool value = ((bool)  tolua_toboolean(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetBool'", NULL);
#endif
  {
   self->SetBool(key,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetBool'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBool of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetBool00
static int tolua_SandboxToLua_Blackboard_GetBool00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  bool defaultValue = ((bool)  tolua_toboolean(tolua_S,3,false));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBool'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->GetBool(key,defaultValue);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBool'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetString of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetString00
static int tolua_SandboxToLua_Blackboard_SetString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const std::string value = ((const std::string)  tolua_tocppstring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetString'", NULL);
#endif
  {
   self->SetString(key,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
   tolua_pushcppstring(tolua_S,(const char*)value);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetString of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetString00
static int tolua_SandboxToLua_Blackboard_GetString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetString'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->GetString(key);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetVec3 of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetVec300
static int tolua_SandboxToLua_Blackboard_SetVec300(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const Ogre::Vector3* value = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetVec3'", NULL);
#endif
  {
   self->SetVec3(key,*value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetVec3'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetVec3 of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetVec300
static int tolua_SandboxToLua_Blackboard_GetVec300(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetVec3'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->GetVec3(key);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetVec3'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearIntArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_ClearIntArray00
static int tolua_SandboxToLua_Blackboard_ClearIntArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearIntArray'", NULL);
#endif
  {
   self->ClearIntArray(key);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearIntArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddIntToArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_AddIntToArray00
static int tolua_SandboxToLua_Blackboard_AddIntToArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddIntToArray'", NULL);
#endif
  {
   self->AddIntToArray(key,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddIntToArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetIntArrayValue of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetIntArrayValue00
static int tolua_SandboxToLua_Blackboard_SetIntArrayValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int luaIndex = ((int)  tolua_tonumber(tolua_S,3,0));
  int value = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetIntArrayValue'", NULL);
#endif
  {
   self->SetIntArrayValue(key,luaIndex,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetIntArrayValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetIntArrayValue of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetIntArrayValue00
static int tolua_SandboxToLua_Blackboard_GetIntArrayValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int luaIndex = ((int)  tolua_tonumber(tolua_S,3,0));
  int defaultValue = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetIntArrayValue'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetIntArrayValue(key,luaIndex,defaultValue);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetIntArrayValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetIntArrayCount of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetIntArrayCount00
static int tolua_SandboxToLua_Blackboard_GetIntArrayCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetIntArrayCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetIntArrayCount(key);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetIntArrayCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ContainsIntInArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_ContainsIntInArray00
static int tolua_SandboxToLua_Blackboard_ContainsIntInArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ContainsIntInArray'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ContainsIntInArray(key,value);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ContainsIntInArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearFloatArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_ClearFloatArray00
static int tolua_SandboxToLua_Blackboard_ClearFloatArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearFloatArray'", NULL);
#endif
  {
   self->ClearFloatArray(key);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearFloatArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddFloatToArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_AddFloatToArray00
static int tolua_SandboxToLua_Blackboard_AddFloatToArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddFloatToArray'", NULL);
#endif
  {
   self->AddFloatToArray(key,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddFloatToArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetFloatArrayValue of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetFloatArrayValue00
static int tolua_SandboxToLua_Blackboard_SetFloatArrayValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int luaIndex = ((int)  tolua_tonumber(tolua_S,3,0));
  float value = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetFloatArrayValue'", NULL);
#endif
  {
   self->SetFloatArrayValue(key,luaIndex,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetFloatArrayValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFloatArrayValue of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetFloatArrayValue00
static int tolua_SandboxToLua_Blackboard_GetFloatArrayValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int luaIndex = ((int)  tolua_tonumber(tolua_S,3,0));
  float defaultValue = ((float)  tolua_tonumber(tolua_S,4,0.0f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFloatArrayValue'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetFloatArrayValue(key,luaIndex,defaultValue);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFloatArrayValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFloatArrayCount of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetFloatArrayCount00
static int tolua_SandboxToLua_Blackboard_GetFloatArrayCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFloatArrayCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetFloatArrayCount(key);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFloatArrayCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearStringArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_ClearStringArray00
static int tolua_SandboxToLua_Blackboard_ClearStringArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearStringArray'", NULL);
#endif
  {
   self->ClearStringArray(key);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearStringArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddStringToArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_AddStringToArray00
static int tolua_SandboxToLua_Blackboard_AddStringToArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const std::string value = ((const std::string)  tolua_tocppstring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddStringToArray'", NULL);
#endif
  {
   self->AddStringToArray(key,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
   tolua_pushcppstring(tolua_S,(const char*)value);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddStringToArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetStringArrayValue of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetStringArrayValue00
static int tolua_SandboxToLua_Blackboard_SetStringArrayValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int luaIndex = ((int)  tolua_tonumber(tolua_S,3,0));
  const std::string value = ((const std::string)  tolua_tocppstring(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetStringArrayValue'", NULL);
#endif
  {
   self->SetStringArrayValue(key,luaIndex,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
   tolua_pushcppstring(tolua_S,(const char*)value);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetStringArrayValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStringArrayValue of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetStringArrayValue00
static int tolua_SandboxToLua_Blackboard_GetStringArrayValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int luaIndex = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStringArrayValue'", NULL);
#endif
  {
   std::string tolua_ret = (std::string)  self->GetStringArrayValue(key,luaIndex);
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStringArrayValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStringArrayCount of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetStringArrayCount00
static int tolua_SandboxToLua_Blackboard_GetStringArrayCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStringArrayCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetStringArrayCount(key);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStringArrayCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ContainsStringInArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_ContainsStringInArray00
static int tolua_SandboxToLua_Blackboard_ContainsStringInArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  const std::string value = ((const std::string)  tolua_tocppstring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ContainsStringInArray'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ContainsStringInArray(key,value);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
   tolua_pushcppstring(tolua_S,(const char*)value);
  }
 }
 return 3;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ContainsStringInArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearObjectIdArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_ClearObjectIdArray00
static int tolua_SandboxToLua_Blackboard_ClearObjectIdArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearObjectIdArray'", NULL);
#endif
  {
   self->ClearObjectIdArray(key);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearObjectIdArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddObjectIdToArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_AddObjectIdToArray00
static int tolua_SandboxToLua_Blackboard_AddObjectIdToArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddObjectIdToArray'", NULL);
#endif
  {
   self->AddObjectIdToArray(key,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddObjectIdToArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectIdArrayValue of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_SetObjectIdArrayValue00
static int tolua_SandboxToLua_Blackboard_SetObjectIdArrayValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int luaIndex = ((int)  tolua_tonumber(tolua_S,3,0));
  int value = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectIdArrayValue'", NULL);
#endif
  {
   self->SetObjectIdArrayValue(key,luaIndex,value);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectIdArrayValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectIdArrayValue of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetObjectIdArrayValue00
static int tolua_SandboxToLua_Blackboard_GetObjectIdArrayValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int luaIndex = ((int)  tolua_tonumber(tolua_S,3,0));
  int defaultValue = ((int)  tolua_tonumber(tolua_S,4,-1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectIdArrayValue'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetObjectIdArrayValue(key,luaIndex,defaultValue);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectIdArrayValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectIdArrayCount of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_GetObjectIdArrayCount00
static int tolua_SandboxToLua_Blackboard_GetObjectIdArrayCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectIdArrayCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetObjectIdArrayCount(key);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectIdArrayCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ContainsObjectIdInArray of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_ContainsObjectIdInArray00
static int tolua_SandboxToLua_Blackboard_ContainsObjectIdInArray00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  int value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ContainsObjectIdInArray'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ContainsObjectIdInArray(key,value);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ContainsObjectIdInArray'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Has of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_Has00
static int tolua_SandboxToLua_Blackboard_Has00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Blackboard* self = (const Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Has'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->Has(key);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Has'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Remove of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_Remove00
static int tolua_SandboxToLua_Blackboard_Remove00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
  const std::string key = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Remove'", NULL);
#endif
  {
   self->Remove(key);
   tolua_pushcppstring(tolua_S,(const char*)key);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Remove'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Clear of class  Blackboard */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_Blackboard_Clear00
static int tolua_SandboxToLua_Blackboard_Clear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Blackboard",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Blackboard* self = (Blackboard*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Clear'", NULL);
#endif
  {
   self->Clear();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Clear'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetName of class  DecisionAction */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionAction_GetName00
static int tolua_SandboxToLua_DecisionAction_GetName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const DecisionAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const DecisionAction* self = (const DecisionAction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetName'", NULL);
#endif
  {
   const std::string tolua_ret = (const std::string)  self->GetName();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStatus of class  DecisionAction */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionAction_GetStatus00
static int tolua_SandboxToLua_DecisionAction_GetStatus00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const DecisionAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const DecisionAction* self = (const DecisionAction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStatus'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetStatus();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStatus'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddChild of class  DecisionBranch */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionBranch_AddChild00
static int tolua_SandboxToLua_DecisionBranch_AddChild00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DecisionBranch",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"DecisionNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DecisionBranch* self = (DecisionBranch*)  tolua_tousertype(tolua_S,1,0);
  DecisionNode* child = ((DecisionNode*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddChild'", NULL);
#endif
  {
   self->AddChild(child);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddChild'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetChildCount of class  DecisionBranch */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionBranch_GetChildCount00
static int tolua_SandboxToLua_DecisionBranch_GetChildCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const DecisionBranch",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const DecisionBranch* self = (const DecisionBranch*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetChildCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetChildCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetChildCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetRoot of class  DecisionTree */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionTree_SetRoot00
static int tolua_SandboxToLua_DecisionTree_SetRoot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DecisionTree",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"DecisionNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DecisionTree* self = (DecisionTree*)  tolua_tousertype(tolua_S,1,0);
  DecisionNode* root = ((DecisionNode*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRoot'", NULL);
#endif
  {
   self->SetRoot(root);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetRoot'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Tick of class  DecisionTree */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionTree_Tick00
static int tolua_SandboxToLua_DecisionTree_Tick00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DecisionTree",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DecisionTree* self = (DecisionTree*)  tolua_tousertype(tolua_S,1,0);
  float deltaMs = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Tick'", NULL);
#endif
  {
   self->Tick(deltaMs);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Tick'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: BindToScript of class  LuaDecisionAction */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_LuaDecisionAction_BindToScript00
static int tolua_SandboxToLua_LuaDecisionAction_BindToScript00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaDecisionAction",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaDecisionAction* self = (LuaDecisionAction*)  tolua_tousertype(tolua_S,1,0);
  const std::string filepath = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'BindToScript'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->BindToScript(filepath);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)filepath);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BindToScript'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetOwner of class  LuaDecisionAction */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_LuaDecisionAction_GetOwner00
static int tolua_SandboxToLua_LuaDecisionAction_GetOwner00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const LuaDecisionAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const LuaDecisionAction* self = (const LuaDecisionAction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetOwner'", NULL);
#endif
  {
   SoldierObject* tolua_ret = (SoldierObject*)  self->GetOwner();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SoldierObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetOwner'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetOwner of class  DecisionTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionTreeDriver_GetOwner00
static int tolua_SandboxToLua_DecisionTreeDriver_GetOwner00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const DecisionTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const DecisionTreeDriver* self = (const DecisionTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetOwner'", NULL);
#endif
  {
   SoldierObject* tolua_ret = (SoldierObject*)  self->GetOwner();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SoldierObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetOwner'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBlackboard of class  DecisionTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionTreeDriver_GetBlackboard00
static int tolua_SandboxToLua_DecisionTreeDriver_GetBlackboard00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DecisionTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DecisionTreeDriver* self = (DecisionTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBlackboard'", NULL);
#endif
  {
   Blackboard* tolua_ret = (Blackboard*)  self->GetBlackboard();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Blackboard");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBlackboard'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewTree of class  DecisionTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionTreeDriver_NewTree00
static int tolua_SandboxToLua_DecisionTreeDriver_NewTree00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DecisionTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DecisionTreeDriver* self = (DecisionTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewTree'", NULL);
#endif
  {
   DecisionTree* tolua_ret = (DecisionTree*)  self->NewTree();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"DecisionTree");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewTree'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewBranch of class  DecisionTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionTreeDriver_NewBranch00
static int tolua_SandboxToLua_DecisionTreeDriver_NewBranch00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DecisionTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DecisionTreeDriver* self = (DecisionTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewBranch'", NULL);
#endif
  {
   DecisionBranch* tolua_ret = (DecisionBranch*)  self->NewBranch();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"DecisionBranch");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewBranch'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewLuaAction of class  DecisionTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionTreeDriver_NewLuaAction00
static int tolua_SandboxToLua_DecisionTreeDriver_NewLuaAction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DecisionTreeDriver",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DecisionTreeDriver* self = (DecisionTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  const std::string name = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewLuaAction'", NULL);
#endif
  {
   LuaDecisionAction* tolua_ret = (LuaDecisionAction*)  self->NewLuaAction(name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"LuaDecisionAction");
   tolua_pushcppstring(tolua_S,(const char*)name);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewLuaAction'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetTree of class  DecisionTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionTreeDriver_SetTree00
static int tolua_SandboxToLua_DecisionTreeDriver_SetTree00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DecisionTreeDriver",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"DecisionTree",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DecisionTreeDriver* self = (DecisionTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  DecisionTree* tree = ((DecisionTree*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetTree'", NULL);
#endif
  {
   self->SetTree(tree);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetTree'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTree of class  DecisionTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DecisionTreeDriver_GetTree00
static int tolua_SandboxToLua_DecisionTreeDriver_GetTree00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const DecisionTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const DecisionTreeDriver* self = (const DecisionTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTree'", NULL);
#endif
  {
   DecisionTree* tolua_ret = (DecisionTree*)  self->GetTree();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"DecisionTree");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTree'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetDebugName of class  BehaviorNode */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorNode_SetDebugName00
static int tolua_SandboxToLua_BehaviorNode_SetDebugName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorNode",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorNode* self = (BehaviorNode*)  tolua_tousertype(tolua_S,1,0);
  const std::string name = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetDebugName'", NULL);
#endif
  {
   self->SetDebugName(name);
   tolua_pushcppstring(tolua_S,(const char*)name);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetDebugName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDebugName of class  BehaviorNode */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorNode_GetDebugName00
static int tolua_SandboxToLua_BehaviorNode_GetDebugName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BehaviorNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BehaviorNode* self = (const BehaviorNode*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDebugName'", NULL);
#endif
  {
   const std::string tolua_ret = (const std::string)  self->GetDebugName();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDebugName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetName of class  BehaviorAction */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorAction_GetName00
static int tolua_SandboxToLua_BehaviorAction_GetName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BehaviorAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BehaviorAction* self = (const BehaviorAction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetName'", NULL);
#endif
  {
   const std::string tolua_ret = (const std::string)  self->GetName();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddChild of class  BehaviorComposite */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorComposite_AddChild00
static int tolua_SandboxToLua_BehaviorComposite_AddChild00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorComposite",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"BehaviorNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorComposite* self = (BehaviorComposite*)  tolua_tousertype(tolua_S,1,0);
  BehaviorNode* child = ((BehaviorNode*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddChild'", NULL);
#endif
  {
   self->AddChild(child);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddChild'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetChildCount of class  BehaviorComposite */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorComposite_GetChildCount00
static int tolua_SandboxToLua_BehaviorComposite_GetChildCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BehaviorComposite",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BehaviorComposite* self = (const BehaviorComposite*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetChildCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetChildCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetChildCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetRoot of class  BehaviorTree */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTree_SetRoot00
static int tolua_SandboxToLua_BehaviorTree_SetRoot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTree",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"BehaviorNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTree* self = (BehaviorTree*)  tolua_tousertype(tolua_S,1,0);
  BehaviorNode* root = ((BehaviorNode*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRoot'", NULL);
#endif
  {
   self->SetRoot(root);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetRoot'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Tick of class  BehaviorTree */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTree_Tick00
static int tolua_SandboxToLua_BehaviorTree_Tick00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTree",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTree* self = (BehaviorTree*)  tolua_tousertype(tolua_S,1,0);
  float deltaMs = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Tick'", NULL);
#endif
  {
   self->Tick(deltaMs);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Tick'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: BindToScript of class  LuaBehaviorAction */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_LuaBehaviorAction_BindToScript00
static int tolua_SandboxToLua_LuaBehaviorAction_BindToScript00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaBehaviorAction",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaBehaviorAction* self = (LuaBehaviorAction*)  tolua_tousertype(tolua_S,1,0);
  const std::string filepath = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'BindToScript'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->BindToScript(filepath);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)filepath);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'BindToScript'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetOwner of class  LuaBehaviorAction */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_LuaBehaviorAction_GetOwner00
static int tolua_SandboxToLua_LuaBehaviorAction_GetOwner00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const LuaBehaviorAction",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const LuaBehaviorAction* self = (const LuaBehaviorAction*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetOwner'", NULL);
#endif
  {
   SoldierObject* tolua_ret = (SoldierObject*)  self->GetOwner();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SoldierObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetOwner'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetOwner of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_GetOwner00
static int tolua_SandboxToLua_BehaviorTreeDriver_GetOwner00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BehaviorTreeDriver* self = (const BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetOwner'", NULL);
#endif
  {
   SoldierObject* tolua_ret = (SoldierObject*)  self->GetOwner();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SoldierObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetOwner'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetBlackboard of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_GetBlackboard00
static int tolua_SandboxToLua_BehaviorTreeDriver_GetBlackboard00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetBlackboard'", NULL);
#endif
  {
   Blackboard* tolua_ret = (Blackboard*)  self->GetBlackboard();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Blackboard");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetBlackboard'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewTree of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewTree00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewTree00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewTree'", NULL);
#endif
  {
   BehaviorTree* tolua_ret = (BehaviorTree*)  self->NewTree();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorTree");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewTree'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewSequence of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewSequence00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewSequence00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  float reevaluateMs = ((float)  tolua_tonumber(tolua_S,2,-1.0f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewSequence'", NULL);
#endif
  {
   BehaviorSequence* tolua_ret = (BehaviorSequence*)  self->NewSequence(reevaluateMs);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorSequence");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewSequence'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewSelector of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewSelector00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewSelector00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  float reevaluateMs = ((float)  tolua_tonumber(tolua_S,2,-1.0f));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewSelector'", NULL);
#endif
  {
   BehaviorSelector* tolua_ret = (BehaviorSelector*)  self->NewSelector(reevaluateMs);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorSelector");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewSelector'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewParallel of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewParallel00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewParallel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  int successPolicy = ((int)  tolua_tonumber(tolua_S,2,2));
  int failurePolicy = ((int)  tolua_tonumber(tolua_S,3,1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewParallel'", NULL);
#endif
  {
   BehaviorParallel* tolua_ret = (BehaviorParallel*)  self->NewParallel(successPolicy,failurePolicy);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorParallel");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewParallel'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewRandomSelector of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewRandomSelector00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewRandomSelector00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewRandomSelector'", NULL);
#endif
  {
   BehaviorRandomSelector* tolua_ret = (BehaviorRandomSelector*)  self->NewRandomSelector();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorRandomSelector");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewRandomSelector'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewLuaAction of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewLuaAction00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewLuaAction00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  const std::string name = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewLuaAction'", NULL);
#endif
  {
   LuaBehaviorAction* tolua_ret = (LuaBehaviorAction*)  self->NewLuaAction(name);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"LuaBehaviorAction");
   tolua_pushcppstring(tolua_S,(const char*)name);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewLuaAction'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewCondition of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewCondition00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewCondition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewCondition'", NULL);
#endif
  {
   LuaCondition* tolua_ret = (LuaCondition*)  self->NewCondition();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"LuaCondition");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewCondition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewWait of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewWait00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewWait00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  float waitMs = ((float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewWait'", NULL);
#endif
  {
   BehaviorNode* tolua_ret = (BehaviorNode*)  self->NewWait(waitMs);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewWait'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewInverter of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewInverter00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewInverter00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"BehaviorNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  BehaviorNode* child = ((BehaviorNode*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewInverter'", NULL);
#endif
  {
   BehaviorNode* tolua_ret = (BehaviorNode*)  self->NewInverter(child);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewInverter'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewForceSuccess of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewForceSuccess00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewForceSuccess00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"BehaviorNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  BehaviorNode* child = ((BehaviorNode*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewForceSuccess'", NULL);
#endif
  {
   BehaviorNode* tolua_ret = (BehaviorNode*)  self->NewForceSuccess(child);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewForceSuccess'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: NewForceFailure of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_NewForceFailure00
static int tolua_SandboxToLua_BehaviorTreeDriver_NewForceFailure00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"BehaviorNode",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  BehaviorNode* child = ((BehaviorNode*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'NewForceFailure'", NULL);
#endif
  {
   BehaviorNode* tolua_ret = (BehaviorNode*)  self->NewForceFailure(child);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'NewForceFailure'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetTree of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_SetTree00
static int tolua_SandboxToLua_BehaviorTreeDriver_SetTree00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"BehaviorTree",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  BehaviorTree* tree = ((BehaviorTree*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetTree'", NULL);
#endif
  {
   self->SetTree(tree);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetTree'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTree of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_GetTree00
static int tolua_SandboxToLua_BehaviorTreeDriver_GetTree00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BehaviorTreeDriver* self = (const BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTree'", NULL);
#endif
  {
   BehaviorTree* tolua_ret = (BehaviorTree*)  self->GetTree();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BehaviorTree");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTree'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetDebugTraceEnabled of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_SetDebugTraceEnabled00
static int tolua_SandboxToLua_BehaviorTreeDriver_SetDebugTraceEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  bool enabled = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetDebugTraceEnabled'", NULL);
#endif
  {
   self->SetDebugTraceEnabled(enabled);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetDebugTraceEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsDebugTraceEnabled of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_IsDebugTraceEnabled00
static int tolua_SandboxToLua_BehaviorTreeDriver_IsDebugTraceEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BehaviorTreeDriver* self = (const BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsDebugTraceEnabled'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsDebugTraceEnabled();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsDebugTraceEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetDebugTracePrintEnabled of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_SetDebugTracePrintEnabled00
static int tolua_SandboxToLua_BehaviorTreeDriver_SetDebugTracePrintEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  bool enabled = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetDebugTracePrintEnabled'", NULL);
#endif
  {
   self->SetDebugTracePrintEnabled(enabled);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetDebugTracePrintEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsDebugTracePrintEnabled of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_IsDebugTracePrintEnabled00
static int tolua_SandboxToLua_BehaviorTreeDriver_IsDebugTracePrintEnabled00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BehaviorTreeDriver* self = (const BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsDebugTracePrintEnabled'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsDebugTracePrintEnabled();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsDebugTracePrintEnabled'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastDebugTrace of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_GetLastDebugTrace00
static int tolua_SandboxToLua_BehaviorTreeDriver_GetLastDebugTrace00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BehaviorTreeDriver* self = (const BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastDebugTrace'", NULL);
#endif
  {
   const std::string tolua_ret = (const std::string)  self->GetLastDebugTrace();
   tolua_pushcppstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastDebugTrace'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetDebugTraceFrame of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_GetDebugTraceFrame00
static int tolua_SandboxToLua_BehaviorTreeDriver_GetDebugTraceFrame00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const BehaviorTreeDriver* self = (const BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetDebugTraceFrame'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetDebugTraceFrame();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetDebugTraceFrame'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetNodeDebugName of class  BehaviorTreeDriver */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BehaviorTreeDriver_SetNodeDebugName00
static int tolua_SandboxToLua_BehaviorTreeDriver_SetNodeDebugName00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"BehaviorTreeDriver",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"BehaviorNode",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  BehaviorTreeDriver* self = (BehaviorTreeDriver*)  tolua_tousertype(tolua_S,1,0);
  BehaviorNode* node = ((BehaviorNode*)  tolua_tousertype(tolua_S,2,0));
  const std::string name = ((const std::string)  tolua_tocppstring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetNodeDebugName'", NULL);
#endif
  {
   self->SetNodeDebugName(node,name);
   tolua_pushcppstring(tolua_S,(const char*)name);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetNodeDebugName'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_SandboxToLua_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"Ogre",0);
  tolua_beginmodule(tolua_S,"Ogre");
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"Vector2","Ogre::Vector2","",tolua_collect_Ogre__Vector2);
   #else
   tolua_cclass(tolua_S,"Vector2","Ogre::Vector2","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"Vector2");
   tolua_endmodule(tolua_S);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"Vector3","Ogre::Vector3","",tolua_collect_Ogre__Vector3);
   #else
   tolua_cclass(tolua_S,"Vector3","Ogre::Vector3","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"Vector3");
   tolua_endmodule(tolua_S);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"Quaternion","Ogre::Quaternion","",tolua_collect_Ogre__Quaternion);
   #else
   tolua_cclass(tolua_S,"Quaternion","Ogre::Quaternion","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"Quaternion");
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"ColourValue","Ogre::ColourValue","",NULL);
   tolua_beginmodule(tolua_S,"ColourValue");
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"Camera","Ogre::Camera","",NULL);
   tolua_beginmodule(tolua_S,"Camera");
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"Node","Ogre::Node","",NULL);
   tolua_beginmodule(tolua_S,"Node");
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"SceneNode","Ogre::SceneNode","",NULL);
   tolua_beginmodule(tolua_S,"SceneNode");
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"AnimationState","Ogre::AnimationState","",NULL);
   tolua_beginmodule(tolua_S,"AnimationState");
   tolua_endmodule(tolua_S);
  tolua_endmodule(tolua_S);
  tolua_module(tolua_S,"OIS",0);
  tolua_beginmodule(tolua_S,"OIS");
  tolua_endmodule(tolua_S);
  tolua_module(tolua_S,"Gorilla",0);
  tolua_beginmodule(tolua_S,"Gorilla");
  tolua_endmodule(tolua_S);
  tolua_module(tolua_S,"std",0);
  tolua_beginmodule(tolua_S,"std");
   tolua_cclass(tolua_S,"vector_Ogre__Vector3_","std::vector<Ogre::Vector3>","",NULL);
   tolua_beginmodule(tolua_S,"vector_Ogre__Vector3_");
   tolua_endmodule(tolua_S);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"vector_AgentObject__","std::vector<AgentObject*>","",tolua_collect_std__vector_AgentObject__);
   #else
   tolua_cclass(tolua_S,"vector_AgentObject__","std::vector<AgentObject*>","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"vector_AgentObject__");
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"vector_BlockObject__","std::vector<BlockObject*>","",NULL);
   tolua_beginmodule(tolua_S,"vector_BlockObject__");
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"vector_EntityObject__","std::vector<EntityObject*>","",NULL);
   tolua_beginmodule(tolua_S,"vector_EntityObject__");
   tolua_endmodule(tolua_S);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"rcConfig","rcConfig","",NULL);
  tolua_beginmodule(tolua_S,"rcConfig");
  tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"SOLDIER_STAND_SPEED",SOLDIER_STAND_SPEED);
  tolua_constant(tolua_S,"SOLDIER_STAND_HEIGHT",SOLDIER_STAND_HEIGHT);
  tolua_constant(tolua_S,"SOLDIER_CROUCH_SPEED",SOLDIER_CROUCH_SPEED);
  tolua_constant(tolua_S,"SOLDIER_CROUCH_HEIGHT",SOLDIER_CROUCH_HEIGHT);
  tolua_constant(tolua_S,"AGENT_OBJ_NONE",AGENT_OBJ_NONE);
  tolua_constant(tolua_S,"AGENT_OBJ_SEEKING",AGENT_OBJ_SEEKING);
  tolua_constant(tolua_S,"AGENT_OBJ_FOLLOWER",AGENT_OBJ_FOLLOWER);
  tolua_constant(tolua_S,"AGENT_OBJ_PATHING",AGENT_OBJ_PATHING);
  tolua_constant(tolua_S,"AGENT_OBJ_PURSUING",AGENT_OBJ_PURSUING);
  tolua_constant(tolua_S,"SSTATE_DEAD",SSTATE_DEAD);
  tolua_constant(tolua_S,"SSTATE_FIRE",SSTATE_FIRE);
  tolua_constant(tolua_S,"SSTATE_IDLE_AIM",SSTATE_IDLE_AIM);
  tolua_constant(tolua_S,"SSTATE_RUN_FORWARD",SSTATE_RUN_FORWARD);
  tolua_constant(tolua_S,"SSTATE_RUN_BACKWARD",SSTATE_RUN_BACKWARD);
  tolua_constant(tolua_S,"SSTATE_DEAD_HEADSHOT",SSTATE_DEAD_HEADSHOT);
  tolua_constant(tolua_S,"SSTATE_FALL_DEAD",SSTATE_FALL_DEAD);
  tolua_constant(tolua_S,"SSTATE_FALL_IDLE",SSTATE_FALL_IDLE);
  tolua_constant(tolua_S,"SSTATE_JUMP_LAND",SSTATE_JUMP_LAND);
  tolua_constant(tolua_S,"SSTATE_JUMP_UP",SSTATE_JUMP_UP);
  tolua_constant(tolua_S,"SSTATE_MELEE",SSTATE_MELEE);
  tolua_constant(tolua_S,"SSTATE_RELOAD",SSTATE_RELOAD);
  tolua_constant(tolua_S,"SSTATE_SMG_TRANSFORM",SSTATE_SMG_TRANSFORM);
  tolua_constant(tolua_S,"SSTATE_SNIPER_TRANSFORM",SSTATE_SNIPER_TRANSFORM);
  tolua_constant(tolua_S,"CROUCH_SSTATE_DEAD",CROUCH_SSTATE_DEAD);
  tolua_constant(tolua_S,"CROUCH_SSTATE_FIRE",CROUCH_SSTATE_FIRE);
  tolua_constant(tolua_S,"CROUCH_SSTATE_IDLE_AIM",CROUCH_SSTATE_IDLE_AIM);
  tolua_constant(tolua_S,"CROUCH_SSTATE_FORWARD",CROUCH_SSTATE_FORWARD);
  tolua_constant(tolua_S,"SSTATE_MAXCOUNT",SSTATE_MAXCOUNT);
  tolua_constant(tolua_S,"SOLDIER_STAND",SOLDIER_STAND);
  tolua_constant(tolua_S,"SOLDIER_CROUCH",SOLDIER_CROUCH);
  tolua_function(tolua_S,"QuaternionFromRotationDegrees",tolua_SandboxToLua_QuaternionFromRotationDegrees00);
  tolua_function(tolua_S,"QuaternionToRotationDegrees",tolua_SandboxToLua_QuaternionToRotationDegrees00);
  tolua_function(tolua_S,"BtVector3ToVector3",tolua_SandboxToLua_BtVector3ToVector300);
  tolua_function(tolua_S,"BtQuaternionToQuaternion",tolua_SandboxToLua_BtQuaternionToQuaternion00);
  tolua_function(tolua_S,"GetForward",tolua_SandboxToLua_GetForward00);
  tolua_cclass(tolua_S,"UIFrame","UIFrame","",NULL);
  tolua_beginmodule(tolua_S,"UIFrame");
   tolua_constant(tolua_S,"FONT_SMALL",UIFrame::FONT_SMALL);
   tolua_constant(tolua_S,"FONT_SMALL_MONO",UIFrame::FONT_SMALL_MONO);
   tolua_constant(tolua_S,"FONT_MEDIUM",UIFrame::FONT_MEDIUM);
   tolua_constant(tolua_S,"FONT_MEDIUM_MONO",UIFrame::FONT_MEDIUM_MONO);
   tolua_constant(tolua_S,"FONT_LARGE",UIFrame::FONT_LARGE);
   tolua_constant(tolua_S,"FONT_LARGE_MONO",UIFrame::FONT_LARGE_MONO);
   tolua_constant(tolua_S,"FONT_UNKNOWN",UIFrame::FONT_UNKNOWN);
   tolua_function(tolua_S,"addChild",tolua_SandboxToLua_UIFrame_addChild00);
   tolua_function(tolua_S,"setFontType",tolua_SandboxToLua_UIFrame_setFontType00);
   tolua_function(tolua_S,"setText",tolua_SandboxToLua_UIFrame_setText00);
   tolua_function(tolua_S,"setMarkupText",tolua_SandboxToLua_UIFrame_setMarkupText00);
   tolua_function(tolua_S,"setPosition",tolua_SandboxToLua_UIFrame_setPosition00);
   tolua_function(tolua_S,"setDimension",tolua_SandboxToLua_UIFrame_setDimension00);
   tolua_function(tolua_S,"setTextMargin",tolua_SandboxToLua_UIFrame_setTextMargin00);
   tolua_function(tolua_S,"setBackgroundColor",tolua_SandboxToLua_UIFrame_setBackgroundColor00);
   tolua_function(tolua_S,"setGradientColor",tolua_SandboxToLua_UIFrame_setGradientColor00);
   tolua_function(tolua_S,"setVisible",tolua_SandboxToLua_UIFrame_setVisible00);
   tolua_function(tolua_S,"isVisible",tolua_SandboxToLua_UIFrame_isVisible00);
   tolua_function(tolua_S,"GetDimension",tolua_SandboxToLua_UIFrame_GetDimension00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"UIManager","UIManager","",NULL);
  tolua_beginmodule(tolua_S,"UIManager");
   tolua_function(tolua_S,"CreateUIFrame",tolua_SandboxToLua_UIManager_CreateUIFrame00);
   tolua_function(tolua_S,"SetMarkupColor",tolua_SandboxToLua_UIManager_SetMarkupColor00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"IPlayerInput","IPlayerInput","",NULL);
  tolua_beginmodule(tolua_S,"IPlayerInput");
   tolua_function(tolua_S,"isKeyDown",tolua_SandboxToLua_IPlayerInput_isKeyDown00);
   tolua_function(tolua_S,"isKeyPressed",tolua_SandboxToLua_IPlayerInput_isKeyPressed00);
   tolua_function(tolua_S,"isKeyReleased",tolua_SandboxToLua_IPlayerInput_isKeyReleased00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"CameraService","CameraService","",NULL);
  tolua_beginmodule(tolua_S,"CameraService");
   tolua_function(tolua_S,"GetCamera",tolua_SandboxToLua_CameraService_GetCamera00);
   tolua_function(tolua_S,"GetCameraUp",tolua_SandboxToLua_CameraService_GetCameraUp00);
   tolua_function(tolua_S,"GetCameraLeft",tolua_SandboxToLua_CameraService_GetCameraLeft00);
   tolua_function(tolua_S,"GetCameraForward",tolua_SandboxToLua_CameraService_GetCameraForward00);
   tolua_function(tolua_S,"GetCameraPosition",tolua_SandboxToLua_CameraService_GetCameraPosition00);
   tolua_function(tolua_S,"GetCameraRotation",tolua_SandboxToLua_CameraService_GetCameraRotation00);
   tolua_function(tolua_S,"GetCameraOrientation",tolua_SandboxToLua_CameraService_GetCameraOrientation00);
   tolua_function(tolua_S,"GetRenderTime",tolua_SandboxToLua_CameraService_GetRenderTime00);
   tolua_function(tolua_S,"GetSimulateTime",tolua_SandboxToLua_CameraService_GetSimulateTime00);
   tolua_function(tolua_S,"GetTotalSimulateTime",tolua_SandboxToLua_CameraService_GetTotalSimulateTime00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"AgentAnim","AgentAnim","",NULL);
  tolua_beginmodule(tolua_S,"AgentAnim");
   tolua_function(tolua_S,"Init",tolua_SandboxToLua_AgentAnim_Init00);
   tolua_function(tolua_S,"Clear",tolua_SandboxToLua_AgentAnim_Clear00);
   tolua_function(tolua_S,"SetEnabled",tolua_SandboxToLua_AgentAnim_SetEnabled00);
   tolua_function(tolua_S,"SetLooping",tolua_SandboxToLua_AgentAnim_SetLooping00);
   tolua_function(tolua_S,"AddTime",tolua_SandboxToLua_AgentAnim_AddTime00);
   tolua_function(tolua_S,"GetTime",tolua_SandboxToLua_AgentAnim_GetTime00);
   tolua_function(tolua_S,"GetLength",tolua_SandboxToLua_AgentAnim_GetLength00);
   tolua_function(tolua_S,"GetWeight",tolua_SandboxToLua_AgentAnim_GetWeight00);
   tolua_function(tolua_S,"SetWeight",tolua_SandboxToLua_AgentAnim_SetWeight00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"AgentAnimStateMachine","AgentAnimStateMachine","",NULL);
  tolua_beginmodule(tolua_S,"AgentAnimStateMachine");
   tolua_function(tolua_S,"RequestState",tolua_SandboxToLua_AgentAnimStateMachine_RequestState00);
   tolua_function(tolua_S,"RequestState",tolua_SandboxToLua_AgentAnimStateMachine_RequestState01);
   tolua_function(tolua_S,"GetCurrStateID",tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateID00);
   tolua_function(tolua_S,"GetCurrStateName",tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateName00);
   tolua_function(tolua_S,"GetNextStateName",tolua_SandboxToLua_AgentAnimStateMachine_GetNextStateName00);
   tolua_function(tolua_S,"GetDesiredStateName",tolua_SandboxToLua_AgentAnimStateMachine_GetDesiredStateName00);
   tolua_function(tolua_S,"HasNextState",tolua_SandboxToLua_AgentAnimStateMachine_HasNextState00);
   tolua_function(tolua_S,"IsTransitioning",tolua_SandboxToLua_AgentAnimStateMachine_IsTransitioning00);
   tolua_function(tolua_S,"IsCurrentState",tolua_SandboxToLua_AgentAnimStateMachine_IsCurrentState00);
   tolua_function(tolua_S,"IsNextState",tolua_SandboxToLua_AgentAnimStateMachine_IsNextState00);
   tolua_function(tolua_S,"GetCurrStateTime",tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateTime00);
   tolua_function(tolua_S,"GetCurrStateLength",tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateLength00);
   tolua_function(tolua_S,"GetCurrStateProgress",tolua_SandboxToLua_AgentAnimStateMachine_GetCurrStateProgress00);
   tolua_function(tolua_S,"AddState",tolua_SandboxToLua_AgentAnimStateMachine_AddState00);
   tolua_function(tolua_S,"AddTransition",tolua_SandboxToLua_AgentAnimStateMachine_AddTransition00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"AgentAttrib","AgentAttrib","IComponent",NULL);
  tolua_beginmodule(tolua_S,"AgentAttrib");
   tolua_function(tolua_S,"SetHealth",tolua_SandboxToLua_AgentAttrib_SetHealth00);
   tolua_function(tolua_S,"GetHealth",tolua_SandboxToLua_AgentAttrib_GetHealth00);
   tolua_function(tolua_S,"SetMaxHealth",tolua_SandboxToLua_AgentAttrib_SetMaxHealth00);
   tolua_function(tolua_S,"GetMaxHealth",tolua_SandboxToLua_AgentAttrib_GetMaxHealth00);
   tolua_function(tolua_S,"SetStanceType",tolua_SandboxToLua_AgentAttrib_SetStanceType00);
   tolua_function(tolua_S,"GetStanceType",tolua_SandboxToLua_AgentAttrib_GetStanceType00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"AnimComponent","AnimComponent","IComponent",NULL);
  tolua_beginmodule(tolua_S,"AnimComponent");
   tolua_function(tolua_S,"GetBodyAnimation",tolua_SandboxToLua_AnimComponent_GetBodyAnimation00);
   tolua_function(tolua_S,"GetBodyAsm",tolua_SandboxToLua_AnimComponent_GetBodyAsm00);
   tolua_function(tolua_S,"GetWeaponAnimation",tolua_SandboxToLua_AnimComponent_GetWeaponAnimation00);
   tolua_function(tolua_S,"GetWeaponAsm",tolua_SandboxToLua_AnimComponent_GetWeaponAsm00);
   tolua_function(tolua_S,"HasNextAnim",tolua_SandboxToLua_AnimComponent_HasNextAnim00);
   tolua_function(tolua_S,"IsAnimReadyForMove",tolua_SandboxToLua_AnimComponent_IsAnimReadyForMove00);
   tolua_function(tolua_S,"IsAnimReadyForShoot",tolua_SandboxToLua_AnimComponent_IsAnimReadyForShoot00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BaseObject","BaseObject","SandboxObject",NULL);
  tolua_beginmodule(tolua_S,"BaseObject");
   tolua_function(tolua_S,"GetObjId",tolua_SandboxToLua_BaseObject_GetObjId00);
   tolua_function(tolua_S,"GetTeamId",tolua_SandboxToLua_BaseObject_GetTeamId00);
   tolua_function(tolua_S,"SetTeamId",tolua_SandboxToLua_BaseObject_SetTeamId00);
   tolua_function(tolua_S,"HasComponent",tolua_SandboxToLua_BaseObject_HasComponent00);
   tolua_function(tolua_S,"GetComponentCount",tolua_SandboxToLua_BaseObject_GetComponentCount00);
   tolua_function(tolua_S,"BuildComponentDebugString",tolua_SandboxToLua_BaseObject_BuildComponentDebugString00);
   tolua_function(tolua_S,"GetAIComponent",tolua_SandboxToLua_BaseObject_GetAIComponent00);
   tolua_function(tolua_S,"GetWeaponComponent",tolua_SandboxToLua_BaseObject_GetWeaponComponent00);
   tolua_function(tolua_S,"GetAnimComponent",tolua_SandboxToLua_BaseObject_GetAnimComponent00);
   tolua_function(tolua_S,"GetAttribComponent",tolua_SandboxToLua_BaseObject_GetAttribComponent00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BlockObject","BlockObject","BaseObject",NULL);
  tolua_beginmodule(tolua_S,"BlockObject");
   tolua_function(tolua_S,"setPosition",tolua_SandboxToLua_BlockObject_setPosition00);
   tolua_function(tolua_S,"setRotation",tolua_SandboxToLua_BlockObject_setRotation00);
   tolua_function(tolua_S,"setOrientation",tolua_SandboxToLua_BlockObject_setOrientation00);
   tolua_function(tolua_S,"GetMass",tolua_SandboxToLua_BlockObject_GetMass00);
   tolua_function(tolua_S,"SetMass",tolua_SandboxToLua_BlockObject_SetMass00);
   tolua_function(tolua_S,"GetRadius",tolua_SandboxToLua_BlockObject_GetRadius00);
   tolua_function(tolua_S,"GetPosition",tolua_SandboxToLua_BlockObject_GetPosition00);
   tolua_function(tolua_S,"setMaterial",tolua_SandboxToLua_BlockObject_setMaterial00);
   tolua_function(tolua_S,"applyImpulse",tolua_SandboxToLua_BlockObject_applyImpulse00);
   tolua_function(tolua_S,"applyAngularImpulse",tolua_SandboxToLua_BlockObject_applyAngularImpulse00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"AgentObject","AgentObject","BaseObject",NULL);
  tolua_beginmodule(tolua_S,"AgentObject");
   tolua_function(tolua_S,"initBody",tolua_SandboxToLua_AgentObject_initBody00);
   tolua_function(tolua_S,"getBody",tolua_SandboxToLua_AgentObject_getBody00);
   tolua_function(tolua_S,"GetAnimation",tolua_SandboxToLua_AgentObject_GetAnimation00);
   tolua_function(tolua_S,"GetObjectASM",tolua_SandboxToLua_AgentObject_GetObjectASM00);
   tolua_function(tolua_S,"SetRenderVisible",tolua_SandboxToLua_AgentObject_SetRenderVisible00);
   tolua_function(tolua_S,"getAgentType",tolua_SandboxToLua_AgentObject_getAgentType00);
   tolua_function(tolua_S,"setAgentType",tolua_SandboxToLua_AgentObject_setAgentType00);
   tolua_function(tolua_S,"GetPosition",tolua_SandboxToLua_AgentObject_GetPosition00);
   tolua_function(tolua_S,"GetOrientation",tolua_SandboxToLua_AgentObject_GetOrientation00);
   tolua_function(tolua_S,"GetUp",tolua_SandboxToLua_AgentObject_GetUp00);
   tolua_function(tolua_S,"GetLeft",tolua_SandboxToLua_AgentObject_GetLeft00);
   tolua_function(tolua_S,"GetForward",tolua_SandboxToLua_AgentObject_GetForward00);
   tolua_function(tolua_S,"setPosition",tolua_SandboxToLua_AgentObject_setPosition00);
   tolua_function(tolua_S,"setRotation",tolua_SandboxToLua_AgentObject_setRotation00);
   tolua_function(tolua_S,"setOrientation",tolua_SandboxToLua_AgentObject_setOrientation00);
   tolua_function(tolua_S,"SetForward",tolua_SandboxToLua_AgentObject_SetForward00);
   tolua_function(tolua_S,"SetVelocity",tolua_SandboxToLua_AgentObject_SetVelocity00);
   tolua_function(tolua_S,"SetTarget",tolua_SandboxToLua_AgentObject_SetTarget00);
   tolua_function(tolua_S,"SetTargetRadius",tolua_SandboxToLua_AgentObject_SetTargetRadius00);
   tolua_function(tolua_S,"GetTarget",tolua_SandboxToLua_AgentObject_GetTarget00);
   tolua_function(tolua_S,"GetVelocity",tolua_SandboxToLua_AgentObject_GetVelocity00);
   tolua_function(tolua_S,"SetMass",tolua_SandboxToLua_AgentObject_SetMass00);
   tolua_function(tolua_S,"SetHeight",tolua_SandboxToLua_AgentObject_SetHeight00);
   tolua_function(tolua_S,"SetRadius",tolua_SandboxToLua_AgentObject_SetRadius00);
   tolua_function(tolua_S,"SetSpeed",tolua_SandboxToLua_AgentObject_SetSpeed00);
   tolua_function(tolua_S,"SetHealth",tolua_SandboxToLua_AgentObject_SetHealth00);
   tolua_function(tolua_S,"GetMass",tolua_SandboxToLua_AgentObject_GetMass00);
   tolua_function(tolua_S,"GetSpeed",tolua_SandboxToLua_AgentObject_GetSpeed00);
   tolua_function(tolua_S,"GetHeight",tolua_SandboxToLua_AgentObject_GetHeight00);
   tolua_function(tolua_S,"GetRadius",tolua_SandboxToLua_AgentObject_GetRadius00);
   tolua_function(tolua_S,"GetHealth",tolua_SandboxToLua_AgentObject_GetHealth00);
   tolua_function(tolua_S,"SetMaxForce",tolua_SandboxToLua_AgentObject_SetMaxForce00);
   tolua_function(tolua_S,"SetMaxSpeed",tolua_SandboxToLua_AgentObject_SetMaxSpeed00);
   tolua_function(tolua_S,"GetMaxForce",tolua_SandboxToLua_AgentObject_GetMaxForce00);
   tolua_function(tolua_S,"GetMaxSpeed",tolua_SandboxToLua_AgentObject_GetMaxSpeed00);
   tolua_function(tolua_S,"GetTargetRadius",tolua_SandboxToLua_AgentObject_GetTargetRadius00);
   tolua_function(tolua_S,"PredictFuturePosition",tolua_SandboxToLua_AgentObject_PredictFuturePosition00);
   tolua_function(tolua_S,"ForceToPosition",tolua_SandboxToLua_AgentObject_ForceToPosition00);
   tolua_function(tolua_S,"ForceToFollowPath",tolua_SandboxToLua_AgentObject_ForceToFollowPath00);
   tolua_function(tolua_S,"ForceToStayOnPath",tolua_SandboxToLua_AgentObject_ForceToStayOnPath00);
   tolua_function(tolua_S,"ForceToWander",tolua_SandboxToLua_AgentObject_ForceToWander00);
   tolua_function(tolua_S,"ForceToTargetSpeed",tolua_SandboxToLua_AgentObject_ForceToTargetSpeed00);
   tolua_function(tolua_S,"ForceToAvoidAgents",tolua_SandboxToLua_AgentObject_ForceToAvoidAgents00);
   tolua_function(tolua_S,"ForceToAvoidObjects",tolua_SandboxToLua_AgentObject_ForceToAvoidObjects00);
   tolua_function(tolua_S,"ForceToCombine",tolua_SandboxToLua_AgentObject_ForceToCombine00);
   tolua_function(tolua_S,"ForceToSeparate",tolua_SandboxToLua_AgentObject_ForceToSeparate00);
   tolua_function(tolua_S,"ApplyForce",tolua_SandboxToLua_AgentObject_ApplyForce00);
   tolua_function(tolua_S,"SetPath",tolua_SandboxToLua_AgentObject_SetPath00);
   tolua_function(tolua_S,"GetPath",tolua_SandboxToLua_AgentObject_GetPath00);
   tolua_function(tolua_S,"HasPath",tolua_SandboxToLua_AgentObject_HasPath00);
   tolua_function(tolua_S,"GetDistanceAlongPath",tolua_SandboxToLua_AgentObject_GetDistanceAlongPath00);
   tolua_function(tolua_S,"GetNearestPointOnPath",tolua_SandboxToLua_AgentObject_GetNearestPointOnPath00);
   tolua_function(tolua_S,"GetPointOnPath",tolua_SandboxToLua_AgentObject_GetPointOnPath00);
   tolua_function(tolua_S,"IsMoving",tolua_SandboxToLua_AgentObject_IsMoving00);
   tolua_function(tolua_S,"IsFalling",tolua_SandboxToLua_AgentObject_IsFalling00);
   tolua_function(tolua_S,"OnGround",tolua_SandboxToLua_AgentObject_OnGround00);
   tolua_function(tolua_S,"GetUseCppFSM",tolua_SandboxToLua_AgentObject_GetUseCppFSM00);
   tolua_function(tolua_S,"GetCurStateId",tolua_SandboxToLua_AgentObject_GetCurStateId00);
   tolua_function(tolua_S,"GetCurStateName",tolua_SandboxToLua_AgentObject_GetCurStateName00);
   tolua_function(tolua_S,"GetInput",tolua_SandboxToLua_AgentObject_GetInput00);
   tolua_function(tolua_S,"RequestState",tolua_SandboxToLua_AgentObject_RequestState00);
   tolua_function(tolua_S,"HasNextAnim",tolua_SandboxToLua_AgentObject_HasNextAnim00);
   tolua_function(tolua_S,"IsAnimReadyForMove",tolua_SandboxToLua_AgentObject_IsAnimReadyForMove00);
   tolua_function(tolua_S,"IsAnimReadyForShoot",tolua_SandboxToLua_AgentObject_IsAnimReadyForShoot00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"WeaponComponent","WeaponComponent","IComponent",NULL);
  tolua_beginmodule(tolua_S,"WeaponComponent");
   tolua_function(tolua_S,"GetAnimation",tolua_SandboxToLua_WeaponComponent_GetAnimation00);
   tolua_function(tolua_S,"GetObjectASM",tolua_SandboxToLua_WeaponComponent_GetObjectASM00);
   tolua_function(tolua_S,"SyncToHandBone",tolua_SandboxToLua_WeaponComponent_SyncToHandBone00);
   tolua_function(tolua_S,"ShootBullet",tolua_SandboxToLua_WeaponComponent_ShootBullet00);
   tolua_function(tolua_S,"SetAmmo",tolua_SandboxToLua_WeaponComponent_SetAmmo00);
   tolua_function(tolua_S,"GetAmmo",tolua_SandboxToLua_WeaponComponent_GetAmmo00);
   tolua_function(tolua_S,"SetMaxAmmo",tolua_SandboxToLua_WeaponComponent_SetMaxAmmo00);
   tolua_function(tolua_S,"GetMaxAmmo",tolua_SandboxToLua_WeaponComponent_GetMaxAmmo00);
   tolua_function(tolua_S,"HasAmmo",tolua_SandboxToLua_WeaponComponent_HasAmmo00);
   tolua_function(tolua_S,"ConsumeAmmo",tolua_SandboxToLua_WeaponComponent_ConsumeAmmo00);
   tolua_function(tolua_S,"RestoreAmmo",tolua_SandboxToLua_WeaponComponent_RestoreAmmo00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"SoldierObject","SoldierObject","AgentObject",NULL);
  tolua_beginmodule(tolua_S,"SoldierObject");
   tolua_function(tolua_S,"initWeapon",tolua_SandboxToLua_SoldierObject_initWeapon00);
   tolua_function(tolua_S,"getWeapon",tolua_SandboxToLua_SoldierObject_getWeapon00);
   tolua_function(tolua_S,"SetRenderVisible",tolua_SandboxToLua_SoldierObject_SetRenderVisible00);
   tolua_function(tolua_S,"changeStanceType",tolua_SandboxToLua_SoldierObject_changeStanceType00);
   tolua_function(tolua_S,"getStanceType",tolua_SandboxToLua_SoldierObject_getStanceType00);
   tolua_function(tolua_S,"ShootBullet",tolua_SandboxToLua_SoldierObject_ShootBullet00);
   tolua_function(tolua_S,"GetInput",tolua_SandboxToLua_SoldierObject_GetInput00);
   tolua_function(tolua_S,"RequestState",tolua_SandboxToLua_SoldierObject_RequestState00);
   tolua_function(tolua_S,"HasNextAnim",tolua_SandboxToLua_SoldierObject_HasNextAnim00);
   tolua_function(tolua_S,"IsAnimReadyForMove",tolua_SandboxToLua_SoldierObject_IsAnimReadyForMove00);
   tolua_function(tolua_S,"IsAnimReadyForShoot",tolua_SandboxToLua_SoldierObject_IsAnimReadyForShoot00);
   tolua_function(tolua_S,"GetAnimController",tolua_SandboxToLua_SoldierObject_GetAnimController00);
   tolua_function(tolua_S,"GetAIController",tolua_SandboxToLua_SoldierObject_GetAIController00);
   tolua_function(tolua_S,"GetAI",tolua_SandboxToLua_SoldierObject_GetAI00);
   tolua_function(tolua_S,"SetMaxHealth",tolua_SandboxToLua_SoldierObject_SetMaxHealth00);
   tolua_function(tolua_S,"GetMaxHealth",tolua_SandboxToLua_SoldierObject_GetMaxHealth00);
   tolua_function(tolua_S,"SetAmmo",tolua_SandboxToLua_SoldierObject_SetAmmo00);
   tolua_function(tolua_S,"GetAmmo",tolua_SandboxToLua_SoldierObject_GetAmmo00);
   tolua_function(tolua_S,"SetMaxAmmo",tolua_SandboxToLua_SoldierObject_SetMaxAmmo00);
   tolua_function(tolua_S,"GetMaxAmmo",tolua_SandboxToLua_SoldierObject_GetMaxAmmo00);
   tolua_function(tolua_S,"HasAmmo",tolua_SandboxToLua_SoldierObject_HasAmmo00);
   tolua_function(tolua_S,"ConsumeAmmo",tolua_SandboxToLua_SoldierObject_ConsumeAmmo00);
   tolua_function(tolua_S,"RestoreAmmo",tolua_SandboxToLua_SoldierObject_RestoreAmmo00);
   tolua_function(tolua_S,"HasEnemy",tolua_SandboxToLua_SoldierObject_HasEnemy00);
   tolua_function(tolua_S,"HasEnemy",tolua_SandboxToLua_SoldierObject_HasEnemy01);
   tolua_function(tolua_S,"CanShootEnemy",tolua_SandboxToLua_SoldierObject_CanShootEnemy00);
   tolua_function(tolua_S,"CanShootEnemy",tolua_SandboxToLua_SoldierObject_CanShootEnemy01);
   tolua_function(tolua_S,"GetEnemy",tolua_SandboxToLua_SoldierObject_GetEnemy00);
   tolua_function(tolua_S,"GetBonePosition",tolua_SandboxToLua_SoldierObject_GetBonePosition00);
   tolua_function(tolua_S,"GetBoneForward",tolua_SandboxToLua_SoldierObject_GetBoneForward00);
   tolua_function(tolua_S,"HasMovePosition",tolua_SandboxToLua_SoldierObject_HasMovePosition00);
   tolua_function(tolua_S,"SetMovePosition",tolua_SandboxToLua_SoldierObject_SetMovePosition00);
   tolua_function(tolua_S,"ClearMovePosition",tolua_SandboxToLua_SoldierObject_ClearMovePosition00);
   tolua_function(tolua_S,"IsTargetReached",tolua_SandboxToLua_SoldierObject_IsTargetReached00);
   tolua_function(tolua_S,"EnterIdleAnim",tolua_SandboxToLua_SoldierObject_EnterIdleAnim00);
   tolua_function(tolua_S,"EnterMoveAnim",tolua_SandboxToLua_SoldierObject_EnterMoveAnim00);
   tolua_function(tolua_S,"EnterShootAnim",tolua_SandboxToLua_SoldierObject_EnterShootAnim00);
   tolua_function(tolua_S,"EnterReloadAnim",tolua_SandboxToLua_SoldierObject_EnterReloadAnim00);
   tolua_function(tolua_S,"EnterDeathAnim",tolua_SandboxToLua_SoldierObject_EnterDeathAnim00);
   tolua_variable(tolua_S,"__IAnimContextProvider__",tolua_get_SoldierObject___IAnimContextProvider__,NULL);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"AIController","AIController","IComponent",NULL);
  tolua_beginmodule(tolua_S,"AIController");
   tolua_function(tolua_S,"GetOwner",tolua_SandboxToLua_AIController_GetOwner00);
   tolua_function(tolua_S,"GetBlackboard",tolua_SandboxToLua_AIController_GetBlackboard00);
   tolua_function(tolua_S,"GetEnemy",tolua_SandboxToLua_AIController_GetEnemy00);
   tolua_function(tolua_S,"HasEnemy",tolua_SandboxToLua_AIController_HasEnemy00);
   tolua_function(tolua_S,"HasEnemy",tolua_SandboxToLua_AIController_HasEnemy01);
   tolua_function(tolua_S,"CanShootEnemy",tolua_SandboxToLua_AIController_CanShootEnemy00);
   tolua_function(tolua_S,"CanShootEnemy",tolua_SandboxToLua_AIController_CanShootEnemy01);
   tolua_function(tolua_S,"HasMovePosition",tolua_SandboxToLua_AIController_HasMovePosition00);
   tolua_function(tolua_S,"SetMovePosition",tolua_SandboxToLua_AIController_SetMovePosition00);
   tolua_function(tolua_S,"ClearMovePosition",tolua_SandboxToLua_AIController_ClearMovePosition00);
   tolua_function(tolua_S,"IsTargetReached",tolua_SandboxToLua_AIController_IsTargetReached00);
   tolua_function(tolua_S,"SetDriverByType",tolua_SandboxToLua_AIController_SetDriverByType00);
   tolua_function(tolua_S,"GetDecisionTreeDriver",tolua_SandboxToLua_AIController_GetDecisionTreeDriver00);
   tolua_function(tolua_S,"GetBehaviorTreeDriver",tolua_SandboxToLua_AIController_GetBehaviorTreeDriver00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"ObjectFactory","ObjectFactory","",NULL);
  tolua_beginmodule(tolua_S,"ObjectFactory");
   tolua_function(tolua_S,"CreatePlane",tolua_SandboxToLua_ObjectFactory_CreatePlane00);
   tolua_function(tolua_S,"CreateBlockObject",tolua_SandboxToLua_ObjectFactory_CreateBlockObject00);
   tolua_function(tolua_S,"CreateBlockBox",tolua_SandboxToLua_ObjectFactory_CreateBlockBox00);
   tolua_function(tolua_S,"CreateBullet",tolua_SandboxToLua_ObjectFactory_CreateBullet00);
   tolua_function(tolua_S,"CreateAgent",tolua_SandboxToLua_ObjectFactory_CreateAgent00);
   tolua_function(tolua_S,"CreateSoldier",tolua_SandboxToLua_ObjectFactory_CreateSoldier00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"SandboxMgr","SandboxMgr","",NULL);
  tolua_beginmodule(tolua_S,"SandboxMgr");
   tolua_function(tolua_S,"CallFile",tolua_SandboxToLua_SandboxMgr_CallFile00);
   tolua_function(tolua_S,"SetSkyBox",tolua_SandboxToLua_SandboxMgr_SetSkyBox00);
   tolua_function(tolua_S,"SetAmbientLight",tolua_SandboxToLua_SandboxMgr_SetAmbientLight00);
   tolua_function(tolua_S,"CreateDirectionalLight",tolua_SandboxToLua_SandboxMgr_CreateDirectionalLight00);
   tolua_function(tolua_S,"setMaterial",tolua_SandboxToLua_SandboxMgr_setMaterial00);
   tolua_function(tolua_S,"setMaterial",tolua_SandboxToLua_SandboxMgr_setMaterial01);
   tolua_function(tolua_S,"SetUseCppFsmFlag",tolua_SandboxToLua_SandboxMgr_SetUseCppFsmFlag00);
   tolua_function(tolua_S,"UpdateSceneGraph",tolua_SandboxToLua_SandboxMgr_UpdateSceneGraph00);
   tolua_function(tolua_S,"DefaultConfig",tolua_SandboxToLua_SandboxMgr_DefaultConfig00);
   tolua_function(tolua_S,"ApplySettingConfig",tolua_SandboxToLua_SandboxMgr_ApplySettingConfig00);
   tolua_function(tolua_S,"CreateNavigationMesh",tolua_SandboxToLua_SandboxMgr_CreateNavigationMesh00);
   tolua_function(tolua_S,"RandomPoint",tolua_SandboxToLua_SandboxMgr_RandomPoint00);
   tolua_function(tolua_S,"FindClosestPoint",tolua_SandboxToLua_SandboxMgr_FindClosestPoint00);
   tolua_function(tolua_S,"FindPath",tolua_SandboxToLua_SandboxMgr_FindPath00);
   tolua_function(tolua_S,"RayCastObjectId",tolua_SandboxToLua_SandboxMgr_RayCastObjectId00);
  tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"MGR_OBJ_NONE",MGR_OBJ_NONE);
  tolua_constant(tolua_S,"MGR_OBJ_BLOCK",MGR_OBJ_BLOCK);
  tolua_constant(tolua_S,"MGR_OBJ_AGENT",MGR_OBJ_AGENT);
  tolua_constant(tolua_S,"MGR_OBJ_ALLS",MGR_OBJ_ALLS);
  tolua_cclass(tolua_S,"ObjectManager","ObjectManager","",NULL);
  tolua_beginmodule(tolua_S,"ObjectManager");
   tolua_function(tolua_S,"clearAllObjects",tolua_SandboxToLua_ObjectManager_clearAllObjects00);
   tolua_function(tolua_S,"getAllAgents",tolua_SandboxToLua_ObjectManager_getAllAgents00);
   tolua_function(tolua_S,"getAllBlocks",tolua_SandboxToLua_ObjectManager_getAllBlocks00);
   tolua_function(tolua_S,"getSpecifyAgents",tolua_SandboxToLua_ObjectManager_getSpecifyAgents00);
   tolua_function(tolua_S,"getObjectCount",tolua_SandboxToLua_ObjectManager_getObjectCount00);
   tolua_function(tolua_S,"getAiAgentCount",tolua_SandboxToLua_ObjectManager_getAiAgentCount00);
   tolua_function(tolua_S,"getAiSoldierCount",tolua_SandboxToLua_ObjectManager_getAiSoldierCount00);
   tolua_function(tolua_S,"buildAiDebugSummary",tolua_SandboxToLua_ObjectManager_buildAiDebugSummary00);
   tolua_function(tolua_S,"configureAiScheduler",tolua_SandboxToLua_ObjectManager_configureAiScheduler00);
   tolua_function(tolua_S,"buildAiSchedulerDebugSummary",tolua_SandboxToLua_ObjectManager_buildAiSchedulerDebugSummary00);
   tolua_function(tolua_S,"clearTeamBlackboardFacts",tolua_SandboxToLua_ObjectManager_clearTeamBlackboardFacts00);
   tolua_function(tolua_S,"configureTeamBlackboard",tolua_SandboxToLua_ObjectManager_configureTeamBlackboard00);
   tolua_function(tolua_S,"rememberTeamEnemyFact",tolua_SandboxToLua_ObjectManager_rememberTeamEnemyFact00);
   tolua_function(tolua_S,"writeBestTeamEnemyFactToBlackboard",tolua_SandboxToLua_ObjectManager_writeBestTeamEnemyFactToBlackboard00);
   tolua_function(tolua_S,"getTeamBlackboardFactCount",tolua_SandboxToLua_ObjectManager_getTeamBlackboardFactCount00);
   tolua_function(tolua_S,"getTeamBlackboardReportCount",tolua_SandboxToLua_ObjectManager_getTeamBlackboardReportCount00);
   tolua_function(tolua_S,"buildTeamBlackboardDebugSummary",tolua_SandboxToLua_ObjectManager_buildTeamBlackboardDebugSummary00);
   tolua_function(tolua_S,"clearTacticalInfluence",tolua_SandboxToLua_ObjectManager_clearTacticalInfluence00);
   tolua_function(tolua_S,"configureTacticalInfluence",tolua_SandboxToLua_ObjectManager_configureTacticalInfluence00);
   tolua_function(tolua_S,"configureTacticalInfluenceFromNavMesh",tolua_SandboxToLua_ObjectManager_configureTacticalInfluenceFromNavMesh00);
   tolua_function(tolua_S,"clearTacticalInfluenceLayer",tolua_SandboxToLua_ObjectManager_clearTacticalInfluenceLayer00);
   tolua_function(tolua_S,"setTacticalInfluenceLayerOptions",tolua_SandboxToLua_ObjectManager_setTacticalInfluenceLayerOptions00);
   tolua_function(tolua_S,"addTacticalInfluenceSource",tolua_SandboxToLua_ObjectManager_addTacticalInfluenceSource00);
   tolua_function(tolua_S,"addTacticalInfluencePoint",tolua_SandboxToLua_ObjectManager_addTacticalInfluencePoint00);
   tolua_function(tolua_S,"spreadTacticalInfluenceLayer",tolua_SandboxToLua_ObjectManager_spreadTacticalInfluenceLayer00);
   tolua_function(tolua_S,"sampleTacticalInfluence",tolua_SandboxToLua_ObjectManager_sampleTacticalInfluence00);
   tolua_function(tolua_S,"scoreTacticalPosition",tolua_SandboxToLua_ObjectManager_scoreTacticalPosition00);
   tolua_function(tolua_S,"findBestTacticalPosition",tolua_SandboxToLua_ObjectManager_findBestTacticalPosition00);
   tolua_function(tolua_S,"scoreTacticalQueryPosition",tolua_SandboxToLua_ObjectManager_scoreTacticalQueryPosition00);
   tolua_function(tolua_S,"findBestTacticalQueryPosition",tolua_SandboxToLua_ObjectManager_findBestTacticalQueryPosition00);
   tolua_function(tolua_S,"findBestSupportPosition",tolua_SandboxToLua_ObjectManager_findBestSupportPosition00);
   tolua_function(tolua_S,"findLowThreatPosition",tolua_SandboxToLua_ObjectManager_findLowThreatPosition00);
   tolua_function(tolua_S,"configureTacticalEvents",tolua_SandboxToLua_ObjectManager_configureTacticalEvents00);
   tolua_function(tolua_S,"clearTacticalEvents",tolua_SandboxToLua_ObjectManager_clearTacticalEvents00);
   tolua_function(tolua_S,"publishTacticalEvent",tolua_SandboxToLua_ObjectManager_publishTacticalEvent00);
   tolua_function(tolua_S,"getTacticalEventCount",tolua_SandboxToLua_ObjectManager_getTacticalEventCount00);
   tolua_function(tolua_S,"getTacticalEventTypeCount",tolua_SandboxToLua_ObjectManager_getTacticalEventTypeCount00);
   tolua_function(tolua_S,"getLastTacticalEventPosition",tolua_SandboxToLua_ObjectManager_getLastTacticalEventPosition00);
   tolua_function(tolua_S,"getTacticalEventDebugRecordCount",tolua_SandboxToLua_ObjectManager_getTacticalEventDebugRecordCount00);
   tolua_function(tolua_S,"getTacticalEventDebugType",tolua_SandboxToLua_ObjectManager_getTacticalEventDebugType00);
   tolua_function(tolua_S,"getTacticalEventDebugSenderId",tolua_SandboxToLua_ObjectManager_getTacticalEventDebugSenderId00);
   tolua_function(tolua_S,"getTacticalEventDebugTargetId",tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTargetId00);
   tolua_function(tolua_S,"getTacticalEventDebugTeamId",tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTeamId00);
   tolua_function(tolua_S,"getTacticalEventDebugTargetTeamId",tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTargetTeamId00);
   tolua_function(tolua_S,"getTacticalEventDebugPosition",tolua_SandboxToLua_ObjectManager_getTacticalEventDebugPosition00);
   tolua_function(tolua_S,"getTacticalEventDebugTimeMs",tolua_SandboxToLua_ObjectManager_getTacticalEventDebugTimeMs00);
   tolua_function(tolua_S,"getTacticalEventDebugRemainingTtlMs",tolua_SandboxToLua_ObjectManager_getTacticalEventDebugRemainingTtlMs00);
   tolua_function(tolua_S,"rebuildTacticalDangerLayer",tolua_SandboxToLua_ObjectManager_rebuildTacticalDangerLayer00);
   tolua_function(tolua_S,"rebuildTacticalTeamLayer",tolua_SandboxToLua_ObjectManager_rebuildTacticalTeamLayer00);
   tolua_function(tolua_S,"rebuildTacticalObjectiveLayer",tolua_SandboxToLua_ObjectManager_rebuildTacticalObjectiveLayer00);
   tolua_function(tolua_S,"getTacticalInfluenceLayerActiveCellCount",tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerActiveCellCount00);
   tolua_function(tolua_S,"getTacticalInfluenceLayerCellWriteCount",tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerCellWriteCount00);
   tolua_function(tolua_S,"getTacticalInfluenceLayerDebugCellCount",tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerDebugCellCount00);
   tolua_function(tolua_S,"getTacticalInfluenceLayerDebugCellPosition",tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerDebugCellPosition00);
   tolua_function(tolua_S,"getTacticalInfluenceLayerDebugCellValue",tolua_SandboxToLua_ObjectManager_getTacticalInfluenceLayerDebugCellValue00);
   tolua_function(tolua_S,"drawTacticalInfluenceLayer",tolua_SandboxToLua_ObjectManager_drawTacticalInfluenceLayer00);
   tolua_function(tolua_S,"rebuildTacticalInfluenceLayerDebugVisual",tolua_SandboxToLua_ObjectManager_rebuildTacticalInfluenceLayerDebugVisual00);
   tolua_function(tolua_S,"setTacticalInfluenceDebugVisible",tolua_SandboxToLua_ObjectManager_setTacticalInfluenceDebugVisible00);
   tolua_function(tolua_S,"clearTacticalInfluenceDebugVisuals",tolua_SandboxToLua_ObjectManager_clearTacticalInfluenceDebugVisuals00);
   tolua_function(tolua_S,"getTacticalInfluenceActiveCellCount",tolua_SandboxToLua_ObjectManager_getTacticalInfluenceActiveCellCount00);
   tolua_function(tolua_S,"getTacticalInfluenceCellWriteCount",tolua_SandboxToLua_ObjectManager_getTacticalInfluenceCellWriteCount00);
   tolua_function(tolua_S,"getTacticalInfluenceQueryCount",tolua_SandboxToLua_ObjectManager_getTacticalInfluenceQueryCount00);
   tolua_function(tolua_S,"buildTacticalInfluenceDebugSummary",tolua_SandboxToLua_ObjectManager_buildTacticalInfluenceDebugSummary00);
   tolua_function(tolua_S,"buildAiEventDebugSummary",tolua_SandboxToLua_ObjectManager_buildAiEventDebugSummary00);
   tolua_function(tolua_S,"runAiEventScopeSelfTest",tolua_SandboxToLua_ObjectManager_runAiEventScopeSelfTest00);
   tolua_function(tolua_S,"buildObjectDebugSummary",tolua_SandboxToLua_ObjectManager_buildObjectDebugSummary00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"NavigationMesh","NavigationMesh","",NULL);
  tolua_beginmodule(tolua_S,"NavigationMesh");
   tolua_function(tolua_S,"IsValid",tolua_SandboxToLua_NavigationMesh_IsValid00);
   tolua_function(tolua_S,"FindClosestPoint",tolua_SandboxToLua_NavigationMesh_FindClosestPoint00);
   tolua_function(tolua_S,"FindPath",tolua_SandboxToLua_NavigationMesh_FindPath00);
   tolua_function(tolua_S,"RandomPoint",tolua_SandboxToLua_NavigationMesh_RandomPoint00);
   tolua_function(tolua_S,"HasDebugVisual",tolua_SandboxToLua_NavigationMesh_HasDebugVisual00);
   tolua_function(tolua_S,"SetDebugVisible",tolua_SandboxToLua_NavigationMesh_SetDebugVisible00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"Blackboard","Blackboard","",NULL);
  tolua_beginmodule(tolua_S,"Blackboard");
   tolua_function(tolua_S,"GetAgentOwner",tolua_SandboxToLua_Blackboard_GetAgentOwner00);
   tolua_function(tolua_S,"GetOwner",tolua_SandboxToLua_Blackboard_GetOwner00);
   tolua_function(tolua_S,"SetAgent",tolua_SandboxToLua_Blackboard_SetAgent00);
   tolua_function(tolua_S,"GetAgent",tolua_SandboxToLua_Blackboard_GetAgent00);
   tolua_function(tolua_S,"SetFloat",tolua_SandboxToLua_Blackboard_SetFloat00);
   tolua_function(tolua_S,"GetFloat",tolua_SandboxToLua_Blackboard_GetFloat00);
   tolua_function(tolua_S,"SetInt",tolua_SandboxToLua_Blackboard_SetInt00);
   tolua_function(tolua_S,"GetInt",tolua_SandboxToLua_Blackboard_GetInt00);
   tolua_function(tolua_S,"SetObjectId",tolua_SandboxToLua_Blackboard_SetObjectId00);
   tolua_function(tolua_S,"GetObjectId",tolua_SandboxToLua_Blackboard_GetObjectId00);
   tolua_function(tolua_S,"SetBool",tolua_SandboxToLua_Blackboard_SetBool00);
   tolua_function(tolua_S,"GetBool",tolua_SandboxToLua_Blackboard_GetBool00);
   tolua_function(tolua_S,"SetString",tolua_SandboxToLua_Blackboard_SetString00);
   tolua_function(tolua_S,"GetString",tolua_SandboxToLua_Blackboard_GetString00);
   tolua_function(tolua_S,"SetVec3",tolua_SandboxToLua_Blackboard_SetVec300);
   tolua_function(tolua_S,"GetVec3",tolua_SandboxToLua_Blackboard_GetVec300);
   tolua_function(tolua_S,"ClearIntArray",tolua_SandboxToLua_Blackboard_ClearIntArray00);
   tolua_function(tolua_S,"AddIntToArray",tolua_SandboxToLua_Blackboard_AddIntToArray00);
   tolua_function(tolua_S,"SetIntArrayValue",tolua_SandboxToLua_Blackboard_SetIntArrayValue00);
   tolua_function(tolua_S,"GetIntArrayValue",tolua_SandboxToLua_Blackboard_GetIntArrayValue00);
   tolua_function(tolua_S,"GetIntArrayCount",tolua_SandboxToLua_Blackboard_GetIntArrayCount00);
   tolua_function(tolua_S,"ContainsIntInArray",tolua_SandboxToLua_Blackboard_ContainsIntInArray00);
   tolua_function(tolua_S,"ClearFloatArray",tolua_SandboxToLua_Blackboard_ClearFloatArray00);
   tolua_function(tolua_S,"AddFloatToArray",tolua_SandboxToLua_Blackboard_AddFloatToArray00);
   tolua_function(tolua_S,"SetFloatArrayValue",tolua_SandboxToLua_Blackboard_SetFloatArrayValue00);
   tolua_function(tolua_S,"GetFloatArrayValue",tolua_SandboxToLua_Blackboard_GetFloatArrayValue00);
   tolua_function(tolua_S,"GetFloatArrayCount",tolua_SandboxToLua_Blackboard_GetFloatArrayCount00);
   tolua_function(tolua_S,"ClearStringArray",tolua_SandboxToLua_Blackboard_ClearStringArray00);
   tolua_function(tolua_S,"AddStringToArray",tolua_SandboxToLua_Blackboard_AddStringToArray00);
   tolua_function(tolua_S,"SetStringArrayValue",tolua_SandboxToLua_Blackboard_SetStringArrayValue00);
   tolua_function(tolua_S,"GetStringArrayValue",tolua_SandboxToLua_Blackboard_GetStringArrayValue00);
   tolua_function(tolua_S,"GetStringArrayCount",tolua_SandboxToLua_Blackboard_GetStringArrayCount00);
   tolua_function(tolua_S,"ContainsStringInArray",tolua_SandboxToLua_Blackboard_ContainsStringInArray00);
   tolua_function(tolua_S,"ClearObjectIdArray",tolua_SandboxToLua_Blackboard_ClearObjectIdArray00);
   tolua_function(tolua_S,"AddObjectIdToArray",tolua_SandboxToLua_Blackboard_AddObjectIdToArray00);
   tolua_function(tolua_S,"SetObjectIdArrayValue",tolua_SandboxToLua_Blackboard_SetObjectIdArrayValue00);
   tolua_function(tolua_S,"GetObjectIdArrayValue",tolua_SandboxToLua_Blackboard_GetObjectIdArrayValue00);
   tolua_function(tolua_S,"GetObjectIdArrayCount",tolua_SandboxToLua_Blackboard_GetObjectIdArrayCount00);
   tolua_function(tolua_S,"ContainsObjectIdInArray",tolua_SandboxToLua_Blackboard_ContainsObjectIdInArray00);
   tolua_function(tolua_S,"Has",tolua_SandboxToLua_Blackboard_Has00);
   tolua_function(tolua_S,"Remove",tolua_SandboxToLua_Blackboard_Remove00);
   tolua_function(tolua_S,"Clear",tolua_SandboxToLua_Blackboard_Clear00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DecisionNode","DecisionNode","",NULL);
  tolua_beginmodule(tolua_S,"DecisionNode");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DecisionAction","DecisionAction","DecisionNode",NULL);
  tolua_beginmodule(tolua_S,"DecisionAction");
   tolua_function(tolua_S,"GetName",tolua_SandboxToLua_DecisionAction_GetName00);
   tolua_function(tolua_S,"GetStatus",tolua_SandboxToLua_DecisionAction_GetStatus00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DecisionBranch","DecisionBranch","DecisionNode",NULL);
  tolua_beginmodule(tolua_S,"DecisionBranch");
   tolua_function(tolua_S,"AddChild",tolua_SandboxToLua_DecisionBranch_AddChild00);
   tolua_function(tolua_S,"GetChildCount",tolua_SandboxToLua_DecisionBranch_GetChildCount00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DecisionTree","DecisionTree","",NULL);
  tolua_beginmodule(tolua_S,"DecisionTree");
   tolua_function(tolua_S,"SetRoot",tolua_SandboxToLua_DecisionTree_SetRoot00);
   tolua_function(tolua_S,"Tick",tolua_SandboxToLua_DecisionTree_Tick00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"LuaDecisionAction","LuaDecisionAction","DecisionAction",NULL);
  tolua_beginmodule(tolua_S,"LuaDecisionAction");
   tolua_function(tolua_S,"BindToScript",tolua_SandboxToLua_LuaDecisionAction_BindToScript00);
   tolua_function(tolua_S,"GetOwner",tolua_SandboxToLua_LuaDecisionAction_GetOwner00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DecisionTreeDriver","DecisionTreeDriver","IDecisionDriver",NULL);
  tolua_beginmodule(tolua_S,"DecisionTreeDriver");
   tolua_function(tolua_S,"GetOwner",tolua_SandboxToLua_DecisionTreeDriver_GetOwner00);
   tolua_function(tolua_S,"GetBlackboard",tolua_SandboxToLua_DecisionTreeDriver_GetBlackboard00);
   tolua_function(tolua_S,"NewTree",tolua_SandboxToLua_DecisionTreeDriver_NewTree00);
   tolua_function(tolua_S,"NewBranch",tolua_SandboxToLua_DecisionTreeDriver_NewBranch00);
   tolua_function(tolua_S,"NewLuaAction",tolua_SandboxToLua_DecisionTreeDriver_NewLuaAction00);
   tolua_function(tolua_S,"SetTree",tolua_SandboxToLua_DecisionTreeDriver_SetTree00);
   tolua_function(tolua_S,"GetTree",tolua_SandboxToLua_DecisionTreeDriver_GetTree00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BehaviorNode","BehaviorNode","",NULL);
  tolua_beginmodule(tolua_S,"BehaviorNode");
   tolua_function(tolua_S,"SetDebugName",tolua_SandboxToLua_BehaviorNode_SetDebugName00);
   tolua_function(tolua_S,"GetDebugName",tolua_SandboxToLua_BehaviorNode_GetDebugName00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BehaviorAction","BehaviorAction","BehaviorNode",NULL);
  tolua_beginmodule(tolua_S,"BehaviorAction");
   tolua_function(tolua_S,"GetName",tolua_SandboxToLua_BehaviorAction_GetName00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BehaviorComposite","BehaviorComposite","BehaviorNode",NULL);
  tolua_beginmodule(tolua_S,"BehaviorComposite");
   tolua_function(tolua_S,"AddChild",tolua_SandboxToLua_BehaviorComposite_AddChild00);
   tolua_function(tolua_S,"GetChildCount",tolua_SandboxToLua_BehaviorComposite_GetChildCount00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BehaviorSequence","BehaviorSequence","BehaviorComposite",NULL);
  tolua_beginmodule(tolua_S,"BehaviorSequence");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BehaviorSelector","BehaviorSelector","BehaviorComposite",NULL);
  tolua_beginmodule(tolua_S,"BehaviorSelector");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BehaviorParallel","BehaviorParallel","BehaviorComposite",NULL);
  tolua_beginmodule(tolua_S,"BehaviorParallel");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BehaviorRandomSelector","BehaviorRandomSelector","BehaviorComposite",NULL);
  tolua_beginmodule(tolua_S,"BehaviorRandomSelector");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BehaviorTree","BehaviorTree","",NULL);
  tolua_beginmodule(tolua_S,"BehaviorTree");
   tolua_function(tolua_S,"SetRoot",tolua_SandboxToLua_BehaviorTree_SetRoot00);
   tolua_function(tolua_S,"Tick",tolua_SandboxToLua_BehaviorTree_Tick00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"LuaBehaviorAction","LuaBehaviorAction","BehaviorAction",NULL);
  tolua_beginmodule(tolua_S,"LuaBehaviorAction");
   tolua_function(tolua_S,"BindToScript",tolua_SandboxToLua_LuaBehaviorAction_BindToScript00);
   tolua_function(tolua_S,"GetOwner",tolua_SandboxToLua_LuaBehaviorAction_GetOwner00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"LuaCondition","LuaCondition","BehaviorNode",NULL);
  tolua_beginmodule(tolua_S,"LuaCondition");
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"BehaviorTreeDriver","BehaviorTreeDriver","IDecisionDriver",NULL);
  tolua_beginmodule(tolua_S,"BehaviorTreeDriver");
   tolua_function(tolua_S,"GetOwner",tolua_SandboxToLua_BehaviorTreeDriver_GetOwner00);
   tolua_function(tolua_S,"GetBlackboard",tolua_SandboxToLua_BehaviorTreeDriver_GetBlackboard00);
   tolua_function(tolua_S,"NewTree",tolua_SandboxToLua_BehaviorTreeDriver_NewTree00);
   tolua_function(tolua_S,"NewSequence",tolua_SandboxToLua_BehaviorTreeDriver_NewSequence00);
   tolua_function(tolua_S,"NewSelector",tolua_SandboxToLua_BehaviorTreeDriver_NewSelector00);
   tolua_function(tolua_S,"NewParallel",tolua_SandboxToLua_BehaviorTreeDriver_NewParallel00);
   tolua_function(tolua_S,"NewRandomSelector",tolua_SandboxToLua_BehaviorTreeDriver_NewRandomSelector00);
   tolua_function(tolua_S,"NewLuaAction",tolua_SandboxToLua_BehaviorTreeDriver_NewLuaAction00);
   tolua_function(tolua_S,"NewCondition",tolua_SandboxToLua_BehaviorTreeDriver_NewCondition00);
   tolua_function(tolua_S,"NewWait",tolua_SandboxToLua_BehaviorTreeDriver_NewWait00);
   tolua_function(tolua_S,"NewInverter",tolua_SandboxToLua_BehaviorTreeDriver_NewInverter00);
   tolua_function(tolua_S,"NewForceSuccess",tolua_SandboxToLua_BehaviorTreeDriver_NewForceSuccess00);
   tolua_function(tolua_S,"NewForceFailure",tolua_SandboxToLua_BehaviorTreeDriver_NewForceFailure00);
   tolua_function(tolua_S,"SetTree",tolua_SandboxToLua_BehaviorTreeDriver_SetTree00);
   tolua_function(tolua_S,"GetTree",tolua_SandboxToLua_BehaviorTreeDriver_GetTree00);
   tolua_function(tolua_S,"SetDebugTraceEnabled",tolua_SandboxToLua_BehaviorTreeDriver_SetDebugTraceEnabled00);
   tolua_function(tolua_S,"IsDebugTraceEnabled",tolua_SandboxToLua_BehaviorTreeDriver_IsDebugTraceEnabled00);
   tolua_function(tolua_S,"SetDebugTracePrintEnabled",tolua_SandboxToLua_BehaviorTreeDriver_SetDebugTracePrintEnabled00);
   tolua_function(tolua_S,"IsDebugTracePrintEnabled",tolua_SandboxToLua_BehaviorTreeDriver_IsDebugTracePrintEnabled00);
   tolua_function(tolua_S,"GetLastDebugTrace",tolua_SandboxToLua_BehaviorTreeDriver_GetLastDebugTrace00);
   tolua_function(tolua_S,"GetDebugTraceFrame",tolua_SandboxToLua_BehaviorTreeDriver_GetDebugTraceFrame00);
   tolua_function(tolua_S,"SetNodeDebugName",tolua_SandboxToLua_BehaviorTreeDriver_SetNodeDebugName00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_SandboxToLua (lua_State* tolua_S) {
 return tolua_SandboxToLua_open(tolua_S);
};
#endif

