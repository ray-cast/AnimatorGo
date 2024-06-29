#ifndef OCTOON_CUBE_MESH_H_
#define OCTOON_CUBE_MESH_H_

#include <octoon/mesh/mesh.h>

namespace octoon
{
	class OCTOON_EXPORT CubeMesh final : public Mesh
	{
		OctoonDeclareSubClass(CubeMesh, Mesh)
	public:
		CubeMesh() noexcept;
		CubeMesh(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept;
	};
}

#endif