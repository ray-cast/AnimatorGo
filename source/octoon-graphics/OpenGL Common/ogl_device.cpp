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
#if defined(OCTOON_BUILD_PLATFORM_WINDOWS)
			if (swapchain->setup(desc, this->getDeviceDesc().getDeviceType()))
				return swapchain;
#else
			if (swapchain->setup(desc))
				return swapchain;
#endif
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