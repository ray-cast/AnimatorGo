#include <octoon/video/draw_selector_pass.h>
#include <octoon/runtime/except.h>
#include <octoon/hal/graphics_device.h>
#include <octoon/hal/graphics_texture.h>
#include <octoon/hal/graphics_framebuffer.h>

namespace octoon
{
	DrawSelectorPass::DrawSelectorPass() noexcept
		: width_(0)
		, height_(0)
		, edgeMaterial_(std::make_shared<EdgeMaterial>())
		, copyMaterial_(std::make_shared<MeshCopyMaterial>())
	{
		edgeMaterial_->setColor(math::float3(1.0f, 0.0f, 1.0f));
		edgeMaterial_->setDepthFunc(CompareFunction::Always);
		edgeMaterial_->setBlendEnable(true);
		edgeMaterial_->setBlendSrc(BlendMode::SrcAlpha);
		edgeMaterial_->setBlendDest(BlendMode::OneMinusSrcAlpha);
	}

	void
	DrawSelectorPass::setupFramebuffers(ScriptableRenderContext& context, std::uint32_t w, std::uint32_t h) except
	{
		if (width_ == w && height_ == h)
			return;

		this->width_ = w;
		this->height_ = h;

		GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
		framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(0, GraphicsImageLayout::ColorAttachmentOptimal, GraphicsFormat::R8G8B8A8UNorm));
		framebufferLayoutDesc.addComponent(GraphicsAttachmentLayout(1, GraphicsImageLayout::DepthStencilAttachmentOptimal, GraphicsFormat::D32_SFLOAT));

		GraphicsTextureDesc depthTextureDesc;
		depthTextureDesc.setWidth(w);
		depthTextureDesc.setHeight(h);
		depthTextureDesc.setTexDim(TextureDimension::Texture2D);
		depthTextureDesc.setTexFormat(GraphicsFormat::D32_SFLOAT);
		depthTexture_ = context.createTexture(depthTextureDesc);
		if (!depthTexture_)
			throw runtime_error::create("createTexture() failed");

		GraphicsTextureDesc colorTextureDesc;
		colorTextureDesc.setWidth(w);
		colorTextureDesc.setHeight(h);
		colorTextureDesc.setTexDim(TextureDimension::Texture2D);
		colorTextureDesc.setTexFormat(GraphicsFormat::R8G8B8A8UNorm);
		colorTexture_ = context.createTexture(colorTextureDesc);
		if (!colorTexture_)
			throw runtime_error::create("createTexture() failed");

		GraphicsFramebufferDesc framebufferDesc;
		framebufferDesc.setWidth(w);
		framebufferDesc.setHeight(h);
		framebufferDesc.setFramebufferLayout(context.createFramebufferLayout(framebufferLayoutDesc));
		framebufferDesc.setDepthStencilAttachment(GraphicsAttachmentBinding(depthTexture_, 0, 0));
		framebufferDesc.addColorAttachment(GraphicsAttachmentBinding(colorTexture_, 0, 0));

		colorFramebuffer_ = context.createFramebuffer(framebufferDesc);
		if (!colorFramebuffer_)
			throw runtime_error::create("createFramebuffer() failed");

		edgeMaterial_->setColorMap(colorTexture_);
	}

	void
	DrawSelectorPass::Execute(ScriptableRenderContext& context, const RenderingData& renderingData) noexcept(false)
	{
		auto& camera = renderingData.camera;
		auto& vp = camera->getPixelViewport();

		auto targetFramebuffer = context.getFramebuffer();

		this->setupFramebuffers(context, (std::uint32_t)vp.width, (std::uint32_t)vp.height);

		context.compileMaterial(edgeMaterial_, renderingData);
		context.compileMaterial(copyMaterial_, renderingData);

		context.configureTarget(colorFramebuffer_);
		context.configureClear(ClearFlagBits::AllBit, math::float4::Zero, 1.0f, 0);
		context.setViewport(0, math::float4((float)vp.x, (float)vp.y, (float)vp.width, (float)vp.height));

		for (auto& geometry : renderingData.geometries)
		{
			if (geometry->getRendererPriority() == 1)
				context.drawRenderers(*geometry, *camera, renderingData);
		}

		context.configureTarget(targetFramebuffer);
		context.setViewport(0, math::float4((float)vp.x, (float)vp.y, (float)vp.width, (float)vp.height));

		context.setMaterial(edgeMaterial_, renderingData, *camera, *renderingData.screenQuad);
		context.drawMesh(renderingData.screenQuad->getMesh(), 0, renderingData);
	}
}