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
		Geometry::setMesh(const MeshPtr& mesh) noexcept
		{
			mesh_ = mesh;
		}

		const MeshPtr&
		Geometry::getMesh() const noexcept
		{
			return mesh_;
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