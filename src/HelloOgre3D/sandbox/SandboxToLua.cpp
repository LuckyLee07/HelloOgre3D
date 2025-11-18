/*
** Lua binding: SandboxToLua
** Generated automatically by tolua++-1.0.92 on Tue Nov 18 20:19:22 2025.
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
#include "ogre3d/include/Ogre.h"
#include "ois/include/OISKeyboard.h"
#include "GameDefine.h"
#include "GameFunction.h"
#include "core/object/BaseObject.h"
#include "object/UIComponent.h"
#include "input/IPlayerInput.h"
#include "animation/AgentAnim.h"
#include "animation/AgentAnimStateMachine.h"
#include "object/BlockObject.h"
#include "object/VehicleObject.h"
#include "object/AgentObject.h"
#include "object/SoldierObject.h"
#include "manager/SandboxMgr.h"
#include "manager/ObjectManager.h"
#include "debug/DebugDrawer.h"

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
 tolua_usertype(tolua_S,"AgentObject");
 tolua_usertype(tolua_S,"Ogre::AnimationState");
 tolua_usertype(tolua_S,"std::vector<Ogre::Vector3>");
 tolua_usertype(tolua_S,"btQuaternion");
 tolua_usertype(tolua_S,"SandboxObject");
 tolua_usertype(tolua_S,"std::vector<EntityObject*>");
 tolua_usertype(tolua_S,"Ogre::Light");
 tolua_usertype(tolua_S,"BaseObject");
 tolua_usertype(tolua_S,"Ogre::Quaternion");
 tolua_usertype(tolua_S,"AgentAnimStateMachine");
 tolua_usertype(tolua_S,"BlockObject");
 tolua_usertype(tolua_S,"Ogre::SceneNode");
 tolua_usertype(tolua_S,"SandboxMgr");
 tolua_usertype(tolua_S,"SoldierObject");
 tolua_usertype(tolua_S,"std::vector<AgentObject*>");
 tolua_usertype(tolua_S,"DebugDrawer");
 tolua_usertype(tolua_S,"Ogre::ColourValue");
 tolua_usertype(tolua_S,"AgentAnim");
 tolua_usertype(tolua_S,"ObjectManager");
 tolua_usertype(tolua_S,"VehicleObject");
 tolua_usertype(tolua_S,"btVector3");
 tolua_usertype(tolua_S,"Ogre::Vector3");
 tolua_usertype(tolua_S,"UIComponent");
 tolua_usertype(tolua_S,"RenderableObject");
 tolua_usertype(tolua_S,"IPlayerInput");
 tolua_usertype(tolua_S,"Ogre::Node");
 tolua_usertype(tolua_S,"std::vector<BlockObject*>");
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

/* method: setObjId of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_setObjId00
static int tolua_SandboxToLua_BaseObject_setObjId00(lua_State* tolua_S)
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
  unsigned int objId = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setObjId'", NULL);
#endif
  {
   self->setObjId(objId);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setObjId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getObjId of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_getObjId00
static int tolua_SandboxToLua_BaseObject_getObjId00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getObjId'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->getObjId();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getObjId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setObjType of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_setObjType00
static int tolua_SandboxToLua_BaseObject_setObjType00(lua_State* tolua_S)
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
  BaseObject::OBJTYPE objType = ((BaseObject::OBJTYPE) (int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setObjType'", NULL);
#endif
  {
   self->setObjType(objType);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setObjType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getObjType of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_BaseObject_getObjType00
static int tolua_SandboxToLua_BaseObject_getObjType00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getObjType'", NULL);
#endif
  {
   BaseObject::OBJTYPE tolua_ret = (BaseObject::OBJTYPE)  self->getObjType();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getObjType'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addChild of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_addChild00
static int tolua_SandboxToLua_UIComponent_addChild00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"UIComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
  UIComponent* pChild = ((UIComponent*)  tolua_tousertype(tolua_S,2,0));
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

/* method: setFontType of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_setFontType00
static int tolua_SandboxToLua_UIComponent_setFontType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
  UIComponent::FontType fontType = ((UIComponent::FontType) (int)  tolua_tonumber(tolua_S,2,0));
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

/* method: setText of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_setText00
static int tolua_SandboxToLua_UIComponent_setText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setMarkupText of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_setMarkupText00
static int tolua_SandboxToLua_UIComponent_setMarkupText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setPosition of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_setPosition00
static int tolua_SandboxToLua_UIComponent_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector2",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setDimension of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_setDimension00
static int tolua_SandboxToLua_UIComponent_setDimension00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector2",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setTextMargin of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_setTextMargin00
static int tolua_SandboxToLua_UIComponent_setTextMargin00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setBackgroundColor of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_setBackgroundColor00
static int tolua_SandboxToLua_UIComponent_setBackgroundColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setGradientColor of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_setGradientColor00
static int tolua_SandboxToLua_UIComponent_setGradientColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::ColourValue",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setVisible of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_setVisible00
static int tolua_SandboxToLua_UIComponent_setVisible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
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

/* method: isVisible of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_isVisible00
static int tolua_SandboxToLua_UIComponent_isVisible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetDimension of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_UIComponent_GetDimension00
static int tolua_SandboxToLua_UIComponent_GetDimension00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"UIComponent",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  UIComponent* self = (UIComponent*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setPosition of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_setPosition00
static int tolua_SandboxToLua_VehicleObject_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setRotation of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_setRotation00
static int tolua_SandboxToLua_VehicleObject_setRotation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setOrientation of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_setOrientation00
static int tolua_SandboxToLua_VehicleObject_setOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Quaternion",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetPosition of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetPosition00
static int tolua_SandboxToLua_VehicleObject_GetPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetOrientation of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetOrientation00
static int tolua_SandboxToLua_VehicleObject_GetOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetForward of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetForward00
static int tolua_SandboxToLua_VehicleObject_SetForward00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetVelocity of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetVelocity00
static int tolua_SandboxToLua_VehicleObject_SetVelocity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetTarget of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetTarget00
static int tolua_SandboxToLua_VehicleObject_SetTarget00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetTargetRadius of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetTargetRadius00
static int tolua_SandboxToLua_VehicleObject_SetTargetRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetTarget of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetTarget00
static int tolua_SandboxToLua_VehicleObject_GetTarget00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetVelocity of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetVelocity00
static int tolua_SandboxToLua_VehicleObject_GetVelocity00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetUp of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetUp00
static int tolua_SandboxToLua_VehicleObject_GetUp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetLeft of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetLeft00
static int tolua_SandboxToLua_VehicleObject_GetLeft00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetForward of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetForward00
static int tolua_SandboxToLua_VehicleObject_GetForward00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetMass of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetMass00
static int tolua_SandboxToLua_VehicleObject_SetMass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetHeight of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetHeight00
static int tolua_SandboxToLua_VehicleObject_SetHeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetRadius of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetRadius00
static int tolua_SandboxToLua_VehicleObject_SetRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetSpeed of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetSpeed00
static int tolua_SandboxToLua_VehicleObject_SetSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetHealth of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetHealth00
static int tolua_SandboxToLua_VehicleObject_SetHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetMass of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetMass00
static int tolua_SandboxToLua_VehicleObject_GetMass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetSpeed of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetSpeed00
static int tolua_SandboxToLua_VehicleObject_GetSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetHeight of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetHeight00
static int tolua_SandboxToLua_VehicleObject_GetHeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetRadius of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetRadius00
static int tolua_SandboxToLua_VehicleObject_GetRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetHealth of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetHealth00
static int tolua_SandboxToLua_VehicleObject_GetHealth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetMaxForce of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetMaxForce00
static int tolua_SandboxToLua_VehicleObject_SetMaxForce00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetMaxSpeed of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetMaxSpeed00
static int tolua_SandboxToLua_VehicleObject_SetMaxSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetMaxForce of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetMaxForce00
static int tolua_SandboxToLua_VehicleObject_GetMaxForce00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetMaxSpeed of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetMaxSpeed00
static int tolua_SandboxToLua_VehicleObject_GetMaxSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetTargetRadius of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetTargetRadius00
static int tolua_SandboxToLua_VehicleObject_GetTargetRadius00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: PredictFuturePosition of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_PredictFuturePosition00
static int tolua_SandboxToLua_VehicleObject_PredictFuturePosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: ForceToPosition of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_ForceToPosition00
static int tolua_SandboxToLua_VehicleObject_ForceToPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: ForceToFollowPath of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_ForceToFollowPath00
static int tolua_SandboxToLua_VehicleObject_ForceToFollowPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: ForceToStayOnPath of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_ForceToStayOnPath00
static int tolua_SandboxToLua_VehicleObject_ForceToStayOnPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: ForceToWander of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_ForceToWander00
static int tolua_SandboxToLua_VehicleObject_ForceToWander00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: ForceToTargetSpeed of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_ForceToTargetSpeed00
static int tolua_SandboxToLua_VehicleObject_ForceToTargetSpeed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: ForceToAvoidAgents of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_ForceToAvoidAgents00
static int tolua_SandboxToLua_VehicleObject_ForceToAvoidAgents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: ForceToAvoidObjects of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_ForceToAvoidObjects00
static int tolua_SandboxToLua_VehicleObject_ForceToAvoidObjects00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: ForceToCombine of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_ForceToCombine00
static int tolua_SandboxToLua_VehicleObject_ForceToCombine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const std::vector<AgentObject*>",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: ForceToSeparate of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_ForceToSeparate00
static int tolua_SandboxToLua_VehicleObject_ForceToSeparate00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const std::vector<AgentObject*>",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: ApplyForce of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_ApplyForce00
static int tolua_SandboxToLua_VehicleObject_ApplyForce00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: SetPath of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_SetPath00
static int tolua_SandboxToLua_VehicleObject_SetPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const std::vector<Ogre::Vector3>",0,&tolua_err)) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetPath of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetPath00
static int tolua_SandboxToLua_VehicleObject_GetPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  VehicleObject* self = (VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: HasPath of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_HasPath00
static int tolua_SandboxToLua_VehicleObject_HasPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetDistanceAlongPath of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetDistanceAlongPath00
static int tolua_SandboxToLua_VehicleObject_GetDistanceAlongPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetNearestPointOnPath of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetNearestPointOnPath00
static int tolua_SandboxToLua_VehicleObject_GetNearestPointOnPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: GetPointOnPath of class  VehicleObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_VehicleObject_GetPointOnPath00
static int tolua_SandboxToLua_VehicleObject_GetPointOnPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const VehicleObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const VehicleObject* self = (const VehicleObject*)  tolua_tousertype(tolua_S,1,0);
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
   RenderableObject* tolua_ret = (RenderableObject*)  self->getBody();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"RenderableObject");
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

/* method: IsHasNextAnim of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_AgentObject_IsHasNextAnim00
static int tolua_SandboxToLua_AgentObject_IsHasNextAnim00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsHasNextAnim'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsHasNextAnim();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsHasNextAnim'.",&tolua_err);
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
   RenderableObject* tolua_ret = (RenderableObject*)  self->getWeapon();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"RenderableObject");
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
     !tolua_isusertype(tolua_S,1,"SoldierObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SoldierObject* self = (SoldierObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: IsHasNextAnim of class  SoldierObject */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SoldierObject_IsHasNextAnim00
static int tolua_SandboxToLua_SoldierObject_IsHasNextAnim00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsHasNextAnim'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsHasNextAnim();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsHasNextAnim'.",&tolua_err);
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

/* method: GetCamera of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_GetCamera00
static int tolua_SandboxToLua_SandboxMgr_GetCamera00(lua_State* tolua_S)
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

/* method: GetCameraUp of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_GetCameraUp00
static int tolua_SandboxToLua_SandboxMgr_GetCameraUp00(lua_State* tolua_S)
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

/* method: GetCameraLeft of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_GetCameraLeft00
static int tolua_SandboxToLua_SandboxMgr_GetCameraLeft00(lua_State* tolua_S)
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

/* method: GetCameraForward of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_GetCameraForward00
static int tolua_SandboxToLua_SandboxMgr_GetCameraForward00(lua_State* tolua_S)
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

/* method: GetCameraPosition of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_GetCameraPosition00
static int tolua_SandboxToLua_SandboxMgr_GetCameraPosition00(lua_State* tolua_S)
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

/* method: GetCameraRotation of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_GetCameraRotation00
static int tolua_SandboxToLua_SandboxMgr_GetCameraRotation00(lua_State* tolua_S)
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

/* method: GetCameraOrientation of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_GetCameraOrientation00
static int tolua_SandboxToLua_SandboxMgr_GetCameraOrientation00(lua_State* tolua_S)
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

/* method: GetRenderTime of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_GetRenderTime00
static int tolua_SandboxToLua_SandboxMgr_GetRenderTime00(lua_State* tolua_S)
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

/* method: GetSimulateTime of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_GetSimulateTime00
static int tolua_SandboxToLua_SandboxMgr_GetSimulateTime00(lua_State* tolua_S)
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

/* method: GetTotalSimulateTime of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_GetTotalSimulateTime00
static int tolua_SandboxToLua_SandboxMgr_GetTotalSimulateTime00(lua_State* tolua_S)
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

/* method: CreateUIComponent of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_CreateUIComponent00
static int tolua_SandboxToLua_SandboxMgr_CreateUIComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,1));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateUIComponent'", NULL);
#endif
  {
   UIComponent* tolua_ret = (UIComponent*)  self->CreateUIComponent(index);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"UIComponent");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateUIComponent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMarkupColor of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_SetMarkupColor00
static int tolua_SandboxToLua_SandboxMgr_SetMarkupColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
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

/* method: CreatePlane of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_CreatePlane00
static int tolua_SandboxToLua_SandboxMgr_CreatePlane00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
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

/* method: CreateBlockObject of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_CreateBlockObject00
static int tolua_SandboxToLua_SandboxMgr_CreateBlockObject00(lua_State* tolua_S)
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

/* method: CreateBlockBox of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_CreateBlockBox00
static int tolua_SandboxToLua_SandboxMgr_CreateBlockBox00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
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
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
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

/* method: CreateBullet of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_CreateBullet00
static int tolua_SandboxToLua_SandboxMgr_CreateBullet00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
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

/* method: CreateAgent of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_CreateAgent00
static int tolua_SandboxToLua_SandboxMgr_CreateAgent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
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

/* method: CreateSoldier of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_SandboxMgr_CreateSoldier00
static int tolua_SandboxToLua_SandboxMgr_CreateSoldier00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
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

/* method: getIsEnable of class  DebugDrawer */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DebugDrawer_getIsEnable00
static int tolua_SandboxToLua_DebugDrawer_getIsEnable00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DebugDrawer",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DebugDrawer* self = (DebugDrawer*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getIsEnable'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->getIsEnable();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getIsEnable'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setIsEnable of class  DebugDrawer */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DebugDrawer_setIsEnable00
static int tolua_SandboxToLua_DebugDrawer_setIsEnable00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DebugDrawer",0,&tolua_err) ||
     !tolua_isboolean(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DebugDrawer* self = (DebugDrawer*)  tolua_tousertype(tolua_S,1,0);
  bool isEnable = ((bool)  tolua_toboolean(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setIsEnable'", NULL);
#endif
  {
   self->setIsEnable(isEnable);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setIsEnable'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: drawLine of class  DebugDrawer */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DebugDrawer_drawLine00
static int tolua_SandboxToLua_DebugDrawer_drawLine00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DebugDrawer",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::Vector3",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DebugDrawer* self = (DebugDrawer*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* start = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  const Ogre::Vector3* end = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
  const Ogre::ColourValue* color = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'drawLine'", NULL);
#endif
  {
   self->drawLine(*start,*end,*color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'drawLine'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: drawCircle of class  DebugDrawer */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DebugDrawer_drawCircle00
static int tolua_SandboxToLua_DebugDrawer_drawCircle00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DebugDrawer",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,5,&tolua_err) || !tolua_isusertype(tolua_S,5,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isboolean(tolua_S,6,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DebugDrawer* self = (DebugDrawer*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* centre = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  float radius = ((float)  tolua_tonumber(tolua_S,3,0));
  int segmentsCount = ((int)  tolua_tonumber(tolua_S,4,0));
  const Ogre::ColourValue* color = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,5,0));
  bool isFilled = ((bool)  tolua_toboolean(tolua_S,6,false));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'drawCircle'", NULL);
#endif
  {
   self->drawCircle(*centre,radius,segmentsCount,*color,isFilled);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'drawCircle'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: drawSquare of class  DebugDrawer */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DebugDrawer_drawSquare00
static int tolua_SandboxToLua_DebugDrawer_drawSquare00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DebugDrawer",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,4,&tolua_err) || !tolua_isusertype(tolua_S,4,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isboolean(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DebugDrawer* self = (DebugDrawer*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* position = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
   float length = ((  float)  tolua_tonumber(tolua_S,3,0));
  const Ogre::ColourValue* color = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,4,0));
  bool isFilled = ((bool)  tolua_toboolean(tolua_S,5,false));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'drawSquare'", NULL);
#endif
  {
   self->drawSquare(*position,length,*color,isFilled);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'drawSquare'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: drawPath of class  DebugDrawer */
#ifndef TOLUA_DISABLE_tolua_SandboxToLua_DebugDrawer_drawPath00
static int tolua_SandboxToLua_DebugDrawer_drawPath00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"DebugDrawer",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const std::vector<Ogre::Vector3>",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isboolean(tolua_S,4,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,5,&tolua_err) || !tolua_isusertype(tolua_S,5,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  DebugDrawer* self = (DebugDrawer*)  tolua_tousertype(tolua_S,1,0);
  const std::vector<Ogre::Vector3> points = *((const std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,2,0));
  const Ogre::ColourValue* color = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,3,0));
  bool cyclic = ((bool)  tolua_toboolean(tolua_S,4,0));
  const Ogre::Vector3* offset = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'drawPath'", NULL);
#endif
  {
   self->drawPath(points,*color,cyclic,*offset);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'drawPath'.",&tolua_err);
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
  tolua_cclass(tolua_S,"BaseObject","BaseObject","SandboxObject",NULL);
  tolua_beginmodule(tolua_S,"BaseObject");
   tolua_constant(tolua_S,"OBJ_TYPE_NONE",BaseObject::OBJ_TYPE_NONE);
   tolua_constant(tolua_S,"OBJ_TYPE_UIOBJ",BaseObject::OBJ_TYPE_UIOBJ);
   tolua_constant(tolua_S,"OBJ_TYPE_ENTITY",BaseObject::OBJ_TYPE_ENTITY);
   tolua_constant(tolua_S,"OBJ_TYPE_BLOCK",BaseObject::OBJ_TYPE_BLOCK);
   tolua_constant(tolua_S,"OBJ_TYPE_PLANE",BaseObject::OBJ_TYPE_PLANE);
   tolua_constant(tolua_S,"OBJ_TYPE_BULLET",BaseObject::OBJ_TYPE_BULLET);
   tolua_constant(tolua_S,"OBJ_TYPE_AGENT",BaseObject::OBJ_TYPE_AGENT);
   tolua_constant(tolua_S,"OBJ_TYPE_SOLDIER",BaseObject::OBJ_TYPE_SOLDIER);
   tolua_function(tolua_S,"setObjId",tolua_SandboxToLua_BaseObject_setObjId00);
   tolua_function(tolua_S,"getObjId",tolua_SandboxToLua_BaseObject_getObjId00);
   tolua_function(tolua_S,"setObjType",tolua_SandboxToLua_BaseObject_setObjType00);
   tolua_function(tolua_S,"getObjType",tolua_SandboxToLua_BaseObject_getObjType00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"UIComponent","UIComponent","BaseObject",NULL);
  tolua_beginmodule(tolua_S,"UIComponent");
   tolua_constant(tolua_S,"FONT_SMALL",UIComponent::FONT_SMALL);
   tolua_constant(tolua_S,"FONT_SMALL_MONO",UIComponent::FONT_SMALL_MONO);
   tolua_constant(tolua_S,"FONT_MEDIUM",UIComponent::FONT_MEDIUM);
   tolua_constant(tolua_S,"FONT_MEDIUM_MONO",UIComponent::FONT_MEDIUM_MONO);
   tolua_constant(tolua_S,"FONT_LARGE",UIComponent::FONT_LARGE);
   tolua_constant(tolua_S,"FONT_LARGE_MONO",UIComponent::FONT_LARGE_MONO);
   tolua_constant(tolua_S,"FONT_UNKNOWN",UIComponent::FONT_UNKNOWN);
   tolua_function(tolua_S,"addChild",tolua_SandboxToLua_UIComponent_addChild00);
   tolua_function(tolua_S,"setFontType",tolua_SandboxToLua_UIComponent_setFontType00);
   tolua_function(tolua_S,"setText",tolua_SandboxToLua_UIComponent_setText00);
   tolua_function(tolua_S,"setMarkupText",tolua_SandboxToLua_UIComponent_setMarkupText00);
   tolua_function(tolua_S,"setPosition",tolua_SandboxToLua_UIComponent_setPosition00);
   tolua_function(tolua_S,"setDimension",tolua_SandboxToLua_UIComponent_setDimension00);
   tolua_function(tolua_S,"setTextMargin",tolua_SandboxToLua_UIComponent_setTextMargin00);
   tolua_function(tolua_S,"setBackgroundColor",tolua_SandboxToLua_UIComponent_setBackgroundColor00);
   tolua_function(tolua_S,"setGradientColor",tolua_SandboxToLua_UIComponent_setGradientColor00);
   tolua_function(tolua_S,"setVisible",tolua_SandboxToLua_UIComponent_setVisible00);
   tolua_function(tolua_S,"isVisible",tolua_SandboxToLua_UIComponent_isVisible00);
   tolua_function(tolua_S,"GetDimension",tolua_SandboxToLua_UIComponent_GetDimension00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"IPlayerInput","IPlayerInput","",NULL);
  tolua_beginmodule(tolua_S,"IPlayerInput");
   tolua_function(tolua_S,"isKeyDown",tolua_SandboxToLua_IPlayerInput_isKeyDown00);
   tolua_function(tolua_S,"isKeyPressed",tolua_SandboxToLua_IPlayerInput_isKeyPressed00);
   tolua_function(tolua_S,"isKeyReleased",tolua_SandboxToLua_IPlayerInput_isKeyReleased00);
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
   tolua_function(tolua_S,"HasNextState",tolua_SandboxToLua_AgentAnimStateMachine_HasNextState00);
   tolua_function(tolua_S,"AddState",tolua_SandboxToLua_AgentAnimStateMachine_AddState00);
   tolua_function(tolua_S,"AddTransition",tolua_SandboxToLua_AgentAnimStateMachine_AddTransition00);
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
   tolua_function(tolua_S,"applyImpulse",tolua_SandboxToLua_BlockObject_applyImpulse00);
   tolua_function(tolua_S,"applyAngularImpulse",tolua_SandboxToLua_BlockObject_applyAngularImpulse00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"VehicleObject","VehicleObject","BaseObject",NULL);
  tolua_beginmodule(tolua_S,"VehicleObject");
   tolua_function(tolua_S,"setPosition",tolua_SandboxToLua_VehicleObject_setPosition00);
   tolua_function(tolua_S,"setRotation",tolua_SandboxToLua_VehicleObject_setRotation00);
   tolua_function(tolua_S,"setOrientation",tolua_SandboxToLua_VehicleObject_setOrientation00);
   tolua_function(tolua_S,"GetPosition",tolua_SandboxToLua_VehicleObject_GetPosition00);
   tolua_function(tolua_S,"GetOrientation",tolua_SandboxToLua_VehicleObject_GetOrientation00);
   tolua_function(tolua_S,"SetForward",tolua_SandboxToLua_VehicleObject_SetForward00);
   tolua_function(tolua_S,"SetVelocity",tolua_SandboxToLua_VehicleObject_SetVelocity00);
   tolua_function(tolua_S,"SetTarget",tolua_SandboxToLua_VehicleObject_SetTarget00);
   tolua_function(tolua_S,"SetTargetRadius",tolua_SandboxToLua_VehicleObject_SetTargetRadius00);
   tolua_function(tolua_S,"GetTarget",tolua_SandboxToLua_VehicleObject_GetTarget00);
   tolua_function(tolua_S,"GetVelocity",tolua_SandboxToLua_VehicleObject_GetVelocity00);
   tolua_function(tolua_S,"GetUp",tolua_SandboxToLua_VehicleObject_GetUp00);
   tolua_function(tolua_S,"GetLeft",tolua_SandboxToLua_VehicleObject_GetLeft00);
   tolua_function(tolua_S,"GetForward",tolua_SandboxToLua_VehicleObject_GetForward00);
   tolua_function(tolua_S,"SetMass",tolua_SandboxToLua_VehicleObject_SetMass00);
   tolua_function(tolua_S,"SetHeight",tolua_SandboxToLua_VehicleObject_SetHeight00);
   tolua_function(tolua_S,"SetRadius",tolua_SandboxToLua_VehicleObject_SetRadius00);
   tolua_function(tolua_S,"SetSpeed",tolua_SandboxToLua_VehicleObject_SetSpeed00);
   tolua_function(tolua_S,"SetHealth",tolua_SandboxToLua_VehicleObject_SetHealth00);
   tolua_function(tolua_S,"GetMass",tolua_SandboxToLua_VehicleObject_GetMass00);
   tolua_function(tolua_S,"GetSpeed",tolua_SandboxToLua_VehicleObject_GetSpeed00);
   tolua_function(tolua_S,"GetHeight",tolua_SandboxToLua_VehicleObject_GetHeight00);
   tolua_function(tolua_S,"GetRadius",tolua_SandboxToLua_VehicleObject_GetRadius00);
   tolua_function(tolua_S,"GetHealth",tolua_SandboxToLua_VehicleObject_GetHealth00);
   tolua_function(tolua_S,"SetMaxForce",tolua_SandboxToLua_VehicleObject_SetMaxForce00);
   tolua_function(tolua_S,"SetMaxSpeed",tolua_SandboxToLua_VehicleObject_SetMaxSpeed00);
   tolua_function(tolua_S,"GetMaxForce",tolua_SandboxToLua_VehicleObject_GetMaxForce00);
   tolua_function(tolua_S,"GetMaxSpeed",tolua_SandboxToLua_VehicleObject_GetMaxSpeed00);
   tolua_function(tolua_S,"GetTargetRadius",tolua_SandboxToLua_VehicleObject_GetTargetRadius00);
   tolua_function(tolua_S,"PredictFuturePosition",tolua_SandboxToLua_VehicleObject_PredictFuturePosition00);
   tolua_function(tolua_S,"ForceToPosition",tolua_SandboxToLua_VehicleObject_ForceToPosition00);
   tolua_function(tolua_S,"ForceToFollowPath",tolua_SandboxToLua_VehicleObject_ForceToFollowPath00);
   tolua_function(tolua_S,"ForceToStayOnPath",tolua_SandboxToLua_VehicleObject_ForceToStayOnPath00);
   tolua_function(tolua_S,"ForceToWander",tolua_SandboxToLua_VehicleObject_ForceToWander00);
   tolua_function(tolua_S,"ForceToTargetSpeed",tolua_SandboxToLua_VehicleObject_ForceToTargetSpeed00);
   tolua_function(tolua_S,"ForceToAvoidAgents",tolua_SandboxToLua_VehicleObject_ForceToAvoidAgents00);
   tolua_function(tolua_S,"ForceToAvoidObjects",tolua_SandboxToLua_VehicleObject_ForceToAvoidObjects00);
   tolua_function(tolua_S,"ForceToCombine",tolua_SandboxToLua_VehicleObject_ForceToCombine00);
   tolua_function(tolua_S,"ForceToSeparate",tolua_SandboxToLua_VehicleObject_ForceToSeparate00);
   tolua_function(tolua_S,"ApplyForce",tolua_SandboxToLua_VehicleObject_ApplyForce00);
   tolua_function(tolua_S,"SetPath",tolua_SandboxToLua_VehicleObject_SetPath00);
   tolua_function(tolua_S,"GetPath",tolua_SandboxToLua_VehicleObject_GetPath00);
   tolua_function(tolua_S,"HasPath",tolua_SandboxToLua_VehicleObject_HasPath00);
   tolua_function(tolua_S,"GetDistanceAlongPath",tolua_SandboxToLua_VehicleObject_GetDistanceAlongPath00);
   tolua_function(tolua_S,"GetNearestPointOnPath",tolua_SandboxToLua_VehicleObject_GetNearestPointOnPath00);
   tolua_function(tolua_S,"GetPointOnPath",tolua_SandboxToLua_VehicleObject_GetPointOnPath00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"AgentObject","AgentObject","VehicleObject",NULL);
  tolua_beginmodule(tolua_S,"AgentObject");
   tolua_function(tolua_S,"initBody",tolua_SandboxToLua_AgentObject_initBody00);
   tolua_function(tolua_S,"getBody",tolua_SandboxToLua_AgentObject_getBody00);
   tolua_function(tolua_S,"getAgentType",tolua_SandboxToLua_AgentObject_getAgentType00);
   tolua_function(tolua_S,"setAgentType",tolua_SandboxToLua_AgentObject_setAgentType00);
   tolua_function(tolua_S,"GetPosition",tolua_SandboxToLua_AgentObject_GetPosition00);
   tolua_function(tolua_S,"GetOrientation",tolua_SandboxToLua_AgentObject_GetOrientation00);
   tolua_function(tolua_S,"GetUp",tolua_SandboxToLua_AgentObject_GetUp00);
   tolua_function(tolua_S,"GetLeft",tolua_SandboxToLua_AgentObject_GetLeft00);
   tolua_function(tolua_S,"GetForward",tolua_SandboxToLua_AgentObject_GetForward00);
   tolua_function(tolua_S,"setPosition",tolua_SandboxToLua_AgentObject_setPosition00);
   tolua_function(tolua_S,"setOrientation",tolua_SandboxToLua_AgentObject_setOrientation00);
   tolua_function(tolua_S,"IsMoving",tolua_SandboxToLua_AgentObject_IsMoving00);
   tolua_function(tolua_S,"IsFalling",tolua_SandboxToLua_AgentObject_IsFalling00);
   tolua_function(tolua_S,"OnGround",tolua_SandboxToLua_AgentObject_OnGround00);
   tolua_function(tolua_S,"GetUseCppFSM",tolua_SandboxToLua_AgentObject_GetUseCppFSM00);
   tolua_function(tolua_S,"GetCurStateId",tolua_SandboxToLua_AgentObject_GetCurStateId00);
   tolua_function(tolua_S,"GetCurStateName",tolua_SandboxToLua_AgentObject_GetCurStateName00);
   tolua_function(tolua_S,"GetInput",tolua_SandboxToLua_AgentObject_GetInput00);
   tolua_function(tolua_S,"RequestState",tolua_SandboxToLua_AgentObject_RequestState00);
   tolua_function(tolua_S,"IsHasNextAnim",tolua_SandboxToLua_AgentObject_IsHasNextAnim00);
   tolua_function(tolua_S,"IsAnimReadyForMove",tolua_SandboxToLua_AgentObject_IsAnimReadyForMove00);
   tolua_function(tolua_S,"IsAnimReadyForShoot",tolua_SandboxToLua_AgentObject_IsAnimReadyForShoot00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"SoldierObject","SoldierObject","AgentObject",NULL);
  tolua_beginmodule(tolua_S,"SoldierObject");
   tolua_function(tolua_S,"initWeapon",tolua_SandboxToLua_SoldierObject_initWeapon00);
   tolua_function(tolua_S,"getWeapon",tolua_SandboxToLua_SoldierObject_getWeapon00);
   tolua_function(tolua_S,"changeStanceType",tolua_SandboxToLua_SoldierObject_changeStanceType00);
   tolua_function(tolua_S,"getStanceType",tolua_SandboxToLua_SoldierObject_getStanceType00);
   tolua_function(tolua_S,"ShootBullet",tolua_SandboxToLua_SoldierObject_ShootBullet00);
   tolua_function(tolua_S,"GetInput",tolua_SandboxToLua_SoldierObject_GetInput00);
   tolua_function(tolua_S,"RequestState",tolua_SandboxToLua_SoldierObject_RequestState00);
   tolua_function(tolua_S,"IsHasNextAnim",tolua_SandboxToLua_SoldierObject_IsHasNextAnim00);
   tolua_function(tolua_S,"IsAnimReadyForMove",tolua_SandboxToLua_SoldierObject_IsAnimReadyForMove00);
   tolua_function(tolua_S,"IsAnimReadyForShoot",tolua_SandboxToLua_SoldierObject_IsAnimReadyForShoot00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"SandboxMgr","SandboxMgr","",NULL);
  tolua_beginmodule(tolua_S,"SandboxMgr");
   tolua_function(tolua_S,"GetCamera",tolua_SandboxToLua_SandboxMgr_GetCamera00);
   tolua_function(tolua_S,"GetCameraUp",tolua_SandboxToLua_SandboxMgr_GetCameraUp00);
   tolua_function(tolua_S,"GetCameraLeft",tolua_SandboxToLua_SandboxMgr_GetCameraLeft00);
   tolua_function(tolua_S,"GetCameraForward",tolua_SandboxToLua_SandboxMgr_GetCameraForward00);
   tolua_function(tolua_S,"GetCameraPosition",tolua_SandboxToLua_SandboxMgr_GetCameraPosition00);
   tolua_function(tolua_S,"GetCameraRotation",tolua_SandboxToLua_SandboxMgr_GetCameraRotation00);
   tolua_function(tolua_S,"GetCameraOrientation",tolua_SandboxToLua_SandboxMgr_GetCameraOrientation00);
   tolua_function(tolua_S,"GetRenderTime",tolua_SandboxToLua_SandboxMgr_GetRenderTime00);
   tolua_function(tolua_S,"GetSimulateTime",tolua_SandboxToLua_SandboxMgr_GetSimulateTime00);
   tolua_function(tolua_S,"GetTotalSimulateTime",tolua_SandboxToLua_SandboxMgr_GetTotalSimulateTime00);
   tolua_function(tolua_S,"CallFile",tolua_SandboxToLua_SandboxMgr_CallFile00);
   tolua_function(tolua_S,"SetSkyBox",tolua_SandboxToLua_SandboxMgr_SetSkyBox00);
   tolua_function(tolua_S,"SetAmbientLight",tolua_SandboxToLua_SandboxMgr_SetAmbientLight00);
   tolua_function(tolua_S,"CreateDirectionalLight",tolua_SandboxToLua_SandboxMgr_CreateDirectionalLight00);
   tolua_function(tolua_S,"setMaterial",tolua_SandboxToLua_SandboxMgr_setMaterial00);
   tolua_function(tolua_S,"setMaterial",tolua_SandboxToLua_SandboxMgr_setMaterial01);
   tolua_function(tolua_S,"CreateUIComponent",tolua_SandboxToLua_SandboxMgr_CreateUIComponent00);
   tolua_function(tolua_S,"SetMarkupColor",tolua_SandboxToLua_SandboxMgr_SetMarkupColor00);
   tolua_function(tolua_S,"CreatePlane",tolua_SandboxToLua_SandboxMgr_CreatePlane00);
   tolua_function(tolua_S,"CreateBlockObject",tolua_SandboxToLua_SandboxMgr_CreateBlockObject00);
   tolua_function(tolua_S,"CreateBlockBox",tolua_SandboxToLua_SandboxMgr_CreateBlockBox00);
   tolua_function(tolua_S,"CreateBullet",tolua_SandboxToLua_SandboxMgr_CreateBullet00);
   tolua_function(tolua_S,"CreateAgent",tolua_SandboxToLua_SandboxMgr_CreateAgent00);
   tolua_function(tolua_S,"CreateSoldier",tolua_SandboxToLua_SandboxMgr_CreateSoldier00);
  tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"MGR_OBJ_NONE",MGR_OBJ_NONE);
  tolua_constant(tolua_S,"MGR_OBJ_UIOBJ",MGR_OBJ_UIOBJ);
  tolua_constant(tolua_S,"MGR_OBJ_ENTITY",MGR_OBJ_ENTITY);
  tolua_constant(tolua_S,"MGR_OBJ_BLOCK",MGR_OBJ_BLOCK);
  tolua_constant(tolua_S,"MGR_OBJ_AGENT",MGR_OBJ_AGENT);
  tolua_constant(tolua_S,"MGR_OBJ_ALLS",MGR_OBJ_ALLS);
  tolua_cclass(tolua_S,"ObjectManager","ObjectManager","",NULL);
  tolua_beginmodule(tolua_S,"ObjectManager");
   tolua_function(tolua_S,"clearAllObjects",tolua_SandboxToLua_ObjectManager_clearAllObjects00);
   tolua_function(tolua_S,"getAllAgents",tolua_SandboxToLua_ObjectManager_getAllAgents00);
   tolua_function(tolua_S,"getAllBlocks",tolua_SandboxToLua_ObjectManager_getAllBlocks00);
   tolua_function(tolua_S,"getSpecifyAgents",tolua_SandboxToLua_ObjectManager_getSpecifyAgents00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DebugDrawer","DebugDrawer","",NULL);
  tolua_beginmodule(tolua_S,"DebugDrawer");
   tolua_function(tolua_S,"getIsEnable",tolua_SandboxToLua_DebugDrawer_getIsEnable00);
   tolua_function(tolua_S,"setIsEnable",tolua_SandboxToLua_DebugDrawer_setIsEnable00);
   tolua_function(tolua_S,"drawLine",tolua_SandboxToLua_DebugDrawer_drawLine00);
   tolua_function(tolua_S,"drawCircle",tolua_SandboxToLua_DebugDrawer_drawCircle00);
   tolua_function(tolua_S,"drawSquare",tolua_SandboxToLua_DebugDrawer_drawSquare00);
   tolua_function(tolua_S,"drawPath",tolua_SandboxToLua_DebugDrawer_drawPath00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_SandboxToLua (lua_State* tolua_S) {
 return tolua_SandboxToLua_open(tolua_S);
};
#endif

