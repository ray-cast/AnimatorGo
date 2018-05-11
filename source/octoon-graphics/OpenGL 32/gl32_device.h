#ifndef OCTOON_GL32_DEVICE_H_
#define OCTOON_GL32_DEVICE_H_

#include "gl32_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL32Device final : public GraphicsDevice
		{
			OctoonDeclareSubClass(GL32Device, GraphicsDevice)
		public:
			GL32Device() noexcept;
			virtual ~GL32Device() noexcept;

			bool setup(const GraphicsDeviceDesc& desc) noexcept;
			void close() noexcept;

			GraphicsSwapchainPtr createSwapchain(const GraphicsSwapchainDesc& desc) noexcept;
			GraphicsContextPtr createDeviceContext(const GraphicsContextDesc& desc) noexcept;
			GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept;
			GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
			GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept;
			GraphicsSamplerPtr createSampler(const GraphicsSamplerDesc& desc) noexcept;
			GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept;
			GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept;
			GraphicsShaderPtr createShader(const GraphicsShaderDesc& desc) noexcept;
			GraphicsProgramPtr createProgram(const GraphicsProgramDesc& desc) noexcept;
			GraphicsStatePtr createRenderState(const GraphicsStateDesc& desc) noexcept;
			GraphicsPipelinePtr createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept;
			GraphicsDescriptorSetPtr createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept;
			GraphicsDescriptorSetLayoutPtr createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept;
			GraphicsDescriptorPoolPtr createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept;

			void enableDebugControl(bool enable) noexcept;

			void copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept;

			const GraphicsDeviceProperty& getDeviceProperty() const noexcept;
			const GraphicsDeviceDesc& getDeviceDesc() const noexcept;

		private:
			GL32Device(const GL32Device&) noexcept = delete;
			GL32Device& operator=(const GL32Device&) noexcept = delete;

		private:
			GraphicsDeviceDesc _deviceDesc;
			GraphicsContextWeaks _deviceContexts;
			GraphicsDevicePropertyPtr _deviceProperty;
		};
	}
}

#endif