#ifndef OCTOON_MODEL_JOINT_H_
#define OCTOON_MODEL_JOINT_H_

#include <octoon/model/modtypes.h>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT Joint
		{
		public:
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
	}
}

#endif