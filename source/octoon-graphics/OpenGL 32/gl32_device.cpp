#include "gl32_device.h"
#include "gl32_device_context.h"
#include "gl32_device_property.h"
#include "gl32_state.h"
#include "gl32_shader.h"
#include "gl32_texture.h"
#include "gl32_framebuffer.h"
#include "gl32_input_layout.h"
#include "gl32_sampler.h"
#include "gl32_pipeline.h"
#include "gl32_descriptor.h"
#include "gl32_graphics_data.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(GL32Device, OGLDevice, "GL32Device")

		GL32Device::GL32Device() noexcept
		{
		}

		GL32Device::~GL32Device() noexcept
		{
			this->close();
		}

		bool
		GL32Device::setup(const GraphicsDeviceDesc& desc) noexcept
		{
			auto deviceProperty = std::make_shared<GL32DeviceProperty>();
			if (!deviceProperty->setup(desc))
				return false;

			_deviceProperty = deviceProperty;
			_deviceDesc = desc;
			return true;
		}

		void
		GL32Device::close() noexcept
		{
			_deviceProperty.reset();
		}

		GraphicsContextPtr
		GL32Device::createDeviceContext(const GraphicsContextDesc& desc) noexcept
		{
			auto context = std::make_shared<GL32DeviceContext>();
			context->setDevice(this->downcast_pointer<GL32Device>());
			if (context->setup(desc))
			{
				_deviceContexts.push_back(context);
				return context;
			}

			return nullptr;
		}

		GraphicsInputLayoutPtr
		GL32Device::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
		{
			auto inputLayout = std::make_shared<GL32InputLayout>();
			inputLayout->setDevice(this->downcast_pointer<GL32Device>());
			if (inputLayout->setup(desc))
				return inputLayout;
			return nullptr;
		}

		GraphicsDataPtr
		GL32Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
		{
			auto data = std::make_shared<GL32GraphicsData>();
			data->setDevice(this->downcast_pointer<GL32Device>());
			if (data->setup(desc))
				return data;
			return nullptr;
		}

		GraphicsTexturePtr
		GL32Device::createTexture(const GraphicsTextureDesc& desc) noexcept
		{
			auto texture = std::make_shared<GL32Texture>();
			texture->setDevice(this->downcast_pointer<GL32Device>());
			if (texture->setup(desc))
				return texture;
			return nullptr;
		}

		GraphicsSamplerPtr
		GL32Device::createSampler(const GraphicsSamplerDesc& desc) noexcept
		{
			auto sampler = std::make_shared<GL32Sampler>();
			sampler->setDevice(this->downcast_pointer<GL32Device>());
			if (sampler->setup(desc))
				return sampler;
			return nullptr;
		}

		GraphicsFramebufferPtr
		GL32Device::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
		{
			auto framebuffer = std::make_shared<GL32Framebuffer>();
			framebuffer->setDevice(this->downcast_pointer<GL32Device>());
			if (framebuffer->setup(desc))
				return framebuffer;
			return nullptr;
		}

		GraphicsFramebufferLayoutPtr
		GL32Device::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
		{
			auto framebufferLayout = std::make_shared<GL32FramebufferLayout>();
			framebufferLayout->setDevice(this->downcast_pointer<GL32Device>());
			if (framebufferLayout->setup(desc))
				return framebufferLayout;
			return nullptr;
		}

		GraphicsStatePtr
		GL32Device::createRenderState(const GraphicsStateDesc& desc) noexcept
		{
			auto state = std::make_shared<GL32GraphicsState>();
			state->setDevice(this->downcast_pointer<GL32Device>());
			if (state->setup(desc))
				return state;
			return nullptr;
		}

		GraphicsShaderPtr
		GL32Device::createShader(const GraphicsShaderDesc& desc) noexcept
		{
			auto shader = std::make_shared<GL32Shader>();
			shader->setDevice(this->downcast_pointer<GL32Device>());
			if (shader->setup(desc))
				return shader;
			return nullptr;
		}

		GraphicsProgramPtr
		GL32Device::createProgram(const GraphicsProgramDesc& desc) noexcept
		{
			auto program = std::make_shared<GL32Program>();
			program->setDevice(this->downcast_pointer<GL32Device>());
			if (program->setup(desc))
				return program;
			return nullptr;
		}

		GraphicsPipelinePtr
		GL32Device::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
		{
			auto pipeline = std::make_shared<GL32Pipeline>();
			pipeline->setDevice(this->downcast_pointer<GL32Device>());
			if (pipeline->setup(desc))
				return pipeline;
			return nullptr;
		}

		GraphicsDescriptorSetPtr
		GL32Device::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
		{
			auto descriptorSet = std::make_shared<GL32DescriptorSet>();
			descriptorSet->setDevice(this->downcast_pointer<GL32Device>());
			if (descriptorSet->setup(desc))
				return descriptorSet;
			return nullptr;
		}

		GraphicsDescriptorSetLayoutPtr
		GL32Device::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
		{
			auto descriptorSetLayout = std::make_shared<GL32DescriptorSetLayout>();
			descriptorSetLayout->setDevice(this->downcast_pointer<GL32Device>());
			if (descriptorSetLayout->setup(desc))
				return descriptorSetLayout;
			return nullptr;
		}

		GraphicsDescriptorPoolPtr
		GL32Device::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
		{
			auto descriptorPool = std::make_shared<GL32DescriptorPool>();
			descriptorPool->setDevice(this->downcast_pointer<GL32Device>());
			if (descriptorPool->setup(desc))
				return descriptorPool;
			return nullptr;
		}

		void
		GL32Device::enableDebugControl(bool enable) noexcept
		{
			for (auto& it : _deviceContexts)
			{
				auto deviceContext = it.lock();
				if (deviceContext)
					deviceContext->downcast<GL32DeviceContext>()->enableDebugControl(enable);
			}
		}

		void
		GL32Device::copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
		{
			assert(source);
			source->downcast<GL32DescriptorSet>()->copy(descriptorCopyCount, descriptorCopies);
		}

		const GraphicsDeviceProperty&
		GL32Device::getDeviceProperty() const noexcept
		{
			return *_deviceProperty;
		}

		const GraphicsDeviceDesc&
		GL32Device::getDeviceDesc() const noexcept
		{
			return _deviceDesc;
		}
	}
}