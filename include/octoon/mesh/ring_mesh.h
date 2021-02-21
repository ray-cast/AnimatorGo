#ifndef OCTOON_RING_MESH_H_
#define OCTOON_RING_MESH_H_

#include <octoon/mesh/mesh.h>

namespace octoon
{
	class OCTOON_EXPORT RingMesh final : public Mesh
	{
		OctoonDeclareSubClass(RingMesh, Mesh)
	public:
		RingMesh() noexcept;
		RingMesh(float innerRadius, float outerRadius, std::uint32_t thetaSegments = 24, std::uint32_t phiSegments = 32, float thetaStart = 0, float thetaLength = math::PI) noexcept;
	};
}

#endif