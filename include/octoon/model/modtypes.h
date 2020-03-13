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
		class Mesh;
		class Material;

		class Bone;
		class CombineMesh;

		struct IKAttr;
		struct Model;
		struct Morph;
		struct Rigidbody;
		struct Softbody;
		struct Joint;
		struct Texture;

		namespace detail
		{
			template<typename _Tx = std::uint16_t, typename _Ty = float>
			class VertexWeight;
		}

		// default
		using VertexWeight = detail::VertexWeight<std::uint16_t, float>;

		typedef std::shared_ptr<Mesh> MeshPtr;
		typedef std::shared_ptr<Material> MaterialPtr;
		typedef std::shared_ptr<class Path> PathPtr;
		typedef std::shared_ptr<class PathEdge> PathEdgePtr;
		typedef std::shared_ptr<class PathGroup> PathGroupPtr;		
		typedef std::shared_ptr<class Contour> ContourPtr;
		typedef std::shared_ptr<class ContourGroup> ContourGroupPtr;
		typedef std::shared_ptr<class TextFile> TextFilePtr;
		typedef std::shared_ptr<class TextMeshing> TextMeshingPtr;

		typedef std::shared_ptr<Bone> BonePtr;
		typedef std::shared_ptr<IKAttr> IKAttrPtr;
		typedef std::shared_ptr<VertexWeight> VertexWeightPtr;

		typedef std::vector<VertexWeight> VertexWeights;
		typedef std::vector<MeshPtr> Meshes;
		typedef std::vector<Bone> Bones;
		typedef std::vector<MaterialPtr> Materials;
		typedef std::vector<IKAttr> InverseKinematics;
		typedef std::vector<CombineMesh> CombineMeshes;
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

		enum PropertyTypeInfo
		{
			PropertyTypeInfoFloat = 0x01,
			PropertyTypeInfoString = 0x02,
			PropertyTypeInfoInt = 0x04,
			PropertyTypeInfoBuffer = 0x08,
		};

		using istream = io::istream;
		using ostream = io::ostream;
	}
}

#endif
