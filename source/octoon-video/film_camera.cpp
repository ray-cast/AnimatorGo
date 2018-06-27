#include <octoon/video/film_camera.h>
#include <octoon/video/render_system.h>

namespace octoon
{
	namespace video
	{
		OctoonImplementSubClass(FilmCamera, Camera, "FilmCamera")

		FilmCamera::FilmCamera() noexcept
			: aperture_(45.0f)
			, ratio_(1.0f)
			, znear_(0.01f)
			, zfar_(65535.0f)
			, width_(0)
			, height_(0)
			, filmSize_(36.0f) // 35mm
			, focalLength_(50.0f) // 50mm
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
				needUpdateViewProject_= true;
				znear_ = znear;
			}
		}

		void
		FilmCamera::setFar(float zfar) noexcept
		{
			if (zfar_ != zfar)
			{
				needUpdateViewProject_= true;
				zfar_ = zfar;
			}
		}

		void
		FilmCamera::setRatio(float ratio) noexcept
		{
			if (ratio_ != ratio)
			{
				needUpdateViewProject_= true;
				ratio_ = ratio;
			}
		}

		void
		FilmCamera::setZoom(float zoom) noexcept
		{
			if (zoom_ != zoom)
			{
				float comp = this->getPixelViewport().width;
				aperture_ = math::degress(std::atan(comp / zoom * 0.5f)) * 2.0f;
				needUpdateViewProject_ = true;
				zoom_ = zoom;
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

		void
		FilmCamera::setFilmSize(float filmSize) noexcept
		{
			if (filmSize_ != filmSize)
			{
				float comp = this->getPixelViewport().w;
				float zoom = focalLength_ / filmSize * comp;
				this->setZoom(zoom);
				filmSize_ = filmSize;
			}
		}

		void
		FilmCamera::setFocalLength(float length) noexcept
		{
			if (focalLength_ != length)
			{
				float comp = this->getPixelViewport().w;
				float zoom = length / filmSize_ * comp;
				this->setZoom(zoom);
				focalLength_ = length;
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
			return aperture_;
		}

		float
		FilmCamera::getFocalLength() const noexcept
		{
			return aperture_;
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

		float
		FilmCamera::getRatio() const noexcept
		{
			return ratio_;
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

				float fov = math::degress(std::atan(width / zoom_ * 0.5f)) * 2.0f;
				project_ = adjustment * math::makePerspectiveFovLH(fov, ratio_ * ((float)width / height), znear_, zfar_);
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
		FilmCamera::onMoveAfter() noexcept
		{
			RenderObject::onMoveAfter();
			needUpdateViewProject_= true;
		}
	}
}