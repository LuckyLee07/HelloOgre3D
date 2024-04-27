/*
** Lua binding: ClientToLua
** Generated automatically by tolua++-1.0.92 on Sat Apr 27 14:40:50 2024.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_ClientToLua_open (lua_State* tolua_S);

#include "Ogre.h"
#include "base/LuaInterface.h"
#include "base/BaseObject.h"
#include "sandbox/SandboxDef.h"
#include "sandbox/SandboxObject.h"
#include "sandbox/SandboxMgr.h"
#include "game/GameManager.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_Ogre__Vector3 (lua_State* tolua_S)
{
 Ogre::Vector3* self = (Ogre::Vector3*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_BaseObject (lua_State* tolua_S)
{
 BaseObject* self = (BaseObject*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_Ogre__ColourValue (lua_State* tolua_S)
{
 Ogre::ColourValue* self = (Ogre::ColourValue*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_Ogre__Quaternion (lua_State* tolua_S)
{
 Ogre::Quaternion* self = (Ogre::Quaternion*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}

static int tolua_collect_SandboxObject (lua_State* tolua_S)
{
 SandboxObject* self = (SandboxObject*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"LuaInterface");
 tolua_usertype(tolua_S,"Ogre::SceneManager");
 tolua_usertype(tolua_S,"Ogre::ColourValue");
 tolua_usertype(tolua_S,"SandboxMgr");
 tolua_usertype(tolua_S,"btRigidBody");
 tolua_usertype(tolua_S,"SandboxObject");
 tolua_usertype(tolua_S,"GameManager");
 tolua_usertype(tolua_S,"Ogre::Vector3");
 tolua_usertype(tolua_S,"Ogre::Light");
 tolua_usertype(tolua_S,"BaseObject");
 tolua_usertype(tolua_S,"Ogre::Quaternion");
 tolua_usertype(tolua_S,"Ogre::Node");
 tolua_usertype(tolua_S,"Ogre::SceneNode");
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
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"SandboxMgr","SandboxMgr","",NULL);
  tolua_beginmodule(tolua_S,"SandboxMgr");
   tolua_function(tolua_S,"GetCamera",tolua_ClientToLua_SandboxMgr_GetCamera00);
   tolua_function(tolua_S,"GetSceneManager",tolua_ClientToLua_SandboxMgr_GetSceneManager00);
   tolua_function(tolua_S,"SetSkyBox",tolua_ClientToLua_SandboxMgr_SetSkyBox00);
   tolua_function(tolua_S,"SetAmbientLight",tolua_ClientToLua_SandboxMgr_SetAmbientLight00);
   tolua_function(tolua_S,"CreateDirectionalLight",tolua_ClientToLua_SandboxMgr_CreateDirectionalLight00);
   tolua_function(tolua_S,"setMaterial",tolua_ClientToLua_SandboxMgr_setMaterial00);
   tolua_function(tolua_S,"setMaterial",tolua_ClientToLua_SandboxMgr_setMaterial01);
   tolua_function(tolua_S,"CreatePlane",tolua_ClientToLua_SandboxMgr_CreatePlane00);
   tolua_function(tolua_S,"CreateSandboxObject",tolua_ClientToLua_SandboxMgr_CreateSandboxObject00);
  tolua_endmodule(tolua_S);
  tolua_cclass(tolua_S,"GameManager","GameManager","",NULL);
  tolua_beginmodule(tolua_S,"GameManager");
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_ClientToLua (lua_State* tolua_S) {
 return tolua_ClientToLua_open(tolua_S);
};
#endif

