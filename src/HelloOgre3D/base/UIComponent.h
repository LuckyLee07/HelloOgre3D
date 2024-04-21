#ifndef __UI_COMPONENT__
#define __UI_COMPONENT__

#include "ogre3d_gorilla/include/Gorilla.h"

class UIComponent //tolua_exports
{ //tolua_exports
public:
	//tolua_begin
	enum OBJTYPE
	{
		OBJ_AGENT = 0,
		OBJ_SANDBOX,
		OBJ_SANDBOX_OBJ,
	};
	//tolua_end
public:
	//tolua_begin
	UIComponent();
	~UIComponent();

	//tolua_end
private:
	
}; //tolua_exports


#endif  // __UI_COMPONENT__
