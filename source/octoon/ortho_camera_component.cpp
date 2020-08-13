#include <octoon/ortho_camera_component.h>

namespace octoon
{
	OctoonImplementSubClass(OrthoCameraComponent, CameraComponent, "OrthoCameraComponent")

	OrthoCameraComponent::OrthoCameraComponent() noexcept
		: CameraComponent(std::make_shared<camera::OrthographicCamera>())
	{
		camera_ = dynamic_cast<camera::OrthographicCamera*>(CameraComponent::camera_.get());
	}

	OrthoCameraComponent::OrthoCameraComponent(const math::float4& ortho, float znear, float zfar) noexcept
		: OrthoCameraComponent()
	{
		this->setOrtho(ortho);
		this->setNear(znear);
		this->setFar(zfar);
	}

	OrthoCameraComponent::OrthoCameraComponent(float left, float right, float bottom, float top, float znear, float zfar) noexcept
		: OrthoCameraComponent(math::float4(left, right, bottom, top), znear, zfar)
	{
	}

	OrthoCameraComponent::~OrthoCameraComponent() noexcept
	{
	}

	void
	OrthoCameraComponent::setNear(float znear) noexcept
	{
		camera_->setNear(znear);
	}

	void
	OrthoCameraComponent::setFar(float zfar) noexcept
	{
		camera_->setFar(zfar);
	}

	void
	OrthoCameraComponent::setOrtho(const math::float4& ortho) noexcept
	{
		camera_->setOrtho(ortho);
	}

	float
	OrthoCameraComponent::getNear() const noexcept
	{
		return camera_->getNear();
	}

	float
	OrthoCameraComponent::getFar() const noexcept
	{
		return camera_->getFar();
	}

	const math::float4&
	OrthoCameraComponent::getOrtho() const noexcept
	{
		return camera_->getOrtho();
	}

	GameComponentPtr
	OrthoCameraComponent::clone() const noexcept
	{
		auto instance = std::make_shared<OrthoCameraComponent>();
		instance->setName(this->getName());
		instance->setNear(this->getNear());
		instance->setFar(this->getFar());
		instance->setOrtho(this->getOrtho());
		instance->setClearColor(this->getClearColor());
		instance->setViewport(this->getViewport());
		instance->setCameraType(this->getCameraType());
		instance->setClearFlags(this->getClearFlags());

		return instance;
	}
}