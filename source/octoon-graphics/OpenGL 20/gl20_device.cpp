#include "gl20_device.h"
#include "gl20_device_context.h"
#include "gl20_device_property.h"
#include "gl20_state.h"
#include "gl20_shader.h"
#include "gl20_texture.h"
#include "gl20_framebuffer.h"
#include "gl20_input_layout.h"
#include "gl20_sampler.h"
#include "gl20_pipeline.h"
#include "gl20_descriptor.h"
#include "gl20_graphics_data.h"

#include "ogl_swapchain.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL20Device, GraphicsDevice, "GL20Device")

		GL20Device::GL20Device() noexcept
		{
		}

		GL20Device::~GL20Device() noexcept
		{
			this->close();
		}

		bool
		GL20Device::setup(const GraphicsDeviceDesc& desc) noexcept
		{
			auto deviceProperty = std::make_shared<GL20DeviceProperty>();
			if (!deviceProperty->setup(desc))
				return false;

			_deviceProperty = deviceProperty;
			_deviceDesc = desc;
			return true;
		}

		void
		GL20Device::close() noexcept
		{
			_deviceProperty.reset();
		}

		GraphicsSwapchainPtr
		GL20Device::createSwapchain(const GraphicsSwapchainDesc& desc) noexcept
		{
			auto swapchain = std::make_shared<OGLSwapchain>();
			swapchain->setDevice(this->downcast_pointer<GL20Device>());
			if (swapchain->setup(desc))
				return swapchain;
			return nullptr;
		}

		GraphicsContextPtr
		GL20Device::createDeviceContext(const GraphicsContextDesc& desc) noexcept
		{
			auto context = std::make_shared<GL20DeviceContext>();
			context->setDevice(this->downcast_pointer<GL20Device>());
			if (context->setup(desc))
			{
				_deviceContexts.push_back(context);
				return context;
			}
			return nullptr;
		}

		GraphicsInputLayoutPtr
		GL20Device::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
		{
			auto inputLayout = std::make_shared<GL20InputLayout>();
			inputLayout->setDevice(this->downcast_pointer<GL20Device>());
			if (inputLayout->setup(desc))
				return inputLayout;
			return nullptr;
		}

		GraphicsDataPtr
		GL20Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
		{
			auto data = std::make_shared<GL20GraphicsData>();
			data->setDevice(this->downcast_pointer<GL20Device>());
			if (data->setup(desc))
				return data;
			return nullptr;
		}

		GraphicsTexturePtr
		GL20Device::createTexture(const GraphicsTextureDesc& desc) noexcept
		{
			auto texture = std::make_shared<GL20Texture>();
			texture->setDevice(this->downcast_pointer<GL20Device>());
			if (texture->setup(desc))
				return texture;
			return nullptr;
		}

		GraphicsSamplerPtr
		GL20Device::createSampler(const GraphicsSamplerDesc& desc) noexcept
		{
			auto sampler = std::make_shared<GL20Sampler>();
			sampler->setDevice(this->downcast_pointer<GL20Device>());
			if (sampler->setup(desc))
				return sampler;
			return nullptr;
		}

		GraphicsFramebufferPtr
		GL20Device::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
		{
			auto framebuffer = std::make_shared<GL20Framebuffer>();
			framebuffer->setDevice(this->downcast_pointer<GL20Device>());
			if (framebuffer->setup(desc))
				return framebuffer;
			return nullptr;
		}

		GraphicsFramebufferLayoutPtr
		GL20Device::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
		{
			auto framebufferLayout = std::make_shared<GL20FramebufferLayout>();
			framebufferLayout->setDevice(this->downcast_pointer<GL20Device>());
			if (framebufferLayout->setup(desc))
				return framebufferLayout;
			return nullptr;
		}

		GraphicsStatePtr
		GL20Device::createRenderState(const GraphicsStateDesc& desc) noexcept
		{
			auto state = std::make_shared<GL20GraphicsState>();
			state->setDevice(this->downcast_pointer<GL20Device>());
			if (state->setup(desc))
				return state;
			return nullptr;
		}

		GraphicsShaderPtr
		GL20Device::createShader(const GraphicsShaderDesc& desc) noexcept
		{
			auto shader = std::make_shared<GL20Shader>();
			shader->setDevice(this->downcast_pointer<GL20Device>());
			if (shader->setup(desc))
				return shader;
			return nullptr;
		}

		GraphicsProgramPtr
		GL20Device::createProgram(const GraphicsProgramDesc& desc) noexcept
		{
			auto program = std::make_shared<GL20Program>();
			program->setDevice(this->downcast_pointer<GL20Device>());
			if (program->setup(desc))
				return program;
			return nullptr;
		}

		GraphicsPipelinePtr
		GL20Device::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
		{
			auto pipeline = std::make_shared<GL20Pipeline>();
			pipeline->setDevice(this->downcast_pointer<GL20Device>());
			if (pipeline->setup(desc))
				return pipeline;
			return nullptr;
		}

		GraphicsDescriptorSetPtr
		GL20Device::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
		{
			auto descriptorSet = std::make_shared<GL20DescriptorSet>();
			descriptorSet->setDevice(this->downcast_pointer<GL20Device>());
			if (descriptorSet->setup(desc))
				return descriptorSet;
			return nullptr;
		}

		GraphicsDescriptorSetLayoutPtr
		GL20Device::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
		{
			auto descriptorSetLayout = std::make_shared<GL20DescriptorSetLayout>();
			descriptorSetLayout->setDevice(this->downcast_pointer<GL20Device>());
			if (descriptorSetLayout->setup(desc))
				return descriptorSetLayout;
			return nullptr;
		}

		GraphicsDescriptorPoolPtr
		GL20Device::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
		{
			auto descriptorPool = std::make_shared<GL20DescriptorPool>();
			descriptorPool->setDevice(this->downcast_pointer<GL20Device>());
			if (descriptorPool->setup(desc))
				return descriptorPool;
			return nullptr;
		}

		void
		GL20Device::enableDebugControl(bool enable) noexcept
		{
			for (auto& it : _deviceContexts)
			{
				auto deviceContext = it.lock();
				if (deviceContext)
					deviceContext->downcast<GL20DeviceContext>()->enableDebugControl(enable);
			}
		}

		void
		GL20Device::copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
		{
			assert(source);
			source->downcast<GL20DescriptorSet>()->copy(descriptorCopyCount, descriptorCopies);
		}

		const GraphicsDeviceProperty&
		GL20Device::getDeviceProperty() const noexcept
		{
			return *_deviceProperty;
		}

		const GraphicsDeviceDesc&
		GL20Device::getDeviceDesc() const noexcept
		{
			return _deviceDesc;
		}
	}
}