#include "ogl_device.h"
#include "ogl_device_context.h"
#include "ogl_device_property.h"
#include "ogl_swapchain.h"
#include "ogl_shader.h"
#include "ogl_texture.h"
#include "ogl_framebuffer.h"
#include "ogl_input_layout.h"
#include "ogl_descriptor_set.h"
#include "ogl_graphics_data.h"
#include "ogl_state.h"
#include "ogl_sampler.h"
#include "ogl_pipeline.h"

#include "ogl_core_device_context.h"
#include "ogl_core_texture.h"
#include "ogl_core_framebuffer.h"
#include "ogl_core_descriptor.h"
#include "ogl_core_graphics_data.h"
#include "ogl_core_pipeline.h"

#include <stdarg.h>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLDevice, GraphicsDevice, "OGLDevice")

		OGLDevice::OGLDevice() noexcept
		{
		}

		OGLDevice::~OGLDevice() noexcept
		{
			this->close();
		}

		bool
		OGLDevice::setup(const GraphicsDeviceDesc& desc) noexcept
		{
			auto deviceProperty = std::make_shared<OGLDeviceProperty>();
			deviceProperty->setDevice(this->downcast_pointer<OGLDevice>());
			if (!deviceProperty->setup())
				return false;

			_deviceProperty = deviceProperty;
			_deviceDesc = desc;
			return true;
		}

		void
		OGLDevice::close() noexcept
		{
			_deviceProperty.reset();
		}

		GraphicsSwapchainPtr
		OGLDevice::createSwapchain(const GraphicsSwapchainDesc& desc) noexcept
		{
			auto swapchain = std::make_shared<OGLSwapchain>();
			swapchain->setDevice(this->downcast_pointer<OGLDevice>());
			if (swapchain->setup(desc))
				return swapchain;
			return nullptr;
		}

		GraphicsContextPtr
		OGLDevice::createDeviceContext(const GraphicsContextDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL)
			{
				auto context = std::make_shared<OGLDeviceContext>();
				context->setDevice(this->downcast_pointer<OGLDevice>());
				if (context->setup(desc))
				{
					_deviceContexts.push_back(context);
					return context;
				}
			}
			else
			{
				auto context = std::make_shared<OGLCoreDeviceContext>();
				context->setDevice(this->downcast_pointer<OGLDevice>());
				if (context->setup(desc))
				{
					_deviceContexts.push_back(context);
					return context;
				}
			}

			return nullptr;
		}

		GraphicsInputLayoutPtr
		OGLDevice::createInputLayout(const GraphicsInputLayoutDesc& desc) noexcept
		{
			auto inputLayout = std::make_shared<OGLInputLayout>();
			inputLayout->setDevice(this->downcast_pointer<OGLDevice>());
			if (inputLayout->setup(desc))
				return inputLayout;
			return nullptr;
		}

		GraphicsDataPtr
		OGLDevice::createGraphicsData(const GraphicsDataDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL)
			{
				auto data = std::make_shared<OGLGraphicsData>();
				data->setDevice(this->downcast_pointer<OGLDevice>());
				if (data->setup(desc))
					return data;
			}
			else
			{
				auto data = std::make_shared<OGLCoreGraphicsData>();
				data->setDevice(this->downcast_pointer<OGLDevice>());
				if (data->setup(desc))
					return data;
			}
			return nullptr;
		}

		GraphicsTexturePtr
		OGLDevice::createTexture(const GraphicsTextureDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL)
			{
				auto texture = std::make_shared<OGLTexture>();
				texture->setDevice(this->downcast_pointer<OGLDevice>());
				if (texture->setup(desc))
					return texture;
			}
			else
			{
				auto texture = std::make_shared<OGLCoreTexture>();
				texture->setDevice(this->downcast_pointer<OGLDevice>());
				if (texture->setup(desc))
					return texture;
			}
			return nullptr;
		}

		GraphicsSamplerPtr
		OGLDevice::createSampler(const GraphicsSamplerDesc& desc) noexcept
		{
			auto sampler = std::make_shared<OGLSampler>();
			sampler->setDevice(this->downcast_pointer<OGLDevice>());
			if (sampler->setup(desc))
				return sampler;
			return nullptr;
		}

		GraphicsFramebufferPtr
		OGLDevice::createFramebuffer(const GraphicsFramebufferDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL)
			{
				auto framebuffer = std::make_shared<OGLFramebuffer>();
				framebuffer->setDevice(this->downcast_pointer<OGLDevice>());
				if (framebuffer->setup(desc))
					return framebuffer;
			}
			else
			{
				auto framebuffer = std::make_shared<OGLCoreFramebuffer>();
				framebuffer->setDevice(this->downcast_pointer<OGLDevice>());
				if (framebuffer->setup(desc))
					return framebuffer;
			}
			return nullptr;
		}

		GraphicsFramebufferLayoutPtr
		OGLDevice::createFramebufferLayout(const GraphicsFramebufferLayoutDesc& desc) noexcept
		{
			auto framebufferLayout = std::make_shared<OGLFramebufferLayout>();
			framebufferLayout->setDevice(this->downcast_pointer<OGLDevice>());
			if (framebufferLayout->setup(desc))
				return framebufferLayout;
			return nullptr;
		}

		GraphicsStatePtr
		OGLDevice::createRenderState(const GraphicsStateDesc& desc) noexcept
		{
			auto state = std::make_shared<OGLGraphicsState>();
			state->setDevice(this->downcast_pointer<OGLDevice>());
			if (state->setup(desc))
				return state;
			return nullptr;
		}

		GraphicsShaderPtr
		OGLDevice::createShader(const GraphicsShaderDesc& desc) noexcept
		{
			auto shader = std::make_shared<OGLShader>();
			shader->setDevice(this->downcast_pointer<OGLDevice>());
			if (shader->setup(desc))
				return shader;
			return nullptr;
		}

		GraphicsProgramPtr
		OGLDevice::createProgram(const GraphicsProgramDesc& desc) noexcept
		{
			auto program = std::make_shared<OGLProgram>();
			program->setDevice(this->downcast_pointer<OGLDevice>());
			if (program->setup(desc))
				return program;
			return nullptr;
		}

		GraphicsPipelinePtr
		OGLDevice::createRenderPipeline(const GraphicsPipelineDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL)
			{
				auto pipeline = std::make_shared<OGLPipeline>();
				pipeline->setDevice(this->downcast_pointer<OGLDevice>());
				if (pipeline->setup(desc))
					return pipeline;
			}
			else
			{
				auto pipeline = std::make_shared<OGLCorePipeline>();
				pipeline->setDevice(this->downcast_pointer<OGLDevice>());
				if (pipeline->setup(desc))
					return pipeline;
			}
			return nullptr;
		}

		GraphicsDescriptorSetPtr
		OGLDevice::createDescriptorSet(const GraphicsDescriptorSetDesc& desc) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL)
			{
				auto descriptorSet = std::make_shared<OGLDescriptorSet>();
				descriptorSet->setDevice(this->downcast_pointer<OGLDevice>());
				if (descriptorSet->setup(desc))
					return descriptorSet;
			}
			else
			{
				auto descriptorSet = std::make_shared<OGLCoreDescriptorSet>();
				descriptorSet->setDevice(this->downcast_pointer<OGLDevice>());
				if (descriptorSet->setup(desc))
					return descriptorSet;
			}

			return nullptr;
		}

		GraphicsDescriptorSetLayoutPtr
		OGLDevice::createDescriptorSetLayout(const GraphicsDescriptorSetLayoutDesc& desc) noexcept
		{
			auto descriptorSetLayout = std::make_shared<OGLDescriptorSetLayout>();
			descriptorSetLayout->setDevice(this->downcast_pointer<OGLDevice>());
			if (descriptorSetLayout->setup(desc))
				return descriptorSetLayout;
			return nullptr;
		}

		GraphicsDescriptorPoolPtr
		OGLDevice::createDescriptorPool(const GraphicsDescriptorPoolDesc& desc) noexcept
		{
			auto descriptorPool = std::make_shared<OGLDescriptorPool>();
			descriptorPool->setDevice(this->downcast_pointer<OGLDevice>());
			if (descriptorPool->setup(desc))
				return descriptorPool;
			return nullptr;
		}

		void
		OGLDevice::enableDebugControl(bool enable) noexcept
		{
			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL)
			{
				for (auto& it : _deviceContexts)
				{
					auto deviceContext = it.lock();
					if (deviceContext)
						deviceContext->downcast<OGLDeviceContext>()->enableDebugControl(enable);
				}
			}
			else if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGLCore)
			{
				for (auto& it : _deviceContexts)
				{
					auto deviceContext = it.lock();
					if (deviceContext)
						deviceContext->downcast<OGLCoreDeviceContext>()->enableDebugControl(enable);
				}
			}
		}

		void
		OGLDevice::copyDescriptorSets(GraphicsDescriptorSetPtr& source, std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept
		{
			assert(source);

			if (_deviceDesc.getDeviceType() == GraphicsDeviceType::OpenGL)
				source->downcast<OGLDescriptorSet>()->copy(descriptorCopyCount, descriptorCopies);
			else
				source->downcast<OGLCoreDescriptorSet>()->copy(descriptorCopyCount, descriptorCopies);
		}

		const GraphicsDeviceProperty&
		OGLDevice::getDeviceProperty() const noexcept
		{
			return *_deviceProperty;
		}

		const GraphicsDeviceDesc&
		OGLDevice::getGraphicsDeviceDesc() const noexcept
		{
			return _deviceDesc;
		}

		void
		OGLDevice::message(const char* message, ...) noexcept
		{
			va_list va;
			va_start(va, message);
			vprintf(message, va);
			printf("\n");
			va_end(va);
		}
	}
}