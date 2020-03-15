#ifndef OCTOON_CIRCLE_MESH_H_
#define OCTOON_CIRCLE_MESH_H_

#include <octoon/mesh/mesh.h>

namespace octoon::mesh
{
	class OCTOON_EXPORT CircleMesh final : public Mesh
	{
		OctoonDeclareSubClass(CircleMesh, Mesh)
	public:
		CircleMesh() noexcept;
		CircleMesh(float radius, std::uint32_t segments = 32, float thetaStart = 0, float thetaLength = math::PI_2) noexcept;
	};
}

#endif