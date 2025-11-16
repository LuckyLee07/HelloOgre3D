#include "SceneFactory.h"
#include "Ogre.h"
#include "Procedural.h"
//#include "OgreSceneManager.h"
#include "manager/ClientManager.h"
#include "GameDefine.h"

Ogre::NameGenerator SceneFactory::s_nameGenerator("UnnamedParticle_");

Ogre::SceneNode* SceneFactory::CreateChildSceneNode()
{
	Ogre::SceneNode* pRootSceneNode = GetClientMgr()->getRootSceneNode();
	return pRootSceneNode->createChildSceneNode();
}

Ogre::SceneNode* SceneFactory::CreateNodePlane(Ogre::Real length, Ogre::Real width)
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

	Ogre::SceneNode* const plane = SceneFactory::CreateChildSceneNode();

	Ogre::Entity* const planeEntity = plane->getCreator()->createEntity(mesh);

	planeEntity->setMaterialName(DEFAULT_MATERIAL);

	plane->attachObject(planeEntity);

	return plane;
}

Ogre::SceneNode* SceneFactory::CreateNodeCapsule(Ogre::Real height, Ogre::Real radius)
{
	const Ogre::Real clampedHeight = std::max(Ogre::Real(0.0f), height);
	const Ogre::Real clampedRadius = std::max(Ogre::Real(0.0f), radius);;

	Procedural::CapsuleGenerator capsuleGenerator;
	capsuleGenerator.setHeight(clampedHeight - clampedRadius * 2);
	capsuleGenerator.setRadius(clampedRadius);
	capsuleGenerator.setNumRings(4);
	capsuleGenerator.setNumSegments(16);

	const Ogre::MeshPtr mesh = capsuleGenerator.realizeMesh();

	Ogre::SceneNode* const capsule = SceneFactory::CreateChildSceneNode();

	Ogre::Entity* const capsuleEntity = capsule->getCreator()->createEntity(mesh);

	capsuleEntity->setMaterialName(DEFAULT_MATERIAL);

	capsule->attachObject(capsuleEntity);

	return capsule;
}

Ogre::SceneNode* SceneFactory::CreateParticle(Ogre::SceneNode* parentNode, const Ogre::String& particleName)
{
	Ogre::SceneNode* particle = parentNode->createChildSceneNode();

	const Ogre::String& particleId = s_nameGenerator.generate();
	Ogre::ParticleSystem* particleSystem = particle->getCreator()->createParticleSystem(particleId, particleName);
	particle->attachObject(particleSystem);
	return particle;
}

void SceneFactory::RemParticleBySceneNode(Ogre::SceneNode* particleNode)
{
	const unsigned short numAttachedObjects = particleNode->numAttachedObjects();
	for (unsigned short index = 0; index < numAttachedObjects; ++index)
	{
		Ogre::MovableObject* pObject = particleNode->getAttachedObject(index);
		if (pObject->getMovableType() == Ogre::ParticleSystemFactory::FACTORY_TYPE_NAME)
		{
			Ogre::ParticleSystem* particleSys = static_cast<Ogre::ParticleSystem*>(pObject);
			particleNode->detachObject(particleSys);
			particleNode->getCreator()->destroyParticleSystem(particleSys);
		}
	}
}

void SceneFactory::GetMeshInfo(const Ogre::Mesh* mesh,
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

bool SceneFactory::GetBonePosition(Ogre::SceneNode& node, const Ogre::String& boneName, Ogre::Vector3& outPosition)
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

bool SceneFactory::GetBonePosition(Ogre::MovableObject& object, const Ogre::String& boneName, Ogre::Vector3& outPosition)
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

bool SceneFactory::GetBoneOrientation(Ogre::SceneNode& node, const Ogre::String& boneName, Ogre::Quaternion& outOrientation)
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

bool SceneFactory::GetBoneOrientation(Ogre::MovableObject& object, const Ogre::String& boneName, Ogre::Quaternion& outOrientation)
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