#include <octoon/model/combine_mesh.h>

using namespace octoon::math;

namespace octoon
{
	namespace model
	{
		CombineMesh::CombineMesh() noexcept
		{
		}

		CombineMesh::CombineMesh(const MeshPtr& mesh, const float4x4 & transform) noexcept
			: _mesh(mesh)
			, _transform(transform)
		{
		}

		void CombineMesh::setMesh(const MeshPtr& other) noexcept
		{
			_mesh = other;
		}

		const MeshPtr& CombineMesh::getMesh() const noexcept
		{
			return _mesh;
		}

		void CombineMesh::setTransform(const float4x4 & m) noexcept
		{
			_transform = m;
		}

		const float4x4& CombineMesh::getTransform() const noexcept
		{
			return _transform;
		}

		void CombineMesh::makeTransform(const float3& translate, const Quaternion& quat, const float3& scale) noexcept
		{
			_transform.make_transform(translate, quat, scale);
		}
	}
}