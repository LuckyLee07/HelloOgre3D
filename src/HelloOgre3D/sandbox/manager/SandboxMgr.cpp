#include "SandboxMgr.h"
#include "Ogre.h"
#include "Procedural.h"
#include "SandboxDef.h"
#include "GameManager.h"
#include "ClientManager.h"
#include "ScriptLuaVM.h"

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

using namespace Ogre;

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

    return new btRigidBody(groundRigidBodyCI);
}

btRigidBody* SandboxMgr::CreateRigidBodyBox(Ogre::Mesh* meshPtr, const btScalar btmass)
{
    const btVector3 position(0, 0, 0);
    const btScalar mass = btmass;

    // ���������ݴ���һ���򻯵�͹����״��
    btConvexHullShape * hullShape = CreateSimplifiedConvexHull(meshPtr);

    // ����AABB�������߽�У�����С�����㡣
    btVector3 aabbMin;
    btVector3 aabbMax;

    // ����͹����״��AABB��
    hullShape->getAabb(
        btTransform(btQuaternion::getIdentity()), aabbMin, aabbMax);

    // �������ģ�������ΪAABB�����ġ������ڶԳ����壬�ǶԳ�������ܳ������⡣
    const btVector3 centerOfMass = (aabbMax + aabbMin) / 2.0f;

    // ����һ��������״������϶����״��������ʱֻ����һ����״��
    btCompoundShape* compoundShape = new btCompoundShape();

    // Ϊ����������ƫ������Ĵ��뱻ע�͵��ˣ��ô�����Ըı���״��ͼ�α�ʾ��
    /*
    compoundShape->addChildShape(
        btTransform(btQuaternion::getIdentity(), -centerOfMass),
        hullShape);
    */

    // �����״��������״�У�δ��������λ�á�
    compoundShape->addChildShape(
        btTransform(btQuaternion::getIdentity()), hullShape);

    // �����˶�״̬���󣬰�������ĳ�ʼλ�á�
    btDefaultMotionState* const motionState = new btDefaultMotionState(
        btTransform(btQuaternion::getIdentity(), position));

    // ��ʼ���ֲ�����Ϊ�㣬��������������ṩ��������
    btVector3 localInertia(0, 0, 0);
    // ���㸴����״�ľֲ����ԡ�
    compoundShape->calculateLocalInertia(mass, localInertia);

    // ���������Ϣ�������������˶�״̬��������״�;ֲ����ԡ�
    btRigidBody::btRigidBodyConstructionInfo
        rigidBodyCI(mass, motionState, compoundShape, localInertia);

    // �趨���������������ֵ��Ӱ������ں�ʱ��������״̬��
    rigidBodyCI.m_linearSleepingThreshold = 0.3f;

    // �����������
    btRigidBody* const rigidBody = new btRigidBody(rigidBodyCI);

    // ����������ײ��⣨CCD������ֵ�����ڴ�������˶������塣
    rigidBody->setCcdMotionThreshold(0.5f);

    // ����CCD�õ�ɨ������뾶������AABB��󳤶ȵ�һ�롣
    rigidBody->setCcdSweptSphereRadius(aabbMax.length() / 2.0f);

    // ���ش����ĸ��塣
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
    // ����һ��������״�����а뾶�ǽ��ҵ�Բ�����ֵİ뾶���߶ȼ�ȥ�����뾶��Բ�����ֵĸ߶ȡ�
    btCapsuleShape* capsuleShape = new btCapsuleShape(radius, height - radius * 2);

    // ����һ��Ĭ�ϵ��˶�״̬���󣬳�ʼʱû����ת��λ�ơ�
    btDefaultMotionState* capsuleMotionState = new btDefaultMotionState();

    // ��ʼ���ֲ�������������ʼʱ����Ϊ0���ֲ����Խ������������㣬�����������ת���ԡ�
    btVector3 localInertia(0, 0, 0);
    // ���㽺����״�ľֲ����ԣ����轺�ҵ�����Ϊ1.0f��
    capsuleShape->calculateLocalInertia(1.0f, localInertia);

    // ��������Ĺ�����Ϣ������������1.0f��ʾ���Ǿ�̬�ģ����˶�״̬����״�;ֲ����ԡ�
    btRigidBody::btRigidBodyConstructionInfo capsuleRigidBodyCI(1.0f, capsuleMotionState, capsuleShape, localInertia);

    // ���ù���Ħ�����������ڷ�ֹ������ƽ�������޹�����
    capsuleRigidBodyCI.m_rollingFriction = 0.2f;

    // ʹ������������Ϣ�����������
    btRigidBody* const rigidBody = new btRigidBody(capsuleRigidBodyCI);

    // ����������ײ��⣨CCD������ֵ�Ͱ뾶�����ڴ�������ƶ���������ܲ����Ĵ�͸���⡣
    // CCD��ֵ��Ϊ0.5f��CCDɨ����뾶����Ϊ���ҵİ뾶��
    rigidBody->setCcdMotionThreshold(0.5f);
    rigidBody->setCcdSweptSphereRadius(radius);

    return rigidBody;
}

btConvexHullShape* SandboxMgr::CreateSimplifiedConvexHull(Ogre::Mesh* meshPtr)
{
    // ��ȡ Ogre::Mesh �Ķ�������
    Ogre::Vector3* vertices = nullptr;
    unsigned long* indices = nullptr;
    size_t vertex_count = 0, index_count = 0;

    GetMeshInfo(meshPtr, vertex_count, vertices, index_count, indices);

    // ʹ�� Bullet3 �ṩ��͹�����㹤��
    btConvexHullShape* hullShape = new btConvexHullShape();
    hullShape->setMargin(0.01f);
    hullShape->setSafeMargin(0.01f);

    // �� Ogre �Ķ�������ת���� Bullet3 �Ķ�������
    for (size_t i = 0; i < vertex_count; ++i) {
        const Ogre::Vector3& vertex = vertices[i];
        hullShape->addPoint(btVector3(vertex.x, vertex.y, vertex.z));
    }

    delete[] vertices;
    delete[] indices;

    // ʹ�� btShapeHull ���о�ϸ������
    btShapeHull* hull = new btShapeHull(hullShape);
    btScalar margin = hullShape->getMargin();
    hull->buildHull(margin);

    // ����һ���µ� btConvexHullShape���Ա��澫ϸ�����͹������
    btConvexHullShape* simplifiedHullShape = new btConvexHullShape();
    const btVector3* const btVertices = hull->getVertexPointer();
    for (int i = 0; i < hull->numVertices(); ++i) {
        simplifiedHullShape->addPoint(btVertices[i]);
    }
    simplifiedHullShape->setMargin(0.01f);

    // �ͷ���Դ
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

    g_GameManager->addBlockObject(pObject);

    return pObject;
}

BlockObject* SandboxMgr::CreateBlockObject(const Ogre::String& meshFilePath)
{
    BlockObject* pObject = new BlockObject(meshFilePath);

    g_GameManager->addBlockObject(pObject);

    return pObject;
}

EntityObject* SandboxMgr::CreateEntityObject(const Ogre::String& meshFilePath)
{
    EntityObject* pObject = new EntityObject(meshFilePath);

	g_GameManager->addEntityObject(pObject);

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

    Ogre::SceneNode* capsuleNode = nullptr;
    if (agentType != AGENT_OBJ_NONE)
    {
		capsuleNode = SandboxMgr::CreateNodeCapsule(height, radius);
		this->setMaterial(capsuleNode, "Ground2");
    }

    btRigidBody* capsuleRigidBody = SandboxMgr::CreateRigidBodyCapsule(height, radius);
    capsuleRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

    AgentObject* pObject = new AgentObject(capsuleNode, capsuleRigidBody);
    pObject->setAgentType(agentType);

    g_GameManager->addAgentObject(pObject);

    return pObject;
}