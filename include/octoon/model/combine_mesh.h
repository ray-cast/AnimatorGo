#ifndef OCTOON_COMBINE_MESH_H_
#define OCTOON_COMBINE_MESH_H_

#include <octoon/model/modtypes.h>
#include <octoon/math/mat4.h>

namespace octoon
{
	namespace model
	{
		class CombineMesh
		{
		public:
			CombineMesh() noexcept;
			CombineMesh(const MeshPropertyPtr mesh, const math::float4x4& transform = math::float4x4::One) noexcept;

			void setMesh(MeshPropertyPtr other) noexcept;
			MeshPropertyPtr getMesh() const noexcept;

			void setTransform(const math::float4x4& m) noexcept;
			const math::float4x4& getTransform() const noexcept;

			void makeTransform(const math::float3& translate, const math::Quaternion& quat, const math::float3& scale) noexcept;

		private:
			MeshPropertyPtr _mesh;
			math::float4x4 _transform;
		};
	}
}

#endif // !OCTOON_COMBINE_MESH_H_
