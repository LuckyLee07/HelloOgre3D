#include "AgentPath.h"
#include "OgreVector3.h"
#include "GameFunction.h"

AgentPath::AgentPath()
{

}

AgentPath::AgentPath(const AgentPath& cpath)
{
	initialize(cpath.pointCount, cpath.points, cpath.radius, cpath.cyclic);
}

AgentPath::AgentPath(const std::vector<Ogre::Vector3>& cpoints, Ogre::Real radius, bool cyclic)
{
	OpenSteer::Vec3 vec3Point[MAX_PATH_POINTS];
	assert(cpoints.size() < MAX_PATH_POINTS);

	size_t pcount = 0;
	std::vector<Ogre::Vector3>::const_iterator iter;
	for (iter = cpoints.begin(); iter != cpoints.end(); iter++)
	{
		const Ogre::Vector3& vec = *iter;
		vec3Point[pcount].x = vec.x;
		vec3Point[pcount].y = vec.y;
		vec3Point[pcount].z = vec.z;
		pcount++;
	}
	initialize(pcount, vec3Point, radius, cyclic);
}

AgentPath::~AgentPath()
{
	
}

AgentPath& AgentPath::operator=(const AgentPath& cpath)
{
	initialize(cpath.pointCount, cpath.points, cpath.radius, cpath.cyclic);
	return *this;
}

size_t AgentPath::GetNumberOfPathPoints() const
{
	return pointCount;
}

void AgentPath::GetPathPoints(std::vector<Ogre::Vector3>& cpoints) const
{
	cpoints.clear();
	size_t pointsize = GetNumberOfPathPoints();
	for (size_t index = 0; index < pointsize; index++)
	{
		const OpenSteer::Vec3& point = points[index];
		cpoints.push_back(Vec3ToVector3(point));
	}
}

Ogre::Real AgentPath::GetPathLength() const
{
	return Ogre::Real(totalPathLength);
}

int AgentPath::GetSegmentCount() const
{
	return pointCount - 1;
}

Ogre::Real AgentPath::GetDistanceAlongPath(const Ogre::Vector3& position) const
{
	const OpenSteer::Vec3 vec3(position.x, position.y, position.z);

	return const_cast<AgentPath*>(this)->mapPointToPathDistance(vec3);
}

Ogre::Vector3 AgentPath::GetNearestPointOnPath(const Ogre::Vector3& position) const
{
	const OpenSteer::Vec3 vec3(position.x, position.y, position.z);
	OpenSteer::Vec3 tangent;

	float outside;

	const OpenSteer::Vec3 pointOnPath = const_cast<AgentPath*>(this)->mapPointToPath(vec3, tangent, outside);

	return Ogre::Vector3(pointOnPath.x, pointOnPath.y, pointOnPath.z);
}

Ogre::Vector3 AgentPath::GetPointOnPath(const Ogre::Real distance) const
{
	const OpenSteer::Vec3 pointOnPath = const_cast<AgentPath*>(this)->mapPathDistanceToPoint(distance);

	return Ogre::Vector3(pointOnPath.x, pointOnPath.y, pointOnPath.z);
}