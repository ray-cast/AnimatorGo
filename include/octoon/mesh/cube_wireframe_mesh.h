#ifndef OCTOON_CUBE_WIREFRAME_MESH_H_
#define OCTOON_CUBE_WIREFRAME_MESH_H_

#include <octoon/mesh/mesh.h>

namespace octoon
{
	class OCTOON_EXPORT CubeWireframeMesh final : public Mesh
	{
		OctoonDeclareSubClass(CubeWireframeMesh, Mesh)
	public:
		CubeWireframeMesh() noexcept;
		CubeWireframeMesh(float width, float height, float depth, std::uint32_t widthSegments = 1, std::uint32_t heightSegments = 1, std::uint32_t depthSegments = 1) noexcept;
	};
}

#endif