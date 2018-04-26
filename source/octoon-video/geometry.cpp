#include <octoon/video/geometry.h>
#include <octoon/video/camera.h>
#include <octoon/video/render_system.h>

namespace octoon
{
	namespace video
	{
		OctoonImplementSubClass(Geometry, RenderObject, "Geometry")

		Geometry::Geometry() noexcept
			: isCastShadow_(true)
			, isReceiveShadow_(true)
			, drawType_(DrawType::Triangles)
			, indexType_(GraphicsIndexType::Uint32)
			, _vertexOffset(0)
			, _indexOffset(0)
		{
		}

		Geometry::~Geometry() noexcept
		{
		}

		void
		Geometry::setReceiveShadow(bool enable) noexcept
		{
			isReceiveShadow_ = enable;
		}

		bool
		Geometry::getReceiveShadow() const noexcept
		{
			return isReceiveShadow_;
		}

		void
		Geometry::setCastShadow(bool value) noexcept
		{
			isCastShadow_ = value;
		}

		bool
		Geometry::getCastShadow()  const noexcept
		{
			return isCastShadow_;
		}

		void
		Geometry::setMaterial(video::MaterialPtr&& material) noexcept
		{
			material_ = std::move(material);
		}

		void
		Geometry::setMaterial(const video::MaterialPtr& material) noexcept
		{
			material_ = material;
		}

		const video::MaterialPtr&
		Geometry::getMaterial() const noexcept
		{
			return material_;
		}

		void
		Geometry::setMesh(const model::MeshPtr& mesh) noexcept
		{
			if (mesh)
			{
				auto& vertices = mesh->getVertexArray();
				auto& normals = mesh->getNormalArray();

				graphics::GraphicsDataDesc dataDesc;
				dataDesc.setType(graphics::GraphicsDataType::StorageVertexBuffer);
				dataDesc.setStream(0);
				dataDesc.setStreamSize(vertices.size() * sizeof(math::float3) * 2);
				dataDesc.setUsage(graphics::GraphicsUsageFlagBits::WriteBit);

				vertices_ = RenderSystem::instance()->createGraphicsData(dataDesc);

				math::float3* data = nullptr;
				if (vertices_->map(0, vertices.size() * sizeof(math::float3) * 2, (void**)&data))
				{
					auto v = data;
					for (auto& it : vertices)
					{
						*v = it;
						v += 2;
					}

					auto n = ++data;
					for (auto& it : normals)
					{
						*n = it;
						n += 2;
					}

					vertices_->unmap();
				}

				auto& indices = mesh->getIndicesArray();
				if (!indices.empty())
				{
					graphics::GraphicsDataDesc indiceDesc;
					indiceDesc.setType(graphics::GraphicsDataType::StorageIndexBuffer);
					indiceDesc.setStream((std::uint8_t*)indices.data());
					indiceDesc.setStreamSize(indices.size() * sizeof(std::uint32_t));
					indiceDesc.setUsage(graphics::GraphicsUsageFlagBits::ReadBit);

					indices_ = RenderSystem::instance()->createGraphicsData(indiceDesc);
				}
			}

			mesh_ = mesh;
		}

		const model::MeshPtr&
		Geometry::getMesh() const noexcept
		{
			return mesh_;
		}

		graphics::GraphicsDataPtr
		Geometry::getVertexBuffer() const noexcept
		{
			return vertices_;
		}

		graphics::GraphicsDataPtr
		Geometry::getIndexBuffer() const noexcept
		{
			return indices_;
		}

		void
		Geometry::setDrawType(DrawType type) noexcept
		{
			drawType_ = type;
		}

		DrawType
		Geometry::getDrawType() const noexcept
		{
			return drawType_;
		}
	}
}