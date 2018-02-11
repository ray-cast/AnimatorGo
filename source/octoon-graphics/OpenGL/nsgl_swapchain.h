#ifndef OCTOON_NSGL_SWAPCHAIN_H_
#define OCTOON_NSGL_SWAPCHAIN_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class NSGLSwapchain : public GraphicsSwapchain
		{
			OctoonDeclareSubClass(NSGLSwapchain, GraphicsSwapchain)
		public:
			NSGLSwapchain() noexcept;
			NSGLSwapchain(GLuint major, GLuint minor) noexcept;
			virtual ~NSGLSwapchain() noexcept;

			bool setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept;
			void close() noexcept;

			void setActive(bool active) noexcept;
			bool getActive() const noexcept;

			void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept;
			void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

			void setSwapInterval(GraphicsSwapInterval interval) noexcept;
			GraphicsSwapInterval getSwapInterval() const noexcept;

			void present() noexcept;

			const GraphicsSwapchainDesc& getGraphicsSwapchainDesc() const noexcept;

		private:
			bool initSurface(const GraphicsSwapchainDesc& swapchainDesc);
			bool initPixelFormat(const GraphicsSwapchainDesc& swapchainDesc) noexcept;
			bool initSwapchain(const GraphicsSwapchainDesc& swapchainDesc) noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			NSGLSwapchain(const NSGLSwapchain&) noexcept = delete;
			NSGLSwapchain& operator=(const NSGLSwapchain&) noexcept = delete;

		private:
			bool _isActive;

			GLuint _major;
			GLuint _minor;

			void* _context;
			void* _pixelFormat;

			GraphicsSwapchainDesc _swapchainDesc;
			GraphicsDeviceWeakPtr _device;

			static NSGLSwapchain* _swapchain;
		};
	}
}

#endif