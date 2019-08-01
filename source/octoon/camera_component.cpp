#include <octoon/camera_component.h>
#include <octoon/transform_component.h>
#include <octoon/video_feature.h>
#include <octoon/game_scene.h>

namespace octoon
{
	OctoonImplementSubInterface(CameraComponent, GameComponent, "CameraComponent")

	CameraComponent::CameraComponent() noexcept
		: camera_(std::make_shared<video::Camera>())
	{
		camera_->setOwnerListener(this);
	}

	CameraComponent::CameraComponent(video::CameraPtr&& camera) noexcept
		: camera_(camera)
	{
		camera_->setOwnerListener(this);
	}

	CameraComponent::~CameraComponent() noexcept
	{
	}

	void
	CameraComponent::setClearColor(const math::float4& color) noexcept
	{
		camera_->setClearColor(color);
	}

	void
	CameraComponent::setViewport(const math::float4& viewport) noexcept
	{
		camera_->setViewport(viewport);
	}

	void
	CameraComponent::setClearFlags(hal::GraphicsClearFlags clearflags) noexcept
	{
		camera_->setClearFlags(clearflags);
	}

	void
	CameraComponent::setCameraType(video::CameraType type) noexcept
	{
		camera_->setCameraType(type);
	}

	void
	CameraComponent::setFramebuffer(const hal::GraphicsFramebufferPtr& framebuffer) noexcept
	{
		camera_->setFramebuffer(framebuffer);
	}

	video::CameraType
	CameraComponent::getCameraType() const noexcept
	{
		return camera_->getCameraType();
	}

	hal::GraphicsClearFlags
	CameraComponent::getClearFlags() const noexcept
	{
		return camera_->getClearFlags();
	}

	const hal::GraphicsFramebufferPtr&
	CameraComponent::getFramebuffer() const noexcept
	{
		if (camera_->getFramebuffer())
			return camera_->getFramebuffer();
		auto feature = this->getGameObject()->getGameScene()->getFeature<VideoFeature>();
		if (feature)
			return feature->getFramebuffer();
		return nullptr;
	}

	const hal::GraphicsFramebufferPtr&
	CameraComponent::getSwapFramebuffer() const noexcept
	{
		return camera_->getSwapFramebuffer();
	}

	math::float3
	CameraComponent::worldToScreen(const math::float3& pos) const noexcept
	{
		return camera_->worldToScreen(pos);
	}

	math::float3
	CameraComponent::worldToProject(const math::float3& pos) const noexcept
	{
		return camera_->worldToProject(pos);
	}

	math::float3
	CameraComponent::screenToWorld(const math::float3& pos) const noexcept
	{
		return camera_->screenToWorld(pos);
	}

	math::float3
	CameraComponent::screenToView(const math::float2& pos) const noexcept
	{
		return camera_->screenToView(pos);
	}

	const math::float4&
	CameraComponent::getClearColor() const noexcept
	{
		return camera_->getClearColor();
	}

	const math::float4&
	CameraComponent::getViewport() const noexcept
	{
		return camera_->getViewport();
	}

	const math::float4&
	CameraComponent::getPixelViewport() const noexcept
	{
		return camera_->getPixelViewport();
	}

	const math::float4x4&
	CameraComponent::getView() const noexcept
	{
		return camera_->getView();
	}

	const math::float4x4&
	CameraComponent::getViewInverse() const noexcept
	{
		return camera_->getViewInverse();
	}

	const math::float4x4&
	CameraComponent::getViewProjection() const noexcept
	{
		return camera_->getViewProjection();
	}

	const math::float4x4&
	CameraComponent::getViewProjectionInverse() const noexcept
	{
		return camera_->getViewProjectionInverse();
	}

	const math::float4x4&
	CameraComponent::getProjection() const noexcept
	{
		return camera_->getProjection();
	}

	const math::float4x4&
	CameraComponent::getProjectionInverse() const noexcept
	{
		return camera_->getProjectionInverse();
	}

	void
	CameraComponent::setupFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample, hal::GraphicsFormat format, hal::GraphicsFormat depthStencil) except
	{
		camera_->setupFramebuffers(w, h, multisample, format, depthStencil);
	}

	void
	CameraComponent::setupSwapFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample, hal::GraphicsFormat format, hal::GraphicsFormat depthStencil) except
	{
		camera_->setupSwapFramebuffers(w, h, multisample, format, depthStencil);
	}

	void
	CameraComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);

		auto transform = this->getComponent<TransformComponent>();
		camera_->setActive(true);
		camera_->setTransform(transform->getTransform(), transform->getTransformInverse());
	}

	void
	CameraComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		camera_->setActive(false);
	}

	void
	CameraComponent::onMoveBefore() noexcept
	{
	}

	void
	CameraComponent::onMoveAfter() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		camera_->setTransform(transform->getTransform(), transform->getTransformInverse());
	}

	void
	CameraComponent::onRenderObjectPre(const video::Camera& camera) noexcept
	{
	}

	void
	CameraComponent::onRenderObjectPost(const video::Camera& camera) noexcept
	{
	}

	void
	CameraComponent::onLayerChangeAfter() noexcept
	{
		camera_->setLayer(this->getGameObject()->getLayer());
	}
}