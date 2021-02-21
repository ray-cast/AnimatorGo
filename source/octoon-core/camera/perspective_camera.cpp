#include <octoon/camera/perspective_camera.h>
#include <octoon/video/renderer.h>

namespace octoon
{
	OctoonImplementSubClass(PerspectiveCamera, Camera, "PerspectiveCamera")

	PerspectiveCamera::PerspectiveCamera() noexcept
		: PerspectiveCamera(45.0f, 0.1f, std::numeric_limits<float>::max())
	{
	}

	PerspectiveCamera::PerspectiveCamera(float fov, float znear, float zfar) noexcept
		: fov_(fov)
		, znear_(znear)
		, zfar_(zfar)
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
	PerspectiveCamera::setFov(float aperture) noexcept
	{
		if (fov_ != aperture)
		{
			this->setDirty(true);
			needUpdateViewProject_= true;
			fov_ = aperture;
		}
	}

	float
	PerspectiveCamera::getFov() const noexcept
	{
		return fov_;
	}

	void
	PerspectiveCamera::setNear(float znear) noexcept
	{
		if (znear_ != znear)
		{
			this->setDirty(true);
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
			this->setDirty(true);
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
			this->setDirty(true);
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
	PerspectiveCamera::onMoveAfter() noexcept
	{
		video::RenderObject::onMoveAfter();
		needUpdateViewProject_= true;
	}
}