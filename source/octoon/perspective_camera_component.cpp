#include <octoon/perspective_camera_component.h>

namespace octoon
{
	OctoonImplementSubClass(PerspectiveCameraComponent, CameraComponent, "PerspectiveCameraComponent")

	PerspectiveCameraComponent::PerspectiveCameraComponent() noexcept
		: CameraComponent(std::make_shared<PerspectiveCamera>())
	{
		camera_ = dynamic_cast<PerspectiveCamera*>(CameraComponent::camera_.get());
	}

	PerspectiveCameraComponent::PerspectiveCameraComponent(float fov, const math::float2& sensorSize, float znear, float zfar) noexcept
		: PerspectiveCameraComponent()
	{
		this->setFov(fov);
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
	PerspectiveCameraComponent::setFov(float fov) noexcept
	{
		camera_->setFov(fov);
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
	PerspectiveCameraComponent::getFov() const noexcept
	{
		return camera_->getFov();
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
		instance->setFov(this->getFov());
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
			this->setFov(std::any_cast<float>(data));
	}
}