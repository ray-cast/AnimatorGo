#ifndef OCTOON_MODEL_MORPH_H_
#define OCTOON_MODEL_MORPH_H_

#include <octoon/model/modtypes.h>

namespace octoon
{
	namespace model
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
	}
}

#endif