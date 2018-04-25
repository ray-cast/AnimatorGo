#include <octoon/camera_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(CameraComponent, GameComponent, "CameraComponent")

	CameraComponent::CameraComponent() noexcept
		: camera_(std::make_shared<video::Camera>())
	{
		camera_->setOwnerListener(this);
		camera_->setCameraType(video::CameraType::Ortho);
	}

	CameraComponent::~CameraComponent() noexcept
	{
	}

	void
	CameraComponent::setAperture(float fov) noexcept
	{
		camera_->setAperture(fov);
	}

	void
	CameraComponent::setNear(float znear) noexcept
	{
		camera_->setNear(znear);
	}

	void
	CameraComponent::setFar(float zfar) noexcept
	{
		camera_->setFar(zfar);
	}

	void
	CameraComponent::setRatio(float ratio) noexcept
	{
		camera_->setRatio(ratio);
	}

	void
	CameraComponent::setOrtho(const math::float4& ortho) noexcept
	{
		camera_->setOrtho(ortho);
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

	float
	CameraComponent::getAperture() const noexcept
	{
		return camera_->getAperture();
	}

	float
	CameraComponent::getNear() const noexcept
	{
		return camera_->getNear();
	}

	float
	CameraComponent::getFar() const noexcept
	{
		return camera_->getFar();
	}

	float
	CameraComponent::getRatio() const noexcept
	{
		return camera_->getRatio();
	}

	void
	CameraComponent::setClearFlags(graphics::GraphicsClearFlags clearflags) noexcept
	{
		camera_->setClearFlags(clearflags);
	}

	void
	CameraComponent::setCameraOrder(video::CameraOrder order) noexcept
	{
		camera_->setCameraOrder(order);
	}

	void
	CameraComponent::setCameraType(video::CameraType type) noexcept
	{
		camera_->setCameraType(type);
	}

	video::CameraOrder
	CameraComponent::getCameraOrder() const noexcept
	{
		return camera_->getCameraOrder();
	}

	video::CameraType
	CameraComponent::getCameraType() const noexcept
	{
		return camera_->getCameraType();
	}

	graphics::GraphicsClearFlags
	CameraComponent::getClearFlags() const noexcept
	{
		return camera_->getClearFlags();
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
	CameraComponent::getOrtho() const noexcept
	{
		return camera_->getOrtho();
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

	GameComponentPtr
	CameraComponent::clone() const noexcept
	{
		auto instance = std::make_shared<CameraComponent>();
		instance->setAperture(this->getAperture());
		instance->setNear(this->getNear());
		instance->setFar(this->getFar());
		instance->setRatio(this->getRatio());
		instance->setOrtho(this->getOrtho());
		instance->setClearColor(this->getClearColor());
		instance->setViewport(this->getViewport());
		instance->setCameraType(this->getCameraType());

		return instance;
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
}