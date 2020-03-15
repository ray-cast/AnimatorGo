#include <octoon/video/geometry.h>

namespace octoon::video
{
	OctoonImplementSubClass(Geometry, RenderObject, "Geometry")

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
	Geometry::setRenderPipeline(video::RenderPipelinePtr&& pipeline) noexcept
	{
		pipeline_ = std::move(pipeline);
	}

	void
	Geometry::setRenderPipeline(const video::RenderPipelinePtr& pipeline) noexcept
	{
		pipeline_ = pipeline;
	}

	const video::RenderPipelinePtr&
	Geometry::getRenderPipeline() const noexcept
	{
		return pipeline_;
	}
}