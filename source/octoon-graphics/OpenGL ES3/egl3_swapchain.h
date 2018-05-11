#ifndef OCTOON_EGL3_SWAPCHAIN_H_
#define OCTOON_EGL3_SWAPCHAIN_H_

#include "egl3_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL3Swapchain final : public GraphicsSwapchain
		{
			OctoonDeclareSubClass(EGL3Swapchain, GraphicsSwapchain)
		public:
			EGL3Swapchain() noexcept;
			~EGL3Swapchain() noexcept;

			bool setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept;
			void close() noexcept;

			void setActive(bool active) noexcept;
			bool getActive() const noexcept;

			void setSwapInterval(GraphicsSwapInterval interval) noexcept;
			GraphicsSwapInterval getSwapInterval() const noexcept;

			void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept;
			void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

			void present() noexcept;

			const GraphicsSwapchainDesc& getGraphicsSwapchainDesc() const noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			bool initSurface(const GraphicsSwapchainDesc& swapchainDesc);
			bool initPixelFormat(const GraphicsSwapchainDesc& swapchainDesc) noexcept;
			bool initSwapchain(const GraphicsSwapchainDesc& swapchainDesc) noexcept;

		private:
			EGL3Swapchain(const EGL3Swapchain&) noexcept = delete;
			EGL3Swapchain& operator=(const EGL3Swapchain&) noexcept = delete;

		private:
			bool _isActive;

			EGLDisplay _display;
			EGLSurface _surface;
			EGLConfig _config;
			EGLContext _context;

			GraphicsSwapchainDesc _swapchainDesc;
			GraphicsDeviceWeakPtr _device;

			static EGL3Swapchain* _swapchain;
		};
	}
}

#endif
