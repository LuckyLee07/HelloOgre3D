#include "PhysicsFactory.h"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#include "SceneFactory.h"

btRigidBody* PhysicsFactory::CreateRigidBodyBox(Ogre::Mesh* meshPtr, const btScalar btmass)
{
	const btVector3 position(0, 0, 0);
	const btScalar mass = btmass;

	// 从网格数据创建一个简化的凸包形状。
	btConvexHullShape * hullShape = CreateSimplifiedConvexHull(meshPtr);

	// 定义AABB（轴对齐边界盒）的最小和最大点。
	btVector3 aabbMin;
	btVector3 aabbMax;

	// 计算凸包形状的AABB。
	hullShape->getAabb(
		btTransform(btQuaternion::getIdentity()), aabbMin, aabbMax);

	// 计算质心，将其设为AABB的中心。适用于对称物体，非对称物体可能出现问题。
	const btVector3 centerOfMass = (aabbMax + aabbMin) / 2.0f;

	// 创建一个复合形状用于组合多个形状，这里暂时只包含一个形状。
	btCompoundShape* compoundShape = new btCompoundShape();

	// 为了修正质心偏移问题的代码被注释掉了，该代码可以改变形状的图形表示。
	/*
	compoundShape->addChildShape(
		btTransform(btQuaternion::getIdentity(), -centerOfMass),
		hullShape);
	*/

	// 添加形状到复合形状中，未调整质心位置。
	compoundShape->addChildShape(
		btTransform(btQuaternion::getIdentity()), hullShape);

	// 创建运动状态对象，包含物体的初始位置。
	btDefaultMotionState* const motionState = new btDefaultMotionState(
		btTransform(btQuaternion::getIdentity(), position));

	// 初始化局部惯性为零，它将被计算基于提供的质量。
	btVector3 localInertia(0, 0, 0);
	// 计算复合形状的局部惯性。
	compoundShape->calculateLocalInertia(mass, localInertia);

	// 构造刚体信息，包括质量、运动状态、复合形状和局部惯性。
	btRigidBody::btRigidBodyConstructionInfo
		rigidBodyCI(mass, motionState, compoundShape, localInertia);

	// 设定刚体的线性休眠阈值，影响刚体在何时进入休眠状态。
	rigidBodyCI.m_linearSleepingThreshold = 0.3f;

	// 创建刚体对象。
	btRigidBody* const rigidBody = new btRigidBody(rigidBodyCI);

	// 设置连续碰撞检测（CCD）的阈值，用于处理高速运动的物体。
	rigidBody->setCcdMotionThreshold(0.5f);

	// 设置CCD用的扫掠球体半径，基于AABB最大长度的一半。
	rigidBody->setCcdSweptSphereRadius(aabbMax.length() / 2.0f);

	// 返回创建的刚体。
	return rigidBody;
}

btRigidBody* PhysicsFactory::CreateRigidBodyBox(Ogre::Real width, Ogre::Real height, Ogre::Real length)
{
	// 创建一个盒子形状
	btBoxShape* const boxShape = new btBoxShape(btVector3(width / 2, height / 2, length / 2));

	// 创建一个默认的运动状态对象，初始时没有旋转和位移。
	btDefaultMotionState* capsuleMotionState = new btDefaultMotionState();

	// 初始化局部惯性向量，开始时设置为0。局部惯性将基于质量计算，决定物体的旋转惯性。
	btVector3 localInertia(0, 0, 0);
	// 计算胶囊形状的局部惯性，假设胶囊的质量为1.0f。
	boxShape->calculateLocalInertia(1.0f, localInertia);

	// 创建刚体的构造信息，包括质量（1.0f表示不是静态的）、运动状态、形状和局部惯性。
	btRigidBody::btRigidBodyConstructionInfo capsuleRigidBodyCI(1.0f, capsuleMotionState, boxShape, localInertia);

	// 使用上述构造信息创建刚体对象。
	btRigidBody* const rigidBody = new btRigidBody(capsuleRigidBodyCI);

	// 设置连续碰撞检测（CCD）的阈值和半径，用于处理高速移动的物体可能产生的穿透问题。
	// CCD阈值设为0.5f，CCD扫掠球半径设置为胶囊的半径。
	rigidBody->setCcdMotionThreshold(0.5f);
	rigidBody->setCcdSweptSphereRadius(width / 2.0f);

	return rigidBody;
}

btRigidBody* PhysicsFactory::CreateRigidBodyPlane(const btVector3& normal, const btScalar originOffset)
{
	btCollisionShape* const groundShape = new btStaticPlaneShape(normal, originOffset);

	btDefaultMotionState* const groundMotionState = new btDefaultMotionState();

	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));

	groundRigidBodyCI.m_rollingFriction = 0.1f;

	// 创建刚体对象
	btRigidBody* pRigidBody = new btRigidBody(groundRigidBodyCI);
	return pRigidBody;
}

btRigidBody* PhysicsFactory::CreateRigidBodyCapsule(Ogre::Real height, Ogre::Real radius)
{
	// 创建一个胶囊形状，其中半径是胶囊的圆柱部分的半径，高度减去两倍半径是圆柱部分的高度。
	btCapsuleShape* capsuleShape = new btCapsuleShape(radius, height - radius * 2);

	// 创建一个默认的运动状态对象，初始时没有旋转和位移。
	btDefaultMotionState* capsuleMotionState = new btDefaultMotionState();

	// 初始化局部惯性向量，开始时设置为0。局部惯性将基于质量计算，决定物体的旋转惯性。
	btVector3 localInertia(0, 0, 0);
	// 计算胶囊形状的局部惯性，假设胶囊的质量为1.0f。
	capsuleShape->calculateLocalInertia(1.0f, localInertia);

	// 创建刚体的构造信息，包括质量（1.0f表示不是静态的）、运动状态、形状和局部惯性。
	btRigidBody::btRigidBodyConstructionInfo capsuleRigidBodyCI(1.0f, capsuleMotionState, capsuleShape, localInertia);

	// 设置滚动摩擦，这有助于防止刚体在平面上无限滚动。
	capsuleRigidBodyCI.m_rollingFriction = 0.2f;

	// 使用上述构造信息创建刚体对象。
	btRigidBody* const rigidBody = new btRigidBody(capsuleRigidBodyCI);

	// 设置连续碰撞检测（CCD）的阈值和半径，用于处理高速移动的物体可能产生的穿透问题。
	// CCD阈值设为0.5f，CCD扫掠球半径设置为胶囊的半径。
	rigidBody->setCcdMotionThreshold(0.5f);
	rigidBody->setCcdSweptSphereRadius(radius);

	return rigidBody;
}

btConvexHullShape* PhysicsFactory::CreateSimplifiedConvexHull(Ogre::Mesh* meshPtr)
{
	// 获取 Ogre::Mesh 的顶点数据
	Ogre::Vector3* vertices = nullptr;
	unsigned long* indices = nullptr;
	size_t vertex_count = 0, index_count = 0;

	SceneFactory::GetMeshInfo(meshPtr, vertex_count, vertices, index_count, indices);

	// 使用 Bullet3 提供的凸包计算工具
	btConvexHullShape* hullShape = new btConvexHullShape();
	hullShape->setMargin(0.01f);
	hullShape->setSafeMargin(0.01f);

	// 将 Ogre 的顶点数据转换成 Bullet3 的顶点数据
	for (size_t i = 0; i < vertex_count; ++i) {
		const Ogre::Vector3& vertex = vertices[i];
		hullShape->addPoint(btVector3(vertex.x, vertex.y, vertex.z));
	}

	delete[] vertices;
	delete[] indices;

	// 使用 btShapeHull 进行精细化处理
	btShapeHull* hull = new btShapeHull(hullShape);
	btScalar margin = hullShape->getMargin();
	hull->buildHull(margin);

	// 创建一个新的 btConvexHullShape，以保存精细化后的凸包数据
	btConvexHullShape* simplifiedHullShape = new btConvexHullShape();
	const btVector3* const btVertices = hull->getVertexPointer();
	for (int i = 0; i < hull->numVertices(); ++i) {
		simplifiedHullShape->addPoint(btVertices[i]);
	}
	simplifiedHullShape->setMargin(0.01f);

	// 释放资源
	delete hull;
	delete hullShape;

	return simplifiedHullShape;
}
