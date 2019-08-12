#if defined(OCTOON_FEATURE_VIDEO_ENABLE)
#include <octoon/offline_feature.h>

#include <octoon/input_feature.h>
#include <octoon/input/input_event.h>

#include <octoon/game_app.h>
#include <octoon/game_server.h>

#include <octoon/hal_feature.h>
#include <octoon/hal/graphics.h>
#include <octoon/runtime/except.h>

#include <RadeonProRender.h>
#include <RadeonProRender_GL.h>

#include <GL/GL.h>

namespace octoon
{
	OctoonImplementSubClass(OfflineFeature, GameFeature, "OfflineFeature")

	OfflineFeature::OfflineFeature() noexcept
		: framebuffer_w_(0)
		, framebuffer_h_(0)
		, rprContext_(nullptr)
		, rprScene_(nullptr)
		, rprMaterialSystem_(nullptr)
		, rprFramebuffer_(nullptr)
		, rprDenoise_(nullptr)
		, colorFramebuffer_(nullptr)
		, normalFramebuffer_(nullptr)
		, positionFramebuffer_(nullptr)
		, objectIdFramebuffer_(nullptr)
		, albedoFramebuffer_(nullptr)
	{
	}

	OfflineFeature::OfflineFeature(std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept
		: framebuffer_w_(framebuffer_w)
		, framebuffer_h_(framebuffer_h)
		, rprContext_(nullptr)
		, rprScene_(nullptr)
		, rprMaterialSystem_(nullptr)
		, rprFramebuffer_(nullptr)
		, rprDenoise_(nullptr)
		, colorFramebuffer_(nullptr)
		, normalFramebuffer_(nullptr)
		, positionFramebuffer_(nullptr)
		, objectIdFramebuffer_(nullptr)
		, albedoFramebuffer_(nullptr)
	{
	}

	OfflineFeature::~OfflineFeature() noexcept
	{
	}

	void
	OfflineFeature::setFramebufferScale(std::uint32_t w, std::uint32_t h) noexcept
	{
		if (framebuffer_w_ != w || framebuffer_h_ != h)
		{
			framebuffer_w_ = w;
			framebuffer_h_ = h;
		}
	}

	void
	OfflineFeature::getFramebufferScale(std::uint32_t& w, std::uint32_t& h) noexcept
	{
		w = framebuffer_w_;
		h = framebuffer_h_;
	}

	void
	OfflineFeature::setFramebufferDirty(bool dirty) noexcept
	{
		this->dirty_ = dirty;
	}

	bool
	OfflineFeature::getFramebufferDirty() const noexcept
	{
		return this->dirty_;
	}

	void*
	OfflineFeature::getScene() const noexcept
	{
		return this->rprScene_;
	}

	void*
	OfflineFeature::getContext() const noexcept
	{
		return this->rprContext_;
	}

	void*
	OfflineFeature::getMaterialSystem() const noexcept
	{
		return this->rprMaterialSystem_;
	}

	hal::GraphicsFramebufferPtr
	OfflineFeature::getFramebuffer() const noexcept
	{
		return this->framebuffer_;
	}

	void
	OfflineFeature::onActivate() except
	{
		this->addMessageListener("feature:input:event", std::bind(&OfflineFeature::onInputEvent, this, std::placeholders::_1));

		rprCreateContext(RPR_API_VERSION, 0, 0, RPR_CREATION_FLAGS_ENABLE_GPU0 | RPR_CREATION_FLAGS_ENABLE_GL_INTEROP, 0, 0, &this->rprContext_);
		rprContextCreateScene(rprContext_, &rprScene_);
		rprContextCreateMaterialSystem(rprContext_, 0, &this->rprMaterialSystem_);
		rprContextCreatePostEffect(rprContext_, RPR_POST_EFFECT_WAVELET_DENOISER, &rprDenoise_);

		rprContextSetScene(rprContext_, rprScene_);
		rprContextAttachPostEffect(rprContext_, rprDenoise_);

		this->onFramebufferChange(this->framebuffer_w_, this->framebuffer_h_);
	}

	void
	OfflineFeature::onDeactivate() noexcept
	{
		this->removeMessageListener("feature:input:event", std::bind(&OfflineFeature::onInputEvent, this, std::placeholders::_1));

		if (this->rprMaterialSystem_)
		{
			rprObjectDelete(this->rprMaterialSystem_);
			this->rprMaterialSystem_ = nullptr;
		}

		if (this->rprFramebuffer_)
		{
			rprObjectDelete(this->rprFramebuffer_);
			this->rprFramebuffer_ = nullptr;
		}

		if (this->rprDenoise_)
		{
			rprObjectDelete(this->rprDenoise_);
			this->rprDenoise_ = nullptr;
		}

		if (this->rprScene_)
		{
			rprSceneClear(rprScene_);
			rprObjectDelete(this->rprScene_);
			this->rprScene_ = nullptr;
		}

		if (this->rprContext_)
		{
			rprContextClearMemory(rprContext_);
			rprObjectDelete(this->rprContext_);
			this->rprContext_ = nullptr;
		}
	}

	void
	OfflineFeature::onInputEvent(const runtime::any& data) noexcept
	{
		auto event = runtime::any_cast<input::InputEvent>(data);
		switch (event.event)
		{
		case input::InputEvent::SizeChange:
		case input::InputEvent::SizeChangeDPI:
			{
				if (event.change.w > 0 && event.change.h > 0)
				{
					if (this->framebuffer_w_ != event.change.w || this->framebuffer_h_ != event.change.h)
					{
						this->onFramebufferChange(event.change.w, event.change.h);
						this->framebuffer_w_ = event.change.w;
						this->framebuffer_h_ = event.change.h;
					}
				}
			}
			break;
		default:
			return;
		}
	}

	void
	OfflineFeature::onFrameBegin() noexcept
	{
	}

	void
	OfflineFeature::onFrame() noexcept
	{
		rpr_camera camera;
		rprSceneGetCamera(this->rprScene_, &camera);
		if (camera)
		{
			std::size_t count = 0;
			std::size_t type_size = 0;

			rprSceneGetInfo(this->rprScene_, RPR_SCENE_SHAPE_COUNT, 1, &count, &type_size);
			if (count == 0)
				return;

			rprSceneGetInfo(this->rprScene_, RPR_SCENE_LIGHT_COUNT, 1, &count, &type_size);
			if (count == 0)
				return;

			if (this->dirty_)
			{
				if (this->rprFramebuffer_)
					rprFrameBufferClear(this->rprFramebuffer_);

				if (this->colorFramebuffer_)
					rprFrameBufferClear(this->colorFramebuffer_);

				if (this->normalFramebuffer_)
					rprFrameBufferClear(this->normalFramebuffer_);

				if (this->positionFramebuffer_)
					rprFrameBufferClear(this->positionFramebuffer_);

				if (this->objectIdFramebuffer_)
					rprFrameBufferClear(this->objectIdFramebuffer_);

				if (this->albedoFramebuffer_)
					rprFrameBufferClear(this->albedoFramebuffer_);

				this->dirty_ = false;
			}

			rprContextRender(rprContext_);

			auto graphics = this->getFeature<GraphicsFeature>();
			if (graphics)
			{
				auto context = graphics->getContext();
				math::float4 viewport(0, 0, this->framebuffer_w_, this->framebuffer_h_);
				context->blitFramebuffer(framebuffer_, viewport, nullptr, viewport);
			}
		}
	}

	void
	OfflineFeature::onFrameEnd() noexcept
	{
	}

	void
	OfflineFeature::onFramebufferChange(std::uint32_t w, std::uint32_t h) except
	{
		auto context = this->getFeature<GraphicsFeature>()->getContext();
		if (context)
		{
			hal::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(0, hal::GraphicsImageLayout::ColorAttachmentOptimal, hal::GraphicsFormat::R8G8B8A8UNorm));
			framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(1, hal::GraphicsImageLayout::DepthStencilAttachmentOptimal, hal::GraphicsFormat::D32_SFLOAT));

			hal::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2D);
			colorTextureDesc.setTexFormat(hal::GraphicsFormat::R8G8B8A8UNorm);
			auto colorTexture_ = context->getDevice()->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2D);
			depthTextureDesc.setTexFormat(hal::GraphicsFormat::D32_SFLOAT);
			auto depthTexture_ = context->getDevice()->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setGraphicsFramebufferLayout(context->getDevice()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			this->framebuffer_ = context->getDevice()->createFramebuffer(framebufferDesc);
			if (!this->framebuffer_)
				throw runtime::runtime_error::create("createFramebuffer() failed");

			rpr_framebuffer framebuffer;
			if (RPR_SUCCESS == rprContextCreateFramebufferFromGLTexture2D(rprContext_, GL_TEXTURE_2D, 0, colorTexture_->handle(), &framebuffer))
			{
				/*rprContextSetAOV(rprContext_, RPR_AOV_COLOR, framebuffer);

				if (this->rprFramebuffer_)
					rprObjectDelete(this->rprFramebuffer_);*/

				this->rprFramebuffer_ = framebuffer;
			}

			rpr_image_format format = { 4, RPR_COMPONENT_TYPE_FLOAT32 };
			rpr_framebuffer_desc desc;
			desc.fb_width = w;
			desc.fb_height = h;

			rprContextCreateFrameBuffer(rprContext_, format, &desc, &colorFramebuffer_);
			rprContextCreateFrameBuffer(rprContext_, format, &desc, &normalFramebuffer_);
			rprContextCreateFrameBuffer(rprContext_, format, &desc, &positionFramebuffer_);
			rprContextCreateFrameBuffer(rprContext_, format, &desc, &albedoFramebuffer_);
			rprContextCreateFrameBuffer(rprContext_, format, &desc, &objectIdFramebuffer_);

			rprContextSetAOV(rprContext_, RPR_AOV_COLOR, colorFramebuffer_);
			rprContextSetAOV(rprContext_, RPR_AOV_SHADING_NORMAL, normalFramebuffer_);
			rprContextSetAOV(rprContext_, RPR_AOV_WORLD_COORDINATE, positionFramebuffer_);
			rprContextSetAOV(rprContext_, RPR_AOV_ALBEDO, albedoFramebuffer_);
			rprContextSetAOV(rprContext_, RPR_AOV_OBJECT_ID, objectIdFramebuffer_);
			rprContextSetAOV(rprContext_, RPR_AOV_OUTPUT, framebuffer);
		}
	}
}
#endif