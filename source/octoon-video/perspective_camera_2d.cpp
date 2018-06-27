#include <octoon/video/perspective_camera_2d.h>
#include <octoon/video/render_system.h>

namespace octoon
{
	namespace video
	{
		OctoonImplementSubClass(PerspectiveCamera2D, Camera, "PerspectiveCamera2D")

		PerspectiveCamera2D::PerspectiveCamera2D() noexcept
			: aperture_(45.0f)
			, ratio_(1.0f)
			, znear_(0.01f)
			, zfar_(65535.0f)
			, width_(0)
			, height_(0)
			, needUpdateViewProject_(true)
		{
		}

		PerspectiveCamera2D::~PerspectiveCamera2D() noexcept
		{
		}

		void
		PerspectiveCamera2D::setAperture(float aperture) noexcept
		{
			if (aperture_ != aperture)
			{
				needUpdateViewProject_= true;
				aperture_ = aperture;
			}
		}

		float
		PerspectiveCamera2D::getAperture() const noexcept
		{
			return aperture_;
		}

		void
		PerspectiveCamera2D::setNear(float znear) noexcept
		{
			if (znear_ != znear)
			{
				needUpdateViewProject_= true;
				znear_ = znear;
			}
		}

		float
		PerspectiveCamera2D::getNear() const noexcept
		{
			return znear_;
		}

		void
		PerspectiveCamera2D::setFar(float zfar) noexcept
		{
			if (zfar_ != zfar)
			{
				needUpdateViewProject_= true;
				zfar_ = zfar;
			}
		}

		float
		PerspectiveCamera2D::getFar() const noexcept
		{
			return zfar_;
		}

		void
		PerspectiveCamera2D::setRatio(float ratio) noexcept
		{
			if (ratio_ != ratio)
			{
				needUpdateViewProject_= true;
				ratio_ = ratio;
			}
		}

		float
		PerspectiveCamera2D::getRatio() const noexcept
		{
			return ratio_;
		}

		const math::float4x4&
		PerspectiveCamera2D::getProjection() const noexcept
		{
			_updateViewProject();
			return project_;
		}

		const math::float4x4&
		PerspectiveCamera2D::getProjectionInverse() const noexcept
		{
			_updateViewProject();
			return projectInverse_;
		}

		const math::float4x4&
		PerspectiveCamera2D::getViewProjection() const noexcept
		{
			_updateViewProject();
			return viewProject_;
		}

		const math::float4x4&
		PerspectiveCamera2D::getViewProjectionInverse() const noexcept
		{
			_updateViewProject();
			return viewProjectInverse_;
		}

		void
		PerspectiveCamera2D::_updateViewProject() const noexcept
		{
			std::uint32_t width = 1920, height = 1080;

			if (!fbo_[0])
				RenderSystem::instance()->getFramebufferSize(width, height);
			else
			{
				width = fbo_[0]->getGraphicsFramebufferDesc().getWidth();
				height = fbo_[0]->getGraphicsFramebufferDesc().getHeight();
			}

			if (width_ != width || height_ != height)
			{
				math::float4x4 adjustment;
				adjustment.makeScale(1.0, -1.0, 1.0);

				project_ = adjustment * math::makePerspectiveFovLH(aperture_, ratio_ * ((float)width / height), znear_, zfar_);
				projectInverse_ = math::inverse(project_);

				width_ = width;
				height_ = height;
			}

			if (needUpdateViewProject_)
			{
				viewProject_ = project_ * this->getView();
				viewProjectInverse_ = math::inverse(viewProject_);

				needUpdateViewProject_ = false;
			}
		}

		void
		PerspectiveCamera2D::onMoveAfter() noexcept
		{
			RenderObject::onMoveAfter();
			needUpdateViewProject_= true;
		}
	}
}