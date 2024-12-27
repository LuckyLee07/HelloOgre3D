#ifndef __BASE_OBJECT__
#define __BASE_OBJECT__

class BaseObject //tolua_exports
{ //tolua_exports
public:
	//tolua_begin
	enum OBJTYPE
	{
		OBJ_TYPE_NONE = 0,
		OBJ_TYPE_ENTITY,
		OBJ_TYPE_BLOCK,
		OBJ_TYPE_PLANE,
		OBJ_TYPE_AGENT,
		OBJ_TYPE_SOLDIER,
		OBJ_TYPE_UIOBJ,

		OBJ_TYPE_MAX,
	};
	//tolua_end
public:
	//tolua_begin
	BaseObject();
	virtual ~BaseObject();

	void setObjId(unsigned int objId);
	unsigned int getObjId();
	
	void setObjType(OBJTYPE objType);
	OBJTYPE getObjType();
	//tolua_end

	virtual void update(int deltaMsec);

private:
	OBJTYPE m_objType;
	unsigned int m_objId;
}; //tolua_exports


#endif  // __BASE_OBJECT__
