#include "SandboxMgr.h"
#include "Ogre.h"
#include "Procedural.h"
#include "SandboxDef.h"
#include "ClientManager.h"

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

Ogre::SceneNode* SandboxMgr::CreatePlane(Ogre::Real length, Ogre::Real width)
{
    const Ogre::Real clampedLength = Ogre::Real(std::max(0.0f, length));
    const Ogre::Real clampedWidth = Ogre::Real(std::max(0.0f, width));

    Procedural::PlaneGenerator planeGenerator;
    planeGenerator.setSizeX(clampedLength);
    planeGenerator.setSizeY(clampedWidth);

    // TODO : Accept specifiers for UV tiling.
    planeGenerator.setUTile(clampedLength / 2);
    planeGenerator.setVTile(clampedWidth / 2);

    const Ogre::MeshPtr mesh = planeGenerator.realizeMesh();

    Ogre::SceneNode* const plane = GetClientMgr()->getRootSceneNode()->createChildSceneNode();

    Ogre::Entity* const planeEntity = plane->getCreator()->createEntity(mesh);

    planeEntity->setMaterialName(DEFAULT_MATERIAL);

    plane->attachObject(planeEntity);

    return plane;
}

btRigidBody* SandboxMgr::CreatePlane(const btVector3& normal, const btScalar originOffset)
{
    btCollisionShape* const groundShape = new btStaticPlaneShape(normal, originOffset);

    btDefaultMotionState* const groundMotionState = new btDefaultMotionState();

    btRigidBody::btRigidBodyConstructionInfo
        groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));

    groundRigidBodyCI.m_rollingFriction = 0.1f;

    return new btRigidBody(groundRigidBodyCI);
}

btRigidBody* SandboxMgr::CreateRigidBody(Ogre::Mesh* meshPtr, const btScalar btmass)
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

btConvexHullShape* SandboxMgr::CreateSimplifiedConvexHull(Ogre::Mesh* meshPtr)
{
    // 获取 Ogre::Mesh 的顶点数据
    Ogre::Vector3* vertices = nullptr;
    unsigned long* indices = nullptr;
    size_t vertex_count = 0, index_count = 0;

    GetMeshInfo(meshPtr, vertex_count, vertices, index_count, indices);

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

void SandboxMgr::GetMeshInfo(const Ogre::Mesh* mesh, 
    size_t& vertex_count, Ogre::Vector3*& vertices, size_t& index_count, unsigned long*& indices)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
        // We only need to add the shared vertices once
        if (submesh->useSharedVertices)
        {
            if (!added_shared)
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }
        // add the indices
        index_count += submesh->indexData->indexCount;
    }

    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    //Ogre::HardwareBuffer::LockOptions read_only = Ogre::HardwareBuffer::HBL_READ_ONLY;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ?
            mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) ||
            (submesh->useSharedVertices && !added_shared))
        {
            if (submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an
            // Ogre::Real or a double as second argument. So make it float, to
            // avoid trouble when Ogre::Real will be comiled/typedefed as
            // double:

            // Ogre::Real* pReal;
            float* pReal;

            for (size_t j = 0; j < vertex_data->vertexCount; ++j,
                vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
                vertices[current_offset + j] = pt;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT;

        unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

        if (use32bitindexes)
        {
            for (size_t k = 0; k < numTris * 3; ++k)
            {
                indices[index_offset++] =
                    pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for (size_t k = 0; k < numTris * 3; ++k)
            {
                indices[index_offset++] =
                    static_cast<unsigned long>(pShort[k]) +
                    static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}