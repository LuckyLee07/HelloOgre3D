#ifndef __BASE_OBJECT__
#define __BASE_OBJECT__

class BaseObject //tolua_exports
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
	BaseObject(unsigned int objectId, OBJTYPE objType);
	virtual ~BaseObject();

	unsigned int getObjId();
	OBJTYPE getObjType();
	//tolua_end
private:
	OBJTYPE m_objType;
	unsigned int m_objectId;
}; //tolua_exports


#endif  // __BASE_OBJECT__
