/*
** Lua binding: ClientToLua
** Generated automatically by tolua++-1.0.92 on Sat May 18 22:13:39 2024.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_ClientToLua_open (lua_State* tolua_S);

#include <vector>
#include "ogre3d/include/Ogre.h"
#include "ois/include/OISKeyboard.h"
#include "ogre3d_gorilla/include/Gorilla.h"
#include "base/LuaInterface.h"
#include "base/BaseObject.h"
#include "sandbox/SandboxDef.h"
#include "sandbox/UIComponent.h"
#include "sandbox/SandboxObject.h"
#include "sandbox/AgentObject.h"
#include "sandbox/SandboxMgr.h"
#include "sandbox/DebugDrawer.h"
#include "game/GameManager.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_Ogre__Vector2 (lua_State* tolua_S)
{
 Ogre::Vector2* self = (Ogre::Vector2*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_std__vector_int_ (lua_State* tolua_S)
{
 std::vector<int>* self = (std::vector<int>*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_std__vector_AgentObject__ (lua_State* tolua_S)
{
 std::vector<AgentObject*>* self = (std::vector<AgentObject*>*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_std__vector_Ogre__Vector3_ (lua_State* tolua_S)
{
 std::vector<Ogre::Vector3>* self = (std::vector<Ogre::Vector3>*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_std__vector_Ogre__Vector3___iterator (lua_State* tolua_S)
{
 std::vector<Ogre::Vector3>::iterator* self = (std::vector<Ogre::Vector3>::iterator*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_SandboxObject (lua_State* tolua_S)
{
 SandboxObject* self = (SandboxObject*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_Ogre__Vector3 (lua_State* tolua_S)
{
 Ogre::Vector3* self = (Ogre::Vector3*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_Ogre__ColourValue (lua_State* tolua_S)
{
 Ogre::ColourValue* self = (Ogre::ColourValue*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_BaseObject (lua_State* tolua_S)
{
 BaseObject* self = (BaseObject*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_Ogre__Quaternion (lua_State* tolua_S)
{
 Ogre::Quaternion* self = (Ogre::Quaternion*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_OpenSteer__Vec3 (lua_State* tolua_S)
{
 OpenSteer::Vec3* self = (OpenSteer::Vec3*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_std__vector_AgentObject____iterator (lua_State* tolua_S)
{
 std::vector<AgentObject*>::iterator* self = (std::vector<AgentObject*>::iterator*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_std__vector_int___iterator (lua_State* tolua_S)
{
 std::vector<int>::iterator* self = (std::vector<int>::iterator*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"Ogre::Vector2");
 tolua_usertype(tolua_S,"std::vector<int>");
 tolua_usertype(tolua_S,"AgentObject");
 tolua_usertype(tolua_S,"std::vector<Ogre::Vector3>");
 tolua_usertype(tolua_S,"std::vector<Ogre::Vector3>::iterator");
 tolua_usertype(tolua_S,"btQuaternion");
 tolua_usertype(tolua_S,"PhysicsWorld");
 tolua_usertype(tolua_S,"Ogre::Light");
 tolua_usertype(tolua_S,"BaseObject");
 tolua_usertype(tolua_S,"Ogre::Quaternion");
 tolua_usertype(tolua_S,"OpenSteer::Vec3");
 tolua_usertype(tolua_S,"std::vector<AgentObject*>::iterator");
 tolua_usertype(tolua_S,"Ogre::SceneNode");
 tolua_usertype(tolua_S,"SandboxObject");
 tolua_usertype(tolua_S,"Ogre::SceneManager");
 tolua_usertype(tolua_S,"std::vector<AgentObject*>");
 tolua_usertype(tolua_S,"DebugDrawer");
 tolua_usertype(tolua_S,"Ogre::ColourValue");
 tolua_usertype(tolua_S,"SandboxMgr");
 tolua_usertype(tolua_S,"AgentPath");
 tolua_usertype(tolua_S,"btRigidBody");
 tolua_usertype(tolua_S,"GameManager");
 tolua_usertype(tolua_S,"Ogre::Vector3");
 tolua_usertype(tolua_S,"LuaInterface");
 tolua_usertype(tolua_S,"UIComponent");
 tolua_usertype(tolua_S,"btVector3");
 tolua_usertype(tolua_S,"Ogre::Node");
 tolua_usertype(tolua_S,"Ogre::Camera");
 tolua_usertype(tolua_S,"std::vector<int>::iterator");
}

/* method: operator+ of class  iterator */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__iterator__add00
static int tolua_ClientToLua_std_vector_int__iterator__add00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const std::vector<int>::iterator",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::vector<int>::iterator* self = (const std::vector<int>::iterator*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator+'", NULL);
#endif
  {
   std::vector<int>::iterator tolua_ret = (std::vector<int>::iterator)  self->operator+(index);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<int>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<int>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<int>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<int>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.add'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__new00
static int tolua_ClientToLua_std_vector_int__new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   std::vector<int>* tolua_ret = (std::vector<int>*)  Mtolua_new((std::vector<int>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"std::vector<int>");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__new00_local
static int tolua_ClientToLua_std_vector_int__new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   std::vector<int>* tolua_ret = (std::vector<int>*)  Mtolua_new((std::vector<int>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"std::vector<int>");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__delete00
static int tolua_ClientToLua_std_vector_int__delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<int>* self = (std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clear of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__clear00
static int tolua_ClientToLua_std_vector_int__clear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<int>* self = (std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clear'", NULL);
#endif
  {
   self->clear();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clear'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: size of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__size00
static int tolua_ClientToLua_std_vector_int__size00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const std::vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::vector<int>* self = (const std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'size'", NULL);
#endif
  {
   int tolua_ret = (int)  self->size();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'size'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int___geti00
static int tolua_ClientToLua_std_vector_int___geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const std::vector<int>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::vector<int>* self = (const std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   const int tolua_ret = (const int)  self->operator[](index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int___seti00
static int tolua_ClientToLua_std_vector_int___seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<int>* self = (std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
  int tolua_value = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](index) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int___geti01
static int tolua_ClientToLua_std_vector_int___geti01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  std::vector<int>* self = (std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   int tolua_ret = (int)  self->operator[](index);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_std_vector_int___geti00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: push_back of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__push_back00
static int tolua_ClientToLua_std_vector_int__push_back00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<int>* self = (std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
  int val = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'push_back'", NULL);
#endif
  {
   self->push_back(val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'push_back'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: begin of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__begin00
static int tolua_ClientToLua_std_vector_int__begin00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<int>* self = (std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'begin'", NULL);
#endif
  {
   std::vector<int>::iterator tolua_ret = (std::vector<int>::iterator)  self->begin();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<int>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<int>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<int>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<int>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'begin'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: end of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__end00
static int tolua_ClientToLua_std_vector_int__end00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<int>* self = (std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'end'", NULL);
#endif
  {
   std::vector<int>::iterator tolua_ret = (std::vector<int>::iterator)  self->end();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<int>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<int>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<int>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<int>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'end'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: erase of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__erase00
static int tolua_ClientToLua_std_vector_int__erase00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"std::vector<int>::iterator",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<int>* self = (std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
  std::vector<int>::iterator iter = *((std::vector<int>::iterator*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'erase'", NULL);
#endif
  {
   std::vector<int>::iterator tolua_ret = (std::vector<int>::iterator)  self->erase(iter);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<int>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<int>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<int>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<int>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'erase'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: insert of class  std::vector<int> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_int__insert00
static int tolua_ClientToLua_std_vector_int__insert00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<int>",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"std::vector<int>::iterator",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<int>* self = (std::vector<int>*)  tolua_tousertype(tolua_S,1,0);
  std::vector<int>::iterator iter = *((std::vector<int>::iterator*)  tolua_tousertype(tolua_S,2,0));
  int val = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'insert'", NULL);
#endif
  {
   std::vector<int>::iterator tolua_ret = (std::vector<int>::iterator)  self->insert(iter,val);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<int>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<int>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<int>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<int>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'insert'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator+ of class  iterator */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___iterator__add00
static int tolua_ClientToLua_std_vector_AgentObject___iterator__add00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const std::vector<AgentObject*>::iterator",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::vector<AgentObject*>::iterator* self = (const std::vector<AgentObject*>::iterator*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator+'", NULL);
#endif
  {
   std::vector<AgentObject*>::iterator tolua_ret = (std::vector<AgentObject*>::iterator)  self->operator+(index);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<AgentObject*>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<AgentObject*>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.add'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___new00
static int tolua_ClientToLua_std_vector_AgentObject___new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   std::vector<AgentObject*>* tolua_ret = (std::vector<AgentObject*>*)  Mtolua_new((std::vector<AgentObject*>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"std::vector<AgentObject*>");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___new00_local
static int tolua_ClientToLua_std_vector_AgentObject___new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   std::vector<AgentObject*>* tolua_ret = (std::vector<AgentObject*>*)  Mtolua_new((std::vector<AgentObject*>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"std::vector<AgentObject*>");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___delete00
static int tolua_ClientToLua_std_vector_AgentObject___delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<AgentObject*>* self = (std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clear of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___clear00
static int tolua_ClientToLua_std_vector_AgentObject___clear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<AgentObject*>* self = (std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clear'", NULL);
#endif
  {
   self->clear();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clear'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: size of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___size00
static int tolua_ClientToLua_std_vector_AgentObject___size00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::vector<AgentObject*>* self = (const std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'size'", NULL);
#endif
  {
   int tolua_ret = (int)  self->size();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'size'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject____geti00
static int tolua_ClientToLua_std_vector_AgentObject____geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::vector<AgentObject*>* self = (const std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   const AgentObject* tolua_ret = (const AgentObject*)  self->operator[](index);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"const AgentObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject____seti00
static int tolua_ClientToLua_std_vector_AgentObject____seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<AgentObject*>* self = (std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
  AgentObject* tolua_value = ((AgentObject*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](index) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject____geti01
static int tolua_ClientToLua_std_vector_AgentObject____geti01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  std::vector<AgentObject*>* self = (std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   AgentObject* tolua_ret = (AgentObject*)  self->operator[](index);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentObject");
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_std_vector_AgentObject____geti00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: push_back of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___push_back00
static int tolua_ClientToLua_std_vector_AgentObject___push_back00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<AgentObject*>* self = (std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
  AgentObject* val = ((AgentObject*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'push_back'", NULL);
#endif
  {
   self->push_back(val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'push_back'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: begin of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___begin00
static int tolua_ClientToLua_std_vector_AgentObject___begin00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<AgentObject*>* self = (std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'begin'", NULL);
#endif
  {
   std::vector<AgentObject*>::iterator tolua_ret = (std::vector<AgentObject*>::iterator)  self->begin();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<AgentObject*>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<AgentObject*>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'begin'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: end of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___end00
static int tolua_ClientToLua_std_vector_AgentObject___end00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<AgentObject*>* self = (std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'end'", NULL);
#endif
  {
   std::vector<AgentObject*>::iterator tolua_ret = (std::vector<AgentObject*>::iterator)  self->end();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<AgentObject*>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<AgentObject*>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'end'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: erase of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___erase00
static int tolua_ClientToLua_std_vector_AgentObject___erase00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"std::vector<AgentObject*>::iterator",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<AgentObject*>* self = (std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
  std::vector<AgentObject*>::iterator iter = *((std::vector<AgentObject*>::iterator*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'erase'", NULL);
#endif
  {
   std::vector<AgentObject*>::iterator tolua_ret = (std::vector<AgentObject*>::iterator)  self->erase(iter);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<AgentObject*>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<AgentObject*>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'erase'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: insert of class  std::vector<AgentObject*> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_AgentObject___insert00
static int tolua_ClientToLua_std_vector_AgentObject___insert00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<AgentObject*>",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"std::vector<AgentObject*>::iterator",0,&tolua_err)) ||
     !tolua_isusertype(tolua_S,3,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<AgentObject*>* self = (std::vector<AgentObject*>*)  tolua_tousertype(tolua_S,1,0);
  std::vector<AgentObject*>::iterator iter = *((std::vector<AgentObject*>::iterator*)  tolua_tousertype(tolua_S,2,0));
  AgentObject* val = ((AgentObject*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'insert'", NULL);
#endif
  {
   std::vector<AgentObject*>::iterator tolua_ret = (std::vector<AgentObject*>::iterator)  self->insert(iter,val);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<AgentObject*>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<AgentObject*>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<AgentObject*>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'insert'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator+ of class  iterator */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__iterator__add00
static int tolua_ClientToLua_std_vector_Ogre__Vector3__iterator__add00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const std::vector<Ogre::Vector3>::iterator",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::vector<Ogre::Vector3>::iterator* self = (const std::vector<Ogre::Vector3>::iterator*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator+'", NULL);
#endif
  {
   std::vector<Ogre::Vector3>::iterator tolua_ret = (std::vector<Ogre::Vector3>::iterator)  self->operator+(index);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<Ogre::Vector3>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<Ogre::Vector3>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<Ogre::Vector3>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<Ogre::Vector3>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.add'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__new00
static int tolua_ClientToLua_std_vector_Ogre__Vector3__new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   std::vector<Ogre::Vector3>* tolua_ret = (std::vector<Ogre::Vector3>*)  Mtolua_new((std::vector<Ogre::Vector3>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"std::vector<Ogre::Vector3>");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__new00_local
static int tolua_ClientToLua_std_vector_Ogre__Vector3__new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   std::vector<Ogre::Vector3>* tolua_ret = (std::vector<Ogre::Vector3>*)  Mtolua_new((std::vector<Ogre::Vector3>)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"std::vector<Ogre::Vector3>");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__delete00
static int tolua_ClientToLua_std_vector_Ogre__Vector3__delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<Ogre::Vector3>* self = (std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: clear of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__clear00
static int tolua_ClientToLua_std_vector_Ogre__Vector3__clear00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<Ogre::Vector3>* self = (std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'clear'", NULL);
#endif
  {
   self->clear();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'clear'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: size of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__size00
static int tolua_ClientToLua_std_vector_Ogre__Vector3__size00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const std::vector<Ogre::Vector3>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::vector<Ogre::Vector3>* self = (const std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'size'", NULL);
#endif
  {
   int tolua_ret = (int)  self->size();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'size'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3___geti00
static int tolua_ClientToLua_std_vector_Ogre__Vector3___geti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const std::vector<Ogre::Vector3>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::vector<Ogre::Vector3>* self = (const std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   const Ogre::Vector3 tolua_ret = (const Ogre::Vector3)  self->operator[](index);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((Ogre::Vector3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"const Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(const Ogre::Vector3));
     tolua_pushusertype(tolua_S,tolua_obj,"const Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.geti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator&[] of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3___seti00
static int tolua_ClientToLua_std_vector_Ogre__Vector3___seti00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<Ogre::Vector3>* self = (std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
  Ogre::Vector3 tolua_value = *((Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator&[]'", NULL);
#endif
  self->operator[](index) =  tolua_value;
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function '.seti'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator[] of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3___geti01
static int tolua_ClientToLua_std_vector_Ogre__Vector3___geti01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  std::vector<Ogre::Vector3>* self = (std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
  int index = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator[]'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->operator[](index);
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
tolua_lerror:
 return tolua_ClientToLua_std_vector_Ogre__Vector3___geti00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: push_back of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__push_back00
static int tolua_ClientToLua_std_vector_Ogre__Vector3__push_back00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<Ogre::Vector3>* self = (std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
  Ogre::Vector3 val = *((Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'push_back'", NULL);
#endif
  {
   self->push_back(val);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'push_back'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: begin of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__begin00
static int tolua_ClientToLua_std_vector_Ogre__Vector3__begin00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<Ogre::Vector3>* self = (std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'begin'", NULL);
#endif
  {
   std::vector<Ogre::Vector3>::iterator tolua_ret = (std::vector<Ogre::Vector3>::iterator)  self->begin();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<Ogre::Vector3>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<Ogre::Vector3>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<Ogre::Vector3>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<Ogre::Vector3>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'begin'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: end of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__end00
static int tolua_ClientToLua_std_vector_Ogre__Vector3__end00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<Ogre::Vector3>* self = (std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'end'", NULL);
#endif
  {
   std::vector<Ogre::Vector3>::iterator tolua_ret = (std::vector<Ogre::Vector3>::iterator)  self->end();
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<Ogre::Vector3>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<Ogre::Vector3>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<Ogre::Vector3>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<Ogre::Vector3>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'end'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: erase of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__erase00
static int tolua_ClientToLua_std_vector_Ogre__Vector3__erase00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"std::vector<Ogre::Vector3>::iterator",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<Ogre::Vector3>* self = (std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
  std::vector<Ogre::Vector3>::iterator iter = *((std::vector<Ogre::Vector3>::iterator*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'erase'", NULL);
#endif
  {
   std::vector<Ogre::Vector3>::iterator tolua_ret = (std::vector<Ogre::Vector3>::iterator)  self->erase(iter);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<Ogre::Vector3>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<Ogre::Vector3>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<Ogre::Vector3>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<Ogre::Vector3>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'erase'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: insert of class  std::vector<Ogre::Vector3> */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_std_vector_Ogre__Vector3__insert00
static int tolua_ClientToLua_std_vector_Ogre__Vector3__insert00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"std::vector<Ogre::Vector3>",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"std::vector<Ogre::Vector3>::iterator",0,&tolua_err)) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  std::vector<Ogre::Vector3>* self = (std::vector<Ogre::Vector3>*)  tolua_tousertype(tolua_S,1,0);
  std::vector<Ogre::Vector3>::iterator iter = *((std::vector<Ogre::Vector3>::iterator*)  tolua_tousertype(tolua_S,2,0));
  Ogre::Vector3 val = *((Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'insert'", NULL);
#endif
  {
   std::vector<Ogre::Vector3>::iterator tolua_ret = (std::vector<Ogre::Vector3>::iterator)  self->insert(iter,val);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((std::vector<Ogre::Vector3>::iterator)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<Ogre::Vector3>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(std::vector<Ogre::Vector3>::iterator));
     tolua_pushusertype(tolua_S,tolua_obj,"std::vector<Ogre::Vector3>::iterator");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'insert'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: x of class  Ogre::Vector2 */
#ifndef TOLUA_DISABLE_tolua_get_Ogre__Vector2_x
static int tolua_get_Ogre__Vector2_x(lua_State* tolua_S)
{
  Ogre::Vector2* self = (Ogre::Vector2*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->x);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: x of class  Ogre::Vector2 */
#ifndef TOLUA_DISABLE_tolua_set_Ogre__Vector2_x
static int tolua_set_Ogre__Vector2_x(lua_State* tolua_S)
{
  Ogre::Vector2* self = (Ogre::Vector2*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->x = ((  float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: y of class  Ogre::Vector2 */
#ifndef TOLUA_DISABLE_tolua_get_Ogre__Vector2_y
static int tolua_get_Ogre__Vector2_y(lua_State* tolua_S)
{
  Ogre::Vector2* self = (Ogre::Vector2*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->y);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: y of class  Ogre::Vector2 */
#ifndef TOLUA_DISABLE_tolua_set_Ogre__Vector2_y
static int tolua_set_Ogre__Vector2_y(lua_State* tolua_S)
{
  Ogre::Vector2* self = (Ogre::Vector2*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->y = ((  float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Ogre::Vector2 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector2_new00
static int tolua_ClientToLua_Ogre_Vector2_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector2",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   Ogre::Vector2* tolua_ret = (Ogre::Vector2*)  Mtolua_new((Ogre::Vector2)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector2");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Ogre::Vector2 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector2_new00_local
static int tolua_ClientToLua_Ogre_Vector2_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector2",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   Ogre::Vector2* tolua_ret = (Ogre::Vector2*)  Mtolua_new((Ogre::Vector2)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Ogre::Vector2 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector2_new01
static int tolua_ClientToLua_Ogre_Vector2_new01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector2",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float fx = ((float)  tolua_tonumber(tolua_S,2,0));
  float fy = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   Ogre::Vector2* tolua_ret = (Ogre::Vector2*)  Mtolua_new((Ogre::Vector2)(fx,fy));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector2");
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Vector2_new00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Ogre::Vector2 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector2_new01_local
static int tolua_ClientToLua_Ogre_Vector2_new01_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector2",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float fx = ((float)  tolua_tonumber(tolua_S,2,0));
  float fy = ((float)  tolua_tonumber(tolua_S,3,0));
  {
   Ogre::Vector2* tolua_ret = (Ogre::Vector2*)  Mtolua_new((Ogre::Vector2)(fx,fy));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Vector2_new00_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Ogre::Vector2 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector2_new02
static int tolua_ClientToLua_Ogre_Vector2_new02(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector2",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float scaler = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   Ogre::Vector2* tolua_ret = (Ogre::Vector2*)  Mtolua_new((Ogre::Vector2)(scaler));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector2");
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Vector2_new01(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Ogre::Vector2 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector2_new02_local
static int tolua_ClientToLua_Ogre_Vector2_new02_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector2",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float scaler = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   Ogre::Vector2* tolua_ret = (Ogre::Vector2*)  Mtolua_new((Ogre::Vector2)(scaler));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector2");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Vector2_new01_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* get function: x of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_get_Ogre__Vector3_x
static int tolua_get_Ogre__Vector3_x(lua_State* tolua_S)
{
  Ogre::Vector3* self = (Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->x);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: x of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_set_Ogre__Vector3_x
static int tolua_set_Ogre__Vector3_x(lua_State* tolua_S)
{
  Ogre::Vector3* self = (Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->x = ((  float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: y of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_get_Ogre__Vector3_y
static int tolua_get_Ogre__Vector3_y(lua_State* tolua_S)
{
  Ogre::Vector3* self = (Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->y);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: y of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_set_Ogre__Vector3_y
static int tolua_set_Ogre__Vector3_y(lua_State* tolua_S)
{
  Ogre::Vector3* self = (Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->y = ((  float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: z of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_get_Ogre__Vector3_z
static int tolua_get_Ogre__Vector3_z(lua_State* tolua_S)
{
  Ogre::Vector3* self = (Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'z'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->z);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: z of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_set_Ogre__Vector3_z
static int tolua_set_Ogre__Vector3_z(lua_State* tolua_S)
{
  Ogre::Vector3* self = (Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'z'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->z = ((  float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_new00
static int tolua_ClientToLua_Ogre_Vector3_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   Ogre::Vector3* tolua_ret = (Ogre::Vector3*)  Mtolua_new((Ogre::Vector3)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector3");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_new00_local
static int tolua_ClientToLua_Ogre_Vector3_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   Ogre::Vector3* tolua_ret = (Ogre::Vector3*)  Mtolua_new((Ogre::Vector3)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_new01
static int tolua_ClientToLua_Ogre_Vector3_new01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector3",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const Ogre::Vector3* rhs = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  {
   Ogre::Vector3* tolua_ret = (Ogre::Vector3*)  Mtolua_new((Ogre::Vector3)(*rhs));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector3");
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Vector3_new00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_new01_local
static int tolua_ClientToLua_Ogre_Vector3_new01_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector3",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  const Ogre::Vector3* rhs = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
  {
   Ogre::Vector3* tolua_ret = (Ogre::Vector3*)  Mtolua_new((Ogre::Vector3)(*rhs));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Vector3_new00_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_new02
static int tolua_ClientToLua_Ogre_Vector3_new02(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float x1 = ((float)  tolua_tonumber(tolua_S,2,0));
  float y1 = ((float)  tolua_tonumber(tolua_S,3,0));
  float z1 = ((float)  tolua_tonumber(tolua_S,4,0));
  {
   Ogre::Vector3* tolua_ret = (Ogre::Vector3*)  Mtolua_new((Ogre::Vector3)(x1,y1,z1));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector3");
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Vector3_new01(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_new02_local
static int tolua_ClientToLua_Ogre_Vector3_new02_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float x1 = ((float)  tolua_tonumber(tolua_S,2,0));
  float y1 = ((float)  tolua_tonumber(tolua_S,3,0));
  float z1 = ((float)  tolua_tonumber(tolua_S,4,0));
  {
   Ogre::Vector3* tolua_ret = (Ogre::Vector3*)  Mtolua_new((Ogre::Vector3)(x1,y1,z1));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Vector3_new01_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_new03
static int tolua_ClientToLua_Ogre_Vector3_new03(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float v = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   Ogre::Vector3* tolua_ret = (Ogre::Vector3*)  Mtolua_new((Ogre::Vector3)(v));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector3");
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Vector3_new02(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_new03_local
static int tolua_ClientToLua_Ogre_Vector3_new03_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  float v = ((float)  tolua_tonumber(tolua_S,2,0));
  {
   Ogre::Vector3* tolua_ret = (Ogre::Vector3*)  Mtolua_new((Ogre::Vector3)(v));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Vector3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Vector3_new02_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator+ of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3__add00
static int tolua_ClientToLua_Ogre_Vector3__add00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Ogre::Vector3",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Vector3* self = (const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* rkVector = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator+'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->operator+(*rkVector);
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
 tolua_error(tolua_S,"#ferror in function '.add'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator- of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3__sub00
static int tolua_ClientToLua_Ogre_Vector3__sub00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Ogre::Vector3",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Vector3* self = (const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* rkVector = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator-'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->operator-(*rkVector);
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
 tolua_error(tolua_S,"#ferror in function '.sub'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator* of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3__mul00
static int tolua_ClientToLua_Ogre_Vector3__mul00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Vector3* self = (const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
  const float fScalar = ((const float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator*'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->operator*(fScalar);
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
 tolua_error(tolua_S,"#ferror in function '.mul'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: operator/ of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3__div00
static int tolua_ClientToLua_Ogre_Vector3__div00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Vector3* self = (const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
  const float fScalar = ((const float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'operator/'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->operator/(fScalar);
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
 tolua_error(tolua_S,"#ferror in function '.div'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: length of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_length00
static int tolua_ClientToLua_Ogre_Vector3_length00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Vector3* self = (const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'length'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->length();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'length'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: squaredLength of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_squaredLength00
static int tolua_ClientToLua_Ogre_Vector3_squaredLength00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Vector3* self = (const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'squaredLength'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->squaredLength();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'squaredLength'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: distance of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_distance00
static int tolua_ClientToLua_Ogre_Vector3_distance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Ogre::Vector3",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Vector3* self = (const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* rhs = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'distance'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->distance(*rhs);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'distance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: squaredDistance of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_squaredDistance00
static int tolua_ClientToLua_Ogre_Vector3_squaredDistance00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Ogre::Vector3",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Vector3* self = (const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* rhs = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'squaredDistance'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->squaredDistance(*rhs);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'squaredDistance'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: normalisedCopy of class  Ogre::Vector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Vector3_normalisedCopy00
static int tolua_ClientToLua_Ogre_Vector3_normalisedCopy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const Ogre::Vector3",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const Ogre::Vector3* self = (const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'normalisedCopy'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->normalisedCopy();
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
 tolua_error(tolua_S,"#ferror in function 'normalisedCopy'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* get function: w of class  Ogre::Quaternion */
#ifndef TOLUA_DISABLE_tolua_get_Ogre__Quaternion_w
static int tolua_get_Ogre__Quaternion_w(lua_State* tolua_S)
{
  Ogre::Quaternion* self = (Ogre::Quaternion*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'w'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->w);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: w of class  Ogre::Quaternion */
#ifndef TOLUA_DISABLE_tolua_set_Ogre__Quaternion_w
static int tolua_set_Ogre__Quaternion_w(lua_State* tolua_S)
{
  Ogre::Quaternion* self = (Ogre::Quaternion*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'w'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->w = ((  float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: x of class  Ogre::Quaternion */
#ifndef TOLUA_DISABLE_tolua_get_Ogre__Quaternion_x
static int tolua_get_Ogre__Quaternion_x(lua_State* tolua_S)
{
  Ogre::Quaternion* self = (Ogre::Quaternion*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->x);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: x of class  Ogre::Quaternion */
#ifndef TOLUA_DISABLE_tolua_set_Ogre__Quaternion_x
static int tolua_set_Ogre__Quaternion_x(lua_State* tolua_S)
{
  Ogre::Quaternion* self = (Ogre::Quaternion*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'x'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->x = ((  float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: y of class  Ogre::Quaternion */
#ifndef TOLUA_DISABLE_tolua_get_Ogre__Quaternion_y
static int tolua_get_Ogre__Quaternion_y(lua_State* tolua_S)
{
  Ogre::Quaternion* self = (Ogre::Quaternion*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->y);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: y of class  Ogre::Quaternion */
#ifndef TOLUA_DISABLE_tolua_set_Ogre__Quaternion_y
static int tolua_set_Ogre__Quaternion_y(lua_State* tolua_S)
{
  Ogre::Quaternion* self = (Ogre::Quaternion*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'y'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->y = ((  float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* get function: z of class  Ogre::Quaternion */
#ifndef TOLUA_DISABLE_tolua_get_Ogre__Quaternion_z
static int tolua_get_Ogre__Quaternion_z(lua_State* tolua_S)
{
  Ogre::Quaternion* self = (Ogre::Quaternion*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'z'",NULL);
#endif
  tolua_pushnumber(tolua_S,(lua_Number)self->z);
 return 1;
}
#endif //#ifndef TOLUA_DISABLE

/* set function: z of class  Ogre::Quaternion */
#ifndef TOLUA_DISABLE_tolua_set_Ogre__Quaternion_z
static int tolua_set_Ogre__Quaternion_z(lua_State* tolua_S)
{
  Ogre::Quaternion* self = (Ogre::Quaternion*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  tolua_Error tolua_err;
  if (!self) tolua_error(tolua_S,"invalid 'self' in accessing variable 'z'",NULL);
  if (!tolua_isnumber(tolua_S,2,0,&tolua_err))
   tolua_error(tolua_S,"#vinvalid type in variable assignment.",&tolua_err);
#endif
  self->z = ((  float)  tolua_tonumber(tolua_S,2,0))
;
 return 0;
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  Ogre::Quaternion */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Quaternion_new00
static int tolua_ClientToLua_Ogre_Quaternion_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Quaternion",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   Ogre::Quaternion* tolua_ret = (Ogre::Quaternion*)  Mtolua_new((Ogre::Quaternion)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Quaternion");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Ogre::Quaternion */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Quaternion_new00_local
static int tolua_ClientToLua_Ogre_Quaternion_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::Quaternion",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   Ogre::Quaternion* tolua_ret = (Ogre::Quaternion*)  Mtolua_new((Ogre::Quaternion)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::Quaternion");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setPosition of class  Ogre::Camera */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Camera_setPosition00
static int tolua_ClientToLua_Ogre_Camera_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Ogre::Camera",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Ogre::Camera* self = (Ogre::Camera*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* vec = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setPosition'", NULL);
#endif
  {
   self->setPosition(*vec);
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

/* method: setOrientation of class  Ogre::Camera */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Camera_setOrientation00
static int tolua_ClientToLua_Ogre_Camera_setOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Ogre::Camera",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Quaternion",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Ogre::Camera* self = (Ogre::Camera*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Quaternion* q = ((const Ogre::Quaternion*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setOrientation'", NULL);
#endif
  {
   self->setOrientation(*q);
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

/* method: setPosition of class  Ogre::Node */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Node_setPosition00
static int tolua_ClientToLua_Ogre_Node_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Ogre::Node",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Ogre::Node* self = (Ogre::Node*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Vector3* pos = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setPosition'", NULL);
#endif
  {
   self->setPosition(*pos);
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

/* method: setOrientation of class  Ogre::Node */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Node_setOrientation00
static int tolua_ClientToLua_Ogre_Node_setOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Ogre::Node",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Quaternion",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Ogre::Node* self = (Ogre::Node*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::Quaternion* rotation = ((const Ogre::Quaternion*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setOrientation'", NULL);
#endif
  {
   self->setOrientation(*rotation);
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

/* method: new of class  Ogre::ColourValue */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_ColourValue_new00
static int tolua_ClientToLua_Ogre_ColourValue_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::ColourValue",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float red = ((float)  tolua_tonumber(tolua_S,2,1.0f));
  float green = ((float)  tolua_tonumber(tolua_S,3,1.0f));
  float blue = ((float)  tolua_tonumber(tolua_S,4,1.0f));
  float alpha = ((float)  tolua_tonumber(tolua_S,5,1.0f));
  {
   Ogre::ColourValue* tolua_ret = (Ogre::ColourValue*)  Mtolua_new((Ogre::ColourValue)(red,green,blue,alpha));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::ColourValue");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  Ogre::ColourValue */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_ColourValue_new00_local
static int tolua_ClientToLua_Ogre_ColourValue_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"Ogre::ColourValue",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,1,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,1,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  float red = ((float)  tolua_tonumber(tolua_S,2,1.0f));
  float green = ((float)  tolua_tonumber(tolua_S,3,1.0f));
  float blue = ((float)  tolua_tonumber(tolua_S,4,1.0f));
  float alpha = ((float)  tolua_tonumber(tolua_S,5,1.0f));
  {
   Ogre::ColourValue* tolua_ret = (Ogre::ColourValue*)  Mtolua_new((Ogre::ColourValue)(red,green,blue,alpha));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::ColourValue");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDiffuseColour of class  Ogre::Light */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Light_setDiffuseColour00
static int tolua_ClientToLua_Ogre_Light_setDiffuseColour00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Ogre::Light",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Ogre::Light* self = (Ogre::Light*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::ColourValue* colour = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setDiffuseColour'", NULL);
#endif
  {
   self->setDiffuseColour(*colour);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setDiffuseColour'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSpecularColour of class  Ogre::Light */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Light_setSpecularColour00
static int tolua_ClientToLua_Ogre_Light_setSpecularColour00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Ogre::Light",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  Ogre::Light* self = (Ogre::Light*)  tolua_tousertype(tolua_S,1,0);
  const Ogre::ColourValue* colour = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setSpecularColour'", NULL);
#endif
  {
   self->setSpecularColour(*colour);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setSpecularColour'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setDiffuseColour of class  Ogre::Light */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Light_setDiffuseColour01
static int tolua_ClientToLua_Ogre_Light_setDiffuseColour01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Ogre::Light",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Ogre::Light* self = (Ogre::Light*)  tolua_tousertype(tolua_S,1,0);
   float red = ((  float)  tolua_tonumber(tolua_S,2,0));
   float green = ((  float)  tolua_tonumber(tolua_S,3,0));
   float blue = ((  float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setDiffuseColour'", NULL);
#endif
  {
   self->setDiffuseColour(red,green,blue);
  }
 }
 return 0;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Light_setDiffuseColour00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: setSpecularColour of class  Ogre::Light */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Ogre_Light_setSpecularColour01
static int tolua_ClientToLua_Ogre_Light_setSpecularColour01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"Ogre::Light",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Ogre::Light* self = (Ogre::Light*)  tolua_tousertype(tolua_S,1,0);
   float red = ((  float)  tolua_tonumber(tolua_S,2,0));
   float green = ((  float)  tolua_tonumber(tolua_S,3,0));
   float blue = ((  float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setSpecularColour'", NULL);
#endif
  {
   self->setSpecularColour(red,green,blue);
  }
 }
 return 0;
tolua_lerror:
 return tolua_ClientToLua_Ogre_Light_setSpecularColour00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: log of class  LuaInterface */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_LuaInterface_log00
static int tolua_ClientToLua_LuaInterface_log00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"LuaInterface",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  LuaInterface* self = (LuaInterface*)  tolua_tousertype(tolua_S,1,0);
  const char* msg = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'log'", NULL);
#endif
  {
   self->log(msg);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'log'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_BaseObject_new00
static int tolua_ClientToLua_BaseObject_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"BaseObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int objId = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  BaseObject::OBJTYPE objType = ((BaseObject::OBJTYPE) (int)  tolua_tonumber(tolua_S,3,0));
  {
   BaseObject* tolua_ret = (BaseObject*)  Mtolua_new((BaseObject)(objId,objType));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BaseObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_BaseObject_new00_local
static int tolua_ClientToLua_BaseObject_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"BaseObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  unsigned int objId = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  BaseObject::OBJTYPE objType = ((BaseObject::OBJTYPE) (int)  tolua_tonumber(tolua_S,3,0));
  {
   BaseObject* tolua_ret = (BaseObject*)  Mtolua_new((BaseObject)(objId,objType));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"BaseObject");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_BaseObject_delete00
static int tolua_ClientToLua_BaseObject_delete00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setObjId of class  BaseObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_BaseObject_setObjId00
static int tolua_ClientToLua_BaseObject_setObjId00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_BaseObject_getObjId00
static int tolua_ClientToLua_BaseObject_getObjId00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_BaseObject_setObjType00
static int tolua_ClientToLua_BaseObject_setObjType00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_BaseObject_getObjType00
static int tolua_ClientToLua_BaseObject_getObjType00(lua_State* tolua_S)
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

/* function: QuaternionFromRotationDegrees */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_QuaternionFromRotationDegrees00
static int tolua_ClientToLua_QuaternionFromRotationDegrees00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_QuaternionToRotationDegrees00
static int tolua_ClientToLua_QuaternionToRotationDegrees00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_BtVector3ToVector300
static int tolua_ClientToLua_BtVector3ToVector300(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_BtQuaternionToQuaternion00
static int tolua_ClientToLua_BtQuaternionToQuaternion00(lua_State* tolua_S)
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

/* function: Vector3ToVec3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Vector3ToVec300
static int tolua_ClientToLua_Vector3ToVec300(lua_State* tolua_S)
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
  const Ogre::Vector3* vector = ((const Ogre::Vector3*)  tolua_tousertype(tolua_S,1,0));
  {
   OpenSteer::Vec3 tolua_ret = (OpenSteer::Vec3)  Vector3ToVec3(*vector);
   {
#ifdef __cplusplus
    void* tolua_obj = Mtolua_new((OpenSteer::Vec3)(tolua_ret));
     tolua_pushusertype(tolua_S,tolua_obj,"OpenSteer::Vec3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#else
    void* tolua_obj = tolua_copy(tolua_S,(void*)&tolua_ret,sizeof(OpenSteer::Vec3));
     tolua_pushusertype(tolua_S,tolua_obj,"OpenSteer::Vec3");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
#endif
   }
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Vector3ToVec3'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function: Vec3ToVector3 */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_Vec3ToVector300
static int tolua_ClientToLua_Vec3ToVector300(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     (tolua_isvaluenil(tolua_S,1,&tolua_err) || !tolua_isusertype(tolua_S,1,"const OpenSteer::Vec3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const OpenSteer::Vec3* vector = ((const OpenSteer::Vec3*)  tolua_tousertype(tolua_S,1,0));
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  Vec3ToVector3(*vector);
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
 tolua_error(tolua_S,"#ferror in function 'Vec3ToVector3'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addChild of class  UIComponent */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_addChild00
static int tolua_ClientToLua_UIComponent_addChild00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_setFontType00
static int tolua_ClientToLua_UIComponent_setFontType00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_setText00
static int tolua_ClientToLua_UIComponent_setText00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_setMarkupText00
static int tolua_ClientToLua_UIComponent_setMarkupText00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_setPosition00
static int tolua_ClientToLua_UIComponent_setPosition00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_setDimension00
static int tolua_ClientToLua_UIComponent_setDimension00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_setTextMargin00
static int tolua_ClientToLua_UIComponent_setTextMargin00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_setBackgroundColor00
static int tolua_ClientToLua_UIComponent_setBackgroundColor00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_setGradientColor00
static int tolua_ClientToLua_UIComponent_setGradientColor00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_setVisible00
static int tolua_ClientToLua_UIComponent_setVisible00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_isVisible00
static int tolua_ClientToLua_UIComponent_isVisible00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_UIComponent_GetDimension00
static int tolua_ClientToLua_UIComponent_GetDimension00(lua_State* tolua_S)
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

/* method: new of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_new00
static int tolua_ClientToLua_SandboxObject_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::string meshFile = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   SandboxObject* tolua_ret = (SandboxObject*)  Mtolua_new((SandboxObject)(meshFile));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SandboxObject");
   tolua_pushcppstring(tolua_S,(const char*)meshFile);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_new00_local
static int tolua_ClientToLua_SandboxObject_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const std::string meshFile = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  {
   SandboxObject* tolua_ret = (SandboxObject*)  Mtolua_new((SandboxObject)(meshFile));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SandboxObject");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
   tolua_pushcppstring(tolua_S,(const char*)meshFile);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_new01
static int tolua_ClientToLua_SandboxObject_new01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"Ogre::SceneNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"btRigidBody",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Ogre::SceneNode* pSceneNode = ((Ogre::SceneNode*)  tolua_tousertype(tolua_S,2,0));
  btRigidBody* pRigidBody = ((btRigidBody*)  tolua_tousertype(tolua_S,3,0));
  {
   SandboxObject* tolua_ret = (SandboxObject*)  Mtolua_new((SandboxObject)(pSceneNode,pRigidBody));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SandboxObject");
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_SandboxObject_new00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_new01_local
static int tolua_ClientToLua_SandboxObject_new01_local(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"Ogre::SceneNode",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,3,"btRigidBody",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  Ogre::SceneNode* pSceneNode = ((Ogre::SceneNode*)  tolua_tousertype(tolua_S,2,0));
  btRigidBody* pRigidBody = ((btRigidBody*)  tolua_tousertype(tolua_S,3,0));
  {
   SandboxObject* tolua_ret = (SandboxObject*)  Mtolua_new((SandboxObject)(pSceneNode,pRigidBody));
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SandboxObject");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
tolua_lerror:
 return tolua_ClientToLua_SandboxObject_new00_local(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_delete00
static int tolua_ClientToLua_SandboxObject_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxObject* self = (SandboxObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Initialize of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_Initialize00
static int tolua_ClientToLua_SandboxObject_Initialize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxObject* self = (SandboxObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Initialize'", NULL);
#endif
  {
   self->Initialize();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Initialize'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getRigidBody of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_getRigidBody00
static int tolua_ClientToLua_SandboxObject_getRigidBody00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxObject* self = (SandboxObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getRigidBody'", NULL);
#endif
  {
   btRigidBody* tolua_ret = (btRigidBody*)  self->getRigidBody();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"btRigidBody");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getRigidBody'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getSceneNode of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_getSceneNode00
static int tolua_ClientToLua_SandboxObject_getSceneNode00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxObject* self = (SandboxObject*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getSceneNode'", NULL);
#endif
  {
   Ogre::SceneNode* tolua_ret = (Ogre::SceneNode*)  self->getSceneNode();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::SceneNode");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getSceneNode'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMass of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_setMass00
static int tolua_ClientToLua_SandboxObject_setMass00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxObject* self = (SandboxObject*)  tolua_tousertype(tolua_S,1,0);
  const float mass = ((const float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setMass'", NULL);
#endif
  {
   self->setMass(mass);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setMass'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setPosition of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_setPosition00
static int tolua_ClientToLua_SandboxObject_setPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxObject* self = (SandboxObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setRotation of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_setRotation00
static int tolua_ClientToLua_SandboxObject_setRotation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxObject* self = (SandboxObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: setOrientation of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_setOrientation00
static int tolua_ClientToLua_SandboxObject_setOrientation00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Quaternion",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxObject* self = (SandboxObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: applyImpulse of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_applyImpulse00
static int tolua_ClientToLua_SandboxObject_applyImpulse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxObject* self = (SandboxObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: applyAngularImpulse of class  SandboxObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxObject_applyAngularImpulse00
static int tolua_ClientToLua_SandboxObject_applyAngularImpulse00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"const Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxObject* self = (SandboxObject*)  tolua_tousertype(tolua_S,1,0);
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

/* method: getAgentType of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_getAgentType00
static int tolua_ClientToLua_AgentObject_getAgentType00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_setAgentType00
static int tolua_ClientToLua_AgentObject_setAgentType00(lua_State* tolua_S)
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

/* method: SetPosition of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetPosition00
static int tolua_ClientToLua_AgentObject_SetPosition00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetPosition'", NULL);
#endif
  {
   self->SetPosition(*position);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetRotation of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetRotation00
static int tolua_ClientToLua_AgentObject_SetRotation00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRotation'", NULL);
#endif
  {
   self->SetRotation(*rotation);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetRotation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetOrientation of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetOrientation00
static int tolua_ClientToLua_AgentObject_SetOrientation00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetOrientation'", NULL);
#endif
  {
   self->SetOrientation(*quaternion);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetOrientation'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetForward of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetForward00
static int tolua_ClientToLua_AgentObject_SetForward00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetVelocity00
static int tolua_ClientToLua_AgentObject_SetVelocity00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetTarget00
static int tolua_ClientToLua_AgentObject_SetTarget00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetTargetRadius00
static int tolua_ClientToLua_AgentObject_SetTargetRadius00(lua_State* tolua_S)
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

/* method: GetUp of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetUp00
static int tolua_ClientToLua_AgentObject_GetUp00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetLeft00
static int tolua_ClientToLua_AgentObject_GetLeft00(lua_State* tolua_S)
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

/* method: GetTarget of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetTarget00
static int tolua_ClientToLua_AgentObject_GetTarget00(lua_State* tolua_S)
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

/* method: GetForward of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetForward00
static int tolua_ClientToLua_AgentObject_GetForward00(lua_State* tolua_S)
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

/* method: GetVelocity of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetVelocity00
static int tolua_ClientToLua_AgentObject_GetVelocity00(lua_State* tolua_S)
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

/* method: GetPosition of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetPosition00
static int tolua_ClientToLua_AgentObject_GetPosition00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetOrientation00
static int tolua_ClientToLua_AgentObject_GetOrientation00(lua_State* tolua_S)
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

/* method: SetMass of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetMass00
static int tolua_ClientToLua_AgentObject_SetMass00(lua_State* tolua_S)
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
   float objMass = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMass'", NULL);
#endif
  {
   self->SetMass(objMass);
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetHeight00
static int tolua_ClientToLua_AgentObject_SetHeight00(lua_State* tolua_S)
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
   float objMass = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetHeight'", NULL);
#endif
  {
   self->SetHeight(objMass);
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetRadius00
static int tolua_ClientToLua_AgentObject_SetRadius00(lua_State* tolua_S)
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
   float objMass = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetRadius'", NULL);
#endif
  {
   self->SetRadius(objMass);
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetSpeed00
static int tolua_ClientToLua_AgentObject_SetSpeed00(lua_State* tolua_S)
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
   float objMass = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetSpeed'", NULL);
#endif
  {
   self->SetSpeed(objMass);
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetHealth00
static int tolua_ClientToLua_AgentObject_SetHealth00(lua_State* tolua_S)
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
   float objMass = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetHealth'", NULL);
#endif
  {
   self->SetHealth(objMass);
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

/* method: SetMaxForce of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetMaxForce00
static int tolua_ClientToLua_AgentObject_SetMaxForce00(lua_State* tolua_S)
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
   float objMass = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMaxForce'", NULL);
#endif
  {
   self->SetMaxForce(objMass);
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetMaxSpeed00
static int tolua_ClientToLua_AgentObject_SetMaxSpeed00(lua_State* tolua_S)
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
   float objMass = ((  float)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetMaxSpeed'", NULL);
#endif
  {
   self->SetMaxSpeed(objMass);
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

/* method: GetMass of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetMass00
static int tolua_ClientToLua_AgentObject_GetMass00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetSpeed00
static int tolua_ClientToLua_AgentObject_GetSpeed00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetHeight00
static int tolua_ClientToLua_AgentObject_GetHeight00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetRadius00
static int tolua_ClientToLua_AgentObject_GetRadius00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetHealth00
static int tolua_ClientToLua_AgentObject_GetHealth00(lua_State* tolua_S)
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

/* method: GetMaxForce of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetMaxForce00
static int tolua_ClientToLua_AgentObject_GetMaxForce00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetMaxSpeed00
static int tolua_ClientToLua_AgentObject_GetMaxSpeed00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_GetTargetRadius00
static int tolua_ClientToLua_AgentObject_GetTargetRadius00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_PredictFuturePosition00
static int tolua_ClientToLua_AgentObject_PredictFuturePosition00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_ForceToPosition00
static int tolua_ClientToLua_AgentObject_ForceToPosition00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_ForceToFollowPath00
static int tolua_ClientToLua_AgentObject_ForceToFollowPath00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_ForceToStayOnPath00
static int tolua_ClientToLua_AgentObject_ForceToStayOnPath00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_ForceToWander00
static int tolua_ClientToLua_AgentObject_ForceToWander00(lua_State* tolua_S)
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

/* method: ForceToTarget of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_ForceToTarget00
static int tolua_ClientToLua_AgentObject_ForceToTarget00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToTarget'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToTarget(targetSpeed);
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
 tolua_error(tolua_S,"#ferror in function 'ForceToTarget'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToStayOnPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_ForceToStayOnPath01
static int tolua_ClientToLua_AgentObject_ForceToStayOnPath01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"AgentPath",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  AgentPath* path = ((AgentPath*)  tolua_tousertype(tolua_S,2,0));
   float predictionTime = ((  float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToStayOnPath'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToStayOnPath(*path,predictionTime);
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
tolua_lerror:
 return tolua_ClientToLua_AgentObject_ForceToStayOnPath00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: ForceToFollowPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_ForceToFollowPath01
static int tolua_ClientToLua_AgentObject_ForceToFollowPath01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,2,&tolua_err) || !tolua_isusertype(tolua_S,2,"AgentPath",0,&tolua_err)) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  AgentPath* path = ((AgentPath*)  tolua_tousertype(tolua_S,2,0));
   float predictionTime = ((  float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ForceToFollowPath'", NULL);
#endif
  {
   Ogre::Vector3 tolua_ret = (Ogre::Vector3)  self->ForceToFollowPath(*path,predictionTime);
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
tolua_lerror:
 return tolua_ClientToLua_AgentObject_ForceToFollowPath00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: ApplyForce of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_ApplyForce00
static int tolua_ClientToLua_AgentObject_ApplyForce00(lua_State* tolua_S)
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

/* method: HasPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_HasPath00
static int tolua_ClientToLua_AgentObject_HasPath00(lua_State* tolua_S)
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

/* method: SetPath of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_SetPath00
static int tolua_ClientToLua_AgentObject_SetPath00(lua_State* tolua_S)
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

/* method: GetCamera of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetCamera00
static int tolua_ClientToLua_SandboxMgr_GetCamera00(lua_State* tolua_S)
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

/* method: GetSceneManager of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetSceneManager00
static int tolua_ClientToLua_SandboxMgr_GetSceneManager00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSceneManager'", NULL);
#endif
  {
   Ogre::SceneManager* tolua_ret = (Ogre::SceneManager*)  self->GetSceneManager();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::SceneManager");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSceneManager'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetScreenWidth of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetScreenWidth00
static int tolua_ClientToLua_SandboxMgr_GetScreenWidth00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetScreenWidth'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetScreenWidth();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetScreenWidth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetScreenHeight of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetScreenHeight00
static int tolua_ClientToLua_SandboxMgr_GetScreenHeight00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetScreenHeight'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->GetScreenHeight();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetScreenHeight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCameraUp of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetCameraUp00
static int tolua_ClientToLua_SandboxMgr_GetCameraUp00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetCameraLeft00
static int tolua_ClientToLua_SandboxMgr_GetCameraLeft00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetCameraForward00
static int tolua_ClientToLua_SandboxMgr_GetCameraForward00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetCameraPosition00
static int tolua_ClientToLua_SandboxMgr_GetCameraPosition00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetCameraRotation00
static int tolua_ClientToLua_SandboxMgr_GetCameraRotation00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetCameraOrientation00
static int tolua_ClientToLua_SandboxMgr_GetCameraOrientation00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetRenderTime00
static int tolua_ClientToLua_SandboxMgr_GetRenderTime00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetSimulateTime00
static int tolua_ClientToLua_SandboxMgr_GetSimulateTime00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetTotalSimulateTime00
static int tolua_ClientToLua_SandboxMgr_GetTotalSimulateTime00(lua_State* tolua_S)
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

/* method: GetObjectCount of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetObjectCount00
static int tolua_ClientToLua_SandboxMgr_GetObjectCount00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectCount'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->GetObjectCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetSeekingAgent of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_GetSeekingAgent00
static int tolua_ClientToLua_SandboxMgr_GetSeekingAgent00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetSeekingAgent'", NULL);
#endif
  {
   AgentObject* tolua_ret = (AgentObject*)  self->GetSeekingAgent();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"AgentObject");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetSeekingAgent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CallFile of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_CallFile00
static int tolua_ClientToLua_SandboxMgr_CallFile00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_SetSkyBox00
static int tolua_ClientToLua_SandboxMgr_SetSkyBox00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_SetAmbientLight00
static int tolua_ClientToLua_SandboxMgr_SetAmbientLight00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_CreateDirectionalLight00
static int tolua_ClientToLua_SandboxMgr_CreateDirectionalLight00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_setMaterial00
static int tolua_ClientToLua_SandboxMgr_setMaterial00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_setMaterial01
static int tolua_ClientToLua_SandboxMgr_setMaterial01(lua_State* tolua_S)
{
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"SandboxObject",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  SandboxObject* pObject = ((SandboxObject*)  tolua_tousertype(tolua_S,2,0));
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
 return tolua_ClientToLua_SandboxMgr_setMaterial00(tolua_S);
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetMarkupColor of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_SetMarkupColor00
static int tolua_ClientToLua_SandboxMgr_SetMarkupColor00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_CreatePlane00
static int tolua_ClientToLua_SandboxMgr_CreatePlane00(lua_State* tolua_S)
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
   SandboxObject* tolua_ret = (SandboxObject*)  self->CreatePlane(length,width);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SandboxObject");
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

/* method: CreateSandboxObject of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_CreateSandboxObject00
static int tolua_ClientToLua_SandboxMgr_CreateSandboxObject00(lua_State* tolua_S)
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
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateSandboxObject'", NULL);
#endif
  {
   SandboxObject* tolua_ret = (SandboxObject*)  self->CreateSandboxObject(meshfilePath);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"SandboxObject");
   tolua_pushcppstring(tolua_S,(const char*)meshfilePath);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateSandboxObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateUIComponent of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_CreateUIComponent00
static int tolua_ClientToLua_SandboxMgr_CreateUIComponent00(lua_State* tolua_S)
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

/* method: CreateAgent of class  SandboxMgr */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_SandboxMgr_CreateAgent00
static int tolua_ClientToLua_SandboxMgr_CreateAgent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"SandboxMgr",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  SandboxMgr* self = (SandboxMgr*)  tolua_tousertype(tolua_S,1,0);
  AGENT_OBJ_TYPE agentType = ((AGENT_OBJ_TYPE) (int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateAgent'", NULL);
#endif
  {
   AgentObject* tolua_ret = (AgentObject*)  self->CreateAgent(agentType);
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

/* method: getIsEnable of class  DebugDrawer */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_DebugDrawer_getIsEnable00
static int tolua_ClientToLua_DebugDrawer_getIsEnable00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_DebugDrawer_setIsEnable00
static int tolua_ClientToLua_DebugDrawer_setIsEnable00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_DebugDrawer_drawLine00
static int tolua_ClientToLua_DebugDrawer_drawLine00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_DebugDrawer_drawCircle00
static int tolua_ClientToLua_DebugDrawer_drawCircle00(lua_State* tolua_S)
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
#ifndef TOLUA_DISABLE_tolua_ClientToLua_DebugDrawer_drawSquare00
static int tolua_ClientToLua_DebugDrawer_drawSquare00(lua_State* tolua_S)
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

/* method: getScreenWidth of class  GameManager */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_GameManager_getScreenWidth00
static int tolua_ClientToLua_GameManager_getScreenWidth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"GameManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  GameManager* self = (GameManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getScreenWidth'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->getScreenWidth();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getScreenWidth'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getScreenHeight of class  GameManager */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_GameManager_getScreenHeight00
static int tolua_ClientToLua_GameManager_getScreenHeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"GameManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  GameManager* self = (GameManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getScreenHeight'", NULL);
#endif
  {
    float tolua_ret = (  float)  self->getScreenHeight();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getScreenHeight'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getObjectCount of class  GameManager */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_GameManager_getObjectCount00
static int tolua_ClientToLua_GameManager_getObjectCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"GameManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  GameManager* self = (GameManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getObjectCount'", NULL);
#endif
  {
   unsigned int tolua_ret = (unsigned int)  self->getObjectCount();
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

/* method: getPhysicsWorld of class  GameManager */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_GameManager_getPhysicsWorld00
static int tolua_ClientToLua_GameManager_getPhysicsWorld00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"GameManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  GameManager* self = (GameManager*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getPhysicsWorld'", NULL);
#endif
  {
   PhysicsWorld* tolua_ret = (PhysicsWorld*)  self->getPhysicsWorld();
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"PhysicsWorld");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getPhysicsWorld'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: createUIComponent of class  GameManager */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_GameManager_createUIComponent00
static int tolua_ClientToLua_GameManager_createUIComponent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"GameManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  GameManager* self = (GameManager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createUIComponent'", NULL);
#endif
  {
   UIComponent* tolua_ret = (UIComponent*)  self->createUIComponent(index);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"UIComponent");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createUIComponent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addSandboxObject of class  GameManager */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_GameManager_addSandboxObject00
static int tolua_ClientToLua_GameManager_addSandboxObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"GameManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"SandboxObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  GameManager* self = (GameManager*)  tolua_tousertype(tolua_S,1,0);
  SandboxObject* pSandboxObject = ((SandboxObject*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'addSandboxObject'", NULL);
#endif
  {
   self->addSandboxObject(pSandboxObject);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addSandboxObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: addAgentObject of class  GameManager */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_GameManager_addAgentObject00
static int tolua_ClientToLua_GameManager_addAgentObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"GameManager",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"AgentObject",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  GameManager* self = (GameManager*)  tolua_tousertype(tolua_S,1,0);
  AgentObject* pAgentObject = ((AgentObject*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'addAgentObject'", NULL);
#endif
  {
   self->addAgentObject(pAgentObject);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'addAgentObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: setMarkupColor of class  GameManager */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_GameManager_setMarkupColor00
static int tolua_ClientToLua_GameManager_setMarkupColor00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"GameManager",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"const Ogre::ColourValue",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  GameManager* self = (GameManager*)  tolua_tousertype(tolua_S,1,0);
  unsigned int index = ((unsigned int)  tolua_tonumber(tolua_S,2,0));
  const Ogre::ColourValue* color = ((const Ogre::ColourValue*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setMarkupColor'", NULL);
#endif
  {
   self->setMarkupColor(index,*color);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setMarkupColor'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: getAllAgents of class  GameManager */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_GameManager_getAllAgents00
static int tolua_ClientToLua_GameManager_getAllAgents00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"GameManager",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  GameManager* self = (GameManager*)  tolua_tousertype(tolua_S,1,0);
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

/* Open function */
TOLUA_API int tolua_ClientToLua_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"std",0);
  tolua_beginmodule(tolua_S,"std");
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"vector_int_","std::vector<int>","",tolua_collect_std__vector_int_);
   #else
   tolua_cclass(tolua_S,"vector_int_","std::vector<int>","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"vector_int_");
    #ifdef __cplusplus
    tolua_cclass(tolua_S,"iterator","std::vector<int>::iterator","",tolua_collect_std__vector_int___iterator);
    #else
    tolua_cclass(tolua_S,"iterator","std::vector<int>::iterator","",NULL);
    #endif
    tolua_beginmodule(tolua_S,"iterator");
     tolua_function(tolua_S,".add",tolua_ClientToLua_std_vector_int__iterator__add00);
    tolua_endmodule(tolua_S);
    tolua_function(tolua_S,"new",tolua_ClientToLua_std_vector_int__new00);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_std_vector_int__new00_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_std_vector_int__new00_local);
    tolua_function(tolua_S,"delete",tolua_ClientToLua_std_vector_int__delete00);
    tolua_function(tolua_S,"clear",tolua_ClientToLua_std_vector_int__clear00);
    tolua_function(tolua_S,"size",tolua_ClientToLua_std_vector_int__size00);
    tolua_function(tolua_S,".geti",tolua_ClientToLua_std_vector_int___geti00);
    tolua_function(tolua_S,".seti",tolua_ClientToLua_std_vector_int___seti00);
    tolua_function(tolua_S,".geti",tolua_ClientToLua_std_vector_int___geti01);
    tolua_function(tolua_S,"push_back",tolua_ClientToLua_std_vector_int__push_back00);
    tolua_function(tolua_S,"begin",tolua_ClientToLua_std_vector_int__begin00);
    tolua_function(tolua_S,"end",tolua_ClientToLua_std_vector_int__end00);
    tolua_function(tolua_S,"erase",tolua_ClientToLua_std_vector_int__erase00);
    tolua_function(tolua_S,"insert",tolua_ClientToLua_std_vector_int__insert00);
   tolua_endmodule(tolua_S);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"vector_AgentObject__","std::vector<AgentObject*>","",tolua_collect_std__vector_AgentObject__);
   #else
   tolua_cclass(tolua_S,"vector_AgentObject__","std::vector<AgentObject*>","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"vector_AgentObject__");
    #ifdef __cplusplus
    tolua_cclass(tolua_S,"iterator","std::vector<AgentObject*>::iterator","",tolua_collect_std__vector_AgentObject____iterator);
    #else
    tolua_cclass(tolua_S,"iterator","std::vector<AgentObject*>::iterator","",NULL);
    #endif
    tolua_beginmodule(tolua_S,"iterator");
     tolua_function(tolua_S,".add",tolua_ClientToLua_std_vector_AgentObject___iterator__add00);
    tolua_endmodule(tolua_S);
    tolua_function(tolua_S,"new",tolua_ClientToLua_std_vector_AgentObject___new00);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_std_vector_AgentObject___new00_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_std_vector_AgentObject___new00_local);
    tolua_function(tolua_S,"delete",tolua_ClientToLua_std_vector_AgentObject___delete00);
    tolua_function(tolua_S,"clear",tolua_ClientToLua_std_vector_AgentObject___clear00);
    tolua_function(tolua_S,"size",tolua_ClientToLua_std_vector_AgentObject___size00);
    tolua_function(tolua_S,".geti",tolua_ClientToLua_std_vector_AgentObject____geti00);
    tolua_function(tolua_S,".seti",tolua_ClientToLua_std_vector_AgentObject____seti00);
    tolua_function(tolua_S,".geti",tolua_ClientToLua_std_vector_AgentObject____geti01);
    tolua_function(tolua_S,"push_back",tolua_ClientToLua_std_vector_AgentObject___push_back00);
    tolua_function(tolua_S,"begin",tolua_ClientToLua_std_vector_AgentObject___begin00);
    tolua_function(tolua_S,"end",tolua_ClientToLua_std_vector_AgentObject___end00);
    tolua_function(tolua_S,"erase",tolua_ClientToLua_std_vector_AgentObject___erase00);
    tolua_function(tolua_S,"insert",tolua_ClientToLua_std_vector_AgentObject___insert00);
   tolua_endmodule(tolua_S);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"vector_Ogre__Vector3_","std::vector<Ogre::Vector3>","",tolua_collect_std__vector_Ogre__Vector3_);
   #else
   tolua_cclass(tolua_S,"vector_Ogre__Vector3_","std::vector<Ogre::Vector3>","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"vector_Ogre__Vector3_");
    #ifdef __cplusplus
    tolua_cclass(tolua_S,"iterator","std::vector<Ogre::Vector3>::iterator","",tolua_collect_std__vector_Ogre__Vector3___iterator);
    #else
    tolua_cclass(tolua_S,"iterator","std::vector<Ogre::Vector3>::iterator","",NULL);
    #endif
    tolua_beginmodule(tolua_S,"iterator");
     tolua_function(tolua_S,".add",tolua_ClientToLua_std_vector_Ogre__Vector3__iterator__add00);
    tolua_endmodule(tolua_S);
    tolua_function(tolua_S,"new",tolua_ClientToLua_std_vector_Ogre__Vector3__new00);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_std_vector_Ogre__Vector3__new00_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_std_vector_Ogre__Vector3__new00_local);
    tolua_function(tolua_S,"delete",tolua_ClientToLua_std_vector_Ogre__Vector3__delete00);
    tolua_function(tolua_S,"clear",tolua_ClientToLua_std_vector_Ogre__Vector3__clear00);
    tolua_function(tolua_S,"size",tolua_ClientToLua_std_vector_Ogre__Vector3__size00);
    tolua_function(tolua_S,".geti",tolua_ClientToLua_std_vector_Ogre__Vector3___geti00);
    tolua_function(tolua_S,".seti",tolua_ClientToLua_std_vector_Ogre__Vector3___seti00);
    tolua_function(tolua_S,".geti",tolua_ClientToLua_std_vector_Ogre__Vector3___geti01);
    tolua_function(tolua_S,"push_back",tolua_ClientToLua_std_vector_Ogre__Vector3__push_back00);
    tolua_function(tolua_S,"begin",tolua_ClientToLua_std_vector_Ogre__Vector3__begin00);
    tolua_function(tolua_S,"end",tolua_ClientToLua_std_vector_Ogre__Vector3__end00);
    tolua_function(tolua_S,"erase",tolua_ClientToLua_std_vector_Ogre__Vector3__erase00);
    tolua_function(tolua_S,"insert",tolua_ClientToLua_std_vector_Ogre__Vector3__insert00);
   tolua_endmodule(tolua_S);
  tolua_endmodule(tolua_S);
  tolua_module(tolua_S,"Ogre",0);
  tolua_beginmodule(tolua_S,"Ogre");
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"Vector2","Ogre::Vector2","",tolua_collect_Ogre__Vector2);
   #else
   tolua_cclass(tolua_S,"Vector2","Ogre::Vector2","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"Vector2");
    tolua_variable(tolua_S,"x",tolua_get_Ogre__Vector2_x,tolua_set_Ogre__Vector2_x);
    tolua_variable(tolua_S,"y",tolua_get_Ogre__Vector2_y,tolua_set_Ogre__Vector2_y);
    tolua_function(tolua_S,"new",tolua_ClientToLua_Ogre_Vector2_new00);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_Ogre_Vector2_new00_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_Ogre_Vector2_new00_local);
    tolua_function(tolua_S,"new",tolua_ClientToLua_Ogre_Vector2_new01);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_Ogre_Vector2_new01_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_Ogre_Vector2_new01_local);
    tolua_function(tolua_S,"new",tolua_ClientToLua_Ogre_Vector2_new02);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_Ogre_Vector2_new02_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_Ogre_Vector2_new02_local);
   tolua_endmodule(tolua_S);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"Vector3","Ogre::Vector3","",tolua_collect_Ogre__Vector3);
   #else
   tolua_cclass(tolua_S,"Vector3","Ogre::Vector3","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"Vector3");
    tolua_variable(tolua_S,"x",tolua_get_Ogre__Vector3_x,tolua_set_Ogre__Vector3_x);
    tolua_variable(tolua_S,"y",tolua_get_Ogre__Vector3_y,tolua_set_Ogre__Vector3_y);
    tolua_variable(tolua_S,"z",tolua_get_Ogre__Vector3_z,tolua_set_Ogre__Vector3_z);
    tolua_function(tolua_S,"new",tolua_ClientToLua_Ogre_Vector3_new00);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_Ogre_Vector3_new00_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_Ogre_Vector3_new00_local);
    tolua_function(tolua_S,"new",tolua_ClientToLua_Ogre_Vector3_new01);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_Ogre_Vector3_new01_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_Ogre_Vector3_new01_local);
    tolua_function(tolua_S,"new",tolua_ClientToLua_Ogre_Vector3_new02);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_Ogre_Vector3_new02_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_Ogre_Vector3_new02_local);
    tolua_function(tolua_S,"new",tolua_ClientToLua_Ogre_Vector3_new03);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_Ogre_Vector3_new03_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_Ogre_Vector3_new03_local);
    tolua_function(tolua_S,".add",tolua_ClientToLua_Ogre_Vector3__add00);
    tolua_function(tolua_S,".sub",tolua_ClientToLua_Ogre_Vector3__sub00);
    tolua_function(tolua_S,".mul",tolua_ClientToLua_Ogre_Vector3__mul00);
    tolua_function(tolua_S,".div",tolua_ClientToLua_Ogre_Vector3__div00);
    tolua_function(tolua_S,"length",tolua_ClientToLua_Ogre_Vector3_length00);
    tolua_function(tolua_S,"squaredLength",tolua_ClientToLua_Ogre_Vector3_squaredLength00);
    tolua_function(tolua_S,"distance",tolua_ClientToLua_Ogre_Vector3_distance00);
    tolua_function(tolua_S,"squaredDistance",tolua_ClientToLua_Ogre_Vector3_squaredDistance00);
    tolua_function(tolua_S,"normalisedCopy",tolua_ClientToLua_Ogre_Vector3_normalisedCopy00);
   tolua_endmodule(tolua_S);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"Quaternion","Ogre::Quaternion","",tolua_collect_Ogre__Quaternion);
   #else
   tolua_cclass(tolua_S,"Quaternion","Ogre::Quaternion","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"Quaternion");
    tolua_variable(tolua_S,"w",tolua_get_Ogre__Quaternion_w,tolua_set_Ogre__Quaternion_w);
    tolua_variable(tolua_S,"x",tolua_get_Ogre__Quaternion_x,tolua_set_Ogre__Quaternion_x);
    tolua_variable(tolua_S,"y",tolua_get_Ogre__Quaternion_y,tolua_set_Ogre__Quaternion_y);
    tolua_variable(tolua_S,"z",tolua_get_Ogre__Quaternion_z,tolua_set_Ogre__Quaternion_z);
    tolua_function(tolua_S,"new",tolua_ClientToLua_Ogre_Quaternion_new00);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_Ogre_Quaternion_new00_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_Ogre_Quaternion_new00_local);
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"Camera","Ogre::Camera","",NULL);
   tolua_beginmodule(tolua_S,"Camera");
    tolua_function(tolua_S,"setPosition",tolua_ClientToLua_Ogre_Camera_setPosition00);
    tolua_function(tolua_S,"setOrientation",tolua_ClientToLua_Ogre_Camera_setOrientation00);
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"Node","Ogre::Node","",NULL);
   tolua_beginmodule(tolua_S,"Node");
    tolua_function(tolua_S,"setPosition",tolua_ClientToLua_Ogre_Node_setPosition00);
    tolua_function(tolua_S,"setOrientation",tolua_ClientToLua_Ogre_Node_setOrientation00);
   tolua_endmodule(tolua_S);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"ColourValue","Ogre::ColourValue","",tolua_collect_Ogre__ColourValue);
   #else
   tolua_cclass(tolua_S,"ColourValue","Ogre::ColourValue","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"ColourValue");
    tolua_function(tolua_S,"new",tolua_ClientToLua_Ogre_ColourValue_new00);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_Ogre_ColourValue_new00_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_Ogre_ColourValue_new00_local);
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"Light","Ogre::Light","",NULL);
   tolua_beginmodule(tolua_S,"Light");
    tolua_function(tolua_S,"setDiffuseColour",tolua_ClientToLua_Ogre_Light_setDiffuseColour00);
    tolua_function(tolua_S,"setSpecularColour",tolua_ClientToLua_Ogre_Light_setSpecularColour00);
    tolua_function(tolua_S,"setDiffuseColour",tolua_ClientToLua_Ogre_Light_setDiffuseColour01);
    tolua_function(tolua_S,"setSpecularColour",tolua_ClientToLua_Ogre_Light_setSpecularColour01);
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"SceneNode","Ogre::SceneNode","Ogre::Node",NULL);
   tolua_beginmodule(tolua_S,"SceneNode");
   tolua_endmodule(tolua_S);
  tolua_endmodule(tolua_S);
  tolua_module(tolua_S,"OIS",0);
  tolua_beginmodule(tolua_S,"OIS");
   tolua_constant(tolua_S,"KC_UNASSIGNED",OIS::KC_UNASSIGNED);
   tolua_constant(tolua_S,"KC_ESCAPE",OIS::KC_ESCAPE);
   tolua_constant(tolua_S,"KC_1",OIS::KC_1);
   tolua_constant(tolua_S,"KC_2",OIS::KC_2);
   tolua_constant(tolua_S,"KC_3",OIS::KC_3);
   tolua_constant(tolua_S,"KC_4",OIS::KC_4);
   tolua_constant(tolua_S,"KC_5",OIS::KC_5);
   tolua_constant(tolua_S,"KC_6",OIS::KC_6);
   tolua_constant(tolua_S,"KC_7",OIS::KC_7);
   tolua_constant(tolua_S,"KC_8",OIS::KC_8);
   tolua_constant(tolua_S,"KC_9",OIS::KC_9);
   tolua_constant(tolua_S,"KC_0",OIS::KC_0);
   tolua_constant(tolua_S,"KC_MINUS",OIS::KC_MINUS);
   tolua_constant(tolua_S,"KC_EQUALS",OIS::KC_EQUALS);
   tolua_constant(tolua_S,"KC_BACK",OIS::KC_BACK);
   tolua_constant(tolua_S,"KC_TAB",OIS::KC_TAB);
   tolua_constant(tolua_S,"KC_Q",OIS::KC_Q);
   tolua_constant(tolua_S,"KC_W",OIS::KC_W);
   tolua_constant(tolua_S,"KC_E",OIS::KC_E);
   tolua_constant(tolua_S,"KC_R",OIS::KC_R);
   tolua_constant(tolua_S,"KC_T",OIS::KC_T);
   tolua_constant(tolua_S,"KC_Y",OIS::KC_Y);
   tolua_constant(tolua_S,"KC_U",OIS::KC_U);
   tolua_constant(tolua_S,"KC_I",OIS::KC_I);
   tolua_constant(tolua_S,"KC_O",OIS::KC_O);
   tolua_constant(tolua_S,"KC_P",OIS::KC_P);
   tolua_constant(tolua_S,"KC_LBRACKET",OIS::KC_LBRACKET);
   tolua_constant(tolua_S,"KC_RBRACKET",OIS::KC_RBRACKET);
   tolua_constant(tolua_S,"KC_RETURN",OIS::KC_RETURN);
   tolua_constant(tolua_S,"KC_LCONTROL",OIS::KC_LCONTROL);
   tolua_constant(tolua_S,"KC_A",OIS::KC_A);
   tolua_constant(tolua_S,"KC_S",OIS::KC_S);
   tolua_constant(tolua_S,"KC_D",OIS::KC_D);
   tolua_constant(tolua_S,"KC_F",OIS::KC_F);
   tolua_constant(tolua_S,"KC_G",OIS::KC_G);
   tolua_constant(tolua_S,"KC_H",OIS::KC_H);
   tolua_constant(tolua_S,"KC_J",OIS::KC_J);
   tolua_constant(tolua_S,"KC_K",OIS::KC_K);
   tolua_constant(tolua_S,"KC_L",OIS::KC_L);
   tolua_constant(tolua_S,"KC_SEMICOLON",OIS::KC_SEMICOLON);
   tolua_constant(tolua_S,"KC_APOSTROPHE",OIS::KC_APOSTROPHE);
   tolua_constant(tolua_S,"KC_GRAVE",OIS::KC_GRAVE);
   tolua_constant(tolua_S,"KC_LSHIFT",OIS::KC_LSHIFT);
   tolua_constant(tolua_S,"KC_BACKSLASH",OIS::KC_BACKSLASH);
   tolua_constant(tolua_S,"KC_Z",OIS::KC_Z);
   tolua_constant(tolua_S,"KC_X",OIS::KC_X);
   tolua_constant(tolua_S,"KC_C",OIS::KC_C);
   tolua_constant(tolua_S,"KC_V",OIS::KC_V);
   tolua_constant(tolua_S,"KC_B",OIS::KC_B);
   tolua_constant(tolua_S,"KC_N",OIS::KC_N);
   tolua_constant(tolua_S,"KC_M",OIS::KC_M);
   tolua_constant(tolua_S,"KC_COMMA",OIS::KC_COMMA);
   tolua_constant(tolua_S,"KC_PERIOD",OIS::KC_PERIOD);
   tolua_constant(tolua_S,"KC_SLASH",OIS::KC_SLASH);
   tolua_constant(tolua_S,"KC_RSHIFT",OIS::KC_RSHIFT);
   tolua_constant(tolua_S,"KC_MULTIPLY",OIS::KC_MULTIPLY);
   tolua_constant(tolua_S,"KC_LMENU",OIS::KC_LMENU);
   tolua_constant(tolua_S,"KC_SPACE",OIS::KC_SPACE);
   tolua_constant(tolua_S,"KC_CAPITAL",OIS::KC_CAPITAL);
   tolua_constant(tolua_S,"KC_F1",OIS::KC_F1);
   tolua_constant(tolua_S,"KC_F2",OIS::KC_F2);
   tolua_constant(tolua_S,"KC_F3",OIS::KC_F3);
   tolua_constant(tolua_S,"KC_F4",OIS::KC_F4);
   tolua_constant(tolua_S,"KC_F5",OIS::KC_F5);
   tolua_constant(tolua_S,"KC_F6",OIS::KC_F6);
   tolua_constant(tolua_S,"KC_F7",OIS::KC_F7);
   tolua_constant(tolua_S,"KC_F8",OIS::KC_F8);
   tolua_constant(tolua_S,"KC_F9",OIS::KC_F9);
   tolua_constant(tolua_S,"KC_F10",OIS::KC_F10);
   tolua_constant(tolua_S,"KC_NUMLOCK",OIS::KC_NUMLOCK);
   tolua_constant(tolua_S,"KC_SCROLL",OIS::KC_SCROLL);
   tolua_constant(tolua_S,"KC_NUMPAD7",OIS::KC_NUMPAD7);
   tolua_constant(tolua_S,"KC_NUMPAD8",OIS::KC_NUMPAD8);
   tolua_constant(tolua_S,"KC_NUMPAD9",OIS::KC_NUMPAD9);
   tolua_constant(tolua_S,"KC_SUBTRACT",OIS::KC_SUBTRACT);
   tolua_constant(tolua_S,"KC_NUMPAD4",OIS::KC_NUMPAD4);
   tolua_constant(tolua_S,"KC_NUMPAD5",OIS::KC_NUMPAD5);
   tolua_constant(tolua_S,"KC_NUMPAD6",OIS::KC_NUMPAD6);
   tolua_constant(tolua_S,"KC_ADD",OIS::KC_ADD);
   tolua_constant(tolua_S,"KC_NUMPAD1",OIS::KC_NUMPAD1);
   tolua_constant(tolua_S,"KC_NUMPAD2",OIS::KC_NUMPAD2);
   tolua_constant(tolua_S,"KC_NUMPAD3",OIS::KC_NUMPAD3);
   tolua_constant(tolua_S,"KC_NUMPAD0",OIS::KC_NUMPAD0);
   tolua_constant(tolua_S,"KC_DECIMAL",OIS::KC_DECIMAL);
   tolua_constant(tolua_S,"KC_OEM_102",OIS::KC_OEM_102);
   tolua_constant(tolua_S,"KC_F11",OIS::KC_F11);
   tolua_constant(tolua_S,"KC_F12",OIS::KC_F12);
   tolua_constant(tolua_S,"KC_F13",OIS::KC_F13);
   tolua_constant(tolua_S,"KC_F14",OIS::KC_F14);
   tolua_constant(tolua_S,"KC_F15",OIS::KC_F15);
   tolua_constant(tolua_S,"KC_KANA",OIS::KC_KANA);
   tolua_constant(tolua_S,"KC_ABNT_C1",OIS::KC_ABNT_C1);
   tolua_constant(tolua_S,"KC_CONVERT",OIS::KC_CONVERT);
   tolua_constant(tolua_S,"KC_NOCONVERT",OIS::KC_NOCONVERT);
   tolua_constant(tolua_S,"KC_YEN",OIS::KC_YEN);
   tolua_constant(tolua_S,"KC_ABNT_C2",OIS::KC_ABNT_C2);
   tolua_constant(tolua_S,"KC_NUMPADEQUALS",OIS::KC_NUMPADEQUALS);
   tolua_constant(tolua_S,"KC_PREVTRACK",OIS::KC_PREVTRACK);
   tolua_constant(tolua_S,"KC_AT",OIS::KC_AT);
   tolua_constant(tolua_S,"KC_COLON",OIS::KC_COLON);
   tolua_constant(tolua_S,"KC_UNDERLINE",OIS::KC_UNDERLINE);
   tolua_constant(tolua_S,"KC_KANJI",OIS::KC_KANJI);
   tolua_constant(tolua_S,"KC_STOP",OIS::KC_STOP);
   tolua_constant(tolua_S,"KC_AX",OIS::KC_AX);
   tolua_constant(tolua_S,"KC_UNLABELED",OIS::KC_UNLABELED);
   tolua_constant(tolua_S,"KC_NEXTTRACK",OIS::KC_NEXTTRACK);
   tolua_constant(tolua_S,"KC_NUMPADENTER",OIS::KC_NUMPADENTER);
   tolua_constant(tolua_S,"KC_RCONTROL",OIS::KC_RCONTROL);
   tolua_constant(tolua_S,"KC_MUTE",OIS::KC_MUTE);
   tolua_constant(tolua_S,"KC_CALCULATOR",OIS::KC_CALCULATOR);
   tolua_constant(tolua_S,"KC_PLAYPAUSE",OIS::KC_PLAYPAUSE);
   tolua_constant(tolua_S,"KC_MEDIASTOP",OIS::KC_MEDIASTOP);
   tolua_constant(tolua_S,"KC_VOLUMEDOWN",OIS::KC_VOLUMEDOWN);
   tolua_constant(tolua_S,"KC_VOLUMEUP",OIS::KC_VOLUMEUP);
   tolua_constant(tolua_S,"KC_WEBHOME",OIS::KC_WEBHOME);
   tolua_constant(tolua_S,"KC_NUMPADCOMMA",OIS::KC_NUMPADCOMMA);
   tolua_constant(tolua_S,"KC_DIVIDE",OIS::KC_DIVIDE);
   tolua_constant(tolua_S,"KC_SYSRQ",OIS::KC_SYSRQ);
   tolua_constant(tolua_S,"KC_RMENU",OIS::KC_RMENU);
   tolua_constant(tolua_S,"KC_PAUSE",OIS::KC_PAUSE);
   tolua_constant(tolua_S,"KC_HOME",OIS::KC_HOME);
   tolua_constant(tolua_S,"KC_UP",OIS::KC_UP);
   tolua_constant(tolua_S,"KC_PGUP",OIS::KC_PGUP);
   tolua_constant(tolua_S,"KC_LEFT",OIS::KC_LEFT);
   tolua_constant(tolua_S,"KC_RIGHT",OIS::KC_RIGHT);
   tolua_constant(tolua_S,"KC_END",OIS::KC_END);
   tolua_constant(tolua_S,"KC_DOWN",OIS::KC_DOWN);
   tolua_constant(tolua_S,"KC_PGDOWN",OIS::KC_PGDOWN);
   tolua_constant(tolua_S,"KC_INSERT",OIS::KC_INSERT);
   tolua_constant(tolua_S,"KC_DELETE",OIS::KC_DELETE);
   tolua_constant(tolua_S,"KC_LWIN",OIS::KC_LWIN);
   tolua_constant(tolua_S,"KC_RWIN",OIS::KC_RWIN);
   tolua_constant(tolua_S,"KC_APPS",OIS::KC_APPS);
   tolua_constant(tolua_S,"KC_POWER",OIS::KC_POWER);
   tolua_constant(tolua_S,"KC_SLEEP",OIS::KC_SLEEP);
   tolua_constant(tolua_S,"KC_WAKE",OIS::KC_WAKE);
   tolua_constant(tolua_S,"KC_WEBSEARCH",OIS::KC_WEBSEARCH);
   tolua_constant(tolua_S,"KC_WEBFAVORITES",OIS::KC_WEBFAVORITES);
   tolua_constant(tolua_S,"KC_WEBREFRESH",OIS::KC_WEBREFRESH);
   tolua_constant(tolua_S,"KC_WEBSTOP",OIS::KC_WEBSTOP);
   tolua_constant(tolua_S,"KC_WEBFORWARD",OIS::KC_WEBFORWARD);
   tolua_constant(tolua_S,"KC_WEBBACK",OIS::KC_WEBBACK);
   tolua_constant(tolua_S,"KC_MYCOMPUTER",OIS::KC_MYCOMPUTER);
   tolua_constant(tolua_S,"KC_MAIL",OIS::KC_MAIL);
   tolua_constant(tolua_S,"KC_MEDIASELECT",OIS::KC_MEDIASELECT);
  tolua_endmodule(tolua_S);
  tolua_module(tolua_S,"Gorilla",0);
  tolua_beginmodule(tolua_S,"Gorilla");
   tolua_constant(tolua_S,"Gradient_NorthSouth",Gorilla::Gradient_NorthSouth);
   tolua_constant(tolua_S,"Gradient_WestEast",Gorilla::Gradient_WestEast);
   tolua_constant(tolua_S,"Gradient_Diagonal",Gorilla::Gradient_Diagonal);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"LuaInterface","LuaInterface","",NULL);
  tolua_beginmodule(tolua_S,"LuaInterface");
   tolua_function(tolua_S,"log",tolua_ClientToLua_LuaInterface_log00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"BaseObject","BaseObject","",tolua_collect_BaseObject);
  #else
  tolua_cclass(tolua_S,"BaseObject","BaseObject","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"BaseObject");
   tolua_constant(tolua_S,"OBJ_AGENT",BaseObject::OBJ_AGENT);
   tolua_constant(tolua_S,"OBJ_SANDBOX",BaseObject::OBJ_SANDBOX);
   tolua_constant(tolua_S,"OBJ_SANDBOX_UI",BaseObject::OBJ_SANDBOX_UI);
   tolua_constant(tolua_S,"OBJ_SANDBOX_OBJ",BaseObject::OBJ_SANDBOX_OBJ);
   tolua_function(tolua_S,"new",tolua_ClientToLua_BaseObject_new00);
   tolua_function(tolua_S,"new_local",tolua_ClientToLua_BaseObject_new00_local);
   tolua_function(tolua_S,".call",tolua_ClientToLua_BaseObject_new00_local);
   tolua_function(tolua_S,"delete",tolua_ClientToLua_BaseObject_delete00);
   tolua_function(tolua_S,"setObjId",tolua_ClientToLua_BaseObject_setObjId00);
   tolua_function(tolua_S,"getObjId",tolua_ClientToLua_BaseObject_getObjId00);
   tolua_function(tolua_S,"setObjType",tolua_ClientToLua_BaseObject_setObjType00);
   tolua_function(tolua_S,"getObjType",tolua_ClientToLua_BaseObject_getObjType00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"QuaternionFromRotationDegrees",tolua_ClientToLua_QuaternionFromRotationDegrees00);
  tolua_function(tolua_S,"QuaternionToRotationDegrees",tolua_ClientToLua_QuaternionToRotationDegrees00);
  tolua_function(tolua_S,"BtVector3ToVector3",tolua_ClientToLua_BtVector3ToVector300);
  tolua_function(tolua_S,"BtQuaternionToQuaternion",tolua_ClientToLua_BtQuaternionToQuaternion00);
  tolua_function(tolua_S,"Vector3ToVec3",tolua_ClientToLua_Vector3ToVec300);
  tolua_function(tolua_S,"Vec3ToVector3",tolua_ClientToLua_Vec3ToVector300);
  tolua_cclass(tolua_S,"UIComponent","UIComponent","BaseObject",NULL);
  tolua_beginmodule(tolua_S,"UIComponent");
   tolua_constant(tolua_S,"FONT_SMALL",UIComponent::FONT_SMALL);
   tolua_constant(tolua_S,"FONT_SMALL_MONO",UIComponent::FONT_SMALL_MONO);
   tolua_constant(tolua_S,"FONT_MEDIUM",UIComponent::FONT_MEDIUM);
   tolua_constant(tolua_S,"FONT_MEDIUM_MONO",UIComponent::FONT_MEDIUM_MONO);
   tolua_constant(tolua_S,"FONT_LARGE",UIComponent::FONT_LARGE);
   tolua_constant(tolua_S,"FONT_LARGE_MONO",UIComponent::FONT_LARGE_MONO);
   tolua_constant(tolua_S,"FONT_UNKNOWN",UIComponent::FONT_UNKNOWN);
   tolua_function(tolua_S,"addChild",tolua_ClientToLua_UIComponent_addChild00);
   tolua_function(tolua_S,"setFontType",tolua_ClientToLua_UIComponent_setFontType00);
   tolua_function(tolua_S,"setText",tolua_ClientToLua_UIComponent_setText00);
   tolua_function(tolua_S,"setMarkupText",tolua_ClientToLua_UIComponent_setMarkupText00);
   tolua_function(tolua_S,"setPosition",tolua_ClientToLua_UIComponent_setPosition00);
   tolua_function(tolua_S,"setDimension",tolua_ClientToLua_UIComponent_setDimension00);
   tolua_function(tolua_S,"setTextMargin",tolua_ClientToLua_UIComponent_setTextMargin00);
   tolua_function(tolua_S,"setBackgroundColor",tolua_ClientToLua_UIComponent_setBackgroundColor00);
   tolua_function(tolua_S,"setGradientColor",tolua_ClientToLua_UIComponent_setGradientColor00);
   tolua_function(tolua_S,"setVisible",tolua_ClientToLua_UIComponent_setVisible00);
   tolua_function(tolua_S,"isVisible",tolua_ClientToLua_UIComponent_isVisible00);
   tolua_function(tolua_S,"GetDimension",tolua_ClientToLua_UIComponent_GetDimension00);
  tolua_endmodule(tolua_S);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"SandboxObject","SandboxObject","BaseObject",tolua_collect_SandboxObject);
  #else
  tolua_cclass(tolua_S,"SandboxObject","SandboxObject","BaseObject",NULL);
  #endif
  tolua_beginmodule(tolua_S,"SandboxObject");
   tolua_function(tolua_S,"new",tolua_ClientToLua_SandboxObject_new00);
   tolua_function(tolua_S,"new_local",tolua_ClientToLua_SandboxObject_new00_local);
   tolua_function(tolua_S,".call",tolua_ClientToLua_SandboxObject_new00_local);
   tolua_function(tolua_S,"new",tolua_ClientToLua_SandboxObject_new01);
   tolua_function(tolua_S,"new_local",tolua_ClientToLua_SandboxObject_new01_local);
   tolua_function(tolua_S,".call",tolua_ClientToLua_SandboxObject_new01_local);
   tolua_function(tolua_S,"delete",tolua_ClientToLua_SandboxObject_delete00);
   tolua_function(tolua_S,"Initialize",tolua_ClientToLua_SandboxObject_Initialize00);
   tolua_function(tolua_S,"getRigidBody",tolua_ClientToLua_SandboxObject_getRigidBody00);
   tolua_function(tolua_S,"getSceneNode",tolua_ClientToLua_SandboxObject_getSceneNode00);
   tolua_function(tolua_S,"setMass",tolua_ClientToLua_SandboxObject_setMass00);
   tolua_function(tolua_S,"setPosition",tolua_ClientToLua_SandboxObject_setPosition00);
   tolua_function(tolua_S,"setRotation",tolua_ClientToLua_SandboxObject_setRotation00);
   tolua_function(tolua_S,"setOrientation",tolua_ClientToLua_SandboxObject_setOrientation00);
   tolua_function(tolua_S,"applyImpulse",tolua_ClientToLua_SandboxObject_applyImpulse00);
   tolua_function(tolua_S,"applyAngularImpulse",tolua_ClientToLua_SandboxObject_applyAngularImpulse00);
  tolua_endmodule(tolua_S);
  tolua_constant(tolua_S,"AGENT_OBJ_NONE",AGENT_OBJ_NONE);
  tolua_constant(tolua_S,"AGENT_OBJ_SEEKING",AGENT_OBJ_SEEKING);
  tolua_constant(tolua_S,"AGENT_OBJ_FOLLOWER",AGENT_OBJ_FOLLOWER);
  tolua_constant(tolua_S,"AGENT_OBJ_PATHING",AGENT_OBJ_PATHING);
  tolua_constant(tolua_S,"AGENT_OBJ_PURSUING",AGENT_OBJ_PURSUING);
  tolua_cclass(tolua_S,"AgentObject","AgentObject","BaseObject",NULL);
  tolua_beginmodule(tolua_S,"AgentObject");
   tolua_function(tolua_S,"getAgentType",tolua_ClientToLua_AgentObject_getAgentType00);
   tolua_function(tolua_S,"setAgentType",tolua_ClientToLua_AgentObject_setAgentType00);
   tolua_function(tolua_S,"SetPosition",tolua_ClientToLua_AgentObject_SetPosition00);
   tolua_function(tolua_S,"SetRotation",tolua_ClientToLua_AgentObject_SetRotation00);
   tolua_function(tolua_S,"SetOrientation",tolua_ClientToLua_AgentObject_SetOrientation00);
   tolua_function(tolua_S,"SetForward",tolua_ClientToLua_AgentObject_SetForward00);
   tolua_function(tolua_S,"SetVelocity",tolua_ClientToLua_AgentObject_SetVelocity00);
   tolua_function(tolua_S,"SetTarget",tolua_ClientToLua_AgentObject_SetTarget00);
   tolua_function(tolua_S,"SetTargetRadius",tolua_ClientToLua_AgentObject_SetTargetRadius00);
   tolua_function(tolua_S,"GetUp",tolua_ClientToLua_AgentObject_GetUp00);
   tolua_function(tolua_S,"GetLeft",tolua_ClientToLua_AgentObject_GetLeft00);
   tolua_function(tolua_S,"GetTarget",tolua_ClientToLua_AgentObject_GetTarget00);
   tolua_function(tolua_S,"GetForward",tolua_ClientToLua_AgentObject_GetForward00);
   tolua_function(tolua_S,"GetVelocity",tolua_ClientToLua_AgentObject_GetVelocity00);
   tolua_function(tolua_S,"GetPosition",tolua_ClientToLua_AgentObject_GetPosition00);
   tolua_function(tolua_S,"GetOrientation",tolua_ClientToLua_AgentObject_GetOrientation00);
   tolua_function(tolua_S,"SetMass",tolua_ClientToLua_AgentObject_SetMass00);
   tolua_function(tolua_S,"SetHeight",tolua_ClientToLua_AgentObject_SetHeight00);
   tolua_function(tolua_S,"SetRadius",tolua_ClientToLua_AgentObject_SetRadius00);
   tolua_function(tolua_S,"SetSpeed",tolua_ClientToLua_AgentObject_SetSpeed00);
   tolua_function(tolua_S,"SetHealth",tolua_ClientToLua_AgentObject_SetHealth00);
   tolua_function(tolua_S,"SetMaxForce",tolua_ClientToLua_AgentObject_SetMaxForce00);
   tolua_function(tolua_S,"SetMaxSpeed",tolua_ClientToLua_AgentObject_SetMaxSpeed00);
   tolua_function(tolua_S,"GetMass",tolua_ClientToLua_AgentObject_GetMass00);
   tolua_function(tolua_S,"GetSpeed",tolua_ClientToLua_AgentObject_GetSpeed00);
   tolua_function(tolua_S,"GetHeight",tolua_ClientToLua_AgentObject_GetHeight00);
   tolua_function(tolua_S,"GetRadius",tolua_ClientToLua_AgentObject_GetRadius00);
   tolua_function(tolua_S,"GetHealth",tolua_ClientToLua_AgentObject_GetHealth00);
   tolua_function(tolua_S,"GetMaxForce",tolua_ClientToLua_AgentObject_GetMaxForce00);
   tolua_function(tolua_S,"GetMaxSpeed",tolua_ClientToLua_AgentObject_GetMaxSpeed00);
   tolua_function(tolua_S,"GetTargetRadius",tolua_ClientToLua_AgentObject_GetTargetRadius00);
   tolua_function(tolua_S,"PredictFuturePosition",tolua_ClientToLua_AgentObject_PredictFuturePosition00);
   tolua_function(tolua_S,"ForceToPosition",tolua_ClientToLua_AgentObject_ForceToPosition00);
   tolua_function(tolua_S,"ForceToFollowPath",tolua_ClientToLua_AgentObject_ForceToFollowPath00);
   tolua_function(tolua_S,"ForceToStayOnPath",tolua_ClientToLua_AgentObject_ForceToStayOnPath00);
   tolua_function(tolua_S,"ForceToWander",tolua_ClientToLua_AgentObject_ForceToWander00);
   tolua_function(tolua_S,"ForceToTarget",tolua_ClientToLua_AgentObject_ForceToTarget00);
   tolua_function(tolua_S,"ForceToStayOnPath",tolua_ClientToLua_AgentObject_ForceToStayOnPath01);
   tolua_function(tolua_S,"ForceToFollowPath",tolua_ClientToLua_AgentObject_ForceToFollowPath01);
   tolua_function(tolua_S,"ApplyForce",tolua_ClientToLua_AgentObject_ApplyForce00);
   tolua_function(tolua_S,"HasPath",tolua_ClientToLua_AgentObject_HasPath00);
   tolua_function(tolua_S,"SetPath",tolua_ClientToLua_AgentObject_SetPath00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"SandboxMgr","SandboxMgr","",NULL);
  tolua_beginmodule(tolua_S,"SandboxMgr");
   tolua_function(tolua_S,"GetCamera",tolua_ClientToLua_SandboxMgr_GetCamera00);
   tolua_function(tolua_S,"GetSceneManager",tolua_ClientToLua_SandboxMgr_GetSceneManager00);
   tolua_function(tolua_S,"GetScreenWidth",tolua_ClientToLua_SandboxMgr_GetScreenWidth00);
   tolua_function(tolua_S,"GetScreenHeight",tolua_ClientToLua_SandboxMgr_GetScreenHeight00);
   tolua_function(tolua_S,"GetCameraUp",tolua_ClientToLua_SandboxMgr_GetCameraUp00);
   tolua_function(tolua_S,"GetCameraLeft",tolua_ClientToLua_SandboxMgr_GetCameraLeft00);
   tolua_function(tolua_S,"GetCameraForward",tolua_ClientToLua_SandboxMgr_GetCameraForward00);
   tolua_function(tolua_S,"GetCameraPosition",tolua_ClientToLua_SandboxMgr_GetCameraPosition00);
   tolua_function(tolua_S,"GetCameraRotation",tolua_ClientToLua_SandboxMgr_GetCameraRotation00);
   tolua_function(tolua_S,"GetCameraOrientation",tolua_ClientToLua_SandboxMgr_GetCameraOrientation00);
   tolua_function(tolua_S,"GetRenderTime",tolua_ClientToLua_SandboxMgr_GetRenderTime00);
   tolua_function(tolua_S,"GetSimulateTime",tolua_ClientToLua_SandboxMgr_GetSimulateTime00);
   tolua_function(tolua_S,"GetTotalSimulateTime",tolua_ClientToLua_SandboxMgr_GetTotalSimulateTime00);
   tolua_function(tolua_S,"GetObjectCount",tolua_ClientToLua_SandboxMgr_GetObjectCount00);
   tolua_function(tolua_S,"GetSeekingAgent",tolua_ClientToLua_SandboxMgr_GetSeekingAgent00);
   tolua_function(tolua_S,"CallFile",tolua_ClientToLua_SandboxMgr_CallFile00);
   tolua_function(tolua_S,"SetSkyBox",tolua_ClientToLua_SandboxMgr_SetSkyBox00);
   tolua_function(tolua_S,"SetAmbientLight",tolua_ClientToLua_SandboxMgr_SetAmbientLight00);
   tolua_function(tolua_S,"CreateDirectionalLight",tolua_ClientToLua_SandboxMgr_CreateDirectionalLight00);
   tolua_function(tolua_S,"setMaterial",tolua_ClientToLua_SandboxMgr_setMaterial00);
   tolua_function(tolua_S,"setMaterial",tolua_ClientToLua_SandboxMgr_setMaterial01);
   tolua_function(tolua_S,"SetMarkupColor",tolua_ClientToLua_SandboxMgr_SetMarkupColor00);
   tolua_function(tolua_S,"CreatePlane",tolua_ClientToLua_SandboxMgr_CreatePlane00);
   tolua_function(tolua_S,"CreateSandboxObject",tolua_ClientToLua_SandboxMgr_CreateSandboxObject00);
   tolua_function(tolua_S,"CreateUIComponent",tolua_ClientToLua_SandboxMgr_CreateUIComponent00);
   tolua_function(tolua_S,"CreateAgent",tolua_ClientToLua_SandboxMgr_CreateAgent00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"DebugDrawer","DebugDrawer","",NULL);
  tolua_beginmodule(tolua_S,"DebugDrawer");
   tolua_function(tolua_S,"getIsEnable",tolua_ClientToLua_DebugDrawer_getIsEnable00);
   tolua_function(tolua_S,"setIsEnable",tolua_ClientToLua_DebugDrawer_setIsEnable00);
   tolua_function(tolua_S,"drawLine",tolua_ClientToLua_DebugDrawer_drawLine00);
   tolua_function(tolua_S,"drawCircle",tolua_ClientToLua_DebugDrawer_drawCircle00);
   tolua_function(tolua_S,"drawSquare",tolua_ClientToLua_DebugDrawer_drawSquare00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"GameManager","GameManager","",NULL);
  tolua_beginmodule(tolua_S,"GameManager");
   tolua_function(tolua_S,"getScreenWidth",tolua_ClientToLua_GameManager_getScreenWidth00);
   tolua_function(tolua_S,"getScreenHeight",tolua_ClientToLua_GameManager_getScreenHeight00);
   tolua_function(tolua_S,"getObjectCount",tolua_ClientToLua_GameManager_getObjectCount00);
   tolua_function(tolua_S,"getPhysicsWorld",tolua_ClientToLua_GameManager_getPhysicsWorld00);
   tolua_function(tolua_S,"createUIComponent",tolua_ClientToLua_GameManager_createUIComponent00);
   tolua_function(tolua_S,"addSandboxObject",tolua_ClientToLua_GameManager_addSandboxObject00);
   tolua_function(tolua_S,"addAgentObject",tolua_ClientToLua_GameManager_addAgentObject00);
   tolua_function(tolua_S,"setMarkupColor",tolua_ClientToLua_GameManager_setMarkupColor00);
   tolua_function(tolua_S,"getAllAgents",tolua_ClientToLua_GameManager_getAllAgents00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_ClientToLua (lua_State* tolua_S) {
 return tolua_ClientToLua_open(tolua_S);
};
#endif

