#ifndef OCTOON_X11_SWAPCHAIN_H_
#define OCTOON_X11_SWAPCHAIN_H_

#include "ogl_basic.h"

namespace octoon
{
	namespace graphics
	{
		class XGLSwapchain : public GraphicsSwapchain
		{
			OctoonDeclareSubClass(XGLSwapchain, GraphicsSwapchain)
		public:
			XGLSwapchain() noexcept;
			virtual ~XGLSwapchain() noexcept;

			bool setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept;
			void close() noexcept;

			void setActive(bool active) noexcept;
			bool getActive() noexcept;

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

		private:
			XGLSwapchain(const XGLSwapchain&) noexcept = delete;
			XGLSwapchain& operator=(const XGLSwapchain&) noexcept = delete;

		private:
			bool _isActive;

			Window _window;
			Display* _display;
			XVisualInfo* _vi;
			Colormap _cmap;

			GLXContext _glc;
			GLXFBConfig* _cfg;

			GraphicsSwapchainDesc _swapchainDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif