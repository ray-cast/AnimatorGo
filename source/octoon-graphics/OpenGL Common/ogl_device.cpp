#include "ogl_device.h"
#include "ogl_swapchain.h"

#include <stdarg.h>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(OGLDevice, GraphicsDevice, "OGLDevice")

		OGLDevice::OGLDevice() noexcept
		{
		}

		OGLDevice::~OGLDevice() noexcept
		{
		}

		GraphicsSwapchainPtr
		OGLDevice::createSwapchain(const GraphicsSwapchainDesc& desc) noexcept
		{
			auto swapchain = std::make_shared<OGLSwapchain>();
			swapchain->setDevice(this->downcast_pointer<GraphicsDevice>());
			if (swapchain->setup(desc))
				return swapchain;
			return nullptr;
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