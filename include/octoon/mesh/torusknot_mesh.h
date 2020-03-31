#ifndef OCTOON_TORUSKNOT_MESH_H_
#define OCTOON_TORUSKNOT_MESH_H_

#include <octoon/mesh/mesh.h>

namespace octoon::mesh
{
	class OCTOON_EXPORT TorusKnotMesh final : public Mesh
	{
		OctoonDeclareSubClass(TorusKnotMesh, Mesh)
	public:
		TorusKnotMesh() noexcept;
		TorusKnotMesh(float radius, float tube, std::uint32_t tubularSegments = 64, std::uint32_t radialSegments = 8, std::uint32_t p = 2, std::uint32_t q = 3) noexcept;
	};
}

#endif