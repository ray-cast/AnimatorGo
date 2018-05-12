#ifndef OCTOON_GL32_DEVICE_H_
#define OCTOON_GL32_DEVICE_H_

#include "ogl_device.h"

namespace octoon
{
	namespace graphics
	{
		class GL32Device final : public OGLDevice
		{
			OctoonDeclareSubClass(GL32Device, OGLDevice)
		public:
			GL32Device() noexcept;
			virtual ~GL32Device() noexcept;

			bool setup(const GraphicsDeviceDesc& desc) noexcept;
			void close() noexcept;

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