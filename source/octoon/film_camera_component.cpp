#include <octoon/film_camera_component.h>

namespace octoon
{
	OctoonImplementSubClass(FilmCameraComponent, CameraComponent, "FilmCameraComponent")

	FilmCameraComponent::FilmCameraComponent() noexcept
		: CameraComponent(std::make_shared<camera::FilmCamera>())
	{
		camera_ = dynamic_cast<camera::FilmCamera*>(CameraComponent::camera_.get());
	}

	FilmCameraComponent::~FilmCameraComponent() noexcept
	{
	}

	void
	FilmCameraComponent::setNear(float znear) noexcept
	{
		camera_->setNear(znear);
	}

	void
	FilmCameraComponent::setFar(float zfar) noexcept
	{
		camera_->setFar(zfar);
	}

	void
	FilmCameraComponent::setZoom(float zoom) noexcept
	{
		camera_->setZoom(zoom);
	}
	
	void
	FilmCameraComponent::setAperture(float fov) noexcept
	{
		camera_->setAperture(fov);
	}

	void 
	FilmCameraComponent::setFilmSize(float width) noexcept
	{
		camera_->setFilmSize(width);
	}

	void
	FilmCameraComponent::setFocalLength(float length) noexcept
	{
		camera_->setFocalLength(length);
	}

	void
	FilmCameraComponent::setCanvasWidth(float width) noexcept
	{
		camera_->setCanvasWidth(width);
	}

	float
	FilmCameraComponent::getNear() const noexcept
	{
		return camera_->getNear();
	}

	float
	FilmCameraComponent::getFar() const noexcept
	{
		return camera_->getFar();
	}

	float
	FilmCameraComponent::getZoom() const noexcept
	{
		return camera_->getZoom();
	}

	float
	FilmCameraComponent::getAperture() const noexcept
	{
		return camera_->getAperture();
	}

	float
	FilmCameraComponent::getFilmSize() const noexcept
	{
		return camera_->getFilmSize();
	}

	float
	FilmCameraComponent::getFocalLength() const noexcept
	{
		return camera_->getFocalLength();
	}

	float
	FilmCameraComponent::getCanvasWidth() const noexcept
	{
		return camera_->getCanvasWidth();
	}

	void
	FilmCameraComponent::setSensorSize(const math::float2& sensorSize) noexcept
	{
		camera_->setSensorSize(sensorSize);
	}

	const math::float2&
	FilmCameraComponent::getSensorSize() const noexcept
	{
		return camera_->getSensorSize();
	}

	GameComponentPtr
	FilmCameraComponent::clone() const noexcept
	{
		auto instance = std::make_shared<FilmCameraComponent>();
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
}