#ifndef OCTOON_OGL_DEVICE_H_
#define OCTOON_OGL_DEVICE_H_

#include <octoon/graphics/graphics_device.h>

namespace octoon
{
	namespace graphics
	{
		class OGLDevice : public GraphicsDevice
		{
			OctoonDeclareSubInterface(OGLDevice, GraphicsDevice)
		public:
			OGLDevice() noexcept;
			~OGLDevice() noexcept;

			GraphicsSwapchainPtr createSwapchain(const GraphicsSwapchainDesc& desc) noexcept override;

			void message(const char* message, ...) noexcept;

		private:
			OGLDevice(const OGLDevice&) = delete;
			OGLDevice& operator=(const OGLDevice&) = delete;
		};
	}
}

#endif