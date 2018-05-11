#ifndef OCTOON_EGL3_DEVICE_H_
#define OCTOON_EGL3_DEVICE_H_

#include <octoon/graphics/graphics_device.h>

namespace octoon 
{
	namespace graphics
	{
		class EGL3Device final : public GraphicsDevice
		{
			OctoonDeclareSubClass(EGL3Device, GraphicsDevice)
		public:
			EGL3Device() noexcept;
			virtual ~EGL3Device() noexcept;

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
			EGL3Device(const EGL3Device&) noexcept = delete;
			EGL3Device& operator=(const EGL3Device&) noexcept = delete;

		private:
			GraphicsDeviceDesc _deviceDesc;
			GraphicsContextWeaks _deviceContexts;
			GraphicsDevicePropertyPtr _deviceProperty;
		};

	}
}

#endif