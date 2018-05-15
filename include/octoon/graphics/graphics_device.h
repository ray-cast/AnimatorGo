#ifndef OCTOON_GRAPHICS_DEVICE_H_
#define OCTOON_GRAPHICS_DEVICE_H_

#include <octoon/graphics/graphics_types.h>

namespace octoon
{
	namespace graphics
	{
		class OCTOON_EXPORT GraphicsDeviceDesc final
		{
		public:
			GraphicsDeviceDesc() noexcept;
			GraphicsDeviceDesc(GraphicsDeviceType type, bool enable = false) noexcept;
			~GraphicsDeviceDesc() noexcept;

			void setDeviceType(GraphicsDeviceType type) noexcept;
			GraphicsDeviceType getDeviceType() const noexcept;

			void setDebugControl(bool enable) noexcept;
			bool isEnabledDebugControl() const noexcept;

		private:
			bool _enableDebug;
			GraphicsDeviceType _deviceType;
		};

		class OCTOON_EXPORT GraphicsDevice : public runtime::RttiInterface
		{
			OctoonDeclareSubInterface(GraphicsDevice, runtime::RttiInterface)
		public:
			GraphicsDevice() noexcept = default;
			virtual ~GraphicsDevice() = default;

			virtual GraphicsSwapchainPtr createSwapchain(const GraphicsSwapchainDesc& desc) noexcept = 0;
			virtual GraphicsContextPtr createDeviceContext(const GraphicsContextDesc& desc) noexcept = 0;
			virtual GraphicsInputLayoutPtr createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept = 0;
			virtual GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept = 0;
			virtual GraphicsTexturePtr createTexture(const GraphicsTextureDesc& desc) noexcept = 0;
			virtual GraphicsSamplerPtr createSampler(const GraphicsSamplerDesc& desc) noexcept = 0;
			virtual GraphicsFramebufferPtr createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept = 0;
			virtual GraphicsFramebufferLayoutPtr createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept = 0;
			virtual GraphicsStatePtr createRenderState(const GraphicsStateDesc& desc) noexcept = 0;
			virtual GraphicsShaderPtr createShader(const GraphicsShaderDesc& desc) noexcept = 0;
			virtual GraphicsProgramPtr createProgram(const GraphicsProgramDesc& desc) noexcept = 0;
			virtual GraphicsPipelinePtr createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept = 0;
			virtual GraphicsDescriptorSetPtr createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept = 0;
			virtual GraphicsDescriptorSetLayoutPtr createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept = 0;
			virtual GraphicsDescriptorPoolPtr createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept = 0;

			virtual void copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept = 0;

			virtual const GraphicsDeviceProperty& getDeviceProperty() const noexcept = 0;
			virtual const GraphicsDeviceDesc& getDeviceDesc() const noexcept = 0;

		private:
			GraphicsDevice(const GraphicsDevice&) noexcept = delete;
			GraphicsDevice& operator=(const GraphicsDevice&) noexcept = delete;
		};
	}
}

#endif