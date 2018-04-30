#include <octoon/video/geometry.h>
#include <octoon/video/camera.h>

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
			, vertexOffset_(0)
			, indexOffset_(0)
			, numVertices_(0)
			, numIndices_(0)
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
		Geometry::setVertexBuffer(const graphics::GraphicsDataPtr& data) noexcept
		{
			vertices_ = data;
		}

		const graphics::GraphicsDataPtr&
		Geometry::getVertexBuffer() const noexcept
		{
			return vertices_;
		}

		void
		Geometry::setIndexBuffer(const graphics::GraphicsDataPtr& data) noexcept
		{
			indices_ = data;
		}

		const graphics::GraphicsDataPtr&
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

		void
		Geometry::setNumVertices(std::uint32_t numVertice) noexcept
		{
			numVertices_ = numVertice;
		}

		std::uint32_t
		Geometry::getNumVertices() const noexcept
		{
			return numVertices_;
		}

		void
		Geometry::setNumIndices(std::uint32_t numIndices) noexcept
		{
			numIndices_ = numIndices;
		}

		std::uint32_t
		Geometry::getNumIndices() const noexcept
		{
			return numIndices_;
		}
	}
}