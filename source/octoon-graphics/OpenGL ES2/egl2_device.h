#ifndef OCTOON_EGL2_DEVICE_H_
#define OCTOON_EGL2_DEVICE_H_

#include <octoon/graphics/graphics_device.h>

namespace octoon 
{
	namespace graphics
	{
		class EGL2Device final : public GraphicsDevice
		{
			OctoonDeclareSubClass(EGL2Device, GraphicsDevice)
		public:
			EGL2Device() noexcept;
			virtual ~EGL2Device() noexcept;

			bool setup(const GraphicsDeviceDesc& desc) noexcept;
			void close() noexcept;

			GraphicsSwapchainPtr createSwapchain(const GraphicsSwapchainDesc& desc) noexcept override;
			GraphicsContextPtr createDeviceContext(const GraphicsContextDesc& desc) noexcept override;
			GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept override;
			GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept override;
			GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept override;
			GraphicsSamplerPtr createSampler(const GraphicsSamplerDesc& desc) noexcept override;
			GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept override;
			GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept override;
			GraphicsShaderPtr createShader(const GraphicsShaderDesc& desc) noexcept override;
			GraphicsProgramPtr createProgram(const GraphicsProgramDesc& desc) noexcept override;
			GraphicsStatePtr createRenderState(const GraphicsStateDesc& desc) noexcept override;
			GraphicsPipelinePtr createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept override;
			GraphicsDescriptorSetPtr createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept override;
			GraphicsDescriptorSetLayoutPtr createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept override;
			GraphicsDescriptorPoolPtr createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept override;

			void enableDebugControl(bool enable) noexcept;

			void copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept override;

			const GraphicsDeviceProperty& getDeviceProperty() const noexcept override;
			const GraphicsDeviceDesc& getDeviceDesc() const noexcept override;

		private:
			EGL2Device(const EGL2Device&) noexcept = delete;
			EGL2Device& operator=(const EGL2Device&) noexcept = delete;

		private:
			GraphicsDeviceDesc _deviceDesc;
			GraphicsContextWeaks _deviceContexts;
			GraphicsDevicePropertyPtr _deviceProperty;
		};
	}
}

#endif