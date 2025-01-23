#include "SandboxMgr.h"
#include "Ogre.h"
#include "Procedural.h"
#include "SandboxDef.h"
#include "GameManager.h"
#include "ClientManager.h"
#include "ScriptLuaVM.h"
#include "ObjectManager.h"

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

using namespace Ogre;

SandboxMgr* g_SandboxMgr = nullptr;
Ogre::NameGenerator SandboxMgr::s_nameGenerator("UnnamedParticle_");

SandboxMgr::SandboxMgr(Ogre::SceneManager* sceneManager)
    : m_pSceneManager(sceneManager), m_pRootSceneNode(nullptr)
{
    m_pRootSceneNode = m_pSceneManager->getRootSceneNode();
}

SandboxMgr::~SandboxMgr()
{
    m_pSceneManager = nullptr;
    m_pRootSceneNode = nullptr;
}

SandboxMgr* SandboxMgr::GetInstance()
{
	return g_SandboxMgr;
}

void SandboxMgr::CallFile(const Ogre::String& filepath)
{
    GetScriptLuaVM()->callFile(filepath.c_str());
}

Ogre::Camera* SandboxMgr::GetCamera()
{
    return GetClientMgr()->getCamera();
}

SceneManager* SandboxMgr::GetSceneManager()
{
    return m_pRootSceneNode->getCreator();
}

Ogre::Vector3 SandboxMgr::GetCameraUp()
{
    Ogre::Camera* pCamera = GetCamera();
    return -1.0f * pCamera->getDerivedUp();
}

Ogre::Vector3 SandboxMgr::GetCameraLeft()
{
    Ogre::Camera* pCamera = GetCamera();
    return -1.0f * pCamera->getDerivedRight();
}

Ogre::Vector3 SandboxMgr::GetCameraForward()
{
    Ogre::Camera* pCamera = GetCamera();
    return pCamera->getDerivedDirection();
}

Ogre::Vector3 SandboxMgr::GetCameraPosition()
{
    Ogre::Camera* pCamera = GetCamera();
    return pCamera->getDerivedPosition();
}

Ogre::Vector3 SandboxMgr::GetCameraRotation()
{
    return QuaternionToRotationDegrees(GetCameraOrientation());
}

Ogre::Quaternion SandboxMgr::GetCameraOrientation()
{
    Ogre::Camera* pCamera = GetCamera();
    return pCamera->getDerivedOrientation();
}

long long SandboxMgr::GetRenderTime()
{
    ClientManager::ProfileTimeType profileType = ClientManager::ProfileTimeType::P_RENDER_TIME;
    return GetClientMgr()->GetProfileTime(profileType);
}

long long SandboxMgr::GetSimulateTime()
{
    ClientManager::ProfileTimeType profileType = ClientManager::ProfileTimeType::P_SIMULATE_TIME;
    return GetClientMgr()->GetProfileTime(profileType);
}

long long SandboxMgr::GetTotalSimulateTime()
{
    ClientManager::ProfileTimeType profileType = ClientManager::ProfileTimeType::P_TOTAL_SIMULATE_TIME;
    return GetClientMgr()->GetProfileTime(profileType);
}


//---------------------------util static functions---------------------------
Ogre::SceneNode* SandboxMgr::CreateChildSceneNode()
{
    Ogre::SceneNode* pRootSceneNode = GetClientMgr()->getRootSceneNode();
    return pRootSceneNode->createChildSceneNode();
}

Ogre::SceneNode* SandboxMgr::CreateNodePlane(Ogre::Real length, Ogre::Real width)
{
    const Ogre::Real clampedLength = std::max(Ogre::Real(0.0f), length);
    const Ogre::Real clampedWidth = std::max(Ogre::Real(0.0f), width);

    Procedural::PlaneGenerator planeGenerator;
    planeGenerator.setSizeX(clampedLength);
    planeGenerator.setSizeY(clampedWidth);

    // TODO : Accept specifiers for UV tiling.
    planeGenerator.setUTile(clampedLength / 2);
    planeGenerator.setVTile(clampedWidth / 2);

    const Ogre::MeshPtr mesh = planeGenerator.realizeMesh();

    Ogre::SceneNode* const plane = SandboxMgr::CreateChildSceneNode();

    Ogre::Entity* const planeEntity = plane->getCreator()->createEntity(mesh);

    planeEntity->setMaterialName(DEFAULT_MATERIAL);

    plane->attachObject(planeEntity);

    return plane;
}

btRigidBody* SandboxMgr::CreateRigidBodyPlane(const btVector3& normal, const btScalar originOffset)
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

btRigidBody* SandboxMgr::CreateRigidBodyBox(Ogre::Mesh* meshPtr, const btScalar btmass)
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

Ogre::SceneNode* SandboxMgr::CreateNodeCapsule(Ogre::Real height, Ogre::Real radius)
{
    const Ogre::Real clampedHeight = std::max(Ogre::Real(0.0f), height);
    const Ogre::Real clampedRadius = std::max(Ogre::Real(0.0f), radius);;

    Procedural::CapsuleGenerator capsuleGenerator;
    capsuleGenerator.setHeight(clampedHeight - clampedRadius * 2);
    capsuleGenerator.setRadius(clampedRadius);
    capsuleGenerator.setNumRings(4);
    capsuleGenerator.setNumSegments(16);

    const Ogre::MeshPtr mesh = capsuleGenerator.realizeMesh();

    Ogre::SceneNode* const capsule = SandboxMgr::CreateChildSceneNode();

    Ogre::Entity* const capsuleEntity = capsule->getCreator()->createEntity(mesh);

    capsuleEntity->setMaterialName(DEFAULT_MATERIAL);

    capsule->attachObject(capsuleEntity);

    return capsule;
}

btRigidBody* SandboxMgr::CreateRigidBodyCapsule(Ogre::Real height, Ogre::Real radius)
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

btRigidBody* SandboxMgr::CreateRigidBodyBox(Ogre::Real width, Ogre::Real height, Ogre::Real length)
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
	rigidBody->setCcdSweptSphereRadius(width/2.0f);

	return rigidBody;
}

Ogre::SceneNode* SandboxMgr::CreateParticle(Ogre::SceneNode* parentNode, const Ogre::String& particleName)
{
	Ogre::SceneNode* particle = parentNode->createChildSceneNode();

    const Ogre::String& uniqueName = s_nameGenerator.generate();
	Ogre::ParticleSystem* particleSystem = parentNode->getCreator()->createParticleSystem(uniqueName, particleName);
    particle->attachObject(particleSystem);
    return particle;
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

bool SandboxMgr::GetBonePosition(Ogre::SceneNode& node, const Ogre::String& boneName, Ogre::Vector3& outPosition)
{
    unsigned short numAttacheds = node.numAttachedObjects();
    for (unsigned short index = 0; index < numAttacheds; index++)
    {
        Ogre::MovableObject* movable = node.getAttachedObject(index);
        if (GetBonePosition(*movable, boneName, outPosition))
        {
            return true;
        }
    }
    return false;
}

bool SandboxMgr::GetBonePosition(Ogre::MovableObject& object, const Ogre::String& boneName, Ogre::Vector3& outPosition)
{
    //Ogre::String entityName = Ogre::EntityFactory::FACTORY_TYPE_NAME;
    if (object.getMovableType() != "Entity") return false;
    
    Ogre::Entity* entity = static_cast<Ogre::Entity*>(&object);
    if (entity->hasSkeleton())
    {
        Ogre::Skeleton* skeleton = entity->getSkeleton();
        if (skeleton->hasBone(boneName))
        {
            Ogre::SceneNode* node = dynamic_cast<Ogre::SceneNode*>(entity->getParentNode());
            outPosition = skeleton->getBone(boneName)->_getDerivedPosition();
            if (node != nullptr)
            {
                //outPosition = 父节点平移 + (父节点旋转×骨骼局部位置)
                outPosition = node->_getDerivedPosition() + (node->_getDerivedOrientation() * outPosition);
            }
            return true;
        }
    }

    Ogre::Entity::ChildObjectListIterator iter = entity->getAttachedObjectIterator();
    while (iter.hasMoreElements())
    {
        Ogre::MovableObject* attachedObject = iter.getNext();
        if (GetBonePosition(*attachedObject, boneName, outPosition))
        {
            Ogre::Node* parentNode = attachedObject->getParentNode();
            outPosition = parentNode->_getDerivedPosition() + (parentNode->_getDerivedOrientation() * outPosition);
            return true;
        }
    }

    return false;
}

bool SandboxMgr::GetBoneOrientation(Ogre::SceneNode& node, const Ogre::String& boneName, Ogre::Quaternion& outOrientation)
{
	unsigned short numAttacheds = node.numAttachedObjects();
	for (unsigned short index = 0; index < numAttacheds; index++)
	{
		Ogre::MovableObject* movable = node.getAttachedObject(index);
		if (GetBoneOrientation(*movable, boneName, outOrientation))
		{
			return true;
		}
	}
	return false;
}

bool SandboxMgr::GetBoneOrientation(Ogre::MovableObject& object, const Ogre::String& boneName, Ogre::Quaternion& outOrientation)
{
	//Ogre::String entityName = Ogre::EntityFactory::FACTORY_TYPE_NAME;
	if (object.getMovableType() != "Entity") return false;

	Ogre::Entity* entity = static_cast<Ogre::Entity*>(&object);
	if (entity->hasSkeleton())
	{
		Ogre::Skeleton* skeleton = entity->getSkeleton();
		if (skeleton->hasBone(boneName))
		{
			Ogre::SceneNode* node = dynamic_cast<Ogre::SceneNode*>(entity->getParentNode());
            outOrientation = skeleton->getBone(boneName)->_getDerivedOrientation();
			if (node != nullptr)
			{
                outOrientation = node->convertLocalToWorldOrientation(outOrientation);
			}
			return true;
		}
	}

	Ogre::Entity::ChildObjectListIterator iter = entity->getAttachedObjectIterator();
	while (iter.hasMoreElements())
	{
		Ogre::MovableObject* attachedObject = iter.getNext();
		if (GetBoneOrientation(*attachedObject, boneName, outOrientation))
		{
			Ogre::Node* parentNode = attachedObject->getParentNode();
            outOrientation = parentNode->convertLocalToWorldOrientation(outOrientation);
			return true;
		}
	}

	return false;
}


//---------------------------util static functions---------------------------

//---------------------------sandbox tolua functions---------------------------
void SandboxMgr::SetSkyBox(const Ogre::String materialName, const Ogre::Vector3& rotation)
{
    const Ogre::Quaternion& newOrientation = QuaternionFromRotationDegrees(rotation.x, rotation.y, rotation.z);
    GetSceneManager()->setSkyBox(true, materialName, 5000.0f, true, newOrientation);
}

void SandboxMgr::SetAmbientLight(const Ogre::Vector3& colourValue)
{
    Ogre::ColourValue tempColor(colourValue.x, colourValue.y, colourValue.z);
    GetSceneManager()->setAmbientLight(tempColor);
}

Ogre::Light* SandboxMgr::CreateDirectionalLight(const Ogre::Vector3& direction)
{
    Ogre::SceneNode* const lightNode = m_pRootSceneNode->createChildSceneNode();

    Ogre::Light* const lightEntity = GetSceneManager()->createLight();

    lightEntity->setCastShadows(true);
    lightEntity->setType(Ogre::Light::LT_DIRECTIONAL);

    lightEntity->setDiffuseColour(1.0f, 1.0f, 1.0f);
    lightEntity->setSpecularColour(0, 0, 0);
    lightEntity->setDirection(direction);

    lightNode->attachObject(lightEntity);

    return lightEntity;
}

void SandboxMgr::setMaterial(BlockObject* pObject, const Ogre::String& materialName)
{
    assert(pObject != nullptr);
    setMaterial(pObject->getSceneNode(), materialName);
}

void SandboxMgr::setMaterial(Ogre::SceneNode* pNode, const Ogre::String& materialName)
{
    Ogre::SceneNode::ObjectIterator it = pNode->getAttachedObjectIterator();

    while (it.hasMoreElements())
    {
        const Ogre::String movableType =
            it.current()->second->getMovableType();

        if (movableType == Ogre::EntityFactory::FACTORY_TYPE_NAME)
        {
            Ogre::Entity* const entity =
                static_cast<Ogre::Entity*>(it.current()->second);
            entity->setMaterialName(materialName);
        }
        else if (movableType ==
            Ogre::ManualObjectFactory::FACTORY_TYPE_NAME)
        {
            Ogre::ManualObject* const entity =
                static_cast<Ogre::ManualObject*>(it.current()->second);
            unsigned int sections = entity->getNumSections();

            for (unsigned int id = 0; id < sections; ++id)
            {
                entity->setMaterialName(id, materialName);
            }
        }

        it.getNext();
    }
}

void SandboxMgr::SetMarkupColor(unsigned int index, const Ogre::ColourValue& color)
{
    return g_GameManager->setMarkupColor(index, color);
}

BlockObject* SandboxMgr::CreatePlane(float length, float width)
{
    Ogre::SceneNode* planeNode = SandboxMgr::CreateNodePlane(length, width);

    btRigidBody* planeRigidBody = SandboxMgr::CreateRigidBodyPlane(btVector3(0, 1.0f, 0), 0);

    BlockObject* pObject = new BlockObject(planeNode, planeRigidBody);
    pObject->setObjType(BaseObject::OBJ_TYPE_PLANE);

	g_ObjectManager->addNewObject(pObject);

    return pObject;
}

BlockObject* SandboxMgr::CreateBlockObject(const Ogre::String& meshFilePath)
{
    BlockObject* pObject = new BlockObject(meshFilePath);
    pObject->setObjType(BaseObject::OBJ_TYPE_BLOCK);

	g_ObjectManager->addNewObject(pObject);

    return pObject;
}

BlockObject* SandboxMgr::CreateBlockBox(float width, float height, float length, float uTile, float vTile)
{
    Procedural::BoxGenerator boxGenerator;
    boxGenerator.setSizeX(width);
    boxGenerator.setSizeY(height);
    boxGenerator.setSizeZ(length);
    boxGenerator.setUTile(uTile);
    boxGenerator.setVTile(vTile);

    const Ogre::MeshPtr meshPtr = boxGenerator.realizeMesh();

    btRigidBody* planeRigidBody = SandboxMgr::CreateRigidBodyBox(width, height, length);

	BlockObject* pObject = new BlockObject(meshPtr, planeRigidBody);
    pObject->setObjType(BaseObject::OBJ_TYPE_BLOCK);
    pObject->getEntity()->setMaterialName(DEFAULT_MATERIAL);

	g_ObjectManager->addNewObject(pObject);

	return pObject;
}

EntityObject* SandboxMgr::CreateEntityObject(const Ogre::String& meshFilePath)
{
    EntityObject* pObject = new EntityObject(meshFilePath);
    pObject->setObjType(BaseObject::OBJ_TYPE_ENTITY);

	g_ObjectManager->addNewObject(pObject);

	return pObject;
}

UIComponent* SandboxMgr::CreateUIComponent(unsigned int index)
{
    return g_GameManager->createUIComponent(index);
}

AgentObject* SandboxMgr::CreateAgent(AGENT_OBJ_TYPE agentType)
{
    Ogre::Real height = AgentObject::DEFAULT_AGENT_HEIGHT;
    Ogre::Real radius = AgentObject::DEFAULT_AGENT_RADIUS;

	Ogre::SceneNode* capsuleNode = SandboxMgr::CreateNodeCapsule(height, radius);
    EntityObject* pEntityObj = new EntityObject(capsuleNode);
	pEntityObj->setMaterial("Ground2");

    btRigidBody* capsuleRigidBody = SandboxMgr::CreateRigidBodyCapsule(height, radius);
    capsuleRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

    AgentObject* pObject = new AgentObject(pEntityObj, capsuleRigidBody);
    pObject->setObjType(BaseObject::OBJ_TYPE_AGENT);
    pObject->setAgentType(agentType);

	g_ObjectManager->addNewObject(pObject);

    return pObject;
}

AgentObject* SandboxMgr::CreateSoldier(const Ogre::String& meshFilePath)
{
	Ogre::Real height = AgentObject::DEFAULT_AGENT_HEIGHT;
	Ogre::Real radius = AgentObject::DEFAULT_AGENT_RADIUS;

	EntityObject* pEntityObj = new EntityObject(meshFilePath);
    pEntityObj->setOriginPos(Vector3(0.0f, -height * 0.5f, 0.0f));

	btRigidBody* capsuleRigidBody = SandboxMgr::CreateRigidBodyCapsule(height, radius);
	capsuleRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	AgentObject* pObject = new AgentObject(pEntityObj, capsuleRigidBody);
    pObject->setObjType(BaseObject::OBJ_TYPE_SOLDIER);
    g_ObjectManager->addNewObject(pObject);

	return pObject;
}

BlockObject* SandboxMgr::CreateBullet(Ogre::Real height, Ogre::Real radius)
{
    Ogre::SceneNode* pSceneNode = SandboxMgr::CreateChildSceneNode();

	btRigidBody* capsuleRigidBody = SandboxMgr::CreateRigidBodyCapsule(height, radius);

	BlockObject* pBullet = new BlockObject(pSceneNode, capsuleRigidBody);
    pBullet->setObjType(BaseObject::OBJ_TYPE_BULLET);

	g_ObjectManager->addNewObject(pBullet);

	return pBullet;
}
