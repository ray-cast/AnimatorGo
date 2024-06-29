#ifndef OCTOON_GRAPHICS_DEVICE_H_
#define OCTOON_GRAPHICS_DEVICE_H_

#include <octoon/hal/graphics_types.h>
#include <octoon/hal/render_state.h>
#include <octoon/hal/graphics_texture.h>
#include <octoon/hal/graphics_swapchain.h>
#include <octoon/hal/graphics_device_property.h>

namespace octoon
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

	class OCTOON_EXPORT GraphicsDevice : public Object
	{
		OctoonDeclareSubInterface(GraphicsDevice, Object)
	public:
		GraphicsDevice() noexcept = default;
		virtual ~GraphicsDevice() = default;

		virtual std::shared_ptr<GraphicsSwapchain> createSwapchain(const GraphicsSwapchainDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsContext> createDeviceContext(const GraphicsContextDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsInputLayout> createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsData> createGraphicsData(const GraphicsDataDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsTexture> createTexture(const GraphicsTextureDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsSampler> createSampler(const GraphicsSamplerDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsFramebuffer> createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsFramebufferLayout> createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept = 0;
		virtual std::shared_ptr<RenderState> createRenderState(const RenderStateDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsShader> createShader(const GraphicsShaderDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsProgram> createProgram(const GraphicsProgramDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsPipeline> createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsDescriptorSet> createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsDescriptorSetLayout> createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept = 0;
		virtual std::shared_ptr<GraphicsDescriptorPool> createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept = 0;

		virtual void copyDescriptorSets(std::shared_ptr<GraphicsDescriptorSet>& source, std::uint32_t descriptorCopyCount, const std::shared_ptr<GraphicsDescriptorSet> descriptorCopies[]) noexcept = 0;

		virtual const SystemInfo& getSystemInfo() const noexcept = 0;
		virtual const GraphicsDeviceDesc& getDeviceDesc() const noexcept = 0;

	private:
		GraphicsDevice(const GraphicsDevice&) noexcept = delete;
		GraphicsDevice& operator=(const GraphicsDevice&) noexcept = delete;
	};
}

#endif