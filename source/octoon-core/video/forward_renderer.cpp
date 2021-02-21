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
	ForwardRenderer::ForwardRenderer() noexcept
		: width_(0)
		, height_(0)
	{
		screenGeometry_ = std::make_shared<Geometry>();
		screenGeometry_->setMesh(octoon::PlaneMesh::create(2.0f, 2.0f));

		lightsShadowCasterPass_ = std::make_unique<LightsShadowCasterPass>();
	}

	ForwardRenderer::~ForwardRenderer() noexcept
	{
	}

	const hal::GraphicsFramebufferPtr&
	ForwardRenderer::getFramebuffer() const noexcept
	{
		return this->fbo_;
	}

	void
	ForwardRenderer::setupFramebuffers(const std::shared_ptr<ScriptableRenderContext>& context, std::uint32_t w, std::uint32_t h) except
	{
		if (width_ == w && height_ == h)
			return;

		this->width_ = w;
		this->height_ = h;

		hal::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
		framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(0, hal::GraphicsImageLayout::ColorAttachmentOptimal, hal::GraphicsFormat::R32G32B32SFloat));
		framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(1, hal::GraphicsImageLayout::DepthStencilAttachmentOptimal, hal::GraphicsFormat::X8_D24UNormPack32));

		try
		{
			hal::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexMultisample(4);
			colorTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2DMultisample);
			colorTextureDesc.setTexFormat(hal::GraphicsFormat::R32G32B32SFloat);
			colorTexture_ = context->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexMultisample(4);
			depthTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2DMultisample);
			depthTextureDesc.setTexFormat(hal::GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = context->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setFramebufferLayout(context->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = context->createFramebuffer(framebufferDesc);
			if (!fbo_)
				throw runtime::runtime_error::create("createFramebuffer() failed");

			hal::GraphicsTextureDesc colorTextureDesc2;
			colorTextureDesc2.setWidth(w);
			colorTextureDesc2.setHeight(h);
			colorTextureDesc2.setTexDim(hal::GraphicsTextureDim::Texture2D);
			colorTextureDesc2.setTexFormat(hal::GraphicsFormat::R32G32B32SFloat);
			colorTexture2_ = context->createTexture(colorTextureDesc2);
			if (!colorTexture2_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsTextureDesc depthTextureDesc2;
			depthTextureDesc2.setWidth(w);
			depthTextureDesc2.setHeight(h);
			depthTextureDesc2.setTexDim(hal::GraphicsTextureDim::Texture2D);
			depthTextureDesc2.setTexFormat(hal::GraphicsFormat::X8_D24UNormPack32);
			depthTexture2_ = context->createTexture(depthTextureDesc2);
			if (!depthTexture2_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferDesc framebufferDesc2;
			framebufferDesc2.setWidth(w);
			framebufferDesc2.setHeight(h);
			framebufferDesc2.setFramebufferLayout(context->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc2.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture2_, 0, 0));
			framebufferDesc2.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture2_, 0, 0));

			fbo2_ = context->createFramebuffer(framebufferDesc2);
			if (!fbo2_)
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}
		catch (...)
		{
			hal::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexFormat(hal::GraphicsFormat::R32G32B32SFloat);
			colorTexture_ = context->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexFormat(hal::GraphicsFormat::X8_D24UNormPack32);
			depthTexture_ = context->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setFramebufferLayout(context->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			fbo_ = context->createFramebuffer(framebufferDesc);
			if (!fbo_)
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}
	}

	void
	ForwardRenderer::render(const std::shared_ptr<ScriptableRenderContext>& context, const RenderingData& renderingData)
	{
		auto camera = renderingData.camera;
		auto& framebuffer = camera->getFramebuffer();
		auto& swapFramebuffer = camera->getSwapFramebuffer();

		lightsShadowCasterPass_->Execute(*context, renderingData);

		auto fbo = camera->getFramebuffer();
		if (!fbo)
		{
			auto vp = renderingData.camera->getPixelViewport();
			this->setupFramebuffers(context, (int)vp.width, (int)vp.height);
			fbo = fbo_;
		}

		context->setFramebuffer(fbo);
		context->clearFramebuffer(0, camera->getClearFlags(), camera->getClearColor(), 1.0f, 0);
		context->drawRenderers(renderingData.geometries, *camera, this->overrideMaterial_);

		if (framebuffer && swapFramebuffer)
		{
			math::float4 v1(0, 0, (float)framebuffer->getFramebufferDesc().getWidth(), (float)framebuffer->getFramebufferDesc().getHeight());
			math::float4 v2(0, 0, (float)swapFramebuffer->getFramebufferDesc().getWidth(), (float)swapFramebuffer->getFramebufferDesc().getHeight());
			context->blitFramebuffer(framebuffer, v1, swapFramebuffer, v2);
		}

		if (camera->getRenderToScreen())
		{
			auto vp = renderingData.camera->getPixelViewport();
			auto viewport = math::float4((float)vp.x, (float)vp.y, (float)vp.width, (float)vp.height);

			context->setFramebuffer(nullptr);
			context->clearFramebuffer(0, hal::GraphicsClearFlagBits::AllBit, math::float4::Zero, 1.0f, 0);

			if (!fbo->getFramebufferDesc().getColorAttachments().empty())
			{
				auto texture = fbo->getFramebufferDesc().getColorAttachment().getBindingTexture();
				if (texture->getTextureDesc().getTexDim() == hal::GraphicsTextureDim::Texture2DMultisample)
				{
					if (framebuffer && swapFramebuffer)
					{
						context->blitFramebuffer(swapFramebuffer, viewport, nullptr, viewport);
						context->discardFramebuffer(swapFramebuffer, hal::GraphicsClearFlagBits::AllBit);
					}
					else if (fbo == fbo_)
					{
						context->blitFramebuffer(fbo, viewport, fbo2_, viewport);
						context->discardFramebuffer(fbo, hal::GraphicsClearFlagBits::AllBit);
						context->blitFramebuffer(fbo2_, viewport, nullptr, viewport);
						context->discardFramebuffer(fbo2_, hal::GraphicsClearFlagBits::AllBit);
					}
				}
				else
				{
					context->blitFramebuffer(fbo, viewport, nullptr, viewport);
					context->discardFramebuffer(fbo, hal::GraphicsClearFlagBits::AllBit);
				}
			}
		}
	}
}