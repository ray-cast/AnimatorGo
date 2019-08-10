#ifndef OCTOON_MODEL_SOFTBODY_H_
#define OCTOON_MODEL_SOFTBODY_H_

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
		class OCTOON_EXPORT Softbody
		{
		public:
			std::string name;
			std::int32_t materialIndex;

			std::uint8_t group;
			std::uint16_t groupMask;

			std::uint16_t aeroModel;

			std::int32_t blinkLength;
			std::int8_t numClusters;

			float LST;

			float totalMass;
			float collisionMargin;

			std::vector<std::uint32_t> anchorRigidbodies;
			std::vector<std::uint32_t> pinVertexIndices;
		};
	}
}
#endif