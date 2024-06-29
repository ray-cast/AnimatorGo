#include <octoon/mesh/combine_mesh.h>

namespace octoon
{
	CombineMesh::CombineMesh() noexcept
	{
	}

	CombineMesh::CombineMesh(const std::shared_ptr<Mesh>& mesh, const math::float4x4& transform) noexcept
		: _mesh(mesh)
		, _transform(transform)
	{
	}

	void
	CombineMesh::setMesh(const std::shared_ptr<Mesh>& other) noexcept
	{
		_mesh = other;
	}

	const std::shared_ptr<Mesh>&
	CombineMesh::getMesh() const noexcept
	{
		return _mesh;
	}

	void
	CombineMesh::setTransform(const math::float4x4& m) noexcept
	{
		_transform = m;
	}

	const math::float4x4&
	CombineMesh::getTransform() const noexcept
	{
		return _transform;
	}

	void
	CombineMesh::makeTransform(const math::float3& translate, const math::Quaternion& quat, const math::float3& scale) noexcept
	{
		_transform.makeTransform(translate, quat, scale);
	}
}