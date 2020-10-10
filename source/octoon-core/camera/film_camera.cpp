#include <octoon/camera/film_camera.h>
#include <octoon/video/renderer.h>

namespace octoon::camera
{
	OctoonImplementSubClass(FilmCamera, Camera, "FilmCamera")

	FilmCamera::FilmCamera() noexcept
		: fov_(45.0f)
		, aperture_(0.0f)
		, filmSize_(36.0f) // 35mm
		, focalLength_(50.0f) // 50mm
		, focalDistance_(10.0f) // 50mm
		, zoom_(0.0f)
		, sensorSize_(math::float2::One)
		, znear_(0.1f)
		, zfar_(std::numeric_limits<float>::max())
		, width_(0)
		, height_(0)
		, needUpdateViewProject_(true)
	{
	}

	FilmCamera::~FilmCamera() noexcept
	{
	}

	void
	FilmCamera::setFov(float aperture) noexcept
	{
		if (fov_ != aperture)
		{
			auto ratio = std::tan(math::radians(aperture) * 0.5f) * 2.0f;
			auto length = filmSize_ / ratio;
			this->setFocalLength(length);
			fov_ = aperture;
		}
	}

	void
	FilmCamera::setFilmSize(float filmSize) noexcept
	{
		if (filmSize_ != filmSize)
		{
			auto viewport = this->getPixelViewport();
			this->setZoom(focalLength_ / filmSize * viewport.width);
			filmSize_ = filmSize;
		}
	}

	void
	FilmCamera::setAperture(float aperture) noexcept
	{
		this->setDirty(true);
		this->aperture_ = aperture;
	}

	void
	FilmCamera::setFocalLength(float length) noexcept
	{
		if (focalLength_ != length)
		{
			float comp = this->getPixelViewport().width;
			float zoom = length / filmSize_ * comp;
			this->setZoom(zoom);
			focalLength_ = length;
		}
	}

	void
	FilmCamera::setFocalDistance(float distance) noexcept
	{
		if (focalDistance_ != distance)
		{
			this->setDirty(true);
			focalDistance_ = distance;
		}
	}

	void
	FilmCamera::setZoom(float zoom) noexcept
	{
		if (zoom_ != zoom)
		{
			this->setDirty(true);
			needUpdateViewProject_ = true;
			fov_ = math::degrees(std::atan(this->getPixelViewport().width / zoom * 0.5f)) * 2.0f;
			zoom_ = zoom;
		}
	}

	void
	FilmCamera::setCanvasWidth(float width) noexcept
	{
		if (this->getPixelViewport().width != width)
		{
			this->setDirty(true);
			needUpdateViewProject_ = true;
			fov_ = math::degrees(std::atan(width / zoom_ * 0.5f)) * 2.0f;
		}
	}

	void
	FilmCamera::setSensorSize(const math::float2& sensorSize) noexcept
	{
		if (sensorSize_ != sensorSize)
		{
			this->setDirty(true);
			needUpdateViewProject_= true;
			sensorSize_ = sensorSize;
		}
	}

	void
	FilmCamera::setNear(float znear) noexcept
	{
		if (znear_ != znear)
		{
			this->setDirty(true);
			needUpdateViewProject_= true;
			znear_ = znear;
		}
	}

	void
	FilmCamera::setFar(float zfar) noexcept
	{
		if (zfar_ != zfar)
		{
			this->setDirty(true);
			needUpdateViewProject_= true;
			zfar_ = zfar;
		}
	}

	float
	FilmCamera::getFov() const noexcept
	{
		return fov_;
	}

	float
	FilmCamera::getFilmSize() const noexcept
	{
		return filmSize_;
	}

	float
	FilmCamera::getAperture() const noexcept
	{
		return this->aperture_;
	}

	float
	FilmCamera::getFocalLength() const noexcept
	{
		return focalLength_;
	}

	float
	FilmCamera::getFocalDistance() const noexcept
	{
		return this->focalDistance_;
	}

	float
	FilmCamera::getZoom() const noexcept
	{
		return zoom_;
	}

	float
	FilmCamera::getNear() const noexcept
	{
		return znear_;
	}

	float
	FilmCamera::getFar() const noexcept
	{
		return zfar_;
	}

	const math::float2&
	FilmCamera::getSensorSize() const noexcept
	{
		return sensorSize_;
	}

	const math::float4x4&
	FilmCamera::getProjection() const noexcept
	{
		_updateViewProject();
		return project_;
	}

	const math::float4x4&
	FilmCamera::getProjectionInverse() const noexcept
	{
		_updateViewProject();
		return projectInverse_;
	}

	const math::float4x4&
	FilmCamera::getViewProjection() const noexcept
	{
		_updateViewProject();
		return viewProject_;
	}

	const math::float4x4&
	FilmCamera::getViewProjectionInverse() const noexcept
	{
		_updateViewProject();
		return viewProjectInverse_;
	}

	void
	FilmCamera::_updateViewProject() const noexcept
	{
		std::uint32_t width = 1920, height = 1080;

		if (!fbo_[0])
			video::Renderer::instance()->getFramebufferSize(width, height);
		else
		{
			width = fbo_[0]->getFramebufferDesc().getWidth();
			height = fbo_[0]->getFramebufferDesc().getHeight();
		}

		if (width_ != width || height_ != height || this->isDirty())
		{
			width_ = width;
			height_ = height;

			needUpdateViewProject_ = true;
		}

		if (needUpdateViewProject_)
		{
			math::float2 sensorSize;
			sensorSize.x = sensorSize_.x * ((float)width / height);
			sensorSize.y = sensorSize_.y;

			project_ = math::makePerspectiveFovLH(fov_, sensorSize, znear_, zfar_);
			projectInverse_ = math::inverse(project_);

			viewProject_ = project_ * this->getView();
			viewProjectInverse_ = math::inverse(viewProject_);

			needUpdateViewProject_ = false;
		}
	}

	void
	FilmCamera::onMoveAfter() noexcept
	{
		RenderObject::onMoveAfter();
		needUpdateViewProject_= true;
	}
}