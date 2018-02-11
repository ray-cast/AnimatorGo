#ifndef OCTOON_WGL_SWAPCHAIN_H_
#define OCTOON_WGL_SWAPCHAIN_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class WGLSwapchain : public GraphicsSwapchain
		{
			OctoonDeclareSubClass(WGLSwapchain, GraphicsSwapchain)
		public:
			WGLSwapchain() noexcept;
			WGLSwapchain(GLuint major, GLuint minor) noexcept;
			virtual ~WGLSwapchain() noexcept;

			bool setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept;
			void close() noexcept;

			void setActive(bool active) noexcept;
			bool getActive() const noexcept;

			void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept;
			void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

			void setSwapInterval(GraphicsSwapInterval interval) noexcept;
			GraphicsSwapInterval getSwapInterval() const noexcept;

			void present() noexcept;

			const GraphicsSwapchainDesc& getGraphicsSwapchainDesc() const noexcept;

		private:
			bool initSurface(const GraphicsSwapchainDesc& swapchainDesc);
			bool initPixelFormat(const GraphicsSwapchainDesc& swapchainDesc) noexcept;
			bool initSwapchain(const GraphicsSwapchainDesc& swapchainDesc) noexcept;

		private:
			WGLSwapchain(const WGLSwapchain&) noexcept = delete;
			WGLSwapchain& operator=(const WGLSwapchain&) noexcept = delete;

		private:
			bool _isActive;

			HDC _hdc;
			HGLRC _context;
			HWND _hwnd;
			HINSTANCE _hinstance;

			GLuint _major;
			GLuint _minor;

			GraphicsSwapchainDesc _swapchainDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif