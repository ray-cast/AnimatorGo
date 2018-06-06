#include "egl_swapchain.h"
#include "ogl_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(EGLSwapchain, GraphicsSwapchain, "EGLSwapchain")

		EGLSwapchain::EGLSwapchain() noexcept
			: _display(EGL_NO_DISPLAY)
			, _surface(EGL_NO_SURFACE)
			, _context(EGL_NO_CONTEXT)
			, _config(0)
			, _isActive(false)
		{
		}

		EGLSwapchain::~EGLSwapchain() noexcept
		{
			this->close();
		}

		bool
		EGLSwapchain::setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept
		{
			if (!initPixelFormat(swapchainDesc))
				return false;

			if (!initSurface(swapchainDesc))
				return false;

			if (!initSwapchain(swapchainDesc))
				return false;

			_swapchainDesc = swapchainDesc;
			return true;
		}

		void
		EGLSwapchain::close() noexcept
		{
			if (_surface != EGL_NO_SURFACE)
			{
				::eglDestroySurface(_display, _surface);
				_surface = EGL_NO_SURFACE;
			}

			if (_context != EGL_NO_CONTEXT)
			{
				::eglDestroyContext(_display, _context);
				_context = EGL_NO_CONTEXT;
			}

			if (_display != EGL_NO_DISPLAY)
			{
				::eglTerminate(_display);
				_display = EGL_NO_DISPLAY;
			}
		}

		void
		EGLSwapchain::setActive(bool active) noexcept
		{
			static thread_local EGLSwapchain* _swapchain = nullptr;

			if (active)
			{
				if (_swapchain && _swapchain != this)
					_swapchain->setActive(false);

				if (::eglMakeCurrent(_display, _surface, _surface, _context) == EGL_FALSE)
					this->getDevice()->downcast<OGLDevice>()->message("eglMakeCurrent() fail");

				_swapchain = this;
			}
			else
			{
				if (::eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) == EGL_FALSE)
					this->getDevice()->downcast<OGLDevice>()->message("eglMakeCurrent() fail");

				if (_swapchain == this)
					_swapchain = nullptr;
			}

			_isActive = active;
		}

		bool
		EGLSwapchain::getActive() const noexcept
		{
			return _isActive;
		}

		void
		EGLSwapchain::setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept
		{
			_swapchainDesc.setWidth(w);
			_swapchainDesc.setHeight(h);
		}

		void
		EGLSwapchain::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
		{
			w = _swapchainDesc.getWidth();
			h = _swapchainDesc.getHeight();
		}

		void
		EGLSwapchain::setSwapInterval(GraphicsSwapInterval interval) noexcept
		{
			assert(_display != EGL_NO_DISPLAY);

			switch (interval)
			{
			case GraphicsSwapInterval::Free:
				eglSwapInterval(_display, 0);
				break;
			case GraphicsSwapInterval::Vsync:
				eglSwapInterval(_display, 1);
				break;
			case GraphicsSwapInterval::Fps30:
				eglSwapInterval(_display, 2);
				break;
			case GraphicsSwapInterval::Fps15:
				eglSwapInterval(_display, 3);
				break;
			default:
				return;
			}

			_swapchainDesc.setSwapInterval(interval);
		}

		GraphicsSwapInterval
		EGLSwapchain::getSwapInterval() const noexcept
		{
			return _swapchainDesc.getSwapInterval();
		}

		void
		EGLSwapchain::present() noexcept
		{
			assert(_isActive);
			assert(_display != EGL_NO_DISPLAY);
			assert(_surface != EGL_NO_SURFACE);

			::eglSwapBuffers(_display, _surface);
		}

		bool
		EGLSwapchain::initSurface(const GraphicsSwapchainDesc& swapchainDesc)
		{
			EGLNativeWindowType hwnd = (EGLNativeWindowType)swapchainDesc.getWindHandle();
			_surface = ::eglCreateWindowSurface(_display, _config, hwnd, NULL);
			if (eglGetError() != EGL_SUCCESS)
				return false;

			return true;
		}

		bool
		EGLSwapchain::initPixelFormat(const GraphicsSwapchainDesc& swapchainDesc) noexcept
		{
			EGLint pixelFormat[80];
			EGLint index = 0;

			pixelFormat[index++] = EGL_SURFACE_TYPE;
			pixelFormat[index++] = EGL_WINDOW_BIT;

			pixelFormat[index++] = EGL_RENDERABLE_TYPE;
			pixelFormat[index++] = EGL_OPENGL_ES2_BIT;

			if (swapchainDesc.getImageNums() != 2)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid image number");
				return false;
			}

			auto colorFormat = swapchainDesc.getColorFormat();
			if (colorFormat == GraphicsFormat::B8G8R8A8UNorm)
			{
				pixelFormat[index++] = EGL_BUFFER_SIZE;
				pixelFormat[index++] = 32;

				pixelFormat[index++] = EGL_RED_SIZE;
				pixelFormat[index++] = 8;

				pixelFormat[index++] = EGL_GREEN_SIZE;
				pixelFormat[index++] = 8;

				pixelFormat[index++] = EGL_BLUE_SIZE;
				pixelFormat[index++] = 8;

				pixelFormat[index++] = EGL_ALPHA_SIZE;
				pixelFormat[index++] = 8;
			}
			else
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support color format");
				return false;
			}

			auto depthStencilFormat = swapchainDesc.getDepthStencilFormat();
			if (depthStencilFormat == GraphicsFormat::D16UNorm)
			{
				pixelFormat[index++] = EGL_DEPTH_SIZE;
				pixelFormat[index++] = 16;

				pixelFormat[index++] = EGL_STENCIL_SIZE;
				pixelFormat[index++] = 0;
			}
			else if (depthStencilFormat == GraphicsFormat::X8_D24UNormPack32)
			{
				pixelFormat[index++] = EGL_DEPTH_SIZE;
				pixelFormat[index++] = 24;

				pixelFormat[index++] = EGL_STENCIL_SIZE;
				pixelFormat[index++] = 0;
			}
			else if (depthStencilFormat == GraphicsFormat::D32_SFLOAT)
			{
				pixelFormat[index++] = EGL_DEPTH_SIZE;
				pixelFormat[index++] = 32;

				pixelFormat[index++] = EGL_STENCIL_SIZE;
				pixelFormat[index++] = 0;
			}
			else if (depthStencilFormat == GraphicsFormat::D16UNorm_S8UInt)
			{
				pixelFormat[index++] = EGL_DEPTH_SIZE;
				pixelFormat[index++] = 16;

				pixelFormat[index++] = EGL_STENCIL_SIZE;
				pixelFormat[index++] = 8;
			}
			else if (depthStencilFormat == GraphicsFormat::D24UNorm_S8UInt)
			{
				pixelFormat[index++] = EGL_DEPTH_SIZE;
				pixelFormat[index++] = 24;

				pixelFormat[index++] = EGL_STENCIL_SIZE;
				pixelFormat[index++] = 8;
			}
			else if (depthStencilFormat == GraphicsFormat::D32_SFLOAT_S8UInt)
			{
				pixelFormat[index++] = EGL_DEPTH_SIZE;
				pixelFormat[index++] = 32;

				pixelFormat[index++] = EGL_STENCIL_SIZE;
				pixelFormat[index++] = 8;
			}
			else
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support depth stencil format");
				return false;
			}

			pixelFormat[index++] = EGL_NONE;

			_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
			if (_display == EGL_NO_DISPLAY)
			{
				this->getDevice()->downcast<OGLDevice>()->message("eglGetDisplay() fail.");
				return false;
			}

			if (::eglInitialize(_display, nullptr, nullptr) == EGL_FALSE)
			{
				this->getDevice()->downcast<OGLDevice>()->message("eglInitialize() fail.");
				return false;
			}

			if (::eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
			{
				this->getDevice()->downcast<OGLDevice>()->message("eglBindAPI() fail.");
				return false;
			}

			EGLint num = 0;
			if (::eglChooseConfig(_display, pixelFormat, &_config, 1, &num) == EGL_FALSE)
			{
				this->getDevice()->downcast<OGLDevice>()->message("eglChooseConfig() fail.");
				return false;
			}

			return true;
		}

		bool
		EGLSwapchain::initSwapchain(const GraphicsSwapchainDesc& swapchainDesc) noexcept
		{
			EGLint attribs[80];
			EGLint index = 0;

		#if defined(OCTOON_BUILD_PLATFORM_ANDROID)
			attribs[index++] = EGL_CONTEXT_CLIENT_VERSION;
			attribs[index++] = 2;
		#elif defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
			attribs[index++] = EGL_CONTEXT_CLIENT_VERSION;
			attribs[index++] = 2;
		#else
			attribs[index++] = EGL_CONTEXT_MAJOR_VERSION;
			attribs[index++] = 2;

			attribs[index++] = EGL_CONTEXT_MINOR_VERSION;
			attribs[index++] = 0;
		#endif

			attribs[index++] = EGL_NONE;
			attribs[index++] = EGL_NONE;

			_context = ::eglCreateContext(_display, _config, EGL_NO_CONTEXT, attribs);
			if (eglGetError() != EGL_SUCCESS)
			{
				this->getDevice()->downcast<OGLDevice>()->message("eglCreateContext() fail.");
				return false;
			}
#if !defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
			this->setSwapInterval(swapchainDesc.getSwapInterval());
#endif
			return true;
		}

		const GraphicsSwapchainDesc&
		EGLSwapchain::getGraphicsSwapchainDesc() const noexcept
		{
			return _swapchainDesc;
		}

		void
		EGLSwapchain::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGLSwapchain::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}