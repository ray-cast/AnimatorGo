#ifndef OCTOON_COMBINE_MESH_H_
#define OCTOON_COMBINE_MESH_H_

#include <octoon/math/mat4.h>
#include <memory>

namespace octoon
{
	namespace mesh
	{
		class Mesh;
		class CombineMesh
		{
		public:
			CombineMesh() noexcept;
			CombineMesh(const std::shared_ptr<mesh::Mesh>& mesh, const math::float4x4& transform = math::float4x4::One) noexcept;

			void setMesh(const std::shared_ptr<mesh::Mesh>& other) noexcept;
			const std::shared_ptr<mesh::Mesh>& getMesh() const noexcept;

			void setTransform(const math::float4x4& m) noexcept;
			const math::float4x4& getTransform() const noexcept;

			void makeTransform(const math::float3& translate, const math::Quaternion& quat, const math::float3& scale) noexcept;

		private:
			std::shared_ptr<mesh::Mesh> _mesh;
			math::float4x4 _transform;
		};

		typedef std::vector<CombineMesh> CombineMeshes;
	}
}

#endif
