#include <octoon/camera_component.h>
#include <octoon/transform_component.h>
#include <octoon/video_feature.h>

namespace octoon
{
	OctoonImplementSubInterface(CameraComponent, GameComponent, "CameraComponent")

	CameraComponent::CameraComponent() noexcept
		: camera_(std::make_shared<Camera>())
	{
		camera_->setOwnerListener(this);
	}

	CameraComponent::CameraComponent(std::shared_ptr<Camera>&& camera) noexcept
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
	CameraComponent::setClearFlags(ClearFlags clearflags) noexcept
	{
		camera_->setClearFlags(clearflags);
	}

	void
	CameraComponent::setCameraType(CameraType type) noexcept
	{
		if (type == CameraType::Main)
		{
			camera_->setRendererPriority(std::numeric_limits<std::int32_t>::max());
			camera_->setRenderToScreen(true);
		}
		else
		{
			camera_->setRendererPriority(0);
			camera_->setRenderToScreen(false);
		}

		if (this->getActive())
		{
			if (type == CameraType::Main)
			{
				auto videoFeature = this->tryGetFeature<VideoFeature>();
				if (videoFeature)
					videoFeature->setMainCamera(this);
			}
		}

		cameraType_ = type;
	}

	void
	CameraComponent::setCullingMask(std::uint32_t mask) noexcept
	{
		camera_->setCullingMask(mask);
	}

	void
	CameraComponent::setFramebuffer(const GraphicsFramebufferPtr& framebuffer) noexcept
	{
		camera_->setFramebuffer(framebuffer);
	}

	std::uint32_t
	CameraComponent::getCullingMask() const noexcept
	{
		return camera_->getCullingMask();
	}

	CameraType
	CameraComponent::getCameraType() const noexcept
	{
		return cameraType_;
	}

	ClearFlags
	CameraComponent::getClearFlags() const noexcept
	{
		return camera_->getClearFlags();
	}

	GraphicsFramebufferPtr
	CameraComponent::getFramebuffer() const noexcept
	{
		if (camera_->getFramebuffer())
			return camera_->getFramebuffer();
		auto feature = this->tryGetFeature<VideoFeature>();
		if (feature)
			return feature->getFramebuffer();
		return nullptr;
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

	math::Raycast
	CameraComponent::screenToRay(const math::float2& pos) const noexcept
	{
		auto cameraPos = camera_->getTransform().getTranslate();
		auto worldPos = camera_->screenToWorld(math::float3(pos, 0.5));
		auto ray = math::Raycast(cameraPos, math::normalize(worldPos - cameraPos));
		return ray;
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
	CameraComponent::setupFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample, GraphicsFormat format, GraphicsFormat depthStencil) except
	{
		camera_->setupFramebuffers(w, h, multisample, format, depthStencil);
	}

	void
	CameraComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);

		auto transform = this->getComponent<TransformComponent>();
		camera_->setLayer(this->getGameObject()->getLayer());
		camera_->setTransform(transform->getTransform(), transform->getTransformInverse());
		this->getFeature<VideoFeature>()->getMainScene()->addRenderObject(camera_.get());

		if (this->getCameraType() == CameraType::Main)
		{
			auto videoFeature = this->tryGetFeature<VideoFeature>();
			if (videoFeature)
				videoFeature->setMainCamera(this);
		}
	}

	void
	CameraComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->getFeature<VideoFeature>()->getMainScene()->removeRenderObject(camera_.get());

		if (this->getCameraType() == CameraType::Main)
		{
			auto videoFeature = this->tryGetFeature<VideoFeature>();
			if (videoFeature)
				videoFeature->setMainCamera(nullptr);
		}
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
	CameraComponent::onPreRender(const Camera& camera) noexcept
	{
	}

	void
	CameraComponent::onPostRender(const Camera& camera) noexcept
	{
	}

	void
	CameraComponent::onLayerChangeAfter() noexcept
	{
		camera_->setLayer(this->getGameObject()->getLayer());
	}
}