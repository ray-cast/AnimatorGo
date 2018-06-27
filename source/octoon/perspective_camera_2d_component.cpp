#include <octoon/perspective_camera_2d_component.h>

namespace octoon
{
	OctoonImplementSubClass(PerspectiveCamera2DComponent, CameraComponent, "PerspectiveCamera2DComponent")

	PerspectiveCamera2DComponent::PerspectiveCamera2DComponent() noexcept
		: CameraComponent(std::make_shared<video::PerspectiveCamera2D>())
	{
		camera_ = dynamic_cast<video::PerspectiveCamera2D*>(CameraComponent::camera_.get());
	}

	PerspectiveCamera2DComponent::PerspectiveCamera2DComponent(float fov, float ratio, float znear, float zfar) noexcept
		: PerspectiveCamera2DComponent()
	{
		this->setAperture(fov);
		this->setRatio(ratio);
		this->setNear(znear);
		this->setFar(zfar);
	}

	PerspectiveCamera2DComponent::~PerspectiveCamera2DComponent() noexcept
	{
	}

	void
	PerspectiveCamera2DComponent::setNear(float znear) noexcept
	{
		camera_->setNear(znear);
	}

	void
	PerspectiveCamera2DComponent::setFar(float zfar) noexcept
	{
		camera_->setFar(zfar);
	}

	void
	PerspectiveCamera2DComponent::setAperture(float fov) noexcept
	{
		camera_->setAperture(fov);
	}

	void
	PerspectiveCamera2DComponent::setRatio(float ratio) noexcept
	{
		camera_->setRatio(ratio);
	}

	float
	PerspectiveCamera2DComponent::getNear() const noexcept
	{
		return camera_->getNear();
	}

	float
	PerspectiveCamera2DComponent::getFar() const noexcept
	{
		return camera_->getFar();
	}

	float
	PerspectiveCamera2DComponent::getAperture() const noexcept
	{
		return camera_->getAperture();
	}

	float
	PerspectiveCamera2DComponent::getRatio() const noexcept
	{
		return camera_->getRatio();
	}

	GameComponentPtr
	PerspectiveCamera2DComponent::clone() const noexcept
	{
		auto instance = std::make_shared<PerspectiveCamera2DComponent>();
		instance->setName(this->getName());
		instance->setNear(this->getNear());
		instance->setFar(this->getFar());
		instance->setAperture(this->getAperture());
		instance->setRatio(this->getRatio());
		instance->setClearColor(this->getClearColor());
		instance->setViewport(this->getViewport());
		instance->setCameraOrder(this->getCameraOrder());
		instance->setClearFlags(this->getClearFlags());

		return instance;
	}
}