#ifndef OCTOON_MODEL_H_
#define OCTOON_MODEL_H_

#include <octoon/model/modtypes.h>
#include <octoon/mesh/mesh.h>
#include <octoon/material/material.h>

namespace octoon
{
	enum MorphType : std::uint8_t
	{
		MorphTypeGroup = 0,
		MorphTypeVertex = 1,
		MorphTypeBone = 2,
		MorphTypeUV = 3,
		MorphTypeExtraUV1 = 4,
		MorphTypeExtraUV2 = 5,
		MorphTypeExtraUV3 = 6,
		MorphTypeExtraUV4 = 7,
		MorphTypeMaterial = 8
	};

	enum ShapeType
	{
		ShapeTypeSphere,
		ShapeTypeSquare,
		ShapeTypeCapsule
	};

	struct MorphVertex
	{
		std::uint32_t  index;
		math::Vector3 offset;
	};

	struct MorphTexcoord
	{
		std::uint32_t  index;
		math::Vector4 offset;
	};

	struct MorphBone
	{
		std::uint32_t  boneIndex;
		math::Vector3 position;
		math::Quaternion rotation;
	};

	struct MorphMaterial
	{
		std::uint32_t index;
		std::uint8_t  offset;
		math::float4 diffuse;
		float  shininess;
		math::float3 specular;
		math::float3 ambient;
		math::float4 edgeColor;
		float  edgeSize;
		math::float4 tex;
		math::float4 sphere;
		math::float4 toon;
	};

	struct MorphGroup
	{
		std::uint16_t morphIndex;
		float  morphRate;
	};

	struct Morph
	{
		std::string name;

		std::uint8_t  control;
		std::uint8_t  morphType;
		std::uint32_t morphCount;

		std::vector<MorphGroup>    groupList;
		std::vector<MorphVertex>   vertices;
		std::vector<MorphBone>     boneList;
		std::vector<MorphTexcoord> texcoordList;
		std::vector<MorphMaterial> materialList;
	};

	struct Rigidbody
	{
		std::string name;

		std::uint32_t bone;

		std::uint8_t group;
		std::uint16_t groupMask;
		ShapeType shape;

		math::float3 scale;
		math::float3 position;
		math::float3 rotation;

		float mass;
		float movementDecay;
		float rotationDecay;
		float elasticity;
		float friction;

		std::uint8_t physicsOperation; //0:Follow Bone (static), 1:Physics Calc. (dynamic), 2: Physics Calc. + Bone position matching
	};

	struct Joint
	{
		std::string name;

		std::uint32_t type;

		math::float3 position;
		math::float3 rotation;

		std::uint32_t bodyIndexA;
		std::uint32_t bodyIndexB;

		math::float3 movementLowerLimit;
		math::float3 movementUpperLimit;
		math::float3 rotationLowerLimit;
		math::float3 rotationUpperLimit;

		math::float3 springMovementConstant;
		math::float3 springRotationConstant;
	};

	struct Softbody
	{
		std::string name;
		std::int32_t materialIndex;

		std::uint8_t group;
		std::uint16_t groupMask;

		std::int32_t aeroModel;

		std::int32_t blinkLength;
		std::int32_t numClusters;

		float LST;

		float totalMass;
		float collisionMargin;

		std::vector<std::uint32_t> anchorRigidbodies;
		std::vector<std::uint32_t> pinVertexIndices;
	};

	struct Texture final
	{
		std::string   name;
		std::uint32_t width;
		std::uint32_t height;
	};

	struct IKChild
	{
		float angleRadian;

		std::uint8_t rotateLimited;
		std::uint16_t boneIndex;

		math::float3 minimumRadian;
		math::float3 maximumRadian;
	};

	struct IKAttr
	{
		std::uint16_t boneIndex;
		std::uint16_t targetBoneIndex;
		std::uint32_t iterations;
		std::uint32_t chainLength;

		std::vector<IKChild> child;
	};

	struct Model final
	{
		using mesh_array_t = std::vector<std::shared_ptr<Mesh>>;
		using bone_array_t = std::vector<BonePtr>;
		using ik_array_t = std::vector<IKAttrPtr>;
		using rigidbody_array_t = std::vector<std::shared_ptr<Rigidbody>>;
		using softbody_array_t = std::vector<std::shared_ptr<Softbody>>;
		using joint_array_t = std::vector<std::shared_ptr<Joint>>;
		using material_array_t = std::vector<MaterialPtr>;
		using texture_array_t = std::vector<std::shared_ptr<Texture>>;
		using morph_array_t = std::vector<std::shared_ptr<Morph>>;

		std::string _name;

		mesh_array_t meshes;
		bone_array_t bones;
		ik_array_t iks;
		rigidbody_array_t rigidbodies;
		softbody_array_t softbodies;
		joint_array_t joints;
		texture_array_t textures;
		material_array_t materials;
		morph_array_t morphs;
	};
}

#endif
