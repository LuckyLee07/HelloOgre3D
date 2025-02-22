#ifndef __MY_RIGIDBODY__
#define __MY_RIGIDBODY__

class BaseObject;
class btRigidBody;
class MyRigidBody
{
public:
	MyRigidBody(BaseObject *pObject);
	~MyRigidBody();

	BaseObject* getOwner();
	btRigidBody* getRigidBody();

private:
	BaseObject* m_pOwnerObj;
}; //tolua_exports


#endif  // __MY_RIGIDBODY__
