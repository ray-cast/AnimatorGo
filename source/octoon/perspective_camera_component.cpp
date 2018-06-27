#include <octoon/perspective_camera_component.h>

namespace octoon
{
	OctoonImplementSubClass(PerspectiveCameraComponent, CameraComponent, "PerspectiveCameraComponent")

	PerspectiveCameraComponent::PerspectiveCameraComponent() noexcept
		: CameraComponent(std::make_shared<video::PerspectiveCamera>())
	{
		camera_ = dynamic_cast<video::PerspectiveCamera*>(CameraComponent::camera_.get());
	}

	PerspectiveCameraComponent::PerspectiveCameraComponent(float fov, float ratio, float znear, float zfar) noexcept
		: PerspectiveCameraComponent()
	{
		this->setAperture(fov);
		this->setRatio(ratio);
		this->setNear(znear);
		this->setFar(zfar);
	}

	PerspectiveCameraComponent::~PerspectiveCameraComponent() noexcept
	{
	}

	void
	PerspectiveCameraComponent::setNear(float znear) noexcept
	{
		camera_->setNear(znear);
	}

	void
	PerspectiveCameraComponent::setFar(float zfar) noexcept
	{
		camera_->setFar(zfar);
	}

	void
	PerspectiveCameraComponent::setAperture(float fov) noexcept
	{
		camera_->setAperture(fov);
	}

	void
	PerspectiveCameraComponent::setRatio(float ratio) noexcept
	{
		camera_->setRatio(ratio);
	}

	float
	PerspectiveCameraComponent::getNear() const noexcept
	{
		return camera_->getNear();
	}

	float
	PerspectiveCameraComponent::getFar() const noexcept
	{
		return camera_->getFar();
	}

	float
	PerspectiveCameraComponent::getAperture() const noexcept
	{
		return camera_->getAperture();
	}

	float
	PerspectiveCameraComponent::getRatio() const noexcept
	{
		return camera_->getRatio();
	}

	GameComponentPtr
	PerspectiveCameraComponent::clone() const noexcept
	{
		auto instance = std::make_shared<PerspectiveCameraComponent>();
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