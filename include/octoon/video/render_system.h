#ifndef OCTOON_RENDER_SYSTEM_H_
#define OCTOON_RENDER_SYSTEM_H_

#include <octoon/runtime/singleton.h>
#include <octoon/video/render_types.h>
#include <octoon/graphics/graphics.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT RenderSystem final
		{
			OctoonDeclareSingleton(RenderSystem)
		public:
			RenderSystem() noexcept;
			~RenderSystem() noexcept;

			void setup(WindHandle hwnd, std::uint32_t w, std::uint32_t h) noexcept;
			void close() noexcept;

			void setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept;
			void getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept;

			void render() noexcept;

		private:
			RenderSystem(const RenderSystem&) = delete;
			RenderSystem& operator=(const RenderSystem&) = delete;

		private:

			WindHandle winhandle_;
			std::uint32_t width_, height_;

			math::float3s vertices_;
			math::float3s colors_;

			std::uint32_t fbo_;
			std::uint32_t colorTexture_;
			std::uint32_t depthTexture_;

			std::uint32_t fboMSAA_;
			std::uint32_t colorTextureMSAA_;
			std::uint32_t depthTextureMSAA_;

			graphics::GraphicsDataPtr vbo_;
			graphics::GraphicsDataPtr ibo_;
			graphics::GraphicsTexturePtr texture_;
			graphics::GraphicsDescriptorSetPtr descriptorSet_;

			graphics::GraphicsUniformSetPtr proj_;
			graphics::GraphicsUniformSetPtr model_;

			graphics::GraphicsDevicePtr device_;
			graphics::GraphicsContextPtr context_;
			graphics::GraphicsSwapchainPtr swapchain_;
			graphics::GraphicsPipelinePtr pipeline_;
		};
	}
}

#endif