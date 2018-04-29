#ifndef OCTOON_MODTYPES_H_
#define OCTOON_MODTYPES_H_

#include <memory>
#include <vector>

#include <octoon/math/mathfwd.h>

#define MATKEY_NAME               "?mat.name",0,0
#define MATKEY_TWOSIDED           "$mat.twosided",0,0
#define MATKEY_SHADING_MODEL      "$mat.shadingm",0,0
#define MATKEY_ENABLE_WIREFRAME   "$mat.wireframe",0,0
#define MATKEY_BLEND_FUNC         "$mat.blend",0,0
#define MATKEY_OPACITY            "$mat.opacity",0,0
#define MATKEY_BUMPSCALING        "$mat.bumpscaling",0,0
#define MATKEY_SHININESS          "$mat.shininess",0,0
#define MATKEY_REFLECTIVITY       "$mat.reflectivity",0,0
#define MATKEY_SHININESS_STRENGTH "$mat.shinpercent",0,0
#define MATKEY_REFRACTI           "$mat.refracti",0,0
#define MATKEY_EFFECT             "$mat.effect",0,0
#define MATKEY_COLOR_DIFFUSE      "$clr.diffuse",0,0
#define MATKEY_COLOR_AMBIENT      "$clr.ambient",0,0
#define MATKEY_COLOR_SPECULAR     "$clr.specular",0,0
#define MATKEY_COLOR_EMISSIVE     "$clr.emissive",0,0
#define MATKEY_COLOR_SPHEREMAP    "$clr.spheremap",0,0
#define MATKEY_COLOR_TRANSPARENT  "$clr.transparent",0,0
#define MATKEY_COLOR_REFLECTIVE   "$clr.reflective",0,0

#define MATKEY_TEXTURE(type, N)   "$tex.file", type, N

#define MATKEY_TEXTURE_DIFFUSE(N)      MATKEY_TEXTURE(TextureTypeDiffuse, N)
#define MATKEY_TEXTURE_SPECULAR(N)     MATKEY_TEXTURE(TextureTypeSpecular, N)
#define MATKEY_TEXTURE_AMBIENT(N)      MATKEY_TEXTURE(TextureTypeAmbient, N)
#define MATKEY_TEXTURE_EMISSIVE(N)     MATKEY_TEXTURE(TextureTypeEmissive, N)
#define MATKEY_TEXTURE_NORMALS(N)      MATKEY_TEXTURE(TextureTypeNormals, N)
#define MATKEY_TEXTURE_HEIGHT(N)       MATKEY_TEXTURE(TextureTypeHeight, N)
#define MATKEY_TEXTURE_SHININESS(N)    MATKEY_TEXTURE(TextureTypeShininess, N)
#define MATKEY_TEXTURE_OPACITY(N)      MATKEY_TEXTURE(TextureTypeOpacity, N)
#define MATKEY_TEXTURE_DISPLACEMENT(N) MATKEY_TEXTURE(TextureTypeDisplacement, N)
#define MATKEY_TEXTURE_LIGHTMAP(N)     MATKEY_TEXTURE(TextureTypeLightmap, N)
#define MATKEY_TEXTURE_REFLECTION(N)   MATKEY_TEXTURE(TextureTypeReflection, N)

#define TEXTURE_ARRAY_COUNT 4

namespace octoon
{
	namespace model
	{
		class AnimationProperty;
		class TextureProperty;
		class CameraProperty;
		class LightProperty;
		class Mesh;
		class MaterialProperty;

		class Model;
		class Bone;
		class IKAttr;
		class RigidbodyProperty;
		class JointProperty;
		class VertexWeight;
		class CombineMesh;

		typedef std::shared_ptr<AnimationProperty> AnimationPropertyPtr;
		typedef std::shared_ptr<TextureProperty> TexturePropertyPtr;
		typedef std::shared_ptr<CameraProperty> CameraPropertyPtr;
		typedef std::shared_ptr<LightProperty> LightPropertyPtr;
		typedef std::shared_ptr<Mesh> MeshPtr;
		typedef std::shared_ptr<MaterialProperty> MaterialPropertyPtr;
		typedef std::unique_ptr<class Contour> ContourPtr;
		typedef std::shared_ptr<class ContourGroup> ContourGroupPtr;
		typedef std::shared_ptr<class TextFile> TextFilePtr;
		typedef std::shared_ptr<class TextMeshing> TextMeshingPtr;

		typedef std::shared_ptr<Model> ModelPtr;
		typedef std::shared_ptr<Bone> BonePtr;
		typedef std::shared_ptr<IKAttr> IKAttrPtr;
		typedef std::shared_ptr<RigidbodyProperty> RigidbodyPropertyPtr;
		typedef std::shared_ptr<JointProperty> JointPropertyPtr;
		typedef std::shared_ptr<VertexWeight> VertexWeightPtr;

		typedef std::vector<VertexWeight> VertexWeights;
		typedef std::vector<MeshPtr> Meshes;
		typedef std::vector<Bone> Bones;
		typedef std::vector<IKAttr> InverseKinematics;
		typedef std::vector<CombineMesh> CombineMeshes;
		typedef std::vector<TextFilePtr> TextFiles;
		typedef std::vector<ContourPtr> Contours;
		typedef std::vector<ContourGroupPtr> ContourGroups;

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
	}
}

#endif // !OCTOON_MODTYPES_H_
