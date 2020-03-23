#ifndef OCTOON_PLANE_MESH_H_
#define OCTOON_PLANE_MESH_H_

#include <octoon/mesh/mesh.h>

namespace octoon::mesh
{
	class OCTOON_EXPORT PlaneMesh final : public Mesh
	{
		OctoonDeclareSubClass(PlaneMesh, Mesh)
	public:
		PlaneMesh() noexcept;
		PlaneMesh(float width, float height, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, bool wireframe = false) noexcept;
	};
}

#endif