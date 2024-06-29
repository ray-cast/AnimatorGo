#include "gl33_device.h"
#include "gl33_device_context.h"
#include "gl33_device_property.h"
#include "gl33_shader.h"
#include "gl33_texture.h"
#include "gl33_framebuffer.h"
#include "gl33_input_layout.h"
#include "gl33_descriptor_set.h"
#include "gl33_graphics_data.h"
#include "gl33_state.h"
#include "gl33_sampler.h"
#include "gl33_pipeline.h"

#if defined(OCTOON_FEATURE_HAL_USE_OPENGL45)
#	include "gl45_device_context.h"
#	include "gl45_texture.h"
#	include "gl45_framebuffer.h"
#	include "gl45_descriptor.h"
#	include "gl45_graphics_data.h"
#	include "gl45_pipeline.h"
#endif

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL33Device, OGLDevice, "GL33Device")

		GL33Device::GL33Device() noexcept
		{
		}

		GL33Device::~GL33Device() noexcept
		{
			this->close();
		}

		bool
		GL33Device::setup(const GraphicsDeviceDesc& desc) noexcept
		{
			auto deviceProperty = std::make_shared<GL33DeviceProperty>();
			deviceProperty->setDevice(this->downcast_pointer<GL33Device>());
			if (!deviceProperty->setup(desc))
				return false;

			_deviceProperty = deviceProperty;
			_deviceDesc = desc;
			return true;
		}

		void
		GL33Device::close() noexcept
		{
			_deviceProperty.reset();
		}

		GraphicsContextPtr
		GL33Device::createDeviceContext(const GraphicsContextDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL33)
			{
				auto context = std::make_shared<GL33DeviceContext>();
				context->setDevice(this->downcast_pointer<GL33Device>());
				if (context->setup(desc))
				{
					_deviceContexts.push_back(context);
					return context;
				}
			}
			else
			{
#if defined(OCTOON_FEATURE_HAL_USE_OPENGL45)
				auto context = std::make_shared<GL45DeviceContext>();
				context->setDevice(this->downcast_pointer<GL33Device>());
				if (context->setup(desc))
				{
					_deviceContexts.push_back(context);
					return context;
				}
#endif
			}

			return nullptr;
		}

		GraphicsInputLayoutPtr
		GL33Device::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
		{
			auto inputLayout = std::make_shared<GL33InputLayout>();
			inputLayout->setDevice(this->downcast_pointer<GL33Device>());
			if (inputLayout->setup(desc))
				return inputLayout;
			return nullptr;
		}

		GraphicsDataPtr
		GL33Device::createGraphicsData(const GraphicsDataDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL33)
			{
				auto data = std::make_shared<GL33GraphicsData>();
				data->setDevice(this->downcast_pointer<GL33Device>());
				if (data->setup(desc))
					return data;
			}
			else
			{
#if defined(OCTOON_FEATURE_HAL_USE_OPENGL45)
				auto data = std::make_shared<GL45GraphicsData>();
				data->setDevice(this->downcast_pointer<GL33Device>());
				if (data->setup(desc))
					return data;
#endif
			}
			return nullptr;
		}

		std::shared_ptr<GraphicsTexture>
		GL33Device::createTexture(const GraphicsTextureDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL33)
			{
				auto texture = std::make_shared<GL33Texture>();
				texture->setDevice(this->downcast_pointer<GL33Device>());
				if (texture->setup(desc))
					return texture;
			}
			else
			{
#if defined(OCTOON_FEATURE_HAL_USE_OPENGL45)
				auto texture = std::make_shared<GL45Texture>();
				texture->setDevice(this->downcast_pointer<GL33Device>());
				if (texture->setup(desc))
					return texture;
#endif
			}
			return nullptr;
		}

		GraphicsSamplerPtr
		GL33Device::createSampler(const GraphicsSamplerDesc& desc) noexcept
		{
			auto sampler = std::make_shared<GL33Sampler>();
			sampler->setDevice(this->downcast_pointer<GL33Device>());
			if (sampler->setup(desc))
				return sampler;
			return nullptr;
		}

		GraphicsFramebufferPtr
		GL33Device::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL33)
			{
				auto framebuffer = std::make_shared<GL33Framebuffer>();
				framebuffer->setDevice(this->downcast_pointer<GL33Device>());
				if (framebuffer->setup(desc))
					return framebuffer;
			}
			else
			{
#if defined(OCTOON_FEATURE_HAL_USE_OPENGL45)
				auto framebuffer = std::make_shared<GL45Framebuffer>();
				framebuffer->setDevice(this->downcast_pointer<GL33Device>());
				if (framebuffer->setup(desc))
					return framebuffer;
#endif
			}
			return nullptr;
		}

		GraphicsFramebufferLayoutPtr
		GL33Device::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
		{
			auto framebufferLayout = std::make_shared<GL33FramebufferLayout>();
			framebufferLayout->setDevice(this->downcast_pointer<GL33Device>());
			if (framebufferLayout->setup(desc))
				return framebufferLayout;
			return nullptr;
		}

		std::shared_ptr<RenderState>
		GL33Device::createRenderState(const RenderStateDesc& desc) noexcept
		{
			auto state = std::make_shared<GL33GraphicsState>();
			state->setDevice(this->downcast_pointer<GL33Device>());
			if (state->setup(desc))
				return state;
			return nullptr;
		}

		GraphicsShaderPtr
		GL33Device::createShader(const GraphicsShaderDesc& desc) noexcept
		{
			auto shader = std::make_shared<GL33Shader>();
			shader->setDevice(this->downcast_pointer<GL33Device>());
			if (shader->setup(desc))
				return shader;
			return nullptr;
		}

		GraphicsProgramPtr
		GL33Device::createProgram(const GraphicsProgramDesc& desc) noexcept
		{
			auto program = std::make_shared<GL33Program>();
			program->setDevice(this->downcast_pointer<GL33Device>());
			if (program->setup(desc))
				return program;
			return nullptr;
		}

		GraphicsPipelinePtr
		GL33Device::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL33)
			{
				auto pipeline = std::make_shared<GL33Pipeline>();
				pipeline->setDevice(this->downcast_pointer<GL33Device>());
				if (pipeline->setup(desc))
					return pipeline;
			}
			else
			{
#if defined(OCTOON_FEATURE_HAL_USE_OPENGL45)
				auto pipeline = std::make_shared<GL45Pipeline>();
				pipeline->setDevice(this->downcast_pointer<GL33Device>());
				if (pipeline->setup(desc))
					return pipeline;
#endif
			}
			return nullptr;
		}

		GraphicsDescriptorSetPtr
		GL33Device::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL33)
			{
				auto descriptorSet = std::make_shared<GL33DescriptorSet>();
				descriptorSet->setDevice(this->downcast_pointer<GL33Device>());
				if (descriptorSet->setup(desc))
					return descriptorSet;
			}
			else
			{
#if defined(OCTOON_FEATURE_HAL_USE_OPENGL45)
				auto descriptorSet = std::make_shared<GL45DescriptorSet>();
				descriptorSet->setDevice(this->downcast_pointer<GL33Device>());
				if (descriptorSet->setup(desc))
					return descriptorSet;
#endif
			}

			return nullptr;
		}

		GraphicsDescriptorSetLayoutPtr
		GL33Device::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
		{
			auto descriptorSetLayout = std::make_shared<GL33DescriptorSetLayout>();
			descriptorSetLayout->setDevice(this->downcast_pointer<GL33Device>());
			if (descriptorSetLayout->setup(desc))
				return descriptorSetLayout;
			return nullptr;
		}

		GraphicsDescriptorPoolPtr
		GL33Device::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
		{
			auto descriptorPool = std::make_shared<GL33DescriptorPool>();
			descriptorPool->setDevice(this->downcast_pointer<GL33Device>());
			if (descriptorPool->setup(desc))
				return descriptorPool;
			return nullptr;
		}

		void
		GL33Device::copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
		{
			assert(source);

			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL33)
				source->downcast<GL33DescriptorSet>()->copy(descriptorCopyCount, descriptorCopies);
#if defined(OCTOON_FEATURE_HAL_USE_OPENGL45)
			else
				source->downcast<GL45DescriptorSet>()->copy(descriptorCopyCount, descriptorCopies);
#endif
		}

		const SystemInfo&
		GL33Device::getSystemInfo() const noexcept
		{
			return _deviceProperty->getSystemInfo();
		}

		const GraphicsDeviceDesc&
		GL33Device::getDeviceDesc() const noexcept
		{
			return _deviceDesc;
		}
	}
}