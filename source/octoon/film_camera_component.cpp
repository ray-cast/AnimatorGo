#include <octoon/film_camera_component.h>
#include <octoon/math/variant.h>

namespace octoon
{
	OctoonImplementSubClass(FilmCameraComponent, CameraComponent, "FilmCameraComponent")

	FilmCameraComponent::FilmCameraComponent() noexcept
		: CameraComponent(std::make_shared<FilmCamera>())
	{
		camera_ = dynamic_cast<FilmCamera*>(CameraComponent::camera_.get());
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
	FilmCameraComponent::setFov(float fov) noexcept
	{
		camera_->setFov(fov);
	}

	void 
	FilmCameraComponent::setFilmSize(float width) noexcept
	{
		camera_->setFilmSize(width);
	}

	void
	FilmCameraComponent::setAperture(float aperture) noexcept
	{
		camera_->setAperture(aperture);
	}

	void
	FilmCameraComponent::setFocalLength(float length) noexcept
	{
		camera_->setFocalLength(length);
	}

	void
	FilmCameraComponent::setFocusDistance(float distance) noexcept
	{
		camera_->setFocusDistance(distance);
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
	FilmCameraComponent::getFov() const noexcept
	{
		return camera_->getFov();
	}

	float
	FilmCameraComponent::getFilmSize() const noexcept
	{
		return camera_->getFilmSize();
	}

	float
	FilmCameraComponent::getAperture() const noexcept
	{
		return camera_->getAperture();
	}

	float
	FilmCameraComponent::getFocalLength() const noexcept
	{
		return camera_->getFocalLength();
	}

	float
	FilmCameraComponent::getFocalDistance() const noexcept
	{
		return camera_->getFocalDistance();
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
		instance->setFov(this->getFov());
		instance->setSensorSize(this->getSensorSize());
		instance->setClearColor(this->getClearColor());
		instance->setViewport(this->getViewport());
		instance->setCameraType(this->getCameraType());
		instance->setClearFlags(this->getClearFlags());

		return instance;
	}

	void 
	FilmCameraComponent::onActivate() noexcept
	{
		CameraComponent::onActivate();		
		this->addMessageListener("Camera:fov", std::bind(&FilmCameraComponent::onFovChange, this, std::placeholders::_1));
	}

	void
	FilmCameraComponent::onDeactivate() noexcept
	{
		CameraComponent::onDeactivate();
		this->removeMessageListener("Camera:fov", std::bind(&FilmCameraComponent::onFovChange, this, std::placeholders::_1));
	}

	void
	FilmCameraComponent::onFovChange(const std::any& data) noexcept
	{
		if (data.type() == typeid(math::Variant))
			this->setFov(std::any_cast<math::Variant>(data).getFloat());
	}
}