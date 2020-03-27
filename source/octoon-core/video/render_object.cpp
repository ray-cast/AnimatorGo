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
		: active_(false)
		, visible_(true)
		, layer_(0)
		, order_(0)
		, transform_(math::float4x4::One)
		, transformInverse_(math::float4x4::One)
		, renderListener_(nullptr)
	{
	}

	RenderObject::~RenderObject() noexcept
	{
		this->setActive(false);
	}

	void
	RenderObject::setActive(bool active) noexcept
	{
		if (active_ != active)
		{
			if (active)
				this->onActivate();
			else
				this->onDeactivate();

			active_ = active;
		}
	}

	bool
	RenderObject::getActive() const noexcept
	{
		return active_;
	}

	void
	RenderObject::setLayer(std::uint8_t layer) noexcept
	{
		layer_ = layer;
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
	}

	bool
	RenderObject::getVisible() const noexcept
	{
		return visible_;
	}

	void
	RenderObject::setOwnerListener(RenderListener* listener) noexcept
	{
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
	RenderObject::onRenderBefore(const camera::Camera& camera) noexcept
	{
		auto listener = this->getOwnerListener();
		if (listener)
			listener->onPreRender(camera);
	}

	void
	RenderObject::onRenderAfter(const camera::Camera& camera) noexcept
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

	void
	RenderObject::onActivate() noexcept
	{
		RenderScene::instance()->addRenderObject(this);
	}

	void
	RenderObject::onDeactivate() noexcept
	{
		RenderScene::instance()->removeRenderObject(this);
	}
}