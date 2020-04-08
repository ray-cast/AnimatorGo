#include <octoon/camera/camera.h>
#include <octoon/video/renderer.h>
#include <octoon/runtime/except.h>
#include <octoon/hal/graphics.h>

namespace octoon::camera
{
	OctoonImplementSubClass(Camera, runtime::RttiInterface, "Camera")

	Camera::Camera() noexcept
		: viewport_(0.0f, 0.0f, 1.0f, 1.0f)
		, clearColor_(math::float4(0.0f, 0.0f, 0.0f, 1.0f))
		, blitToScreen_(false)
		, project_(math::float4x4::One)
		, projectInverse_(math::float4x4::One)
		, viewProject_(math::float4x4::One)
		, viewProjectInverse_(math::float4x4::One)
		, clearflags_(hal::GraphicsClearFlagBits::AllBit)
	{
	}

	Camera::~Camera() noexcept
	{
	}

	void
	Camera::setClearColor(const math::float4& color) noexcept
	{
		clearColor_ = color;
	}

	void
	Camera::setViewport(const math::float4& viewport) noexcept
	{
		viewport_ = viewport;
	}

	void
	Camera::setRenderToScreen(bool enable) noexcept
	{
		blitToScreen_ = enable;
	}

	void
	Camera::setClearFlags(hal::GraphicsClearFlags clearflags) noexcept
	{
		clearflags_ = clearflags;
	}

	void
	Camera::setFramebuffer(const hal::GraphicsFramebufferPtr& framebuffer) noexcept
	{
		fbo_[0] = framebuffer;
	}

	void
	Camera::setProjection(math::float4x4& projection) const noexcept
	{
		project_ = projection;
	}

	void
	Camera::setProjectionInverse(math::float4x4& projection) const noexcept
	{
		projectInverse_ = projection;
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
		return project_;
	}

	const math::float4x4&
	Camera::getProjectionInverse() const noexcept
	{
		return projectInverse_;
	}

	const math::float4x4&
	Camera::getViewProjection() const noexcept
	{
		return viewProject_;
	}

	const math::float4x4&
	Camera::getViewProjectionInverse() const noexcept
	{
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

	const math::float4&
	Camera::getClearColor() const noexcept
	{
		return clearColor_;
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
			video::Renderer::instance()->getFramebufferSize(width, height);
		else
		{
			width = fbo_[0]->getFramebufferDesc().getWidth();
			height = fbo_[0]->getFramebufferDesc().getHeight();
		}

		math::float4 result;
		result.x = viewport_.x * width;
		result.y = viewport_.y * height;
		result.z = viewport_.z * width;
		result.w = viewport_.w * height;
		screen_ = result;

		return screen_;
	}

	bool
	Camera::getRenderToScreen() const noexcept
	{
		return blitToScreen_;
	}

	hal::GraphicsClearFlags
	Camera::getClearFlags() const noexcept
	{
		return clearflags_;
	}

	const hal::GraphicsFramebufferPtr&
	Camera::getFramebuffer() const noexcept
	{
		return fbo_[0];
	}

	const hal::GraphicsFramebufferPtr&
	Camera::getSwapFramebuffer() const noexcept
	{
		return fbo_[1];
	}

	void
	Camera::setupFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample, hal::GraphicsFormat format, hal::GraphicsFormat depthStencil) except
	{
		hal::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
		framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(0, hal::GraphicsImageLayout::ColorAttachmentOptimal, format));
		framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(1, hal::GraphicsImageLayout::DepthStencilAttachmentOptimal, depthStencil));

		hal::GraphicsTextureDesc colorTextureDesc;
		colorTextureDesc.setWidth(w);
		colorTextureDesc.setHeight(h);
		colorTextureDesc.setTexMultisample(multisample);
		colorTextureDesc.setTexDim(multisample > 0 ? hal::GraphicsTextureDim::Texture2DMultisample : hal::GraphicsTextureDim::Texture2D);
		colorTextureDesc.setTexFormat(format);
		colorTexture_[0] = video::Renderer::instance()->createTexture(colorTextureDesc);
		if (!colorTexture_[0])
			throw runtime::runtime_error::create("createTexture() failed");

		hal::GraphicsTextureDesc depthTextureDesc;
		depthTextureDesc.setWidth(w);
		depthTextureDesc.setHeight(h);
		depthTextureDesc.setTexMultisample(multisample);
		depthTextureDesc.setTexDim(multisample > 0 ? hal::GraphicsTextureDim::Texture2DMultisample : hal::GraphicsTextureDim::Texture2D);
		depthTextureDesc.setTexFormat(depthStencil);
		depthTexture_[0] = video::Renderer::instance()->createTexture(depthTextureDesc);
		if (!depthTexture_[0])
			throw runtime::runtime_error::create("createTexture() failed");

		hal::GraphicsFramebufferDesc framebufferDesc;
		framebufferDesc.setWidth(w);
		framebufferDesc.setHeight(h);
		framebufferDesc.setFramebufferLayout(video::Renderer::instance()->createFramebufferLayout(framebufferLayoutDesc));
		framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_[0], 0, 0));
		framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_[0], 0, 0));

		fbo_[0] = video::Renderer::instance()->createFramebuffer(framebufferDesc);
		if (!fbo_[0])
			throw runtime::runtime_error::create("createFramebuffer() failed");
	}

	void
	Camera::setupSwapFramebuffers(std::uint32_t w, std::uint32_t h, std::uint8_t multisample, hal::GraphicsFormat format, hal::GraphicsFormat depthStencil) except
	{
		hal::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
		framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(0, hal::GraphicsImageLayout::ColorAttachmentOptimal, format));
		framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(1, hal::GraphicsImageLayout::DepthStencilAttachmentOptimal, depthStencil));

		hal::GraphicsTextureDesc colorTextureDesc;
		colorTextureDesc.setWidth(w);
		colorTextureDesc.setHeight(h);
		colorTextureDesc.setTexMultisample(multisample);
		colorTextureDesc.setTexDim(multisample > 0 ? hal::GraphicsTextureDim::Texture2DMultisample : hal::GraphicsTextureDim::Texture2D);
		colorTextureDesc.setTexFormat(format);
		colorTexture_[1] = video::Renderer::instance()->createTexture(colorTextureDesc);
		if (!colorTexture_[1])
			throw runtime::runtime_error::create("createTexture() failed");

		hal::GraphicsTextureDesc depthTextureDesc;
		depthTextureDesc.setWidth(w);
		depthTextureDesc.setHeight(h);
		depthTextureDesc.setTexMultisample(multisample);
		depthTextureDesc.setTexDim(multisample > 0 ? hal::GraphicsTextureDim::Texture2DMultisample : hal::GraphicsTextureDim::Texture2D);
		depthTextureDesc.setTexFormat(depthStencil);
		depthTexture_[1] = video::Renderer::instance()->createTexture(depthTextureDesc);
		if (!depthTexture_[1])
			throw runtime::runtime_error::create("createTexture() failed");

		hal::GraphicsFramebufferDesc framebufferDesc;
		framebufferDesc.setWidth(w);
		framebufferDesc.setHeight(h);
		framebufferDesc.setFramebufferLayout(video::Renderer::instance()->createFramebufferLayout(framebufferLayoutDesc));
		framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_[1], 0, 0));
		framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_[1], 0, 0));

		fbo_[1] = video::Renderer::instance()->createFramebuffer(framebufferDesc);
		if (!fbo_[1])
			throw runtime::runtime_error::create("createFramebuffer() failed");
	}
}