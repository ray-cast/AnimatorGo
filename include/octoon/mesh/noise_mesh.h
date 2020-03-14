#ifndef OCTOON_NOISE_MESH_H_
#define OCTOON_NOISE_MESH_H_

#include <octoon/mesh/mesh.h>

namespace octoon
{
	namespace mesh
	{
		class OCTOON_EXPORT NoiseMesh final : public Mesh
		{
			OctoonDeclareSubClass(NoiseMesh, Mesh)
		public:
			NoiseMesh() noexcept;
			NoiseMesh(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1) noexcept;
		};
	}
}

#endif