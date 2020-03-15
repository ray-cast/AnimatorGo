#ifndef OCTOON_MODTYPES_H_
#define OCTOON_MODTYPES_H_

#include <memory>
#include <vector>
#include <functional>

#include <octoon/math/math.h>
#include <octoon/runtime/platform.h>
#include <octoon/io/iostream.h>

#define TEXTURE_ARRAY_COUNT 4

namespace octoon
{
	namespace model
	{
		class Camera;
		class Light;
		class Bone;
		class CombineMesh;

		struct IKAttr;
		struct Model;
		struct Morph;
		struct Rigidbody;
		struct Softbody;
		struct Joint;
		struct Texture;

		typedef std::shared_ptr<class Path> PathPtr;
		typedef std::shared_ptr<class PathEdge> PathEdgePtr;
		typedef std::shared_ptr<class PathGroup> PathGroupPtr;		
		typedef std::shared_ptr<class Contour> ContourPtr;
		typedef std::shared_ptr<class ContourGroup> ContourGroupPtr;
		typedef std::shared_ptr<class TextFile> TextFilePtr;
		typedef std::shared_ptr<class TextMeshing> TextMeshingPtr;

		typedef std::shared_ptr<IKAttr> IKAttrPtr;
		typedef std::vector<IKAttr> InverseKinematics;
		typedef std::vector<TextFilePtr> TextFiles;
		typedef std::vector<ContourPtr> Contours;
		typedef std::vector<ContourGroupPtr> ContourGroups;
		typedef std::vector<PathPtr> Paths;
		typedef std::vector<PathEdge> PathEdges;
		typedef std::vector<PathGroupPtr> PathGroups;

		enum TextureType
		{
			TextureTypeNone,
			TextureTypeDiffuse,
			TextureTypeSpecular,
			TextureTypeAmbient,
			TextureTypeEmissive,
			TextureTypeHeight,
			TextureTypeNormals,
			TextureTypeShininess,
			TextureTypeOpacity,
			TextureTypeDisplacement,
			TextureTypeLightmap,
			TextureTypeReflection,
		};

		using istream = io::istream;
		using ostream = io::ostream;
	}
}

#endif
