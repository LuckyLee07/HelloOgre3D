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
		OBJ_SANDBOX_UI,
		OBJ_SANDBOX_OBJ,
	};
	//tolua_end
public:
	//tolua_begin
	BaseObject(unsigned int objId, OBJTYPE objType);
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
