#include <octoon/video/mesh.h>

namespace octoon
{
	namespace video
	{
		Mesh::Mesh() noexcept
		{
		}

		Mesh::~Mesh() noexcept
		{
		}

		void
		Mesh::setVertexArray(math::float3s&& array) noexcept
		{
			vertices_ = std::move(array);
		}

		void
		Mesh::setVertexArray(const math::float3s& array) noexcept
		{
			vertices_ = array;
		}

		void
		Mesh::setNormalArray(math::float3s&& array) noexcept
		{
			normals_ = std::move(array);
		}

		void
		Mesh::setNormalArray(const math::float3s& array) noexcept
		{
			normals_ = array;
		}

		void
		Mesh::setColorArray(math::float3s&& array) noexcept
		{
			colors_ = std::move(array);
		}

		void
		Mesh::setColorArray(const math::float3s& array) noexcept
		{
			colors_ = array;
		}

		void
		Mesh::setIndicesArray(math::uint32s&& array) noexcept
		{
			indices_ = std::move(array);
		}

		void
		Mesh::setIndicesArray(const math::uint32s& array) noexcept
		{
			indices_ = array;
		}

		math::float3s&
		Mesh::getVertexArray() noexcept
		{
			return vertices_;
		}

		const math::float3s&
		Mesh::getVertexArray() const noexcept
		{
			return vertices_;
		}

		math::float3s&
		Mesh::getNormalArray() noexcept
		{
			return normals_;
		}

		const math::float3s&
		Mesh::getNormalArray() const noexcept
		{
			return normals_;
		}

		math::float3s&
		Mesh::getColorArray() noexcept
		{
			return colors_;
		}

		const math::float3s&
		Mesh::getColorArray() const noexcept
		{
			return colors_;
		}

		math::uint32s&
		Mesh::getIndicesArray() noexcept
		{
			return indices_;
		}

		const math::uint32s&
		Mesh::getIndicesArray() const noexcept
		{
			return indices_;
		}

		MeshPtr
		Mesh::clone() const noexcept
		{
			return std::make_shared<Mesh>();
		}

		void
		Mesh::computeVerticeNormals() noexcept
		{
			normals_.resize(vertices_.size());

			for (std::size_t i = 0; i < vertices_.size(); i += 3)
			{
				auto& a = vertices_[i];
				auto& b = vertices_[i + 1];
				auto& c = vertices_[i + 2];

				auto ab = a - b;
				auto ac = a - c;

				auto n = math::normalize(math::cross(ab, ac));

				normals_[i + 0] = n;
				normals_[i + 1] = n;
				normals_[i + 2] = n;
			}
		}
	}
}