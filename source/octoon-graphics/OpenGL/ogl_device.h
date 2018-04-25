#ifndef OCTOON_OGL_DEVICE_H_
#define OCTOON_OGL_DEVICE_H_

#include <octoon/graphics/graphics_device.h>

namespace octoon
{
	namespace graphics
	{
		class OGLDevice final : public GraphicsDevice
		{
			OctoonDeclareSubClass(OGLDevice, GraphicsDevice)
		public:
			OGLDevice() noexcept;
			virtual ~OGLDevice() noexcept;

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
			const GraphicsDeviceDesc& getGraphicsDeviceDesc() const noexcept;

			void message(const char* message, ...) noexcept;

		private:
			OGLDevice(const OGLDevice&) noexcept = delete;
			OGLDevice& operator=(const OGLDevice&) noexcept = delete;

		private:
			GraphicsDeviceDesc _deviceDesc;
			GraphicsContextWeaks _deviceContexts;
			GraphicsDevicePropertyPtr _deviceProperty;
		};
	}
}

#endif