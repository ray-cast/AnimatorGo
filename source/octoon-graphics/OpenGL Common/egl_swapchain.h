#ifndef OCTOON_EGL_SWAPCHAIN_H_
#define OCTOON_EGL_SWAPCHAIN_H_

#include "ogl_basic.h"

namespace octoon
{
	namespace graphics
	{
		class EGLSwapchain final : public GraphicsSwapchain
		{
			OctoonDeclareSubClass(EGLSwapchain, GraphicsSwapchain)
		public:
			EGLSwapchain() noexcept;
			~EGLSwapchain() noexcept;

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
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			bool initSurface(const GraphicsSwapchainDesc& swapchainDesc);
			bool initPixelFormat(const GraphicsSwapchainDesc& swapchainDesc) noexcept;
			bool initSwapchain(const GraphicsSwapchainDesc& swapchainDesc) noexcept;

		private:
			EGLSwapchain(const EGLSwapchain&) noexcept = delete;
			EGLSwapchain& operator=(const EGLSwapchain&) noexcept = delete;

		private:
			bool _isActive;

			EGLDisplay _display;
			EGLSurface _surface;
			EGLConfig _config;
			EGLContext _context;

			GraphicsSwapchainDesc _swapchainDesc;
			GraphicsDeviceWeakPtr _device;

			static EGLSwapchain* _swapchain;
		};
	}
}

#endif