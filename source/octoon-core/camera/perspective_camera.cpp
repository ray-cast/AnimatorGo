#include <octoon/camera/perspective_camera.h>
#include <octoon/video/renderer.h>

namespace octoon::camera
{
	OctoonImplementSubClass(PerspectiveCamera, Camera, "PerspectiveCamera")

	PerspectiveCamera::PerspectiveCamera() noexcept
		: aperture_(45.0f)
		, znear_(0.1f)
		, zfar_(std::numeric_limits<float>::max())
		, sensorSize_(math::float2::One)
		, width_(0)
		, height_(0)
		, needUpdateViewProject_(true)
	{
	}

	PerspectiveCamera::~PerspectiveCamera() noexcept
	{
	}

	void
	PerspectiveCamera::setAperture(float aperture) noexcept
	{
		if (aperture_ != aperture)
		{
			needUpdateViewProject_= true;
			aperture_ = aperture;
		}
	}

	float
	PerspectiveCamera::getAperture() const noexcept
	{
		return aperture_;
	}

	void
	PerspectiveCamera::setNear(float znear) noexcept
	{
		if (znear_ != znear)
		{
			needUpdateViewProject_= true;
			znear_ = znear;
		}
	}

	float
	PerspectiveCamera::getNear() const noexcept
	{
		return znear_;
	}

	void
	PerspectiveCamera::setFar(float zfar) noexcept
	{
		if (zfar_ != zfar)
		{
			needUpdateViewProject_= true;
			zfar_ = zfar;
		}
	}

	float
	PerspectiveCamera::getFar() const noexcept
	{
		return zfar_;
	}

	void
	PerspectiveCamera::setSensorSize(const math::float2& sensorSize) noexcept
	{
		if (sensorSize_ != sensorSize)
		{
			needUpdateViewProject_= true;
			sensorSize_ = sensorSize;
		}
	}

	const math::float2&
	PerspectiveCamera::getSensorSize() const noexcept
	{
		return sensorSize_;
	}

	const math::float4x4&
	PerspectiveCamera::getProjection() const noexcept
	{
		_updateViewProject();
		return project_;
	}

	const math::float4x4&
	PerspectiveCamera::getProjectionInverse() const noexcept
	{
		_updateViewProject();
		return projectInverse_;
	}

	const math::float4x4&
	PerspectiveCamera::getViewProjection() const noexcept
	{
		_updateViewProject();
		return viewProject_;
	}

	const math::float4x4&
	PerspectiveCamera::getViewProjectionInverse() const noexcept
	{
		_updateViewProject();
		return viewProjectInverse_;
	}

	void
	PerspectiveCamera::_updateViewProject() const noexcept
	{
		std::uint32_t width = 1920, height = 1080;

		if (!fbo_[0])
			video::Renderer::instance()->getFramebufferSize(width, height);
		else
		{
			width = fbo_[0]->getGraphicsFramebufferDesc().getWidth();
			height = fbo_[0]->getGraphicsFramebufferDesc().getHeight();
		}

		if (width_ != width || height_ != height)
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

			project_ = math::makePerspectiveFovLH(aperture_, sensorSize, znear_, zfar_);
			projectInverse_ = math::inverse(project_);

			viewProject_ = project_ * this->getView();
			viewProjectInverse_ = math::inverse(viewProject_);

			needUpdateViewProject_ = false;
		}
	}

	void
	PerspectiveCamera::onMoveAfter() noexcept
	{
		video::RenderObject::onMoveAfter();
		needUpdateViewProject_= true;
	}
}