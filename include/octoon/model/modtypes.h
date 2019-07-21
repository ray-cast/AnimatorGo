#ifndef OCTOON_MODTYPES_H_
#define OCTOON_MODTYPES_H_

#include <memory>
#include <vector>
#include <functional>

#include <octoon/math/math.h>
#include <octoon/runtime/platform.h>
#include <octoon/io/iostream.h>

#define TEXTURE_ARRAY_COUNT 4

#define MATKEY_NAME               "?mat.name"
#define MATKEY_TWOSIDED           "$mat.twosided"
#define MATKEY_SHADING_MODEL      "$mat.shadingm"
#define MATKEY_ENABLE_WIREFRAME   "$mat.wireframe"
#define MATKEY_BLEND_FUNC         "$mat.blend"
#define MATKEY_OPACITY            "$mat.opacity"
#define MATKEY_BUMPSCALING        "$mat.bumpscaling"
#define MATKEY_SHININESS          "$mat.shininess"
#define MATKEY_REFLECTIVITY       "$mat.reflectivity"
#define MATKEY_SHININESS_STRENGTH "$mat.shinpercent"
#define MATKEY_REFRACTI           "$mat.refracti"
#define MATKEY_EFFECT             "$mat.effect"
#define MATKEY_COLOR_DIFFUSE      "$clr.diffuse"
#define MATKEY_COLOR_AMBIENT      "$clr.ambient"
#define MATKEY_COLOR_SPECULAR     "$clr.specular"
#define MATKEY_COLOR_EMISSIVE     "$clr.emissive"
#define MATKEY_COLOR_SPHEREMAP    "$clr.spheremap"
#define MATKEY_COLOR_TRANSPARENT  "$clr.transparent"
#define MATKEY_COLOR_REFLECTIVE   "$clr.reflective"

#define MATKEY_TEXTURE_DIFFUSE	    "$tex.diffuse"
#define MATKEY_TEXTURE_SPECULAR	    "$tex.specular"
#define MATKEY_TEXTURE_AMBIENT	    "$tex.ambient"
#define MATKEY_TEXTURE_EMISSIVE	    "$tex.emissive"
#define MATKEY_TEXTURE_NORMALS	    "$tex.normals"
#define MATKEY_TEXTURE_HEIGHT	    "$tex.height"
#define MATKEY_TEXTURE_SHININESS	"$tex.shininess"
#define MATKEY_TEXTURE_OPACITY	    "$tex.opacity"
#define MATKEY_TEXTURE_DISPLACEMENT	"$tex.displacement"
#define MATKEY_TEXTURE_LIGHTMAP	    "$tex.lightmap"
#define MATKEY_TEXTURE_REFLECTION	"$tex.reflection"
#define MATKEY_TEXTURE_TOON			"$tex.toon"

namespace octoon
{
	namespace model
	{
		class Animation;
		class Texture;
		class Camera;
		class Light;
		class Mesh;
		class Material;

		class Model;
		class Bone;
		class IKAttr;
		class Rigidbody;
		class Joint;
		class CombineMesh;

		namespace detail
		{
			template<typename _Tx = std::uint16_t, typename _Ty = float>
			class VertexWeight;
		}

		// default
		using VertexWeight = detail::VertexWeight<std::uint16_t, float>;

		typedef std::shared_ptr<Animation> AnimationPtr;
		typedef std::shared_ptr<Texture> TexturePtr;
		typedef std::shared_ptr<Mesh> MeshPtr;
		typedef std::shared_ptr<Material> MaterialPtr;
		typedef std::shared_ptr<class Path> PathPtr;
		typedef std::shared_ptr<class PathEdge> PathEdgePtr;
		typedef std::shared_ptr<class PathGroup> PathGroupPtr;		
		typedef std::shared_ptr<class Contour> ContourPtr;
		typedef std::shared_ptr<class ContourGroup> ContourGroupPtr;
		typedef std::shared_ptr<class TextFile> TextFilePtr;
		typedef std::shared_ptr<class TextMeshing> TextMeshingPtr;
		typedef std::shared_ptr<class ModelLoader> ModelLoaderPtr;

		typedef std::shared_ptr<Model> ModelPtr;
		typedef std::shared_ptr<Bone> BonePtr;
		typedef std::shared_ptr<IKAttr> IKAttrPtr;
		typedef std::shared_ptr<Rigidbody> RigidbodyPtr;
		typedef std::shared_ptr<Joint> JointPtr;
		typedef std::shared_ptr<VertexWeight> VertexWeightPtr;

		typedef std::vector<VertexWeight> VertexWeights;
		typedef std::vector<MeshPtr> Meshes;
		typedef std::vector<Bone> Bones;
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
