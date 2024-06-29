#include "wgl_swapchain.h"
#include "ogl_device.h"

#include <stdexcept>

namespace octoon
{
	namespace hal
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
		WGLSwapchain::setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept(false)
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
				::UnregisterClass("GL33", GetModuleHandle(NULL));
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
		WGLSwapchain::getDevice() const noexcept
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
				wc.hInstance = this->_hinstance = ::GetModuleHandle(NULL);
				wc.lpfnWndProc = ::DefWindowProc;
				wc.lpszClassName = "OctoonWGLSwapchain";
				if (!::RegisterClassEx(&wc))
					return false;

				DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
				_hwnd = CreateWindowEx(WS_EX_APPWINDOW, wc.lpszClassName, "OctoonWGLSwapchain", style, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wc.hInstance, 0);
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
		WGLSwapchain::initPixelFormat(const GraphicsSwapchainDesc& swapchainDesc) noexcept(false)
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

			std::size_t index = 0;

			int attribs[64];
			attribs[index++] = WGL_DRAW_TO_WINDOW_ARB;
			attribs[index++] = GL_TRUE;

			attribs[index++] = WGL_SUPPORT_OPENGL_ARB;
			attribs[index++] = GL_TRUE;

			attribs[index++] = WGL_ACCELERATION_ARB;
			attribs[index++] = WGL_FULL_ACCELERATION_ARB;

			attribs[index++] = WGL_DOUBLE_BUFFER_ARB;
			attribs[index++] = GL_TRUE;

			attribs[index++] = WGL_SWAP_METHOD_ARB;
			attribs[index++] = WGL_SWAP_EXCHANGE_ARB;

			attribs[index++] = WGL_SAMPLES_ARB;
			attribs[index++] = swapchainDesc.getMultiSample();

			switch (swapchainDesc.getColorFormat())
			{
			case GraphicsFormat::B8G8R8A8UNorm:
			{
				attribs[index++] = WGL_PIXEL_TYPE_ARB;
				attribs[index++] = WGL_TYPE_RGBA_ARB;
				attribs[index++] = WGL_COLOR_BITS_ARB;
				attribs[index++] = 32;
				attribs[index++] = WGL_RED_BITS_ARB;
				attribs[index++] = 8;
				attribs[index++] = WGL_GREEN_BITS_ARB;
				attribs[index++] = 8;
				attribs[index++] = WGL_BLUE_BITS_ARB;
				attribs[index++] = 8;
				attribs[index++] = WGL_ALPHA_BITS_ARB;
				attribs[index++] = 8;
			}
			break;
			case GraphicsFormat::R16G16B16A16SFloat:
			{
				attribs[index++] = WGL_PIXEL_TYPE_ARB;
				attribs[index++] = WGL_TYPE_RGBA_FLOAT_ARB;
				attribs[index++] = WGL_RED_BITS_ARB;
				attribs[index++] = 16;
				attribs[index++] = WGL_GREEN_BITS_ARB;
				attribs[index++] = 16;
				attribs[index++] = WGL_BLUE_BITS_ARB;
				attribs[index++] = 16;
				attribs[index++] = WGL_ALPHA_BITS_ARB;
				attribs[index++] = 16;
			}
			break;
			case GraphicsFormat::R32G32B32A32SFloat:
			{
				attribs[index++] = WGL_PIXEL_TYPE_ARB;
				attribs[index++] = WGL_TYPE_RGBA_FLOAT_ARB;
				attribs[index++] = WGL_RED_BITS_ARB;
				attribs[index++] = 32;
				attribs[index++] = WGL_GREEN_BITS_ARB;
				attribs[index++] = 32;
				attribs[index++] = WGL_BLUE_BITS_ARB;
				attribs[index++] = 32;
				attribs[index++] = WGL_ALPHA_BITS_ARB;
				attribs[index++] = 32;
			}
			break;
			default:
				throw std::runtime_error("Can't support color format");
			}

			switch (swapchainDesc.getDepthStencilFormat())
			{
			case GraphicsFormat::D16UNorm:
			{
				attribs[index++] = WGL_DEPTH_BITS_ARB;
				attribs[index++] = 16;
				attribs[index++] = WGL_STENCIL_BITS_ARB;
				attribs[index++] = 0;
			}
			break;
			case GraphicsFormat::X8_D24UNormPack32:
			{
				attribs[index++] = WGL_DEPTH_BITS_ARB;
				attribs[index++] = 24;
				attribs[index++] = WGL_STENCIL_BITS_ARB;
				attribs[index++] = 0;
			}
			break;
			case GraphicsFormat::D32_SFLOAT:
			{
				attribs[index++] = WGL_DEPTH_BITS_ARB;
				attribs[index++] = 32;
				attribs[index++] = WGL_STENCIL_BITS_ARB;
				attribs[index++] = 0;
			}
			break;
			case GraphicsFormat::D16UNorm_S8UInt:
			{
				attribs[index++] = WGL_DEPTH_BITS_ARB;
				attribs[index++] = 16;
				attribs[index++] = WGL_STENCIL_BITS_ARB;
				attribs[index++] = 8;
			}
			break;
			case GraphicsFormat::D24UNorm_S8UInt:
			{
				attribs[index++] = WGL_DEPTH_BITS_ARB;
				attribs[index++] = 24;
				attribs[index++] = WGL_STENCIL_BITS_ARB;
				attribs[index++] = 8;
			}
			break;
			case GraphicsFormat::D32_SFLOAT_S8UInt:
			{
				attribs[index++] = WGL_DEPTH_BITS_ARB;
				attribs[index++] = 32;
				attribs[index++] = WGL_STENCIL_BITS_ARB;
				attribs[index++] = 8;
			}
			break;
			default:
				throw std::runtime_error("Can't support depth stencil format");
			}

			attribs[index++] = GL_NONE;
			attribs[index++] = GL_NONE;

			int numFormat;
			int pixelFormat = 0;
			wglChoosePixelFormatARB(_hdc, attribs, NULL, 1, &pixelFormat, (UINT*)&numFormat);

			if (!::SetPixelFormat(_hdc, pixelFormat, 0))
			{
				this->getDevice()->downcast<OGLDevice>()->message("SetPixelFormat() fail");
				return false;
			}

			return true;
		}

		bool
		WGLSwapchain::initSwapchain(const GraphicsSwapchainDesc& swapchainDesc) noexcept
		{
			auto deviceType = this->getDevice()->getDeviceDesc().getDeviceType();

			int mask = (deviceType == GraphicsDeviceType::OpenGL20) ? WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB : WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
			int flags = 0;
#if defined(__DEBUG__)
			flags = WGL_CONTEXT_DEBUG_BIT_ARB;
#endif
			int major = 0;
			int minor = 0;

			switch (deviceType)
			{
			case GraphicsDeviceType::OpenGL20:
				major = 2;
				minor = 0;
				break;
			case GraphicsDeviceType::OpenGL30:
				major = 3;
				minor = 0;
				break;
			case GraphicsDeviceType::OpenGL32:
				major = 3;
				minor = 2;
				break;
			case GraphicsDeviceType::OpenGL33:
				major = 3;
				minor = 3;
				break;
			case GraphicsDeviceType::OpenGL45:
				major = 4;
				minor = 5;
				break;
			}

			int attribs[] =
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, major,
				WGL_CONTEXT_MINOR_VERSION_ARB, minor,
				WGL_CONTEXT_FLAGS_ARB, flags,
				WGL_CONTEXT_PROFILE_MASK_ARB, mask,
				GL_NONE, GL_NONE
			};

			_context = wglCreateContextAttribsARB(_hdc, nullptr, attribs);
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