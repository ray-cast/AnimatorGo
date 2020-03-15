#ifndef OCTOON_CONE_MESH_H_
#define OCTOON_CONE_MESH_H_

#include <octoon/mesh/mesh.h>

namespace octoon::mesh
{
	class OCTOON_EXPORT ConeMesh final : public Mesh
	{
		OctoonDeclareSubClass(ConeMesh, Mesh)
	public:
		ConeMesh() noexcept;
		ConeMesh(float radius, float height, std::uint32_t segments = 32, float thetaStart = 0, float thetaLength = math::PI_2) noexcept;
	};
}

#endif