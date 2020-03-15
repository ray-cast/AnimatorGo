#include <octoon/mesh/volume_mesh.h>

namespace octoon::mesh
{
	OctoonImplementSubClass(VolumeMesh, Mesh, "VolumeMesh");

	VolumeMesh::VolumeMesh() noexcept
		: VolumeMesh(1.0f,1.0f,1.0f)
	{
	}

	VolumeMesh::VolumeMesh(float fovy, float znear, float zfar) noexcept
	{
		std::vector<math::float3> vertices;

		float tanFovy_2 = math::tan(fovy * math::PI / 360.0f);

		vertices.emplace_back(tanFovy_2 * znear, tanFovy_2 * znear, -znear);
		vertices.emplace_back(-tanFovy_2 * znear, tanFovy_2 * znear, -znear);
		vertices.emplace_back(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
		vertices.emplace_back(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);

		vertices.emplace_back(tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
		vertices.emplace_back(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
		vertices.emplace_back(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
		vertices.emplace_back(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear);

		vertices.emplace_back(tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
		vertices.emplace_back(tanFovy_2 * znear, tanFovy_2 * znear, -znear);
		vertices.emplace_back(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
		vertices.emplace_back(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);

		vertices.emplace_back(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
		vertices.emplace_back(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
		vertices.emplace_back(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
		vertices.emplace_back(-tanFovy_2 * znear, tanFovy_2 * znear, -znear);

		vertices.emplace_back(-tanFovy_2 * znear, tanFovy_2 * znear, -znear);
		vertices.emplace_back(tanFovy_2 * znear, tanFovy_2 * znear, -znear);
		vertices.emplace_back(tanFovy_2 * znear, -tanFovy_2 * znear, -znear);
		vertices.emplace_back(-tanFovy_2 * znear, -tanFovy_2 * znear, -znear);

		vertices.emplace_back(tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
		vertices.emplace_back(-tanFovy_2 * zfar, tanFovy_2 * zfar, -zfar);
		vertices.emplace_back(-tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);
		vertices.emplace_back(tanFovy_2 * zfar, -tanFovy_2 * zfar, -zfar);

		this->setVertexArray(std::move(vertices));

		this->computeTangents();
		this->computeBoundingBox();
	}
}