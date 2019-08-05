#ifndef OCTOON_MODEL_RIGIDBODY_H_
#define OCTOON_MODEL_RIGIDBODY_H_

#include <octoon/model/modtypes.h>
#include <octoon/model/bone.h>
#include <octoon/model/combine_mesh.h>

#include <octoon/math/mathfwd.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <octoon/math/boundingbox.h>

#include <string>

namespace octoon
{
	namespace model
	{
		enum OCTOON_EXPORT ShapeType
		{
			ShapeTypeSphere,
			ShapeTypeSquare,
			ShapeTypeCapsule
		};

		class OCTOON_EXPORT Rigidbody
		{
		public:
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
	}
}
#endif