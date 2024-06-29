#include <octoon/video/forward_renderer.h>
#include <octoon/video/rendering_data.h>

#include <octoon/runtime/except.h>

#include <octoon/hal/graphics_device.h>
#include <octoon/hal/graphics_texture.h>
#include <octoon/hal/graphics_framebuffer.h>

#include <octoon/camera/ortho_camera.h>
#include <octoon/camera/perspective_camera.h>

#include <octoon/light/ambient_light.h>
#include <octoon/light/directional_light.h>
#include <octoon/light/point_light.h>
#include <octoon/light/spot_light.h>
#include <octoon/light/disk_light.h>
#include <octoon/light/rectangle_light.h>
#include <octoon/light/environment_light.h>
#include <octoon/light/tube_light.h>

#include <octoon/mesh/plane_mesh.h>

#include <octoon/material/mesh_copy_material.h>
#include <octoon/material/mesh_depth_material.h>
#include <octoon/material/mesh_standard_material.h>

namespace octoon
{
	ForwardRenderer::ForwardRenderer(const GraphicsContextPtr& context) noexcept
		: width_(0)
		, height_(0)
		, framebufferWidth_(0)
		, framebufferHeight_(0)
	{
		lightsShadowCasterPass_ = std::make_unique<LightsShadowCasterPass>();
		drawOpaquePass_ = std::make_unique<DrawObjectPass>(true);
		drawTranparentPass_ = std::make_unique<DrawObjectPass>(false);
		drawSkyboxPass_ = std::make_unique<DrawSkyboxPass>();
		drawSelectorPass_ = std::make_unique<DrawSelectorPass>();

		Config config;
		config.context = std::make_unique<ScriptableRenderContext>(context);
		config.controller = std::make_unique<ScriptableSceneController>(context);

		this->configs_.push_back(std::move(config));
	}

	ForwardRenderer::~ForwardRenderer() noexcept
	{
	}

	void
	ForwardRenderer::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
	{
		framebufferWidth_ = w;
		framebufferHeight_ = h;
	}

	void
	ForwardRenderer::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
	{
		w = framebufferWidth_;
		h = framebufferHeight_;
	}

	const GraphicsFramebufferPtr&
	ForwardRenderer::getFramebuffer() const noexcept
	{
		return this->edgeFramebuffer_;
	}

	void
	ForwardRenderer::setWorkBufferSize(const std::shared_ptr<ScriptableRenderContext>& context, std::uint32_t w, std::uint32_t h) except
	{
		if (width_ == w && height_ == h)
			return;

		this->width_ = w;
		this->height_ = h;

		GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
		framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, GraphicsFormat::R32G32B32SFloat));
		framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, GraphicsFormat::X8_D24UNormPack32));

		try
		{
			GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexMultisample(4);
			colorTextureDesc.setTexDim(TextureDimension::Texture2DMultisample);
			colorTextureDesc.setTexFormat(GraphicsFormat::R32G32B32SFloat);
			edgeTexture_ = context->createTexture(colorTextureDesc);
			if (!edgeTexture_)
				throw runtime_error::create("createTexture() failed");

			GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexMultisample(4);
			depthTextureDesc.setTexDim(TextureDimension::Texture2DMultisample);
			depthTextureDesc.setTexFormat(GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = context->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime_error::create("createTexture() failed");

			GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setFramebufferLayout(context->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(edgeTexture_, 0, 0));

			edgeFramebuffer_ = context->createFramebuffer(framebufferDesc);
			if (!edgeFramebuffer_)
				throw runtime_error::create("createFramebuffer() failed");

			GraphicsTextureDesc colorTextureDesc2;
			colorTextureDesc2.setWidth(w);
			colorTextureDesc2.setHeight(h);
			colorTextureDesc2.setTexDim(TextureDimension::Texture2D);
			colorTextureDesc2.setTexFormat(GraphicsFormat::R32G32B32SFloat);
			colorTexture_ = context->createTexture(colorTextureDesc2);
			if (!colorTexture_)
				throw runtime_error::create("createTexture() failed");

			GraphicsTextureDesc depthTextureDesc2;
			depthTextureDesc2.setWidth(w);
			depthTextureDesc2.setHeight(h);
			depthTextureDesc2.setTexDim(TextureDimension::Texture2D);
			depthTextureDesc2.setTexFormat(GraphicsFormat::X8_D24UNormPack32);
			depthTexture2_ = context->createTexture(depthTextureDesc2);
			if (!depthTexture2_)
				throw runtime_error::create("createTexture() failed");

			GraphicsFramebufferDesc framebufferDesc2;
			framebufferDesc2.setWidth(w);
			framebufferDesc2.setHeight(h);
			framebufferDesc2.setFramebufferLayout(context->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc2.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture2_, 0, 0));
			framebufferDesc2.addColorAttachment(GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo2_ = context->createFramebuffer(framebufferDesc2);
			if (!fbo2_)
				throw runtime_error::create("createFramebuffer() failed");
		}
		catch (...)
		{
			GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexFormat(GraphicsFormat::R32G32B32SFloat);
			edgeTexture_ = context->createTexture(colorTextureDesc);
			if (!edgeTexture_)
				throw runtime_error::create("createTexture() failed");

			GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexFormat(GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = context->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime_error::create("createTexture() failed");

			GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setFramebufferLayout(context->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(edgeTexture_, 0, 0));

			edgeFramebuffer_ = context->createFramebuffer(framebufferDesc);
			if (!edgeFramebuffer_)
				throw runtime_error::create("createFramebuffer() failed");
		}
	}

	void
	ForwardRenderer::prepareScene(const std::shared_ptr<RenderScene>& scene) noexcept
	{
		for (auto& c : configs_)
		{
			c.controller->cleanCache();
			c.controller->compileScene(scene);
		}
	}

	void
	ForwardRenderer::render(const std::shared_ptr<RenderScene>& scene)
	{
		assert(scene->getMainCamera());

		this->prepareScene(scene);

		for (auto& c : configs_)
		{
			auto& renderingData = c.controller->getCachedScene(scene);

			lightsShadowCasterPass_->Execute(*c.context, renderingData);
			drawOpaquePass_->Execute(*c.context, renderingData);
			drawTranparentPass_->Execute(*c.context, renderingData);
			drawSkyboxPass_->Execute(*c.context, renderingData);
			drawSelectorPass_->Execute(*c.context, renderingData);

			auto fbo = renderingData.camera->getFramebuffer();
			if (fbo && renderingData.camera->getRenderToScreen())
			{
				auto vp = renderingData.camera->getPixelViewport();
				auto viewport = math::float4((float)vp.x, (float)vp.y, (float)vp.width, (float)vp.height);

				c.context->configureTarget(nullptr);
				c.context->configureClear(ClearFlagBits::AllBit, renderingData.camera->getClearColor(), 1.0f, 0);

				if (!fbo->getFramebufferDesc().getColorAttachments().empty())
				{
					auto texture = fbo->getFramebufferDesc().getColorAttachment().getBindingTexture();
					if (texture->getTextureDesc().getTexDim() == TextureDimension::Texture2DMultisample)
					{
						if (fbo == edgeFramebuffer_)
						{
							c.context->blitFramebuffer(fbo, viewport, fbo2_, viewport, SamplerFilter::Nearest);
							c.context->discardFramebuffer(fbo, ClearFlagBits::AllBit);
							c.context->blitFramebuffer(fbo2_, viewport, nullptr, viewport, SamplerFilter::Nearest);
							c.context->discardFramebuffer(fbo2_, ClearFlagBits::AllBit);
						}
					}
					else
					{
						float viewportRatio = viewport.width / viewport.height;

						float framebufferHeight = std::min((float)framebufferHeight_, (float)framebufferWidth_ / viewportRatio);
						float framebufferWidth = framebufferHeight * viewportRatio;

						float framebufferX = (framebufferWidth_ - framebufferWidth) / 2;
						float framebufferY = (framebufferHeight_ - framebufferHeight) / 2;

						auto mode = viewport.width == framebufferWidth_ && viewport.height == framebufferHeight_ ? SamplerFilter::Nearest : SamplerFilter::Linear;
						c.context->blitFramebuffer(fbo, viewport, nullptr, math::float4(framebufferX, framebufferY, framebufferX + framebufferWidth, framebufferY + framebufferHeight), mode);
						c.context->discardFramebuffer(fbo, ClearFlagBits::AllBit);
					}
				}
			}
		}
	}
}