#if defined(OCTOON_FEATURE_VIDEO_ENABLE)
#include <octoon/offline_feature.h>

#include <octoon/input_feature.h>
#include <octoon/input/input_event.h>

#include <octoon/game_app.h>
#include <octoon/game_server.h>
#include <octoon/game_listener.h>

#include <octoon/hal_feature.h>
#include <octoon/hal/graphics.h>
#include <octoon/runtime/except.h>
#include <octoon/image/image.h>

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
		, colorFramebuffer_(nullptr)
		, normalFramebuffer_(nullptr)
		, albedoFramebuffer_(nullptr)
	{
	}

	OfflineFeature::OfflineFeature(std::uint32_t framebuffer_w, std::uint32_t framebuffer_h) noexcept
		: OfflineFeature()
	{
		framebuffer_w_ = framebuffer_w;
		framebuffer_h_ = framebuffer_h;
	}

	OfflineFeature::~OfflineFeature() noexcept
	{
	}

	void
	OfflineFeature::addOfflineListener(OfflineListener* listener) noexcept
	{
		assert(std::find(listener_.begin(), listener_.end(), listener) == listener_.end());
		listener_.push_back(listener);
	}

	void
	OfflineFeature::removeOfflineListener(OfflineListener* listener) noexcept
	{
		auto it = std::find(listener_.begin(), listener_.end(), listener);
		if (it != listener_.end())
			listener_.erase(it);
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
	OfflineFeature::getColorFramebuffer() const noexcept
	{
		return this->framebuffer_;
	}

	void
	OfflineFeature::readColorFramebuffer(float output[]) noexcept
	{
		if (colorTexture_)
		{
			auto& desc = colorTexture_->getTextureDesc();

			math::float4* data = nullptr;
			if (colorTexture_->map(0, 0, desc.getWidth(), desc.getHeight(), 0, (void**)&data))
			{
				auto width = desc.getWidth();
				auto height = desc.getHeight();

				for (std::uint32_t y = 0; y < height; y++)
				{
					for (std::uint32_t x = 0; x < width; x++)
					{
						auto src = y * width + x;
						auto dst = (y * width + x) * 3;

						output[dst] = data[src].x;
						output[dst + 1] = data[src].y;
						output[dst + 2] = data[src].z;
					}
				}

				colorTexture_->unmap();
			}
		}
	}

	void
	OfflineFeature::readNormalFramebuffer(float output[]) noexcept
	{
		if (normalTexture_)
		{
			auto& desc = normalTexture_->getTextureDesc();

			math::float4* data = nullptr;
			if (normalTexture_->map(0, 0, desc.getWidth(), desc.getHeight(), 0, (void**)&data))
			{
				auto width = desc.getWidth();
				auto height = desc.getHeight();

				for (std::uint32_t y = 0; y < height; y++)
				{
					for (std::uint32_t x = 0; x < width; x++)
					{
						auto src = y * width + x;
						auto dst = (y * width + x) * 3;

						output[dst] = data[src].x;
						output[dst + 1] = data[src].y;
						output[dst + 2] = data[src].z;
					}
				}

				normalTexture_->unmap();
			}
		}
	}

	void
	OfflineFeature::readAlbedoFramebuffer(float output[]) noexcept
	{
		if (albedoTexture_)
		{
			auto& desc = albedoTexture_->getTextureDesc();

			math::float4* data = nullptr;
			if (albedoTexture_->map(0, 0, desc.getWidth(), desc.getHeight(), 0, (void**)&data))
			{
				auto width = desc.getWidth();
				auto height = desc.getHeight();

				for (std::uint32_t y = 0; y < height; y++)
				{
					for (std::uint32_t x = 0; x < width; x++)
					{
						auto src = y * width + x;
						auto dst = (y * width + x) * 3;

						output[dst] = data[src].x;
						output[dst + 1] = data[src].y;
						output[dst + 2] = data[src].z;
					}
				}

				albedoTexture_->unmap();
			}
		}
	}

	void
	OfflineFeature::clearMemory() noexcept
	{
		this->cleanupFramebuffers();
		this->cleanupProRender();
		this->setupProRender();
	}

	void
	OfflineFeature::onActivate() noexcept(false)
	{
		this->addMessageListener("feature:input:event", std::bind(&OfflineFeature::onInputEvent, this, std::placeholders::_1));
		this->setupProRender();
	}

	void
	OfflineFeature::onDeactivate() noexcept
	{
		this->removeMessageListener("feature:input:event", std::bind(&OfflineFeature::onInputEvent, this, std::placeholders::_1));
		this->cleanupFramebuffers();
		this->cleanupProRender();		
	}

	void
	OfflineFeature::onInputEvent(const std::any& data) noexcept
	{
		auto event = std::any_cast<input::InputEvent>(data);
		switch (event.event)
		{
		case input::InputEvent::SizeChange:
		case input::InputEvent::SizeChangeDPI:
			{
				if (event.change.w > 0 && event.change.h > 0)
				{
					if (this->framebuffer_w_ != event.change.w || this->framebuffer_h_ != event.change.h)
					{
						for (auto& listener : listener_)
							listener->onFramebufferResize(event.change.w, event.change.h);

						this->setupFramebuffers(event.change.w, event.change.h);
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
	OfflineFeature::onFrame() noexcept(false)
	{
		try
		{
			rpr_camera camera;
			rprSceneGetCamera(this->rprScene_, &camera);
			if (camera)
			{
				std::size_t count = 0;
				std::size_t type_size = 0;

				for (auto& listener : listener_)
					listener->onPreRender();

				rprSceneGetInfo(this->rprScene_, RPR_SCENE_SHAPE_COUNT, 1, &count, &type_size);
				if (count == 0)
					return;

				rprSceneGetInfo(this->rprScene_, RPR_SCENE_LIGHT_COUNT, 1, &count, &type_size);
				if (count == 0)
					return;

				if (this->dirty_)
				{
					if (this->colorFramebuffer_)
						rprFrameBufferClear(this->colorFramebuffer_);

					if (this->normalFramebuffer_)
						rprFrameBufferClear(this->normalFramebuffer_);

					if (this->albedoFramebuffer_)
						rprFrameBufferClear(this->albedoFramebuffer_);

					this->dirty_ = false;
				}

				rprContextRender(rprContext_);

				for (auto& listener : listener_)
					listener->onPostRender();

				auto graphics = this->getFeature<GraphicsFeature>();
				if (graphics)
				{
					auto context = graphics->getContext();
					math::float4 viewport(0, 0, static_cast<float>(this->framebuffer_w_), static_cast<float>(this->framebuffer_h_));
					context->blitFramebuffer(framebuffer_, viewport, nullptr, viewport);
				}
			}
		}
		catch (const std::exception& e)
		{
			this->getGameListener()->onMessage(e.what());
		}
	}

	void
	OfflineFeature::cleanupProRender() noexcept
	{
		for (auto& node : imageNodes_)
		{
			if (node.second.first)
				rprObjectDelete(node.second.first);
			if (node.second.second)
				rprObjectDelete(node.second.second);
		}

		imageNodes_.clear();

		for (auto& image : images_)
		{
			if (image.second.first)
				rprObjectDelete(image.second.first);
			if (image.second.second)
				rprObjectDelete(image.second.second);
		}

		images_.clear();

		if (this->rprMaterialSystem_)
		{
			rprObjectDelete(this->rprMaterialSystem_);
			this->rprMaterialSystem_ = nullptr;
		}

		if (this->rprScene_)
		{
			rprSceneClear(rprScene_);
			rprObjectDelete(this->rprScene_);
			this->rprScene_ = nullptr;
		}

		if (this->rprContext_)
		{
			rprObjectDelete(this->rprContext_);
			this->rprContext_ = nullptr;
		}
	}

	void
	OfflineFeature::cleanupFramebuffers() noexcept
	{
		if (this->colorFramebuffer_)
		{
			rprObjectDelete(this->colorFramebuffer_);
			this->colorFramebuffer_ = nullptr;
		}

		if (this->normalFramebuffer_)
		{
			rprObjectDelete(this->normalFramebuffer_);
			this->normalFramebuffer_ = nullptr;
		}

		if (this->albedoFramebuffer_)
		{
			rprObjectDelete(this->albedoFramebuffer_);
			this->albedoFramebuffer_ = nullptr;
		}
	}

	void
	OfflineFeature::setupProRender() noexcept(false)
	{
		auto GetErrorString = [](int status)
		{
			switch (status)
			{
			case RPR_ERROR_COMPUTE_API_NOT_SUPPORTED:         return "RPR_ERROR_COMPUTE_API_NOT_SUPPORTED";
			case RPR_ERROR_OUT_OF_SYSTEM_MEMORY:              return "RPR_ERROR_OUT_OF_SYSTEM_MEMORY";
			case RPR_ERROR_OUT_OF_VIDEO_MEMORY:               return "RPR_ERROR_OUT_OF_VIDEO_MEMORY";
			case RPR_ERROR_INVALID_LIGHTPATH_EXPR:            return "RPR_ERROR_INVALID_LIGHTPATH_EXPR";
			case RPR_ERROR_INVALID_IMAGE:                     return "RPR_ERROR_INVALID_IMAGE";
			case RPR_ERROR_INVALID_AA_METHOD:                 return "RPR_ERROR_INVALID_AA_METHOD";
			case RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT:          return "RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT";
			case RPR_ERROR_INVALID_GL_TEXTURE:                return "RPR_ERROR_INVALID_GL_TEXTURE";
			case RPR_ERROR_INVALID_CL_IMAGE:                  return "RPR_ERROR_INVALID_CL_IMAGE";
			case RPR_ERROR_INVALID_OBJECT:                    return "RPR_ERROR_INVALID_OBJECT";
			case RPR_ERROR_INVALID_PARAMETER:                 return "RPR_ERROR_INVALID_PARAMETER";
			case RPR_ERROR_INVALID_TAG:                       return "RPR_ERROR_INVALID_TAG";
			case RPR_ERROR_INVALID_LIGHT:                     return "RPR_ERROR_INVALID_LIGHT";
			case RPR_ERROR_INVALID_CONTEXT:                   return "RPR_ERROR_INVALID_CONTEXT";
			case RPR_ERROR_UNIMPLEMENTED:                     return "RPR_ERROR_UNIMPLEMENTED";
			case RPR_ERROR_INVALID_API_VERSION:               return "RPR_ERROR_INVALID_API_VERSION";
			case RPR_ERROR_INTERNAL_ERROR:                    return "RPR_ERROR_INTERNAL_ERROR";
			case RPR_ERROR_IO_ERROR:                          return "RPR_ERROR_IO_ERROR";
			case RPR_ERROR_UNSUPPORTED_SHADER_PARAMETER_TYPE: return "RPR_ERROR_UNSUPPORTED_SHADER_PARAMETER_TYPE";
			case RPR_ERROR_MATERIAL_STACK_OVERFLOW:           return "RPR_ERROR_MATERIAL_STACK_OVERFLOW";
			case RPR_ERROR_INVALID_PARAMETER_TYPE:            return "RPR_ERROR_INVALID_PARAMETER_TYPE";
			case RPR_ERROR_UNSUPPORTED:                       return "RPR_ERROR_UNSUPPORTED";
			case RPR_ERROR_OPENCL_OUT_OF_HOST_MEMORY:         return "RPR_ERROR_OPENCL_OUT_OF_HOST_MEMORY";
			case RPR_ERROR_OPENGL:                            return "RPR_ERROR_OPENGL";
			case RPR_ERROR_OPENCL:                            return "RPR_ERROR_OPENCL";
			case RPR_ERROR_NULLPTR:                           return "RPR_ERROR_NULLPTR";
			case RPR_ERROR_NODETYPE:                          return "RPR_ERROR_NODETYPE";
			default: return "RPR_SUCCESS";
			}
		};

		auto status = rprCreateContext(RPR_API_VERSION, 0, 0, RPR_CREATION_FLAGS_ENABLE_GPU0 | RPR_CREATION_FLAGS_ENABLE_GL_INTEROP, 0, 0, &this->rprContext_);
		if (RPR_SUCCESS != status)
		{
			status = rprCreateContext(RPR_API_VERSION, 0, 0, RPR_CREATION_FLAGS_ENABLE_GPU1 | RPR_CREATION_FLAGS_ENABLE_GL_INTEROP, 0, 0, &this->rprContext_);
			if (RPR_SUCCESS != status)
				throw runtime::runtime_error::create(std::string("rprCreateContext() failed, error : ") + GetErrorString(status));
		}

		status = rprContextCreateScene(rprContext_, &rprScene_);
		if (RPR_SUCCESS != status)
			throw runtime::runtime_error::create(std::string("rprContextCreateScene() failed, error : ") + GetErrorString(status));

		status = rprContextCreateMaterialSystem(rprContext_, 0, &this->rprMaterialSystem_);
		if (RPR_SUCCESS != status)
			throw runtime::runtime_error::create(std::string("rprContextCreateMaterialSystem() failed, error : ") + GetErrorString(status));

		status = rprContextSetScene(rprContext_, rprScene_);
		if (RPR_SUCCESS != status)
			throw runtime::runtime_error::create(std::string("rprContextCreateMaterialSystem() failed, error : ") + GetErrorString(status));

		this->setupFramebuffers(this->framebuffer_w_, this->framebuffer_h_);
	}

	std::pair<void*, void*>
	OfflineFeature::createMaterialTextures(std::string_view path) noexcept(false)
	{
		try
		{
			auto it = imageNodes_.find(std::string(path));
			if (it != imageNodes_.end())
				return (*it).second;

			image::Image image;
			if (!image.load(std::string(path)))
				return std::make_pair(nullptr, nullptr);

			bool bgra = false;
			bool hasAlpha = false;
			std::uint8_t channel = 3;
			switch (image.format())
			{
			case image::Format::R8G8B8SNorm:
			case image::Format::R8G8B8SRGB:
			case image::Format::B8G8R8UNorm:
			case image::Format::B8G8R8SRGB:
				hasAlpha = false;
				break;
			case image::Format::R8G8B8A8SNorm:
			case image::Format::R8G8B8A8SRGB:
				hasAlpha = true;
				channel = 4;
				break;
			case image::Format::B8G8R8A8UNorm:
			case image::Format::B8G8R8A8SRGB:
				bgra = true;
				hasAlpha = true;
				channel = 4;
				break;
			default:
				throw runtime::runtime_error::create("This image type is not supported by this function:" + std::string(path));
			}

			rpr_image_format rgbFormat = { 3, RPR_COMPONENT_TYPE_UINT8 };
			rpr_image_desc rgbDesc;
			rgbDesc.image_width = image.width();
			rgbDesc.image_height = image.height();
			rgbDesc.image_depth = 1;
			rgbDesc.image_row_pitch = image.width() * rgbFormat.num_components;
			rgbDesc.image_slice_pitch = rgbDesc.image_row_pitch * rgbDesc.image_height;

			rpr_image_format alphaFormat = { 1, RPR_COMPONENT_TYPE_UINT8 };
			rpr_image_desc alphaDesc;
			alphaDesc.image_width = image.width();
			alphaDesc.image_height = image.height();
			alphaDesc.image_depth = 1;
			alphaDesc.image_row_pitch = image.width() * alphaFormat.num_components;
			alphaDesc.image_slice_pitch = alphaDesc.image_row_pitch * alphaDesc.image_height;

			auto data = image.data();

			std::vector<std::uint8_t> srgb((rgbDesc.image_width * rgbDesc.image_height * rgbFormat.num_components));
			std::vector<std::uint8_t> alpha;

			if (hasAlpha)
			{
				alpha.resize(alphaDesc.image_width * alphaDesc.image_height * alphaFormat.num_components);

				for (rpr_uint y = 0; y < rgbDesc.image_height; y++)
				{
					auto srcHeight = y * rgbDesc.image_width;
					auto dstHeight = (rgbDesc.image_height - 1 - y) * rgbDesc.image_width;

					for (std::size_t x = 0; x < rgbDesc.image_width; x++)
					{
						auto dstRGB = (dstHeight + x) * rgbFormat.num_components;
						auto dstAlpha = (dstHeight + x) * alphaFormat.num_components;
						auto src = (srcHeight + x) * 4;

						if (bgra)
						{
							srgb[dstRGB] = data[src + 2];
							srgb[dstRGB + 1] = data[src + 1];
							srgb[dstRGB + 2] = data[src];
						}
						else
						{
							srgb[dstRGB] = data[src];
							srgb[dstRGB + 1] = data[src + 1];
							srgb[dstRGB + 2] = data[src + 2];
						}

						alpha[dstAlpha] = static_cast<std::uint8_t>(255 - std::pow(data[src + 3] / 255.0f, 2.2f) * 255);
					}
				}
			}
			else
			{
				for (std::size_t y = 0; y < rgbDesc.image_height; y++)
				{
					auto dst = ((rgbDesc.image_height - 1 - y) * rgbDesc.image_width) * rgbFormat.num_components;
					auto src = y * rgbDesc.image_width * rgbFormat.num_components;

					std::memcpy(srgb.data() + dst, data + src, rgbDesc.image_width * rgbFormat.num_components);
				}
			}

			rpr_image image_ = nullptr;
			rpr_image alphaImage_ = nullptr;

			rpr_material_node textureNode = nullptr;
			rpr_material_node alphaNode = nullptr;

			if (!srgb.empty())
			{
				rprContextCreateImage(this->rprContext_, rgbFormat, &rgbDesc, srgb.data(), &image_);
				rprMaterialSystemCreateNode(this->getMaterialSystem(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &textureNode);
				rprMaterialNodeSetInputImageData(textureNode, "data", image_);
			}

			if (!alpha.empty())
			{
				hasAlpha = false;
				for (auto& value : alpha)
				{
					if (value > 0)
					{
						hasAlpha = true;
						break;
					}
				}

				if (hasAlpha)
				{
					rprContextCreateImage(this->rprContext_, alphaFormat, &alphaDesc, alpha.data(), &alphaImage_);
					rprMaterialSystemCreateNode(this->getMaterialSystem(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &alphaNode);
					rprMaterialNodeSetInputImageData(alphaNode, "data", alphaImage_);
				}
			}

			images_[std::string(path)] = std::make_pair(image_, alphaImage_);
			imageNodes_[std::string(path)] = std::make_pair(textureNode, alphaNode);

			return std::make_pair(textureNode, alphaNode);
		}
		catch (...)
		{
			return std::make_pair(nullptr, nullptr);
		}
	}

	void
	OfflineFeature::setupFramebuffers(std::uint32_t w, std::uint32_t h) noexcept(false)
	{
		auto context = this->getFeature<GraphicsFeature>()->getContext();
		if (context)
		{
			hal::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(w);
			colorTextureDesc.setHeight(h);
			colorTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2D);
			colorTextureDesc.setTexFormat(hal::GraphicsFormat::R32G32B32A32SFloat);
			colorTexture_ = context->getDevice()->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			normalTexture_ = context->getDevice()->createTexture(colorTextureDesc);
			if (!normalTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			albedoTexture_ = context->getDevice()->createTexture(colorTextureDesc);
			if (!albedoTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			rpr_framebuffer colorFramebuffer = nullptr;
			if (RPR_SUCCESS == rprContextCreateFramebufferFromGLTexture2D(rprContext_, GL_TEXTURE_2D, 0, static_cast<rpr_uint>(colorTexture_->handle()), &colorFramebuffer))
			{
				rprContextSetAOV(rprContext_, RPR_AOV_COLOR, colorFramebuffer);

				if (this->colorFramebuffer_)
					rprObjectDelete(this->colorFramebuffer_);

				this->colorFramebuffer_ = colorFramebuffer;
			}

			rpr_framebuffer normalFramebuffer = nullptr;
			if (RPR_SUCCESS == rprContextCreateFramebufferFromGLTexture2D(rprContext_, GL_TEXTURE_2D, 0, static_cast<rpr_uint>(normalTexture_->handle()), &normalFramebuffer))
			{
				rprContextSetAOV(rprContext_, RPR_AOV_SHADING_NORMAL, normalFramebuffer);

				if (this->normalFramebuffer_)
					rprObjectDelete(this->normalFramebuffer_);

				this->normalFramebuffer_ = normalFramebuffer;
			}

			rpr_framebuffer albedoFramebuffer = nullptr;
			if (RPR_SUCCESS == rprContextCreateFramebufferFromGLTexture2D(rprContext_, GL_TEXTURE_2D, 0, static_cast<rpr_uint>(albedoTexture_->handle()), &albedoFramebuffer))
			{
				rprContextSetAOV(rprContext_, RPR_AOV_ALBEDO, albedoFramebuffer);

				if (this->albedoFramebuffer_)
					rprObjectDelete(this->albedoFramebuffer_);

				this->albedoFramebuffer_ = albedoFramebuffer;
			}

			hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(w);
			depthTextureDesc.setHeight(h);
			depthTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2D);
			depthTextureDesc.setTexFormat(hal::GraphicsFormat::D32_SFLOAT);
			auto depthTexture_ = context->getDevice()->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(0, hal::GraphicsImageLayout::ColorAttachmentOptimal, hal::GraphicsFormat::R32G32B32SFloat));
			framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(1, hal::GraphicsImageLayout::DepthStencilAttachmentOptimal, hal::GraphicsFormat::D32_SFLOAT));

			hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(w);
			framebufferDesc.setHeight(h);
			framebufferDesc.setGraphicsFramebufferLayout(context->getDevice()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			this->framebuffer_ = context->getDevice()->createFramebuffer(framebufferDesc);
			if (!this->framebuffer_)
				throw runtime::runtime_error::create("createFramebuffer() failed");
		}
	}
}
#endif