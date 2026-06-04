/*
** Lua binding: RuntimeToLua
** Generated automatically by tolua++-1.0.92 on Thu Jun  4 20:19:01 2026.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_RuntimeToLua_open (lua_State* tolua_S);

#include "ui/fairygui/lua_bridge/FairyGuiLuaApi.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"FairyGuiLuaApi");
}

/* method: IsAvailable of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_IsAvailable00
static int tolua_RuntimeToLua_FairyGuiLuaApi_IsAvailable00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsAvailable'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsAvailable();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsAvailable'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: LoadPackage of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_LoadPackage00
static int tolua_RuntimeToLua_FairyGuiLuaApi_LoadPackage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  const char* packagePath = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'LoadPackage'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->LoadPackage(packagePath);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'LoadPackage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RemovePackage of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_RemovePackage00
static int tolua_RuntimeToLua_FairyGuiLuaApi_RemovePackage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  const char* packageName = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RemovePackage'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RemovePackage(packageName);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RemovePackage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateObject of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_CreateObject00
static int tolua_RuntimeToLua_FairyGuiLuaApi_CreateObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  const char* packageName = ((const char*)  tolua_tostring(tolua_S,2,0));
  const char* objectName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateObject'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CreateObject(packageName,objectName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateContainer of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_CreateContainer00
static int tolua_RuntimeToLua_FairyGuiLuaApi_CreateContainer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  const char* name = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateContainer'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CreateContainer(name);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateContainer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateChildContainer of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_CreateChildContainer00
static int tolua_RuntimeToLua_FairyGuiLuaApi_CreateChildContainer00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int ownerHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* name = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateChildContainer'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CreateChildContainer(ownerHandle,name);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateChildContainer'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateLoader of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_CreateLoader00
static int tolua_RuntimeToLua_FairyGuiLuaApi_CreateLoader00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int ownerHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* name = ((const char*)  tolua_tostring(tolua_S,3,0));
  const char* url = ((const char*)  tolua_tostring(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateLoader'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CreateLoader(ownerHandle,name,url);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateLoader'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateText of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_CreateText00
static int tolua_RuntimeToLua_FairyGuiLuaApi_CreateText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
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
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int ownerHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* name = ((const char*)  tolua_tostring(tolua_S,3,0));
  const char* text = ((const char*)  tolua_tostring(tolua_S,4,0));
  float fontSize = ((float)  tolua_tonumber(tolua_S,5,0));
  float red = ((float)  tolua_tonumber(tolua_S,6,0));
  float green = ((float)  tolua_tonumber(tolua_S,7,0));
  float blue = ((float)  tolua_tonumber(tolua_S,8,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateText'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CreateText(ownerHandle,name,text,fontSize,red,green,blue);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateText'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateTextInput of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_CreateTextInput00
static int tolua_RuntimeToLua_FairyGuiLuaApi_CreateTextInput00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
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
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int ownerHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* name = ((const char*)  tolua_tostring(tolua_S,3,0));
  const char* text = ((const char*)  tolua_tostring(tolua_S,4,0));
  float fontSize = ((float)  tolua_tonumber(tolua_S,5,0));
  float red = ((float)  tolua_tonumber(tolua_S,6,0));
  float green = ((float)  tolua_tonumber(tolua_S,7,0));
  float blue = ((float)  tolua_tonumber(tolua_S,8,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateTextInput'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CreateTextInput(ownerHandle,name,text,fontSize,red,green,blue);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateTextInput'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateGraphRect of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_CreateGraphRect00
static int tolua_RuntimeToLua_FairyGuiLuaApi_CreateGraphRect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,10,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int ownerHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* name = ((const char*)  tolua_tostring(tolua_S,3,0));
  float width = ((float)  tolua_tonumber(tolua_S,4,0));
  float height = ((float)  tolua_tonumber(tolua_S,5,0));
  float red = ((float)  tolua_tonumber(tolua_S,6,0));
  float green = ((float)  tolua_tonumber(tolua_S,7,0));
  float blue = ((float)  tolua_tonumber(tolua_S,8,0));
  float alpha = ((float)  tolua_tonumber(tolua_S,9,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateGraphRect'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CreateGraphRect(ownerHandle,name,width,height,red,green,blue,alpha);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateGraphRect'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateGraphRegularPolygon of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_CreateGraphRegularPolygon00
static int tolua_RuntimeToLua_FairyGuiLuaApi_CreateGraphRegularPolygon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,10,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,11,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int ownerHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* name = ((const char*)  tolua_tostring(tolua_S,3,0));
  float width = ((float)  tolua_tonumber(tolua_S,4,0));
  float height = ((float)  tolua_tonumber(tolua_S,5,0));
  int sides = ((int)  tolua_tonumber(tolua_S,6,0));
  float red = ((float)  tolua_tonumber(tolua_S,7,0));
  float green = ((float)  tolua_tonumber(tolua_S,8,0));
  float blue = ((float)  tolua_tonumber(tolua_S,9,0));
  float alpha = ((float)  tolua_tonumber(tolua_S,10,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateGraphRegularPolygon'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CreateGraphRegularPolygon(ownerHandle,name,width,height,sides,red,green,blue,alpha);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateGraphRegularPolygon'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateModalMask of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_CreateModalMask00
static int tolua_RuntimeToLua_FairyGuiLuaApi_CreateModalMask00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
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
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  float red = ((float)  tolua_tonumber(tolua_S,2,0));
  float green = ((float)  tolua_tonumber(tolua_S,3,0));
  float blue = ((float)  tolua_tonumber(tolua_S,4,0));
  float alpha = ((float)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateModalMask'", NULL);
#endif
  {
   int tolua_ret = (int)  self->CreateModalMask(red,green,blue,alpha);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateModalMask'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastRenderCommandCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastRenderCommandCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastRenderCommandCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastRenderCommandCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastRenderCommandCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastRenderCommandCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastTriangleCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastTriangleCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastTriangleCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastTriangleCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastTriangleCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastTriangleCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetScreenWidth of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetScreenWidth00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetScreenWidth00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetScreenWidth'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetScreenWidth();
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

/* method: GetScreenHeight of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetScreenHeight00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetScreenHeight00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetScreenHeight'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetScreenHeight();
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

/* method: GetRuntimeObjectHandleCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetRuntimeObjectHandleCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetRuntimeObjectHandleCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRuntimeObjectHandleCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetRuntimeObjectHandleCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRuntimeObjectHandleCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetRuntimeListenerBindingCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetRuntimeListenerBindingCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetRuntimeListenerBindingCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetRuntimeListenerBindingCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetRuntimeListenerBindingCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetRuntimeListenerBindingCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMaterialCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetMaterialCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetMaterialCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMaterialCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetMaterialCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaterialCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTextureCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetTextureCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetTextureCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTextureCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetTextureCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTextureCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMaterialAliasCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetMaterialAliasCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetMaterialAliasCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMaterialAliasCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetMaterialAliasCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaterialAliasCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTextureAliasCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetTextureAliasCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetTextureAliasCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTextureAliasCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetTextureAliasCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTextureAliasCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastDrawCommandCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastDrawCommandCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastDrawCommandCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastDrawCommandCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastDrawCommandCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastDrawCommandCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastDrawTriangleCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastDrawTriangleCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastDrawTriangleCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastDrawTriangleCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastDrawTriangleCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastDrawTriangleCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastMaterialSwitchCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastMaterialSwitchCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastMaterialSwitchCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastMaterialSwitchCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastMaterialSwitchCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastMaterialSwitchCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastTextureSwitchCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastTextureSwitchCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastTextureSwitchCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastTextureSwitchCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastTextureSwitchCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastTextureSwitchCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastClippedCommandCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastClippedCommandCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastClippedCommandCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastClippedCommandCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastClippedCommandCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastClippedCommandCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastClippedTriangleCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastClippedTriangleCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastClippedTriangleCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastClippedTriangleCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastClippedTriangleCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastClippedTriangleCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastCulledCommandCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCulledCommandCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCulledCommandCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastCulledCommandCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastCulledCommandCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastCulledCommandCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastStencilCommandCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilCommandCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilCommandCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastStencilCommandCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastStencilCommandCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastStencilCommandCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastStencilTriangleCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilTriangleCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilTriangleCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastStencilTriangleCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastStencilTriangleCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastStencilTriangleCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastCpuClipSourceTriangleCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCpuClipSourceTriangleCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCpuClipSourceTriangleCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastCpuClipSourceTriangleCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastCpuClipSourceTriangleCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastCpuClipSourceTriangleCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastCpuClipOutputTriangleCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCpuClipOutputTriangleCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCpuClipOutputTriangleCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastCpuClipOutputTriangleCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastCpuClipOutputTriangleCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastCpuClipOutputTriangleCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastCpuClipFragmentCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCpuClipFragmentCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCpuClipFragmentCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastCpuClipFragmentCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastCpuClipFragmentCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastCpuClipFragmentCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastStencilClipScopeCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilClipScopeCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilClipScopeCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastStencilClipScopeCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastStencilClipScopeCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastStencilClipScopeCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastStencilClipPolygonCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilClipPolygonCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilClipPolygonCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastStencilClipPolygonCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastStencilClipPolygonCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastStencilClipPolygonCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastCustomCommandCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCustomCommandCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCustomCommandCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastCustomCommandCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastCustomCommandCount();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastCustomCommandCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastMaxBatchTriangles of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastMaxBatchTriangles00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastMaxBatchTriangles00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastMaxBatchTriangles'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastMaxBatchTriangles();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastMaxBatchTriangles'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetLastMaxBatchVertices of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetLastMaxBatchVertices00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetLastMaxBatchVertices00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetLastMaxBatchVertices'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetLastMaxBatchVertices();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetLastMaxBatchVertices'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: IsHardwareStencilSupported of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_IsHardwareStencilSupported00
static int tolua_RuntimeToLua_FairyGuiLuaApi_IsHardwareStencilSupported00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'IsHardwareStencilSupported'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->IsHardwareStencilSupported();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'IsHardwareStencilSupported'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStencilBackendString of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetStencilBackendString00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetStencilBackendString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStencilBackendString'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetStencilBackendString();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStencilBackendString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetStencilBackendDetailString of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetStencilBackendDetailString00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetStencilBackendDetailString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetStencilBackendDetailString'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetStencilBackendDetailString();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetStencilBackendDetailString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetMaterialDetailString of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetMaterialDetailString00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetMaterialDetailString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetMaterialDetailString'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetMaterialDetailString();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetMaterialDetailString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetTextureDetailString of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetTextureDetailString00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetTextureDetailString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetTextureDetailString'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetTextureDetailString();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetTextureDetailString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFrameRenderDetailString of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetFrameRenderDetailString00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetFrameRenderDetailString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFrameRenderDetailString'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetFrameRenderDetailString();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFrameRenderDetailString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PlotServiceStats of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_PlotServiceStats00
static int tolua_RuntimeToLua_FairyGuiLuaApi_PlotServiceStats00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,7,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,8,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,9,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,10,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int serviceOpenTotal = ((int)  tolua_tonumber(tolua_S,2,0));
  int serviceKindCount = ((int)  tolua_tonumber(tolua_S,3,0));
  int toastQueueCount = ((int)  tolua_tonumber(tolua_S,4,0));
  int loadingRefTotal = ((int)  tolua_tonumber(tolua_S,5,0));
  int serviceCreatedTotal = ((int)  tolua_tonumber(tolua_S,6,0));
  int serviceClosedTotal = ((int)  tolua_tonumber(tolua_S,7,0));
  int serviceFailedTotal = ((int)  tolua_tonumber(tolua_S,8,0));
  int servicePeakOpen = ((int)  tolua_tonumber(tolua_S,9,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PlotServiceStats'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->PlotServiceStats(serviceOpenTotal,serviceKindCount,toastQueueCount,loadingRefTotal,serviceCreatedTotal,serviceClosedTotal,serviceFailedTotal,servicePeakOpen);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PlotServiceStats'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetChild of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetChild00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetChild00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* childPath = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetChild'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetChild(objectHandle,childPath);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetChild'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetListItem of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetListItem00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetListItem00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  int itemIndex = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetListItem'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetListItem(objectHandle,itemIndex);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetListItem'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetListItemCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetListItemCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetListItemCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetListItemCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetListItemCount(objectHandle);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetListItemCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddObjectToRoot of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_AddObjectToRoot00
static int tolua_RuntimeToLua_FairyGuiLuaApi_AddObjectToRoot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddObjectToRoot'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddObjectToRoot(objectHandle);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddObjectToRoot'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddObjectToParent of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_AddObjectToParent00
static int tolua_RuntimeToLua_FairyGuiLuaApi_AddObjectToParent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  int parentHandle = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddObjectToParent'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->AddObjectToParent(objectHandle,parentHandle);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddObjectToParent'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectPosition of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectPosition00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectPosition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  float x = ((float)  tolua_tonumber(tolua_S,3,0));
  float y = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectPosition'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectPosition(objectHandle,x,y);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectPosition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectSize of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectSize00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectSize00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  float width = ((float)  tolua_tonumber(tolua_S,3,0));
  float height = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectSize'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectSize(objectHandle,width,height);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectSize'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectVisible of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectVisible00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectVisible00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  bool visible = ((bool)  tolua_toboolean(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectVisible'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectVisible(objectHandle,visible);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectVisible'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectAlpha of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectAlpha00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectAlpha00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  float alpha = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectAlpha'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectAlpha(objectHandle,alpha);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectAlpha'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectTouchable of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectTouchable00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectTouchable00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  bool touchable = ((bool)  tolua_toboolean(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectTouchable'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectTouchable(objectHandle,touchable);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectTouchable'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectMask of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectMask00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectMask00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  int maskHandle = ((int)  tolua_tonumber(tolua_S,3,0));
  bool inverted = ((bool)  tolua_toboolean(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectMask'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectMask(objectHandle,maskHandle,inverted);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectMask'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectSortingOrder of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectSortingOrder00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectSortingOrder00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  int sortingOrder = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectSortingOrder'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectSortingOrder(objectHandle,sortingOrder);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectSortingOrder'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectText of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectText00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* text = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectText'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectText(objectHandle,text);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectText'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectText of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectText00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectText'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetObjectText(objectHandle);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectText'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectIcon of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectIcon00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectIcon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* icon = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectIcon'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectIcon(objectHandle,icon);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectIcon'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectLoaderUrl of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectLoaderUrl00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectLoaderUrl00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* url = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectLoaderUrl'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectLoaderUrl(objectHandle,url);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectLoaderUrl'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectControllerIndex of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectControllerIndex00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectControllerIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* controllerName = ((const char*)  tolua_tostring(tolua_S,3,0));
  int selectedIndex = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectControllerIndex'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectControllerIndex(objectHandle,controllerName,selectedIndex);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectControllerIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectControllerIndex of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerIndex00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* controllerName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectControllerIndex'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetObjectControllerIndex(objectHandle,controllerName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectControllerIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectControllerPage of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectControllerPage00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectControllerPage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isstring(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* controllerName = ((const char*)  tolua_tostring(tolua_S,3,0));
  const char* pageName = ((const char*)  tolua_tostring(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectControllerPage'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectControllerPage(objectHandle,controllerName,pageName);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectControllerPage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectControllerPage of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPage00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* controllerName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectControllerPage'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetObjectControllerPage(objectHandle,controllerName);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectControllerPage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectControllerPageId of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageId00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* controllerName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectControllerPageId'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetObjectControllerPageId(objectHandle,controllerName);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectControllerPageId'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectControllerPageCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* controllerName = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectControllerPageCount'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetObjectControllerPageCount(objectHandle,controllerName);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectControllerPageCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectControllerPageNameAt of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageNameAt00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageNameAt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* controllerName = ((const char*)  tolua_tostring(tolua_S,3,0));
  int pageIndex = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectControllerPageNameAt'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetObjectControllerPageNameAt(objectHandle,controllerName,pageIndex);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectControllerPageNameAt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectControllerPageIdAt of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageIdAt00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageIdAt00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* controllerName = ((const char*)  tolua_tostring(tolua_S,3,0));
  int pageIndex = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectControllerPageIdAt'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetObjectControllerPageIdAt(objectHandle,controllerName,pageIndex);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectControllerPageIdAt'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectValue of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectValue00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  float value = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectValue'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectValue(objectHandle,value);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectValue of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectValue00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectValue'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetObjectValue(objectHandle);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectMin of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectMin00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectMin00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  float minValue = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectMin'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectMin(objectHandle,minValue);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectMin'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectMin of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectMin00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectMin00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectMin'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetObjectMin(objectHandle);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectMin'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetObjectMax of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectMax00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectMax00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  float maxValue = ((float)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetObjectMax'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetObjectMax(objectHandle,maxValue);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetObjectMax'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetObjectMax of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectMax00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectMax00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetObjectMax'", NULL);
#endif
  {
   float tolua_ret = (float)  self->GetObjectMax(objectHandle);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetObjectMax'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetComboBoxSelectedIndex of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetComboBoxSelectedIndex00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetComboBoxSelectedIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  int selectedIndex = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetComboBoxSelectedIndex'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetComboBoxSelectedIndex(objectHandle,selectedIndex);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetComboBoxSelectedIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetComboBoxSelectedIndex of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetComboBoxSelectedIndex00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetComboBoxSelectedIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetComboBoxSelectedIndex'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetComboBoxSelectedIndex(objectHandle);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetComboBoxSelectedIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetComboBoxValue of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetComboBoxValue00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetComboBoxValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* value = ((const char*)  tolua_tostring(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetComboBoxValue'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetComboBoxValue(objectHandle,value);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetComboBoxValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetComboBoxValue of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetComboBoxValue00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetComboBoxValue00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetComboBoxValue'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetComboBoxValue(objectHandle);
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetComboBoxValue'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: PlayTransition of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_PlayTransition00
static int tolua_RuntimeToLua_FairyGuiLuaApi_PlayTransition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,6,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,7,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* transitionName = ((const char*)  tolua_tostring(tolua_S,3,0));
  int times = ((int)  tolua_tonumber(tolua_S,4,0));
  float delay = ((float)  tolua_tonumber(tolua_S,5,0));
  int callbackId = ((int)  tolua_tonumber(tolua_S,6,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'PlayTransition'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->PlayTransition(objectHandle,transitionName,times,delay,callbackId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'PlayTransition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: StopTransition of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_StopTransition00
static int tolua_RuntimeToLua_FairyGuiLuaApi_StopTransition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,4,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* transitionName = ((const char*)  tolua_tostring(tolua_S,3,0));
  bool setToComplete = ((bool)  tolua_toboolean(tolua_S,4,0));
  bool processCallback = ((bool)  tolua_toboolean(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'StopTransition'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->StopTransition(objectHandle,transitionName,setToComplete,processCallback);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'StopTransition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: FocusObject of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_FocusObject00
static int tolua_RuntimeToLua_FairyGuiLuaApi_FocusObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'FocusObject'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->FocusObject(objectHandle);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'FocusObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearFocus of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_ClearFocus00
static int tolua_RuntimeToLua_FairyGuiLuaApi_ClearFocus00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearFocus'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ClearFocus();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearFocus'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetFocusedObject of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetFocusedObject00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetFocusedObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetFocusedObject'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetFocusedObject();
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetFocusedObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetListItemCount of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetListItemCount00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetListItemCount00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  int itemCount = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetListItemCount'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetListItemCount(objectHandle,itemCount);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetListItemCount'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetListSelectedIndex of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetListSelectedIndex00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetListSelectedIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  int selectedIndex = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetListSelectedIndex'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetListSelectedIndex(objectHandle,selectedIndex);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetListSelectedIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetListSelectedIndex of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetListSelectedIndex00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetListSelectedIndex00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetListSelectedIndex'", NULL);
#endif
  {
   int tolua_ret = (int)  self->GetListSelectedIndex(objectHandle);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetListSelectedIndex'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: SetListVirtual of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_SetListVirtual00
static int tolua_RuntimeToLua_FairyGuiLuaApi_SetListVirtual00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  bool loop = ((bool)  tolua_toboolean(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'SetListVirtual'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->SetListVirtual(objectHandle,loop);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'SetListVirtual'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RefreshList of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_RefreshList00
static int tolua_RuntimeToLua_FairyGuiLuaApi_RefreshList00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RefreshList'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RefreshList(objectHandle);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RefreshList'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ScrollListToView of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_ScrollListToView00
static int tolua_RuntimeToLua_FairyGuiLuaApi_ScrollListToView00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  int itemIndex = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ScrollListToView'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ScrollListToView(objectHandle,itemIndex);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ScrollListToView'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CenterObject of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_CenterObject00
static int tolua_RuntimeToLua_FairyGuiLuaApi_CenterObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isboolean(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  bool restraint = ((bool)  tolua_toboolean(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CenterObject'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->CenterObject(objectHandle,restraint);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CenterObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectMouseMove of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseMove00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseMove00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectMouseMove'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectMouseMove(x,y);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectMouseMove'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectMouseDown of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseDown00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseDown00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
  int button = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectMouseDown'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectMouseDown(x,y,button);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectMouseDown'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectMouseUp of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseUp00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseUp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
  int button = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectMouseUp'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectMouseUp(x,y,button);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectMouseUp'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectMouseWheel of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseWheel00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseWheel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
  int wheelDelta = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectMouseWheel'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectMouseWheel(x,y,wheelDelta);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectMouseWheel'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectLogicalMouseMove of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseMove00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseMove00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectLogicalMouseMove'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectLogicalMouseMove(x,y);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectLogicalMouseMove'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectLogicalMouseDown of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseDown00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseDown00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
  int button = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectLogicalMouseDown'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectLogicalMouseDown(x,y,button);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectLogicalMouseDown'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectLogicalMouseUp of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseUp00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseUp00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
  int button = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectLogicalMouseUp'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectLogicalMouseUp(x,y,button);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectLogicalMouseUp'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectLogicalMouseWheel of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseWheel00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseWheel00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
  int wheelDelta = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectLogicalMouseWheel'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectLogicalMouseWheel(x,y,wheelDelta);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectLogicalMouseWheel'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectLogicalClick of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalClick00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalClick00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int x = ((int)  tolua_tonumber(tolua_S,2,0));
  int y = ((int)  tolua_tonumber(tolua_S,3,0));
  int button = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectLogicalClick'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectLogicalClick(x,y,button);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectLogicalClick'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectKeyPressed of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectKeyPressed00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectKeyPressed00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int keyCode = ((int)  tolua_tonumber(tolua_S,2,0));
  int keyText = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectKeyPressed'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectKeyPressed(keyCode,keyText);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectKeyPressed'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectKeyReleased of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectKeyReleased00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectKeyReleased00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,4,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int keyCode = ((int)  tolua_tonumber(tolua_S,2,0));
  int keyText = ((int)  tolua_tonumber(tolua_S,3,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectKeyReleased'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectKeyReleased(keyCode,keyText);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectKeyReleased'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectImeCompositionText of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectImeCompositionText00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectImeCompositionText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  const char* text = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectImeCompositionText'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectImeCompositionText(text);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectImeCompositionText'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: InjectImeCommitText of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_InjectImeCommitText00
static int tolua_RuntimeToLua_FairyGuiLuaApi_InjectImeCommitText00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  const char* text = ((const char*)  tolua_tostring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'InjectImeCommitText'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->InjectImeCommitText(text);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'InjectImeCommitText'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearImeComposition of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_ClearImeComposition00
static int tolua_RuntimeToLua_FairyGuiLuaApi_ClearImeComposition00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearImeComposition'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->ClearImeComposition();
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearImeComposition'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: GetImeDebugString of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_GetImeDebugString00
static int tolua_RuntimeToLua_FairyGuiLuaApi_GetImeDebugString00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  FairyGuiLuaApi* self = (FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'GetImeDebugString'", NULL);
#endif
  {
   const char* tolua_ret = (const char*)  self->GetImeDebugString();
   tolua_pushstring(tolua_S,(const char*)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'GetImeDebugString'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddEventListener of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_AddEventListener00
static int tolua_RuntimeToLua_FairyGuiLuaApi_AddEventListener00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* childPath = ((const char*)  tolua_tostring(tolua_S,3,0));
  int eventType = ((int)  tolua_tonumber(tolua_S,4,0));
  int callbackId = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddEventListener'", NULL);
#endif
  {
   int tolua_ret = (int)  self->AddEventListener(objectHandle,childPath,eventType,callbackId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddEventListener'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddClickListener of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_AddClickListener00
static int tolua_RuntimeToLua_FairyGuiLuaApi_AddClickListener00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* childPath = ((const char*)  tolua_tostring(tolua_S,3,0));
  int callbackId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddClickListener'", NULL);
#endif
  {
   int tolua_ret = (int)  self->AddClickListener(objectHandle,childPath,callbackId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddClickListener'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: AddControllerChangedListener of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_AddControllerChangedListener00
static int tolua_RuntimeToLua_FairyGuiLuaApi_AddControllerChangedListener00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isstring(tolua_S,3,0,&tolua_err) ||
     !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
  const char* controllerName = ((const char*)  tolua_tostring(tolua_S,3,0));
  int callbackId = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'AddControllerChangedListener'", NULL);
#endif
  {
   int tolua_ret = (int)  self->AddControllerChangedListener(objectHandle,controllerName,callbackId);
   tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'AddControllerChangedListener'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RemoveListener of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_RemoveListener00
static int tolua_RuntimeToLua_FairyGuiLuaApi_RemoveListener00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int bindingId = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RemoveListener'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RemoveListener(bindingId);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RemoveListener'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: RemoveObject of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_RemoveObject00
static int tolua_RuntimeToLua_FairyGuiLuaApi_RemoveObject00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
  int objectHandle = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'RemoveObject'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->RemoveObject(objectHandle);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'RemoveObject'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ClearObjects of class  FairyGuiLuaApi */
#ifndef TOLUA_DISABLE_tolua_RuntimeToLua_FairyGuiLuaApi_ClearObjects00
static int tolua_RuntimeToLua_FairyGuiLuaApi_ClearObjects00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"const FairyGuiLuaApi",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  const FairyGuiLuaApi* self = (const FairyGuiLuaApi*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ClearObjects'", NULL);
#endif
  {
   self->ClearObjects();
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ClearObjects'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_RuntimeToLua_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"FairyGuiLuaApi","FairyGuiLuaApi","",NULL);
  tolua_beginmodule(tolua_S,"FairyGuiLuaApi");
   tolua_function(tolua_S,"IsAvailable",tolua_RuntimeToLua_FairyGuiLuaApi_IsAvailable00);
   tolua_function(tolua_S,"LoadPackage",tolua_RuntimeToLua_FairyGuiLuaApi_LoadPackage00);
   tolua_function(tolua_S,"RemovePackage",tolua_RuntimeToLua_FairyGuiLuaApi_RemovePackage00);
   tolua_function(tolua_S,"CreateObject",tolua_RuntimeToLua_FairyGuiLuaApi_CreateObject00);
   tolua_function(tolua_S,"CreateContainer",tolua_RuntimeToLua_FairyGuiLuaApi_CreateContainer00);
   tolua_function(tolua_S,"CreateChildContainer",tolua_RuntimeToLua_FairyGuiLuaApi_CreateChildContainer00);
   tolua_function(tolua_S,"CreateLoader",tolua_RuntimeToLua_FairyGuiLuaApi_CreateLoader00);
   tolua_function(tolua_S,"CreateText",tolua_RuntimeToLua_FairyGuiLuaApi_CreateText00);
   tolua_function(tolua_S,"CreateTextInput",tolua_RuntimeToLua_FairyGuiLuaApi_CreateTextInput00);
   tolua_function(tolua_S,"CreateGraphRect",tolua_RuntimeToLua_FairyGuiLuaApi_CreateGraphRect00);
   tolua_function(tolua_S,"CreateGraphRegularPolygon",tolua_RuntimeToLua_FairyGuiLuaApi_CreateGraphRegularPolygon00);
   tolua_function(tolua_S,"CreateModalMask",tolua_RuntimeToLua_FairyGuiLuaApi_CreateModalMask00);
   tolua_function(tolua_S,"GetLastRenderCommandCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastRenderCommandCount00);
   tolua_function(tolua_S,"GetLastTriangleCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastTriangleCount00);
   tolua_function(tolua_S,"GetScreenWidth",tolua_RuntimeToLua_FairyGuiLuaApi_GetScreenWidth00);
   tolua_function(tolua_S,"GetScreenHeight",tolua_RuntimeToLua_FairyGuiLuaApi_GetScreenHeight00);
   tolua_function(tolua_S,"GetRuntimeObjectHandleCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetRuntimeObjectHandleCount00);
   tolua_function(tolua_S,"GetRuntimeListenerBindingCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetRuntimeListenerBindingCount00);
   tolua_function(tolua_S,"GetMaterialCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetMaterialCount00);
   tolua_function(tolua_S,"GetTextureCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetTextureCount00);
   tolua_function(tolua_S,"GetMaterialAliasCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetMaterialAliasCount00);
   tolua_function(tolua_S,"GetTextureAliasCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetTextureAliasCount00);
   tolua_function(tolua_S,"GetLastDrawCommandCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastDrawCommandCount00);
   tolua_function(tolua_S,"GetLastDrawTriangleCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastDrawTriangleCount00);
   tolua_function(tolua_S,"GetLastMaterialSwitchCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastMaterialSwitchCount00);
   tolua_function(tolua_S,"GetLastTextureSwitchCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastTextureSwitchCount00);
   tolua_function(tolua_S,"GetLastClippedCommandCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastClippedCommandCount00);
   tolua_function(tolua_S,"GetLastClippedTriangleCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastClippedTriangleCount00);
   tolua_function(tolua_S,"GetLastCulledCommandCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCulledCommandCount00);
   tolua_function(tolua_S,"GetLastStencilCommandCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilCommandCount00);
   tolua_function(tolua_S,"GetLastStencilTriangleCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilTriangleCount00);
   tolua_function(tolua_S,"GetLastCpuClipSourceTriangleCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCpuClipSourceTriangleCount00);
   tolua_function(tolua_S,"GetLastCpuClipOutputTriangleCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCpuClipOutputTriangleCount00);
   tolua_function(tolua_S,"GetLastCpuClipFragmentCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCpuClipFragmentCount00);
   tolua_function(tolua_S,"GetLastStencilClipScopeCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilClipScopeCount00);
   tolua_function(tolua_S,"GetLastStencilClipPolygonCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastStencilClipPolygonCount00);
   tolua_function(tolua_S,"GetLastCustomCommandCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastCustomCommandCount00);
   tolua_function(tolua_S,"GetLastMaxBatchTriangles",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastMaxBatchTriangles00);
   tolua_function(tolua_S,"GetLastMaxBatchVertices",tolua_RuntimeToLua_FairyGuiLuaApi_GetLastMaxBatchVertices00);
   tolua_function(tolua_S,"IsHardwareStencilSupported",tolua_RuntimeToLua_FairyGuiLuaApi_IsHardwareStencilSupported00);
   tolua_function(tolua_S,"GetStencilBackendString",tolua_RuntimeToLua_FairyGuiLuaApi_GetStencilBackendString00);
   tolua_function(tolua_S,"GetStencilBackendDetailString",tolua_RuntimeToLua_FairyGuiLuaApi_GetStencilBackendDetailString00);
   tolua_function(tolua_S,"GetMaterialDetailString",tolua_RuntimeToLua_FairyGuiLuaApi_GetMaterialDetailString00);
   tolua_function(tolua_S,"GetTextureDetailString",tolua_RuntimeToLua_FairyGuiLuaApi_GetTextureDetailString00);
   tolua_function(tolua_S,"GetFrameRenderDetailString",tolua_RuntimeToLua_FairyGuiLuaApi_GetFrameRenderDetailString00);
   tolua_function(tolua_S,"PlotServiceStats",tolua_RuntimeToLua_FairyGuiLuaApi_PlotServiceStats00);
   tolua_function(tolua_S,"GetChild",tolua_RuntimeToLua_FairyGuiLuaApi_GetChild00);
   tolua_function(tolua_S,"GetListItem",tolua_RuntimeToLua_FairyGuiLuaApi_GetListItem00);
   tolua_function(tolua_S,"GetListItemCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetListItemCount00);
   tolua_function(tolua_S,"AddObjectToRoot",tolua_RuntimeToLua_FairyGuiLuaApi_AddObjectToRoot00);
   tolua_function(tolua_S,"AddObjectToParent",tolua_RuntimeToLua_FairyGuiLuaApi_AddObjectToParent00);
   tolua_function(tolua_S,"SetObjectPosition",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectPosition00);
   tolua_function(tolua_S,"SetObjectSize",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectSize00);
   tolua_function(tolua_S,"SetObjectVisible",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectVisible00);
   tolua_function(tolua_S,"SetObjectAlpha",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectAlpha00);
   tolua_function(tolua_S,"SetObjectTouchable",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectTouchable00);
   tolua_function(tolua_S,"SetObjectMask",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectMask00);
   tolua_function(tolua_S,"SetObjectSortingOrder",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectSortingOrder00);
   tolua_function(tolua_S,"SetObjectText",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectText00);
   tolua_function(tolua_S,"GetObjectText",tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectText00);
   tolua_function(tolua_S,"SetObjectIcon",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectIcon00);
   tolua_function(tolua_S,"SetObjectLoaderUrl",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectLoaderUrl00);
   tolua_function(tolua_S,"SetObjectControllerIndex",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectControllerIndex00);
   tolua_function(tolua_S,"GetObjectControllerIndex",tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerIndex00);
   tolua_function(tolua_S,"SetObjectControllerPage",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectControllerPage00);
   tolua_function(tolua_S,"GetObjectControllerPage",tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPage00);
   tolua_function(tolua_S,"GetObjectControllerPageId",tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageId00);
   tolua_function(tolua_S,"GetObjectControllerPageCount",tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageCount00);
   tolua_function(tolua_S,"GetObjectControllerPageNameAt",tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageNameAt00);
   tolua_function(tolua_S,"GetObjectControllerPageIdAt",tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectControllerPageIdAt00);
   tolua_function(tolua_S,"SetObjectValue",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectValue00);
   tolua_function(tolua_S,"GetObjectValue",tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectValue00);
   tolua_function(tolua_S,"SetObjectMin",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectMin00);
   tolua_function(tolua_S,"GetObjectMin",tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectMin00);
   tolua_function(tolua_S,"SetObjectMax",tolua_RuntimeToLua_FairyGuiLuaApi_SetObjectMax00);
   tolua_function(tolua_S,"GetObjectMax",tolua_RuntimeToLua_FairyGuiLuaApi_GetObjectMax00);
   tolua_function(tolua_S,"SetComboBoxSelectedIndex",tolua_RuntimeToLua_FairyGuiLuaApi_SetComboBoxSelectedIndex00);
   tolua_function(tolua_S,"GetComboBoxSelectedIndex",tolua_RuntimeToLua_FairyGuiLuaApi_GetComboBoxSelectedIndex00);
   tolua_function(tolua_S,"SetComboBoxValue",tolua_RuntimeToLua_FairyGuiLuaApi_SetComboBoxValue00);
   tolua_function(tolua_S,"GetComboBoxValue",tolua_RuntimeToLua_FairyGuiLuaApi_GetComboBoxValue00);
   tolua_function(tolua_S,"PlayTransition",tolua_RuntimeToLua_FairyGuiLuaApi_PlayTransition00);
   tolua_function(tolua_S,"StopTransition",tolua_RuntimeToLua_FairyGuiLuaApi_StopTransition00);
   tolua_function(tolua_S,"FocusObject",tolua_RuntimeToLua_FairyGuiLuaApi_FocusObject00);
   tolua_function(tolua_S,"ClearFocus",tolua_RuntimeToLua_FairyGuiLuaApi_ClearFocus00);
   tolua_function(tolua_S,"GetFocusedObject",tolua_RuntimeToLua_FairyGuiLuaApi_GetFocusedObject00);
   tolua_function(tolua_S,"SetListItemCount",tolua_RuntimeToLua_FairyGuiLuaApi_SetListItemCount00);
   tolua_function(tolua_S,"SetListSelectedIndex",tolua_RuntimeToLua_FairyGuiLuaApi_SetListSelectedIndex00);
   tolua_function(tolua_S,"GetListSelectedIndex",tolua_RuntimeToLua_FairyGuiLuaApi_GetListSelectedIndex00);
   tolua_function(tolua_S,"SetListVirtual",tolua_RuntimeToLua_FairyGuiLuaApi_SetListVirtual00);
   tolua_function(tolua_S,"RefreshList",tolua_RuntimeToLua_FairyGuiLuaApi_RefreshList00);
   tolua_function(tolua_S,"ScrollListToView",tolua_RuntimeToLua_FairyGuiLuaApi_ScrollListToView00);
   tolua_function(tolua_S,"CenterObject",tolua_RuntimeToLua_FairyGuiLuaApi_CenterObject00);
   tolua_function(tolua_S,"InjectMouseMove",tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseMove00);
   tolua_function(tolua_S,"InjectMouseDown",tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseDown00);
   tolua_function(tolua_S,"InjectMouseUp",tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseUp00);
   tolua_function(tolua_S,"InjectMouseWheel",tolua_RuntimeToLua_FairyGuiLuaApi_InjectMouseWheel00);
   tolua_function(tolua_S,"InjectLogicalMouseMove",tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseMove00);
   tolua_function(tolua_S,"InjectLogicalMouseDown",tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseDown00);
   tolua_function(tolua_S,"InjectLogicalMouseUp",tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseUp00);
   tolua_function(tolua_S,"InjectLogicalMouseWheel",tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalMouseWheel00);
   tolua_function(tolua_S,"InjectLogicalClick",tolua_RuntimeToLua_FairyGuiLuaApi_InjectLogicalClick00);
   tolua_function(tolua_S,"InjectKeyPressed",tolua_RuntimeToLua_FairyGuiLuaApi_InjectKeyPressed00);
   tolua_function(tolua_S,"InjectKeyReleased",tolua_RuntimeToLua_FairyGuiLuaApi_InjectKeyReleased00);
   tolua_function(tolua_S,"InjectImeCompositionText",tolua_RuntimeToLua_FairyGuiLuaApi_InjectImeCompositionText00);
   tolua_function(tolua_S,"InjectImeCommitText",tolua_RuntimeToLua_FairyGuiLuaApi_InjectImeCommitText00);
   tolua_function(tolua_S,"ClearImeComposition",tolua_RuntimeToLua_FairyGuiLuaApi_ClearImeComposition00);
   tolua_function(tolua_S,"GetImeDebugString",tolua_RuntimeToLua_FairyGuiLuaApi_GetImeDebugString00);
   tolua_function(tolua_S,"AddEventListener",tolua_RuntimeToLua_FairyGuiLuaApi_AddEventListener00);
   tolua_function(tolua_S,"AddClickListener",tolua_RuntimeToLua_FairyGuiLuaApi_AddClickListener00);
   tolua_function(tolua_S,"AddControllerChangedListener",tolua_RuntimeToLua_FairyGuiLuaApi_AddControllerChangedListener00);
   tolua_function(tolua_S,"RemoveListener",tolua_RuntimeToLua_FairyGuiLuaApi_RemoveListener00);
   tolua_function(tolua_S,"RemoveObject",tolua_RuntimeToLua_FairyGuiLuaApi_RemoveObject00);
   tolua_function(tolua_S,"ClearObjects",tolua_RuntimeToLua_FairyGuiLuaApi_ClearObjects00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_RuntimeToLua (lua_State* tolua_S) {
 return tolua_RuntimeToLua_open(tolua_S);
};
#endif

