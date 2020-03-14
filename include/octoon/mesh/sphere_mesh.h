#ifndef OCTOON_SPHERE_MESH_H_
#define OCTOON_SPHERE_MESH_H_

#include <octoon/mesh/mesh.h>

namespace octoon
{
	namespace mesh
	{
		class OCTOON_EXPORT SphereMesh final : public Mesh
		{
			OctoonDeclareSubClass(SphereMesh, Mesh)
		public:
			SphereMesh() noexcept;
			SphereMesh(float radius, std::uint32_t widthSegments = 32, std::uint32_t heightSegments = 24, float phiStart = 0.0, float phiLength = math::PI_2, float thetaStart = 0, float thetaLength = math::PI) noexcept;
		};
	}
}

#endif