#include "gl30_device.h"
#include "gl30_device_context.h"
#include "gl30_device_property.h"
#include "gl30_state.h"
#include "gl30_shader.h"
#include "gl30_texture.h"
#include "gl30_framebuffer.h"
#include "gl30_input_layout.h"
#include "gl30_sampler.h"
#include "gl30_pipeline.h"
#include "gl30_descriptor_set.h"
#include "gl30_graphics_data.h"

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL30Device, OGLDevice, "GL30Device")

		GL30Device::GL30Device() noexcept
		{
		}

		GL30Device::~GL30Device() noexcept
		{
			this->close();
		}

		bool
		GL30Device::setup(const GraphicsDeviceDesc& desc) noexcept
		{
			auto deviceProperty = std::make_shared<GL30DeviceProperty>();
			deviceProperty->setDevice(this->downcast_pointer<OGLDevice>());
			if (!deviceProperty->setup(desc))
				return false;

			_deviceProperty = deviceProperty;
			_deviceDesc = desc;
			return true;
		}

		void
		GL30Device::close() noexcept
		{
			_deviceProperty.reset();
		}

		GraphicsContextPtr
		GL30Device::createDeviceContext(const GraphicsContextDesc& desc) noexcept
		{
			auto context = std::make_shared<GL30DeviceContext>();
			context->setDevice(this->downcast_pointer<GL30Device>());
			if (context->setup(desc))
			{
				_deviceContexts.push_back(context);
				return context;
			}

			return nullptr;
		}

		GraphicsInputLayoutPtr
		GL30Device::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
		{
			auto inputLayout = std::make_shared<GL30InputLayout>();
			inputLayout->setDevice(this->downcast_pointer<GL30Device>());
			if (inputLayout->setup(desc))
				return inputLayout;
			return nullptr;
		}

		GraphicsDataPtr
		GL30Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
		{
			auto data = std::make_shared<GL30GraphicsData>();
			data->setDevice(this->downcast_pointer<GL30Device>());
			if (data->setup(desc))
				return data;
			return nullptr;
		}

		GraphicsTexturePtr
		GL30Device::createTexture(const GraphicsTextureDesc& desc) noexcept
		{
			auto texture = std::make_shared<GL30Texture>();
			texture->setDevice(this->downcast_pointer<GL30Device>());
			if (texture->setup(desc))
				return texture;
			return nullptr;
		}

		GraphicsSamplerPtr
		GL30Device::createSampler(const GraphicsSamplerDesc& desc) noexcept
		{
			auto sampler = std::make_shared<GL30Sampler>();
			sampler->setDevice(this->downcast_pointer<GL30Device>());
			if (sampler->setup(desc))
				return sampler;
			return nullptr;
		}

		GraphicsFramebufferPtr
		GL30Device::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
		{
			auto framebuffer = std::make_shared<GL30Framebuffer>();
			framebuffer->setDevice(this->downcast_pointer<GL30Device>());
			if (framebuffer->setup(desc))
				return framebuffer;
			return nullptr;
		}

		GraphicsFramebufferLayoutPtr
		GL30Device::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
		{
			auto framebufferLayout = std::make_shared<GL30FramebufferLayout>();
			framebufferLayout->setDevice(this->downcast_pointer<GL30Device>());
			if (framebufferLayout->setup(desc))
				return framebufferLayout;
			return nullptr;
		}

		GraphicsStatePtr
		GL30Device::createRenderState(const GraphicsStateDesc& desc) noexcept
		{
			auto state = std::make_shared<GL30GraphicsState>();
			state->setDevice(this->downcast_pointer<GL30Device>());
			if (state->setup(desc))
				return state;
			return nullptr;
		}

		GraphicsShaderPtr
		GL30Device::createShader(const GraphicsShaderDesc& desc) noexcept
		{
			auto shader = std::make_shared<GL30Shader>();
			shader->setDevice(this->downcast_pointer<GL30Device>());
			if (shader->setup(desc))
				return shader;
			return nullptr;
		}

		GraphicsProgramPtr
		GL30Device::createProgram(const GraphicsProgramDesc& desc) noexcept
		{
			auto program = std::make_shared<GL30Program>();
			program->setDevice(this->downcast_pointer<GL30Device>());
			if (program->setup(desc))
				return program;
			return nullptr;
		}

		GraphicsPipelinePtr
		GL30Device::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
		{
			auto pipeline = std::make_shared<GL30Pipeline>();
			pipeline->setDevice(this->downcast_pointer<GL30Device>());
			if (pipeline->setup(desc))
				return pipeline;
			return nullptr;
		}

		GraphicsDescriptorSetPtr
		GL30Device::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
		{
			auto descriptorSet = std::make_shared<GL30DescriptorSet>();
			descriptorSet->setDevice(this->downcast_pointer<GL30Device>());
			if (descriptorSet->setup(desc))
				return descriptorSet;
			return nullptr;
		}

		GraphicsDescriptorSetLayoutPtr
		GL30Device::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
		{
			auto descriptorSetLayout = std::make_shared<GL30DescriptorSetLayout>();
			descriptorSetLayout->setDevice(this->downcast_pointer<GL30Device>());
			if (descriptorSetLayout->setup(desc))
				return descriptorSetLayout;
			return nullptr;
		}

		GraphicsDescriptorPoolPtr
		GL30Device::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
		{
			auto descriptorPool = std::make_shared<GL30DescriptorPool>();
			descriptorPool->setDevice(this->downcast_pointer<GL30Device>());
			if (descriptorPool->setup(desc))
				return descriptorPool;
			return nullptr;
		}

		void
		GL30Device::copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
		{
			assert(source);
			source->downcast<GL30DescriptorSet>()->copy(descriptorCopyCount, descriptorCopies);
		}

		const GraphicsDeviceProperty&
		GL30Device::getDeviceProperty() const noexcept
		{
			return *_deviceProperty;
		}

		const GraphicsDeviceDesc&
		GL30Device::getDeviceDesc() const noexcept
		{
			return _deviceDesc;
		}
	}
}