#include <octoon/video/camera.h>
#include <octoon/video/render_system.h>
#include <octoon/runtime/except.h>
#include <octoon/graphics/graphics.h>

using namespace octoon::graphics;

namespace octoon
{
	namespace video
	{
		OctoonImplementSubClass(Camera, runtime::RttiInterface, "Camera")

			Camera::Camera() noexcept
			: ortho_(-1.0, 1.0, -1.0, 1.0) // left, right, bottom, top
			, aperture_(45.0f)
			, ratio_(1.0f)
			, ratioReal_(1.0f)
			, znear_(0.01f)
			, zfar_(65535.0f)
			, viewport_(0.0f, 0.0f, 1.0f, 1.0f)
			, clearColor_(math::float4(0.0f, 0.0f, 0.0f, 1.0f))
			, cameraOrder_(CameraOrder::Custom)
			, cameraType_(CameraType::Perspective)
			, needUpdateViewProject_(true)
			, project_(math::float4x4::One)
			, projectInverse_(math::float4x4::One)
			, viewProject_(math::float4x4::One)
			, viewProjectInverse_(math::float4x4::One)
			, clearflags_(graphics::GraphicsClearFlagBits::AllBit)
		{
		}

		Camera::~Camera() noexcept
		{
		}

		void
		Camera::setAperture(float aspect) noexcept
		{
			aperture_ = aspect;
			needUpdateViewProject_= true;
		}

		float
		Camera::getAperture() const noexcept
		{
			return aperture_;
		}

		void
		Camera::setNear(float znear) noexcept
		{
			znear_ = znear;
			needUpdateViewProject_= true;
		}

		float
		Camera::getNear() const noexcept
		{
			return znear_;
		}

		void
		Camera::setFar(float zfar) noexcept
		{
			zfar_ = zfar;
			needUpdateViewProject_= true;
		}

		float
		Camera::getFar() const noexcept
		{
			return zfar_;
		}

		void
		Camera::setRatio(float ratio) noexcept
		{
			ratio_ = ratio;
			needUpdateViewProject_= true;
		}

		float
		Camera::getRatio() const noexcept
		{
			return ratio_;
		}

		void
		Camera::setOrtho(const math::float4& ortho) noexcept
		{
			ortho_ = ortho;
			needUpdateViewProject_= true;
		}

		const math::float4&
		Camera::getOrtho() const noexcept
		{
			return ortho_;
		}

		const math::float4x4&
		Camera::getView() const noexcept
		{
			return this->getTransformInverse();
		}

		const math::float4x4&
		Camera::getViewInverse() const noexcept
		{
			return this->getTransform();
		}

		const math::float4x4&
		Camera::getProjection() const noexcept
		{
			_updateViewProject();
			return project_;
		}

		const math::float4x4&
		Camera::getProjectionInverse() const noexcept
		{
			_updateViewProject();
			return projectInverse_;
		}

		const math::float4x4&
		Camera::getViewProjection() const noexcept
		{
			_updateViewProject();
			return viewProject_;
		}

		const math::float4x4&
		Camera::getViewProjectionInverse() const noexcept
		{
			_updateViewProject();
			return viewProjectInverse_;
		}

		math::float3
		Camera::worldToScreen(const math::float3& pos) const noexcept
		{
			math::float4 viewport = this->getPixelViewport();

			float w = viewport.z * 0.5f;
			float h = viewport.w * 0.5f;

			math::float4 v = this->getViewProjection() * math::float4(pos, 1.0);
			if (v.w != 0)
				v /= v.w;

			v.x = v.x * w + w + viewport.x;
			v.y = v.y * h + h + viewport.y;

			return math::float3(v.x, v.y, v.z);
		}

		math::float3
		Camera::worldToProject(const math::float3& pos) const noexcept
		{
			math::float4 v(pos, 1.0);

			v = this->getViewProjection() * v;
			if (v.w != 0)
				v /= v.w;

			return v.xyz();
		}

		math::float3
		Camera::screenToWorld(const math::float3& pos) const noexcept
		{
			math::float4 viewport = this->getPixelViewport();

			math::float4 v(pos, 1.0);

			v.y = viewport.w - v.y; // opengl

			v.x = ((v.x - viewport.x) / viewport.z) * 2.0f - 1.0f;
			v.y = ((v.y - viewport.y) / viewport.w) * 2.0f - 1.0f;

			v = this->getViewProjectionInverse() * v;
			if (v.w != 0)
				v /= v.w;

			return math::float3(v.x, v.y, v.z);
		}

		math::float3
		Camera::screenToView(const math::float2& pos) const noexcept
		{
			math::float4 viewport = this->getPixelViewport();

			math::float4 v(pos, 1.0, 1.0);

			v.y = viewport.w - v.y; // opengl

			v.x = ((v.x - viewport.x) / viewport.z) * 2.0f - 1.0f;
			v.y = ((v.y - viewport.y) / viewport.w) * 2.0f - 1.0f;

			return (this->getProjectionInverse() * v).xyz();
		}

		void
		Camera::setClearColor(const math::float4& color) noexcept
		{
			clearColor_ = color;
		}

		const math::float4&
		Camera::getClearColor() const noexcept
		{
			return clearColor_;
		}

		void
		Camera::setViewport(const math::float4& viewport) noexcept
		{
			viewport_ = viewport;
		}

		const math::float4&
		Camera::getViewport() const noexcept
		{
			return viewport_;
		}

		const math::float4&
		Camera::getPixelViewport() const noexcept
		{
			std::uint32_t width = 1920, height = 1080;

			if (!fbo_[0])
				RenderSystem::instance()->getFramebufferSize(width, height);
			else
			{
				width = fbo_[0]->getGraphicsFramebufferDesc().getWidth();
				height = fbo_[0]->getGraphicsFramebufferDesc().getHeight();
			}

			math::float4 result;
			result.x = viewport_.x * width;
			result.y = viewport_.y * height;
			result.z = viewport_.z * width;
			result.w = viewport_.w * height;
			screen_ = result;

			return screen_;
		}

		void
		Camera::setClearFlags(graphics::GraphicsClearFlags clearflags) noexcept
		{
			clearflags_ = clearflags;
		}

		void
		Camera::setCameraOrder(CameraOrder order) noexcept
		{
			cameraOrder_ = order;
		}

		void
		Camera::setCameraType(CameraType type) noexcept
		{
			if (cameraType_ != type)
			{
				needUpdateViewProject_= true;
				cameraType_ = type;
			}
		}

		void
		Camera::setFramebuffer(const graphics::GraphicsFramebufferPtr& framebuffer) noexcept
		{
			fbo_[0] = framebuffer;
		}

		CameraOrder
		Camera::getCameraOrder() const noexcept
		{
			return cameraOrder_;
		}

		CameraType
		Camera::getCameraType() const noexcept
		{
			return cameraType_;
		}

		graphics::GraphicsClearFlags
		Camera::getClearFlags() const noexcept
		{
			return clearflags_;
		}

		const graphics::GraphicsFramebufferPtr&
		Camera::getFramebuffer() const noexcept
		{
			return fbo_[0];
		}

		const graphics::GraphicsFramebufferPtr&
		Camera::getSwapFramebuffer() const noexcept
		{
			return fbo_[1];
		}

		void
		Camera::setupFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample, graphics::GraphicsFormat format, graphics::GraphicsFormat depthStencil) except
		{
			GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, format));
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, depthStencil));

			GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexMultisample(multisample);
			colorTextureDesc.setTexDim(multisample > 0 ? GraphicsTextureDim::Texture2DMultisample : GraphicsTextureDim::Texture2D);
			colorTextureDesc.setTexFormat(format);
			colorTexture_[0] = RenderSystem::instance()->createTexture(colorTextureDesc);
			if (!colorTexture_[0])
				throw runtime::runtime_error::create("createTexture() failed");

			GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexMultisample(multisample);
			depthTextureDesc.setTexDim(multisample > 0 ? GraphicsTextureDim::Texture2DMultisample : GraphicsTextureDim::Texture2D);
			depthTextureDesc.setTexFormat(depthStencil);
			depthTexture_[0] = RenderSystem::instance()->createTexture(depthTextureDesc);
			if (!depthTexture_[0])
				throw runtime::runtime_error::create("createTexture() failed");

			GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setGraphicsFramebufferLayout(RenderSystem::instance()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_[0], 0, 0));
			framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture_[0], 0, 0));

			fbo_[0] = RenderSystem::instance()->createFramebuffer(framebufferDesc);
			if (!fbo_[0])
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}

		void
		Camera::setupSwapFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample, graphics::GraphicsFormat format, graphics::GraphicsFormat depthStencil) except
		{
			GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, format));
			framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, depthStencil));

			GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexMultisample(multisample);
			colorTextureDesc.setTexDim(multisample > 0 ? GraphicsTextureDim::Texture2DMultisample : GraphicsTextureDim::Texture2D);
			colorTextureDesc.setTexFormat(format);
			colorTexture_[1] = RenderSystem::instance()->createTexture(colorTextureDesc);
			if (!colorTexture_[1])
				throw runtime::runtime_error::create("createTexture() failed");

			GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexMultisample(multisample);
			depthTextureDesc.setTexDim(multisample > 0 ? GraphicsTextureDim::Texture2DMultisample : GraphicsTextureDim::Texture2D);
			depthTextureDesc.setTexFormat(depthStencil);
			depthTexture_[1] = RenderSystem::instance()->createTexture(depthTextureDesc);
			if (!depthTexture_[1])
				throw runtime::runtime_error::create("createTexture() failed");

			GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setGraphicsFramebufferLayout(RenderSystem::instance()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_[1], 0, 0));
			framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture_[1], 0, 0));

			fbo_[1] = RenderSystem::instance()->createFramebuffer(framebufferDesc);
			if (!fbo_[1])
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}

		void
		Camera::_updateOrtho() const noexcept
		{
			std::uint32_t width = 1920, height = 1080;

			if (!fbo_[0])
				RenderSystem::instance()->getFramebufferSize(width, height);
			else
			{
				width = fbo_[0]->getGraphicsFramebufferDesc().getWidth();
				height = fbo_[0]->getGraphicsFramebufferDesc().getHeight();
			}

			auto left = width * ortho_.x;
			auto right = width * ortho_.y;
			auto bottom = height * ortho_.z;
			auto top = height * ortho_.w;

			math::float4x4  adjustment;
			adjustment.makeScale(1.0, -1.0, 1.0);

			project_ = adjustment * math::makeOrthoLH(left, right, bottom, top, znear_, zfar_);
			projectInverse_ = math::inverse(project_);
		}

		void
		Camera::_updatePerspective(float ratio) const noexcept
		{
			project_ = math::makePerspectiveFovLH(aperture_, ratio_ * ratio, znear_, zfar_);
			projectInverse_ = math::inverse(project_);
		}

		void
		Camera::_updateViewProject() const noexcept
		{
			std::uint32_t width = 1920, height = 1080;

			if (!fbo_[0])
				RenderSystem::instance()->getFramebufferSize(width, height);
			else
			{
				width = fbo_[0]->getGraphicsFramebufferDesc().getWidth();
				height = fbo_[0]->getGraphicsFramebufferDesc().getHeight();
			}

			float ratio = (float)width / height;

			if (ratioReal_ != ratio)
			{
				if (cameraType_ == CameraType::Perspective)
					this->_updatePerspective(ratio);
				else
					this->_updateOrtho();

				ratioReal_ = ratio;
			}

			if (needUpdateViewProject_)
			{
				viewProject_ = project_ * this->getView();
				viewProjectInverse_ = math::inverse(viewProject_);

				needUpdateViewProject_ = false;
			}
		}

		void
		Camera::onMoveAfter() noexcept
		{
			RenderObject::onMoveAfter();

			needUpdateViewProject_= true;
		}

		void
		Camera::onRenderBefore(const Camera& camera) noexcept
		{
			RenderObject::onRenderBefore(camera);
		}

		void
		Camera::onRenderAfter(const Camera& camera) noexcept
		{
			RenderObject::onRenderAfter(camera);
		}
	}
}