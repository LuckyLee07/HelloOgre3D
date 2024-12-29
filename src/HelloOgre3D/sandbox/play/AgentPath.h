#ifndef __AGENT_PATH__
#define __AGENT_PATH__

#include <vector>
#include "OgrePrerequisites.h"
#include "opensteer/include/Pathway.h"

class AgentPath : private OpenSteer::PolylinePathway
{
	friend class VehicleObject;
public:
	const static size_t MAX_PATH_POINTS = 255;

	AgentPath();
	AgentPath(const AgentPath& cpath);
	AgentPath(const std::vector<Ogre::Vector3>& cpoints, Ogre::Real radius, bool cyclic);
	
	~AgentPath();

	AgentPath& operator=(const AgentPath& cpath);

	size_t GetNumberOfPathPoints() const;

	void GetPathPoints(std::vector<Ogre::Vector3>& cpoints);

	Ogre::Real GetPathLength() const;

	Ogre::Real GetRadius() const { return radius; }

	int GetSegmentCount() const;
};

#endif  // __AGENT_PATH__
