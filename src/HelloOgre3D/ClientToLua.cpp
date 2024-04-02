/*
** Lua binding: ClientToLua
** Generated automatically by tolua++-1.0.92 on Tue Apr  2 02:39:54 2024.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_ClientToLua_open (lua_State* tolua_S);

#include "OgreVector3.h"
#include "game/OgreWrapper.h"
#include "game/LuaInterface.h"
#include "sandbox/SandboxDef.h"

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
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"Ogre::Vector3");
 tolua_usertype(tolua_S,"LuaInterface");
 tolua_usertype(tolua_S,"Ogre::Quaternion");
 tolua_usertype(tolua_S,"Ogre::SceneNode");
 tolua_usertype(tolua_S,"OgreWrapper");
 tolua_usertype(tolua_S,"Ogre::Camera");
}

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

/* method: CreatePlane of class  OgreWrapper */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_OgreWrapper_CreatePlane00
static int tolua_ClientToLua_OgreWrapper_CreatePlane00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"OgreWrapper",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  OgreWrapper* self = (OgreWrapper*)  tolua_tousertype(tolua_S,1,0);
  int length = ((int)  tolua_tonumber(tolua_S,2,0));
  int width = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreatePlane'", NULL);
#endif
  {
   Ogre::SceneNode* tolua_ret = (Ogre::SceneNode*)  self->CreatePlane(length,width);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"Ogre::SceneNode");
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

/* method: CreateSkyBox of class  OgreWrapper */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_OgreWrapper_CreateSkyBox00
static int tolua_ClientToLua_OgreWrapper_CreateSkyBox00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"OgreWrapper",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     (tolua_isvaluenil(tolua_S,3,&tolua_err) || !tolua_isusertype(tolua_S,3,"Ogre::Vector3",0,&tolua_err)) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  OgreWrapper* self = (OgreWrapper*)  tolua_tousertype(tolua_S,1,0);
  const std::string materialName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
  Ogre::Vector3* rotation = ((Ogre::Vector3*)  tolua_tousertype(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateSkyBox'", NULL);
#endif
  {
   self->CreateSkyBox(materialName,*rotation);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateSkyBox'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetCamera of class  OgreWrapper */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_OgreWrapper_GetCamera00
static int tolua_ClientToLua_OgreWrapper_GetCamera00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"OgreWrapper",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  OgreWrapper* self = (OgreWrapper*)  tolua_tousertype(tolua_S,1,0);
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

/* Open function */
TOLUA_API int tolua_ClientToLua_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_module(tolua_S,"Ogre",0);
  tolua_beginmodule(tolua_S,"Ogre");
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"Vector3","Ogre::Vector3","",tolua_collect_Ogre__Vector3);
   #else
   tolua_cclass(tolua_S,"Vector3","Ogre::Vector3","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"Vector3");
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
   tolua_endmodule(tolua_S);
   #ifdef __cplusplus
   tolua_cclass(tolua_S,"Quaternion","Ogre::Quaternion","",tolua_collect_Ogre__Quaternion);
   #else
   tolua_cclass(tolua_S,"Quaternion","Ogre::Quaternion","",NULL);
   #endif
   tolua_beginmodule(tolua_S,"Quaternion");
    tolua_function(tolua_S,"new",tolua_ClientToLua_Ogre_Quaternion_new00);
    tolua_function(tolua_S,"new_local",tolua_ClientToLua_Ogre_Quaternion_new00_local);
    tolua_function(tolua_S,".call",tolua_ClientToLua_Ogre_Quaternion_new00_local);
   tolua_endmodule(tolua_S);
   tolua_cclass(tolua_S,"Camera","Ogre::Camera","",NULL);
   tolua_beginmodule(tolua_S,"Camera");
    tolua_function(tolua_S,"setPosition",tolua_ClientToLua_Ogre_Camera_setPosition00);
    tolua_function(tolua_S,"setOrientation",tolua_ClientToLua_Ogre_Camera_setOrientation00);
   tolua_endmodule(tolua_S);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"OgreWrapper","OgreWrapper","",NULL);
  tolua_beginmodule(tolua_S,"OgreWrapper");
   tolua_function(tolua_S,"CreatePlane",tolua_ClientToLua_OgreWrapper_CreatePlane00);
   tolua_function(tolua_S,"CreateSkyBox",tolua_ClientToLua_OgreWrapper_CreateSkyBox00);
   tolua_function(tolua_S,"GetCamera",tolua_ClientToLua_OgreWrapper_GetCamera00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"LuaInterface","LuaInterface","",NULL);
  tolua_beginmodule(tolua_S,"LuaInterface");
   tolua_function(tolua_S,"log",tolua_ClientToLua_LuaInterface_log00);
  tolua_endmodule(tolua_S);
  tolua_function(tolua_S,"QuaternionFromRotationDegrees",tolua_ClientToLua_QuaternionFromRotationDegrees00);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_ClientToLua (lua_State* tolua_S) {
 return tolua_ClientToLua_open(tolua_S);
};
#endif

