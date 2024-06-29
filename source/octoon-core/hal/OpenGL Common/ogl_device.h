#ifndef OCTOON_OGL_DEVICE_H_
#define OCTOON_OGL_DEVICE_H_

#include <octoon/hal/graphics_device.h>

namespace octoon
{
	namespace hal
	{
		class OGLDevice : public GraphicsDevice
		{
			OctoonDeclareSubInterface(OGLDevice, GraphicsDevice)
		public:
			OGLDevice() noexcept;
			~OGLDevice() noexcept;

			std::shared_ptr<GraphicsSwapchain> createSwapchain(const GraphicsSwapchainDesc& desc) noexcept override;

			void message(const char* message, ...) noexcept;

		private:
			OGLDevice(const OGLDevice&) = delete;
			OGLDevice& operator=(const OGLDevice&) = delete;
		};
	}
}

#endif