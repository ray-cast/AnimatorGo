#include <octoon/camera/film_camera.h>
#include <octoon/video/renderer.h>

namespace octoon::camera
{
	OctoonImplementSubClass(FilmCamera, Camera, "FilmCamera")

	FilmCamera::FilmCamera() noexcept
		: aperture_(45.0f)
		, znear_(0.1f)
		, zfar_(std::numeric_limits<float>::max())
		, width_(0)
		, height_(0)
		, zoom_(0)
		, filmSize_(36.0f) // 35mm
		, focalLength_(50.0f) // 50mm
		, sensorSize_(math::float2::One)
		, canvasWidth_(720.0f)
		, needUpdateViewProject_(true)
	{
	}

	FilmCamera::~FilmCamera() noexcept
	{
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
	FilmCamera::setFilmSize(float filmSize) noexcept
	{
		if (filmSize_ != filmSize)
		{
			this->setZoom(focalLength_ / filmSize * canvasWidth_);
			filmSize_ = filmSize;
		}
	}

	void
	FilmCamera::setZoom(float zoom) noexcept
	{
		if (zoom_ != zoom)
		{
			this->setDirty(true);
			needUpdateViewProject_ = true;
			aperture_ = math::degrees(std::atan(canvasWidth_ / zoom * 0.5f)) * 2.0f;
			zoom_ = zoom;
		}
	}

	void
	FilmCamera::setCanvasWidth(float width) noexcept
	{
		if (canvasWidth_ != width)
		{
			this->setDirty(true);
			needUpdateViewProject_ = true;
			aperture_ = math::degrees(std::atan(width / zoom_ * 0.5f)) * 2.0f;
			canvasWidth_ = width;
		}
	}

	void
	FilmCamera::setFocalLength(float length) noexcept
	{
		if (focalLength_ != length)
		{
			float comp = canvasWidth_;
			float zoom = length / filmSize_ * comp;
			this->setZoom(zoom);
			focalLength_ = length;
		}
	}

	void
	FilmCamera::setAperture(float aperture) noexcept
	{
		if (aperture_ != aperture)
		{
			auto ratio = std::tan(math::radians(aperture) * 0.5f) * 2.0f;
			auto length = filmSize_ / ratio;
			this->setFocalLength(length);
			aperture_ = aperture;
		}
	}

	float
	FilmCamera::getAperture() const noexcept
	{
		return aperture_;
	}

	float
	FilmCamera::getZoom() const noexcept
	{
		return aperture_;
	}

	float
	FilmCamera::getFilmSize() const noexcept
	{
		return filmSize_;
	}

	float
	FilmCamera::getFocalLength() const noexcept
	{
		return focalLength_;
	}

	float
	FilmCamera::getCanvasWidth() const noexcept
	{
		return canvasWidth_;
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

		if (width_ != width || height_ != height)
		{
			width_ = width;
			height_ = height;

			needUpdateViewProject_ = true;
		}

		if (needUpdateViewProject_)
		{
			math::float4x4 adjustment;
			adjustment.makeScale(1.0, -1.0, 1.0);

			math::float2 sensorSize;
			sensorSize.x = sensorSize_.x;
			sensorSize.y = sensorSize_.y * ((float)width / height);

			project_ = math::makePerspectiveFovLH(aperture_, sensorSize, znear_, zfar_);
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