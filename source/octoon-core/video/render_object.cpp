#include <octoon/video/render_object.h>
#include <octoon/video/render_scene.h>

namespace octoon::video
{
	OctoonImplementSubInterface(RenderObject, runtime::RttiInterface, "RenderObject")

	RenderListener::RenderListener() noexcept
	{
	}

	RenderListener::~RenderListener() noexcept
	{
	}

	RenderObject::RenderObject() noexcept
		: visible_(true)
		, dirty_(true)
		, layer_(0)
		, order_(0)
		, transform_(math::float4x4::One)
		, transformInverse_(math::float4x4::One)
		, renderListener_(nullptr)
	{
	}

	RenderObject::~RenderObject() noexcept
	{
	}

	void
	RenderObject::setLayer(std::uint8_t layer) noexcept
	{
		layer_ = layer;
		this->setDirty(true);
	}

	std::uint8_t
	RenderObject::getLayer() const noexcept
	{
		return layer_;
	}

	void
	RenderObject::setRenderOrder(std::int32_t order) noexcept
	{
		order_ = order;
		this->setDirty(true);
	}

	std::int32_t
	RenderObject::getRenderOrder() const noexcept
	{
		return order_;
	}

	void
	RenderObject::setVisible(bool enable) noexcept
	{
		visible_ = enable;
		this->setDirty(true);
	}

	bool
	RenderObject::getVisible() const noexcept
	{
		return visible_;
	}

	void
	RenderObject::setDirty(bool dirty) noexcept
	{
		this->dirty_ = dirty;
	}

	bool
	RenderObject::isDirty() const noexcept
	{
		return this->dirty_;
	}

	void
	RenderObject::setOwnerListener(RenderListener* listener) noexcept
	{
		this->setDirty(true);
		renderListener_ = listener;
	}

	RenderListener*
	RenderObject::getOwnerListener() noexcept
	{
		return renderListener_;
	}

	void
	RenderObject::setBoundingBox(const math::BoundingBox& bound) noexcept
	{
		this->setDirty(true);
		boundingBox_ = bound;
	}

	const math::BoundingBox& 
	RenderObject::getBoundingBox() const noexcept
	{
		return boundingBox_;
	}

	void
	RenderObject::setTransform(const math::float4x4& transform) noexcept
	{
		this->setTransform(transform, math::transformInverse(transform));
	}

	void
	RenderObject::setTransform(const math::float4x4& transform, const math::float4x4& transformInverse) noexcept
	{
		this->onMoveBefore();

		transform_ = transform;
		transformInverse_ = transformInverse;
		
		this->setDirty(true);
		this->onMoveAfter();
	}

	const math::float3&
	RenderObject::getRight() const noexcept
	{
		return transform_.getRight();
	}

	const math::float3&
	RenderObject::getUp() const noexcept
	{
		return transform_.getUp();
	}

	const math::float3&
	RenderObject::getForward() const noexcept
	{
		return transform_.getForward();
	}

	const math::float3&
	RenderObject::getTranslate() const noexcept
	{
		return transform_.getTranslate();
	}

	const math::float4x4&
	RenderObject::getTransform() const noexcept
	{
		return transform_;
	}

	const math::float4x4&
	RenderObject::getTransformInverse() const noexcept
	{
		return transformInverse_;
	}

	void
	RenderObject::onRenderBefore(const Camera& camera) noexcept
	{
		auto listener = this->getOwnerListener();
		if (listener)
			listener->onPreRender(camera);
	}

	void
	RenderObject::onRenderAfter(const Camera& camera) noexcept
	{
		auto listener = this->getOwnerListener();
		if (listener)
			listener->onPostRender(camera);
	}

	void
	RenderObject::onMoveBefore() noexcept
	{
	}

	void
	RenderObject::onMoveAfter() noexcept
	{
	}
}