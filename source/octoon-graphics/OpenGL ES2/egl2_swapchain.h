#ifndef OCTOON_EGL2_SWAPCHAIN_H_
#define OCTOON_EGL2_SWAPCHAIN_H_

#include "egl2_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL2Swapchain final : public GraphicsSwapchain
		{
			OctoonDeclareSubClass(EGL2Swapchain, GraphicsSwapchain)
		public:
			EGL2Swapchain() noexcept;
			~EGL2Swapchain() noexcept;

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
			EGL2Swapchain(const EGL2Swapchain&) noexcept = delete;
			EGL2Swapchain& operator=(const EGL2Swapchain&) noexcept = delete;

		private:
			bool _isActive;

			EGLDisplay _display;
			EGLSurface _surface;
			EGLConfig _config;
			EGLContext _context;

			GraphicsSwapchainDesc _swapchainDesc;
			GraphicsDeviceWeakPtr _device;

			static EGL2Swapchain* _swapchain;
		};
	}
}

#endif