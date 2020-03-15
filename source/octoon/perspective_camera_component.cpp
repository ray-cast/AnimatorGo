#include <octoon/perspective_camera_component.h>

namespace octoon
{
	OctoonImplementSubClass(PerspectiveCameraComponent, CameraComponent, "PerspectiveCameraComponent")

	PerspectiveCameraComponent::PerspectiveCameraComponent() noexcept
		: CameraComponent(std::make_shared<camera::PerspectiveCamera>())
	{
		camera_ = dynamic_cast<camera::PerspectiveCamera*>(CameraComponent::camera_.get());
	}

	PerspectiveCameraComponent::PerspectiveCameraComponent(float fov, const math::float2& sensorSize, float znear, float zfar) noexcept
		: PerspectiveCameraComponent()
	{
		this->setAperture(fov);
		this->setSensorSize(sensorSize);
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

	void
	PerspectiveCameraComponent::setSensorSize(const math::float2& sensorSize) noexcept
	{
		camera_->setSensorSize(sensorSize);
	}

	const math::float2&
	PerspectiveCameraComponent::getSensorSize() const noexcept
	{
		return camera_->getSensorSize();
	}

	GameComponentPtr
	PerspectiveCameraComponent::clone() const noexcept
	{
		auto instance = std::make_shared<PerspectiveCameraComponent>();
		instance->setName(this->getName());
		instance->setNear(this->getNear());
		instance->setFar(this->getFar());
		instance->setAperture(this->getAperture());
		instance->setSensorSize(this->getSensorSize());
		instance->setClearColor(this->getClearColor());
		instance->setViewport(this->getViewport());
		instance->setCameraType(this->getCameraType());
		instance->setClearFlags(this->getClearFlags());

		return instance;
	}

	void 
	PerspectiveCameraComponent::onActivate() noexcept
	{
		CameraComponent::onActivate();
		
		this->addMessageListener("Camera:fov", std::bind(&PerspectiveCameraComponent::onFovChange, this, std::placeholders::_1));
	}

	void
	PerspectiveCameraComponent::onDeactivate() noexcept
	{
		CameraComponent::onDeactivate();

		this->removeMessageListener("Camera:fov", std::bind(&PerspectiveCameraComponent::onFovChange, this, std::placeholders::_1));
	}

	void
	PerspectiveCameraComponent::onFovChange(const std::any& data) noexcept
	{
		if (data.type() == typeid(float))
			this->setAperture(std::any_cast<float>(data));
	}
}