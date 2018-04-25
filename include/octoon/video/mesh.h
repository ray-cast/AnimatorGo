#ifndef OCTOON_MESH_H_
#define OCTOON_MESH_H_

#include <octoon/video/render_types.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT Mesh final
		{
		public:
			Mesh() noexcept;
			~Mesh() noexcept;

			void setVertexArray(math::float3s&& array) noexcept;
			void setVertexArray(const math::float3s& array) noexcept;
			void setNormalArray(math::float3s&& array) noexcept;
			void setNormalArray(const math::float3s& array) noexcept;
			void setColorArray(math::float3s&& array) noexcept;
			void setColorArray(const math::float3s& array) noexcept;
			void setIndicesArray(math::uint32s&& array) noexcept;
			void setIndicesArray(const math::uint32s& array) noexcept;

			math::float3s& getVertexArray() noexcept;
			const math::float3s& getVertexArray() const noexcept;

			math::float3s& getColorArray() noexcept;
			const math::float3s& getColorArray() const noexcept;

			math::float3s& getNormalArray() noexcept;
			const math::float3s& getNormalArray() const noexcept;

			math::uint32s& getIndicesArray() noexcept;
			const math::uint32s& getIndicesArray() const noexcept;

			MeshPtr clone() const noexcept;

			void computeVerticeNormals() noexcept;

		private:
			Mesh(const Mesh&) = delete;
			Mesh& operator=(const Mesh&) = delete;

		private:
			math::float3s vertices_;
			math::float3s normals_;
			math::float3s colors_;
			math::uint32s indices_;
		};
	}
}

#endif