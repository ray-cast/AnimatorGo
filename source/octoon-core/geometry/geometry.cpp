#include <octoon/geometry/geometry.h>

namespace octoon::geometry
{
	OctoonImplementSubClass(Geometry, video::RenderObject, "Geometry")

	Geometry::Geometry() noexcept
		: isCastShadow_(true)
		, isReceiveShadow_(true)
		, numVertices_(0)
		, numIndices_(0)
	{
	}

	Geometry::~Geometry() noexcept
	{
		this->setActive(false);
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
	Geometry::setVertexBuffer(const hal::GraphicsDataPtr& data) noexcept
	{
		vertices_ = data;
	}

	const hal::GraphicsDataPtr&
	Geometry::getVertexBuffer() const noexcept
	{
		return vertices_;
	}

	void
	Geometry::setIndexBuffer(const hal::GraphicsDataPtr& data) noexcept
	{
		indices_ = data;
	}

	const hal::GraphicsDataPtr&
	Geometry::getIndexBuffer() const noexcept
	{
		return indices_;
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

	void
	Geometry::setMaterial(std::shared_ptr<material::Material>&& material) noexcept
	{
		material_ = std::move(material);
	}

	void
	Geometry::setMaterial(const std::shared_ptr<material::Material>& material) noexcept
	{
		material_ = material;
	}

	const std::shared_ptr<material::Material>&
	Geometry::getMaterial() const noexcept
	{
		return material_;
	}
}