#include <octoon/mesh/ring_mesh.h>

namespace octoon::mesh
{
	OctoonImplementSubClass(RingMesh, Mesh, "RingMesh");

	RingMesh::RingMesh() noexcept
		: RingMesh(1, 1)
	{
	}

	RingMesh::RingMesh(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart, float thetaLength) noexcept
	{
		this->clear();

		innerRadius = innerRadius || 0;
		outerRadius = outerRadius || 50;

		thetaStart = thetaStart ? thetaStart : 0;
		thetaLength = thetaLength ? thetaLength : math::PI * 2;

		thetaSegments = thetaSegments ? std::max((std::uint32_t)3, thetaSegments) : 8;
		phiSegments = phiSegments ? std::max((std::uint32_t)3, phiSegments) : 8;

		for (size_t i = 0; i <= phiSegments; i++)
		{
			for (size_t j = 0; j <= thetaSegments; j++)
			{
			}
		}
	}
}