#include "tolua++.h"
#include "object/AgentObject.h"


/* Exported function */
TOLUA_API int tolua_SandboxToLua_Manual(lua_State* tolua_S);


/* method: setPluginEnv of class  AgentObject */
#ifndef TOLUA_DISABLE_tolua_ClientToLua_AgentObject_setPluginEnv00
static int tolua_ClientToLua_AgentObject_setPluginEnv00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"AgentObject",0,&tolua_err) ||
     !tolua_istable(tolua_S, 2, 0, &tolua_err) ||   // 改为检查索引2是否为 table
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  AgentObject* self = (AgentObject*)  tolua_tousertype(tolua_S,1,0);
  lua_State* L =  tolua_S;
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setPluginEnv'", NULL);
#endif
  {
   self->setPluginEnv(L);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setPluginEnv'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* function to register type */
static void tolua_reg_types(lua_State* tolua_S)
{
    tolua_usertype(tolua_S, "VehicleObject");
	tolua_usertype(tolua_S, "AgentObject");
}

/* open function */
TOLUA_API int tolua_SandboxToLua_Manual(lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  tolua_cclass(tolua_S,"AgentObject","AgentObject","VehicleObject",NULL);
  tolua_beginmodule(tolua_S,"AgentObject");
   tolua_function(tolua_S,"setPluginEnv",tolua_ClientToLua_AgentObject_setPluginEnv00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_SandboxToLua_Manual (lua_State* tolua_S) {
 return tolua_SandboxToLua_Manual(tolua_S);
};
#endif

