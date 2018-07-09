#ifndef OCTOON_MODEL_IK_H_
#define OCTOON_MODEL_IK_H_

#include <octoon/math/mat4.h>
#include <octoon/math/quat.h>
#include <octoon/math/vector3.h>
#include <octoon/model/vertex_weight.h>

#include <cstdint>

namespace octoon
{
	namespace model
	{
		class IKChild
		{
		public:
			float angleWeight;

			std::uint8_t rotateLimited;
			std::uint16_t boneIndex;

			math::float3 minimumDegrees;
			math::float3 maximumDegrees;
		};

		class IKAttr
		{
		public:
			std::uint16_t boneIndex;
			std::uint16_t targetBoneIndex;
			std::uint32_t iterations;
			std::uint32_t chainLength;

			std::vector<IKChild> child;
		};
	}
}

#endif