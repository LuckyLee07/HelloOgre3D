#ifndef __LUA_CLASSNAME_TRAITS__
#define __LUA_CLASSNAME_TRAITS__

template<typename T>
struct LuaClassNameTraits;

#define REGISTER_LUA_CLASS_NAME(CLASS)			\
	template<>									\
	struct LuaClassNameTraits<CLASS>			\
	{											\
		static constexpr const char* value = #CLASS;\
	};											\

#endif  // __LUA_CLASSNAME_TRAITS__
