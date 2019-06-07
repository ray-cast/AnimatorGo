#ifndef OCTOON_RENDER_SYSTEM_H_
#define OCTOON_RENDER_SYSTEM_H_

#include <octoon/runtime/singleton.h>
#include <octoon/video/render_types.h>
#include <octoon/hal/graphics.h>

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

			void setup(const hal::GraphicsDevicePtr& device, std::uint32_t w, std::uint32_t h) except;
			void close() noexcept;

			void setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept;
			void getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept;

			hal::GraphicsInputLayoutPtr createInputLayout(const hal::GraphicsInputLayoutDesc& desc) noexcept;
			hal::GraphicsDataPtr createGraphicsData(const hal::GraphicsDataDesc& desc) noexcept;
			hal::GraphicsTexturePtr createTexture(const hal::GraphicsTextureDesc& desc) noexcept;
			hal::GraphicsSamplerPtr createSampler(const hal::GraphicsSamplerDesc& desc) noexcept;
			hal::GraphicsFramebufferPtr createFramebuffer(const hal::GraphicsFramebufferDesc& desc) noexcept;
			hal::GraphicsFramebufferLayoutPtr createFramebufferLayout(const hal::GraphicsFramebufferLayoutDesc& desc) noexcept;
			hal::GraphicsShaderPtr createShader(const hal::GraphicsShaderDesc& desc) noexcept;
			hal::GraphicsProgramPtr createProgram(const hal::GraphicsProgramDesc& desc) noexcept;
			hal::GraphicsStatePtr createRenderState(const hal::GraphicsStateDesc& desc) noexcept;
			hal::GraphicsPipelinePtr createRenderPipeline(const hal::GraphicsPipelineDesc& desc) noexcept;
			hal::GraphicsDescriptorSetPtr createDescriptorSet(const hal::GraphicsDescriptorSetDesc& desc) noexcept;
			hal::GraphicsDescriptorSetLayoutPtr createDescriptorSetLayout(const hal::GraphicsDescriptorSetLayoutDesc& desc) noexcept;
			hal::GraphicsDescriptorPoolPtr createDescriptorPool(const hal::GraphicsDescriptorPoolDesc& desc) noexcept;

			void render(hal::GraphicsContext& context) noexcept;

		private:
			void setupFramebuffers(std::uint32_t w, std::uint32_t h) except;

		private:
			RenderSystem(const RenderSystem&) = delete;
			RenderSystem& operator=(const RenderSystem&) = delete;

		private:
			std::uint32_t width_, height_;

			hal::GraphicsFramebufferPtr fbo_;
			hal::GraphicsTexturePtr colorTexture_;
			hal::GraphicsTexturePtr depthTexture_;

			hal::GraphicsDevicePtr device_;
		};
	}
}

#endif