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
			CombineMesh(const MeshPropertyPtr mesh, const octoon::math::float4x4& transform = octoon::math::float4x4::One) noexcept;

			void setMesh(MeshPropertyPtr other) noexcept;
			MeshPropertyPtr getMesh() const noexcept;

			void setTransform(const octoon::math::float4x4& m) noexcept;
			const octoon::math::float4x4& getTransform() const noexcept;

			void makeTransform(const octoon::math::float3& translate, const octoon::math::Quaternion& quat, const octoon::math::float3& scale) noexcept;

		private:
			MeshPropertyPtr _mesh;
			octoon::math::float4x4 _transform;
		};
	}
}

#endif // !OCTOON_COMBINE_MESH_H_
