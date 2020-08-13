#include <octoon/camera/ortho_camera.h>
#include <octoon/video/renderer.h>

namespace octoon::camera
{
	OctoonImplementSubClass(OrthographicCamera, Camera, "OrthographicCamera")

	OrthographicCamera::OrthographicCamera() noexcept
		: ortho_(-1.0, 1.0, -1.0, 1.0) // left, right, bottom, top
		, znear_(0.1f)
		, zfar_(65535.0f)
		, width_(0)
		, height_(0)
		, needUpdateViewProject_(true)
	{
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float znear, float zfar) noexcept
		: ortho_(left, right, bottom, top) // left, right, bottom, top
		, znear_(znear)
		, zfar_(zfar)
		, width_(0)
		, height_(0)
		, needUpdateViewProject_(true)
	{
	}

	OrthographicCamera::~OrthographicCamera() noexcept
	{
	}

	void
	OrthographicCamera::setOrtho(const math::float4& ortho) noexcept
	{
		ortho_ = ortho;
		needUpdateViewProject_= true;
		this->setDirty(true);
	}

	const math::float4&
	OrthographicCamera::getOrtho() const noexcept
	{
		return ortho_;
	}

	void
	OrthographicCamera::setNear(float znear) noexcept
	{
		if (znear_ != znear)
		{
			this->setDirty(true);
			needUpdateViewProject_= true;
			znear_ = znear;
		}
	}

	float
	OrthographicCamera::getNear() const noexcept
	{
		return znear_;
	}

	void
	OrthographicCamera::setFar(float zfar) noexcept
	{
		if (zfar_ != zfar)
		{
			this->setDirty(true);
			needUpdateViewProject_= true;
			zfar_ = zfar;
		}
	}

	float
	OrthographicCamera::getFar() const noexcept
	{
		return zfar_;
	}

	const math::float4x4&
	OrthographicCamera::getProjection() const noexcept
	{
		_updateViewProject();
		return project_;
	}

	const math::float4x4&
	OrthographicCamera::getProjectionInverse() const noexcept
	{
		_updateViewProject();
		return projectInverse_;
	}

	const math::float4x4&
	OrthographicCamera::getViewProjection() const noexcept
	{
		_updateViewProject();
		return viewProject_;
	}

	const math::float4x4&
	OrthographicCamera::getViewProjectionInverse() const noexcept
	{
		_updateViewProject();
		return viewProjectInverse_;
	}

	void
	OrthographicCamera::_updateViewProject() const noexcept
	{
		if (needUpdateViewProject_)
		{
			auto left = ortho_.x;
			auto right = ortho_.y;
			auto bottom = ortho_.z;
			auto top = ortho_.w;

			math::float4x4 adjustment;
			adjustment.makeScale(1.0, -1.0, 1.0);

			project_ = math::makeOrthoLH(left, right, bottom, top, znear_, zfar_);
			projectInverse_ = math::inverse(project_);

			viewProject_ = project_ * this->getView();
			viewProjectInverse_ = math::inverse(viewProject_);

			needUpdateViewProject_ = false;
		}
	}

	void
	OrthographicCamera::onMoveAfter() noexcept
	{
		RenderObject::onMoveAfter();
		needUpdateViewProject_= true;
	}
}