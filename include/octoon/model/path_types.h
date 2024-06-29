#ifndef OCTOON_PATH_TYPES_H_
#define OCTOON_PATH_TYPES_H_

#include <memory>
#include <vector>
#include <functional>

#include <octoon/math/math.h>
#include <octoon/runtime/platform.h>
#include <octoon/io/iostream.h>

namespace octoon
{
	typedef std::shared_ptr<class Path> PathPtr;
	typedef std::shared_ptr<class PathEdge> PathEdgePtr;
	typedef std::shared_ptr<class PathGroup> PathGroupPtr;		
	typedef std::shared_ptr<class Contour> ContourPtr;
	typedef std::shared_ptr<class ContourGroup> ContourGroupPtr;

	typedef std::vector<ContourPtr> Contours;
	typedef std::vector<ContourGroupPtr> ContourGroups;
	typedef std::vector<PathPtr> Paths;
	typedef std::vector<PathEdge> PathEdges;
	typedef std::vector<PathGroupPtr> PathGroups;
}

#endif
