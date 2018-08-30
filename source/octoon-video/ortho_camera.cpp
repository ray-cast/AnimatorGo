#include <octoon/video/ortho_camera.h>
#include <octoon/video/render_system.h>

namespace octoon
{
	namespace video
	{
		OctoonImplementSubClass(OrthoCamera, Camera, "OrthoCamera")

		OrthoCamera::OrthoCamera() noexcept
			: ortho_(-1.0, 1.0, -1.0, 1.0) // left, right, bottom, top
			, znear_(0.01f)
			, zfar_(65535.0f)
			, width_(0)
			, height_(0)
			, needUpdateViewProject_(true)
		{
		}

		OrthoCamera::~OrthoCamera() noexcept
		{
		}

		void
		OrthoCamera::setOrtho(const math::float4& ortho) noexcept
		{
			ortho_ = ortho;
			needUpdateViewProject_= true;
		}

		const math::float4&
		OrthoCamera::getOrtho() const noexcept
		{
			return ortho_;
		}

		void
		OrthoCamera::setNear(float znear) noexcept
		{
			if (znear_ != znear)
			{
				needUpdateViewProject_= true;
				znear_ = znear;
			}
		}

		float
		OrthoCamera::getNear() const noexcept
		{
			return znear_;
		}

		void
		OrthoCamera::setFar(float zfar) noexcept
		{
			if (zfar_ != zfar)
			{
				needUpdateViewProject_= true;
				zfar_ = zfar;
			}
		}

		float
		OrthoCamera::getFar() const noexcept
		{
			return zfar_;
		}

		const math::float4x4&
		OrthoCamera::getProjection() const noexcept
		{
			_updateViewProject();
			return project_;
		}

		const math::float4x4&
		OrthoCamera::getProjectionInverse() const noexcept
		{
			_updateViewProject();
			return projectInverse_;
		}

		const math::float4x4&
		OrthoCamera::getViewProjection() const noexcept
		{
			_updateViewProject();
			return viewProject_;
		}

		const math::float4x4&
		OrthoCamera::getViewProjectionInverse() const noexcept
		{
			_updateViewProject();
			return viewProjectInverse_;
		}

		void
		OrthoCamera::_updateViewProject() const noexcept
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
				width_ = width;
				height_ = height;

				needUpdateViewProject_ = true;
			}

			if (needUpdateViewProject_)
			{
				auto left = width * ortho_.x;
				auto right = width * ortho_.y;
				auto bottom = height * ortho_.z;
				auto top = height * ortho_.w;

				math::float4x4 adjustment;
				adjustment.makeScale(1.0, -1.0, 1.0);

				project_ = adjustment * math::makeOrthoLH(left, right, bottom, top, znear_, zfar_);
				projectInverse_ = math::inverse(project_);

				viewProject_ = project_ * this->getView();
				viewProjectInverse_ = math::inverse(viewProject_);

				needUpdateViewProject_ = false;
			}
		}

		void
		OrthoCamera::onMoveAfter() noexcept
		{
			RenderObject::onMoveAfter();
			needUpdateViewProject_= true;
		}
	}
}