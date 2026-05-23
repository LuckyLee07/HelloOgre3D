#include "diagnostics/RuntimeResourceDiagnostics.h"
#include "OgreResourceManager.h"
#include "OgreResource.h"
#include "OgreMeshManager.h"
#include "OgreMaterialManager.h"
#include "OgreTextureManager.h"
#include "OgreSkeletonManager.h"
#include "OgreGpuProgramManager.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreResourceGroupManager.h"

#include <algorithm>
#include <sstream>
#include <vector>

namespace
{
	struct ResourceDebugEntry
	{
		std::string name;
		std::string group;
		std::string origin;
		size_t sizeBytes;
		bool loaded;
		bool manual;
		bool reloadable;
	};

	bool EndsWith(const std::string& value, const std::string& suffix)
	{
		if (value.size() < suffix.size())
			return false;
		return value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
	}

	int ClampMaxEntries(int maxEntries)
	{
		if (maxEntries < 0)
			return 0;
		if (maxEntries > 64)
			return 64;
		return maxEntries;
	}

	void AppendResourceManagerDump(std::ostringstream& stream, const char* label, Ogre::ResourceManager* manager, int maxEntries)
	{
		if (manager == nullptr)
		{
			stream << "[ResourceDump] type=" << label << " unavailable\n";
			return;
		}

		int totalCount = 0;
		int loadedCount = 0;
		int manualCount = 0;
		size_t loadedBytes = 0;
		std::vector<ResourceDebugEntry> entries;

		Ogre::ResourceManager::ResourceMapIterator iter = manager->getResourceIterator();
		while (iter.hasMoreElements())
		{
			Ogre::ResourcePtr resource = iter.getNext();
			if (resource.isNull())
				continue;

			++totalCount;
			const bool loaded = resource->isLoaded();
			const bool manual = resource->isManuallyLoaded();
			if (loaded)
			{
				++loadedCount;
				loadedBytes += resource->getSize();
			}
			if (manual)
				++manualCount;

			ResourceDebugEntry entry;
			entry.name = resource->getName();
			entry.group = resource->getGroup();
			entry.origin = resource->getOrigin();
			entry.sizeBytes = resource->getSize();
			entry.loaded = loaded;
			entry.manual = manual;
			entry.reloadable = resource->isReloadable();
			entries.push_back(entry);
		}

		std::sort(entries.begin(), entries.end(), [](const ResourceDebugEntry& left, const ResourceDebugEntry& right) {
			if (left.loaded != right.loaded)
				return left.loaded > right.loaded;
			if (left.sizeBytes != right.sizeBytes)
				return left.sizeBytes > right.sizeBytes;
			return left.name < right.name;
		});

		const int showingCount = maxEntries < static_cast<int>(entries.size()) ? maxEntries : static_cast<int>(entries.size());
		stream << "[ResourceDump] type=" << label
			<< " total=" << totalCount
			<< " loaded=" << loadedCount
			<< " manual=" << manualCount
			<< " loadedKB=" << static_cast<unsigned long long>(loadedBytes / 1024)
			<< " showing=" << showingCount << "\n";

		for (int index = 0; index < showingCount; ++index)
		{
			const ResourceDebugEntry& entry = entries[index];
			stream << "[ResourceDump] " << label
				<< " name=" << entry.name
				<< " group=" << entry.group
				<< " loaded=" << (entry.loaded ? "true" : "false")
				<< " sizeKB=" << static_cast<unsigned long long>(entry.sizeBytes / 1024)
				<< " manual=" << (entry.manual ? "true" : "false")
				<< " reloadable=" << (entry.reloadable ? "true" : "false");
			if (!entry.origin.empty())
				stream << " origin=" << entry.origin;
			stream << "\n";
		}
	}

	void AppendResourceGroupDump(std::ostringstream& stream, int maxEntries)
	{
		Ogre::ResourceGroupManager* groupManager = Ogre::ResourceGroupManager::getSingletonPtr();
		if (groupManager == nullptr)
		{
			stream << "[ResourceDump] groups unavailable\n";
			return;
		}

		Ogre::StringVector groups = groupManager->getResourceGroups();
		stream << "[ResourceDump] groups=" << groups.size() << "\n";
		for (Ogre::StringVector::const_iterator groupIt = groups.begin(); groupIt != groups.end(); ++groupIt)
		{
			Ogre::StringVectorPtr resources = groupManager->listResourceNames(*groupIt, false);
			const size_t total = resources.isNull() ? 0 : resources->size();
			int materials = 0;
			int programs = 0;
			int particles = 0;
			int meshes = 0;
			int skeletons = 0;
			int textures = 0;

			if (!resources.isNull())
			{
				for (Ogre::StringVector::const_iterator nameIt = resources->begin(); nameIt != resources->end(); ++nameIt)
				{
					const std::string name = *nameIt;
					if (EndsWith(name, ".material"))
						++materials;
					else if (EndsWith(name, ".program"))
						++programs;
					else if (EndsWith(name, ".particle"))
						++particles;
					else if (EndsWith(name, ".mesh"))
						++meshes;
					else if (EndsWith(name, ".skeleton"))
						++skeletons;
					else if (EndsWith(name, ".png") || EndsWith(name, ".dds") || EndsWith(name, ".tga") || EndsWith(name, ".jpg"))
						++textures;
				}
			}

			stream << "[ResourceDump] group=" << *groupIt
				<< " files=" << total
				<< " material=" << materials
				<< " program=" << programs
				<< " particle=" << particles
				<< " mesh=" << meshes
				<< " skeleton=" << skeletons
				<< " texture=" << textures << "\n";

			if (!resources.isNull())
			{
				const int showingCount = maxEntries < static_cast<int>(resources->size()) ? maxEntries : static_cast<int>(resources->size());
				for (int index = 0; index < showingCount; ++index)
				{
					stream << "[ResourceDump] groupFile group=" << *groupIt << " name=" << (*resources)[index] << "\n";
				}
			}
		}
	}
}

std::string RuntimeResourceDiagnostics::BuildResourceDump(int maxEntriesPerType)
{
	const int maxEntries = ClampMaxEntries(maxEntriesPerType);
	std::ostringstream stream;
	stream << "[ResourceDump] begin maxEntries=" << maxEntries << "\n";
	AppendResourceManagerDump(stream, "Mesh", Ogre::MeshManager::getSingletonPtr(), maxEntries);
	AppendResourceManagerDump(stream, "Material", Ogre::MaterialManager::getSingletonPtr(), maxEntries);
	AppendResourceManagerDump(stream, "Texture", Ogre::TextureManager::getSingletonPtr(), maxEntries);
	AppendResourceManagerDump(stream, "Skeleton", Ogre::SkeletonManager::getSingletonPtr(), maxEntries);
	AppendResourceManagerDump(stream, "GpuProgram", Ogre::GpuProgramManager::getSingletonPtr(), maxEntries);
	AppendResourceManagerDump(stream, "HighLevelGpuProgram", Ogre::HighLevelGpuProgramManager::getSingletonPtr(), maxEntries);
	AppendResourceGroupDump(stream, maxEntries);
	stream << "[ResourceDump] end";
	return stream.str();
}
