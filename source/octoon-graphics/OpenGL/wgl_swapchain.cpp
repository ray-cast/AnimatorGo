#if defined(OCTOON_BUILD_PLATFORM_WINDOWS)
#include "wgl_swapchain.h"
#include "ogl_device.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(WGLSwapchain, GraphicsSwapchain, "WGLSwapchain")

		WGLSwapchain::WGLSwapchain() noexcept
			: _hdc(nullptr)
			, _context(nullptr)
			, _isActive(false)
			, _major(3)
			, _minor(3)
			, _hwnd(nullptr)
			, _hinstance(nullptr)
		{
		}

		WGLSwapchain::WGLSwapchain(GLuint major, GLuint minor) noexcept
			: _hdc(nullptr)
			, _context(nullptr)
			, _isActive(false)
			, _major(major)
			, _minor(minor)
			, _hwnd(nullptr)
			, _hinstance(nullptr)
		{
		}

		WGLSwapchain::~WGLSwapchain() noexcept
		{
			this->close();
		}

		bool
		WGLSwapchain::setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept
		{
			if (!initSurface(swapchainDesc))
				return false;

			if (!initPixelFormat(swapchainDesc))
				return false;

			if (!initSwapchain(swapchainDesc))
				return false;

			_swapchainDesc = swapchainDesc;
			return true;
		}

		void
		WGLSwapchain::close() noexcept
		{
			this->setActive(false);

			if (_context)
			{
				::wglDeleteContext(_context);
				_context = nullptr;
			}

			if (_hdc)
			{
				::ReleaseDC(_hwnd ? _hwnd : (HWND)_swapchainDesc.getWindHandle(), _hdc);
				_hdc = nullptr;
			}

			if (_hwnd)
			{
				::DestroyWindow(_hwnd);
				_hwnd = nullptr;
			}

			if (_hinstance)
			{
				::UnregisterClass("OGL", GetModuleHandle(NULL));
				_hinstance = nullptr;
			}
		}

		void
		WGLSwapchain::setActive(bool active) noexcept
		{
			static thread_local WGLSwapchain* _swapchain = nullptr;

			if (active)
			{
				if (_swapchain && _swapchain != this)
					_swapchain->setActive(false);

				if (!::wglMakeCurrent(_hdc, _context))
					this->getDevice()->downcast<OGLDevice>()->message("wglMakeCurrent() fail");

				_swapchain = this;
			}
			else
			{
				if (!::wglMakeCurrent(0, 0))
					this->getDevice()->downcast<OGLDevice>()->message("wglMakeCurrent() fail");

				if (_swapchain == this)
					_swapchain = nullptr;
			}

			_isActive = active;
		}

		bool
		WGLSwapchain::getActive() const noexcept
		{
			return _isActive;
		}

		void
		WGLSwapchain::setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept
		{
			_swapchainDesc.setWidth(w);
			_swapchainDesc.setHeight(h);
		}

		void
		WGLSwapchain::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
		{
			w = _swapchainDesc.getWidth();
			h = _swapchainDesc.getHeight();
		}

		void
		WGLSwapchain::setSwapInterval(GraphicsSwapInterval interval) noexcept
		{
			switch (interval)
			{
			case GraphicsSwapInterval::Free:
				if (!wglSwapIntervalEXT(0))
					this->getDevice()->downcast<OGLDevice>()->message("wglSwapInterval(SwapInterval::Free) fail");
				break;
			case GraphicsSwapInterval::Vsync:
				if (!wglSwapIntervalEXT(1))
					this->getDevice()->downcast<OGLDevice>()->message("wglSwapInterval(SwapInterval::Vsync) fail");
				break;
			case GraphicsSwapInterval::Fps30:
				if (!wglSwapIntervalEXT(2))
					this->getDevice()->downcast<OGLDevice>()->message("wglSwapInterval(SwapInterval::Fps30) fail");
				break;
			case GraphicsSwapInterval::Fps15:
				if (!wglSwapIntervalEXT(3))
					this->getDevice()->downcast<OGLDevice>()->message("wglSwapInterval(SwapInterval::Fps15) fail");
				break;
			default:
				this->getDevice()->downcast<OGLDevice>()->message("Invalid swap interval");
			}

			_swapchainDesc.setSwapInterval(interval);
		}

		GraphicsSwapInterval
		WGLSwapchain::getSwapInterval() const noexcept
		{
			return _swapchainDesc.getSwapInterval();
		}

		void
		WGLSwapchain::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		WGLSwapchain::getDevice() noexcept
		{
			return _device.lock();
		}

		void
		WGLSwapchain::present() noexcept
		{
			if (_swapchainDesc.getWindHandle())
				::SwapBuffers(_hdc);
		}

		bool
		WGLSwapchain::initSurface(const GraphicsSwapchainDesc& swapchainDesc)
		{
			if (swapchainDesc.getWindHandle())
			{
				if (!IsWindow((HWND)swapchainDesc.getWindHandle()))
				{
					this->getDevice()->downcast<OGLDevice>()->message("Invlid HWND");
					return false;
				}

				HWND hwnd = (HWND)swapchainDesc.getWindHandle();
				_hdc = ::GetDC(hwnd);
				if (!_hdc)
				{
					this->getDevice()->downcast<OGLDevice>()->message("GetDC() fail");
					return false;
				}
			}
			else
			{
				WNDCLASSEXA wc;
				std::memset(&wc, 0, sizeof(wc));
				wc.cbSize = sizeof(wc);
				wc.hInstance = ::GetModuleHandle(NULL);
				wc.lpfnWndProc = ::DefWindowProc;
				wc.lpszClassName = "OGL";
				if (!::RegisterClassEx(&wc))
					return false;

				DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
				_hwnd = CreateWindowEx(WS_EX_APPWINDOW, wc.lpszClassName, "OGL", style, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wc.hInstance, 0);
				if (!_hwnd)
				{
					this->getDevice()->downcast<OGLDevice>()->message("CreateWindowEx() fail");
					return false;
				}

				_hdc = ::GetDC(_hwnd);
				if (!_hdc)
				{
					this->getDevice()->downcast<OGLDevice>()->message("GetDC() fail");
					return false;
				}
			}

			return true;
		}

		bool
		WGLSwapchain::initPixelFormat(const GraphicsSwapchainDesc& swapchainDesc) noexcept
		{
			PIXELFORMATDESCRIPTOR pfd;
			std::memset(&pfd, 0, sizeof(pfd));
			pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion = 1;
			pfd.dwFlags |= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_STEREO;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.cAccumBits = 0;
			pfd.cAccumRedBits = 0;
			pfd.cAccumGreenBits = 0;
			pfd.cAccumBlueBits = 0;
			pfd.cAccumAlphaBits = 0;
			pfd.cAuxBuffers = 0;
			pfd.iLayerType = PFD_MAIN_PLANE;
			pfd.bReserved = 0;
			pfd.dwLayerMask = 0;
			pfd.dwVisibleMask = 0;
			pfd.dwDamageMask = 0;

			if (swapchainDesc.getImageNums() == 2)
				pfd.dwFlags |= PFD_DOUBLEBUFFER;
			else if (swapchainDesc.getImageNums() != 1)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid image number");
				return false;
			}

			auto colorFormat = swapchainDesc.getColorFormat();
			if (colorFormat == GraphicsFormat::B8G8R8A8UNorm)
			{
				pfd.cColorBits = 32;
				pfd.cBlueBits = 8;
				pfd.cBlueShift = 0;
				pfd.cGreenBits = 8;
				pfd.cGreenShift = 8;
				pfd.cRedBits = 8;
				pfd.cRedShift = 16;
				pfd.cAlphaBits = 8;
				pfd.cAlphaShift = 24;
			}
			else
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support color format");
				return false;
			}

			auto depthStencilFormat = swapchainDesc.getDepthStencilFormat();
			if (depthStencilFormat == GraphicsFormat::D16UNorm)
			{
				pfd.cDepthBits = 16;
				pfd.cStencilBits = 0;
			}
			else if (depthStencilFormat == GraphicsFormat::X8_D24UNormPack32)
			{
				pfd.cDepthBits = 24;
				pfd.cStencilBits = 0;
			}
			else if (depthStencilFormat == GraphicsFormat::D32_SFLOAT)
			{
				pfd.cDepthBits = 32;
				pfd.cStencilBits = 0;
			}
			else if (depthStencilFormat == GraphicsFormat::D16UNorm_S8UInt)
			{
				pfd.cDepthBits = 16;
				pfd.cStencilBits = 8;
			}
			else if (depthStencilFormat == GraphicsFormat::D24UNorm_S8UInt)
			{
				pfd.cDepthBits = 24;
				pfd.cStencilBits = 8;
			}
			else if (depthStencilFormat == GraphicsFormat::D32_SFLOAT_S8UInt)
			{
				pfd.cDepthBits = 32;
				pfd.cStencilBits = 8;
			}
			else
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support depth stencil format");
				return false;
			}

			int pixelFormat = ::ChoosePixelFormat(_hdc, &pfd);
			if (!pixelFormat)
			{
				this->getDevice()->downcast<OGLDevice>()->message("ChoosePixelFormat() fail");
				return false;
			}

			PIXELFORMATDESCRIPTOR pfd2;
			if (!::DescribePixelFormat(_hdc, pixelFormat, sizeof(pfd2), &pfd2))
			{
				this->getDevice()->downcast<OGLDevice>()->message("DescribePixelFormat() fail");
				return false;
			}

			if (pfd2.cRedBits != pfd.cRedBits ||
				pfd2.cRedShift != pfd.cRedShift ||
				pfd2.cGreenBits != pfd.cGreenBits ||
				pfd2.cGreenShift != pfd.cGreenShift ||
				pfd2.cBlueBits != pfd.cBlueBits ||
				pfd2.cBlueShift != pfd.cBlueShift)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support color format");
				return false;
			}

			if (pfd2.cDepthBits != pfd.cDepthBits ||
				pfd2.cStencilBits != pfd.cStencilBits)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Can't support depth stencil format");
				return false;
			}

			if (!::SetPixelFormat(_hdc, pixelFormat, &pfd2))
			{
				this->getDevice()->downcast<OGLDevice>()->message("SetPixelFormat() fail");
				return false;
			}

			return true;
		}

		bool
		WGLSwapchain::initSwapchain(const GraphicsSwapchainDesc& swapchainDesc) noexcept
		{
			if (!wglCreateContextAttribsARB)
				_context = wglCreateContext(_hdc);
			else
			{
				int index = 0;
				int mask = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
				int flags = 0;

#if __DEBUG__
				flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#endif

				int major = 0;
				int minor = 0;

				auto deviceType = this->getDevice()->getGraphicsDeviceDesc().getDeviceType();
				if (deviceType == GraphicsDeviceType::OpenGLCore)
				{
					major = 4;
					minor = 5;
				}
				else
				{
					major = 3;
					minor = 3;
				}

				int attribs[40];
				attribs[index++] = WGL_CONTEXT_MAJOR_VERSION_ARB;
				attribs[index++] = major;

				attribs[index++] = WGL_CONTEXT_MINOR_VERSION_ARB;
				attribs[index++] = minor;

				attribs[index++] = WGL_CONTEXT_FLAGS_ARB;
				attribs[index++] = flags;

				attribs[index++] = WGL_CONTEXT_PROFILE_MASK_ARB;
				attribs[index++] = mask;

				attribs[index++] = 0;
				attribs[index] = 0;

				_context = wglCreateContextAttribsARB(_hdc, nullptr, attribs);
			}

			if (!_context)
			{
				this->getDevice()->downcast<OGLDevice>()->message("wglCreateContextAttribs fail");
				return false;
			}

			if (swapchainDesc.getWindHandle())
			{
				this->setActive(true);
				this->setSwapInterval(swapchainDesc.getSwapInterval());
				this->setActive(false);
			}

			return true;
		}

		const GraphicsSwapchainDesc&
		WGLSwapchain::getGraphicsSwapchainDesc() const noexcept
		{
			return _swapchainDesc;
		}
	}
}

#endif