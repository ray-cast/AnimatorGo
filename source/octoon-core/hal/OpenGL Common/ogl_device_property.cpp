#include "ogl_device_property.h"
#include <octoon/runtime/except.h>

#if defined(OCTOON_BUILD_PLATFORM_WINDOWS)
OCTOON_C_LINKAGE OCTOON_EXPORT DWORD NvOptimusEnablement = 0x00000001;
#endif

namespace octoon
{
	namespace hal
	{
		OGLDeviceProperty::OGLDeviceProperty() noexcept
		{
		}

		OGLDeviceProperty::~OGLDeviceProperty() noexcept
		{
			this->close();
		}

		bool
		OGLDeviceProperty::setup(const GraphicsDeviceDesc& deviceDesc) noexcept
		{
			assert(_device.lock());
#if !defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN) && !defined(OCTOON_BUILD_PLATFORM_JAVA)
			CreateParam params;

			try
			{
				this->setupGLEnvironment(params);

				if (!initGLExtenstion())
					throw runtime_error::create("initGLExtenstion fail");

				if (!initDeviceProperties())
					throw runtime_error::create("initDeviceProperties fail");

				this->closeGLEnvironment(params);
			}
			catch (const std::exception&)
			{
				this->closeGLEnvironment(params);
			}
#endif
			return true;
		}

		void
		OGLDeviceProperty::close() noexcept
		{
		}

		void
		OGLDeviceProperty::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLDeviceProperty::getDevice() const noexcept
		{
			return _device.lock();
		}

#if defined(OCTOON_BUILD_PLATFORM_WINDOWS)
		void
		OGLDeviceProperty::setupGLEnvironment(CreateParam& param) noexcept(false)
		{
			WNDCLASSEXA wc;
			std::memset(&wc, 0, sizeof(wc));
			wc.cbSize = sizeof(wc);
			wc.hInstance = param.hinstance = ::GetModuleHandle(NULL);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.lpszClassName = "OctoonWin32OpenGLWindow";
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			if (!::RegisterClassEx(&wc))
				throw runtime_error::create("RegisterClassEx() fail");

			param.hwnd = CreateWindowEx(WS_EX_APPWINDOW, "OctoonWin32OpenGLWindow", "OGL", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wc.hInstance, NULL);
			if (!param.hwnd)
				throw runtime_error::create("CreateWindowEx() fail");

			param.hdc = ::GetDC(param.hwnd);
			if (!param.hdc)
				throw runtime_error::create("GetDC() fail");

			PIXELFORMATDESCRIPTOR pfd;
			std::memset(&pfd, 0, sizeof(pfd));
			pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion = 1;
			pfd.dwFlags |= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

			int pixelFormat = ::ChoosePixelFormat(param.hdc, &pfd);
			if (!pixelFormat)
				throw runtime_error::create("ChoosePixelFormat() fail");

			if (!::DescribePixelFormat(param.hdc, pixelFormat, sizeof(pfd), &pfd))
				throw runtime_error::create("DescribePixelFormat() fail");

			if (!::SetPixelFormat(param.hdc, pixelFormat, &pfd))
				throw runtime_error::create("SetPixelFormat() fail");

			param.context = ::wglCreateContext(param.hdc);
			if (!param.context)
				throw runtime_error::create("wglCreateContext fail");

			if (!::wglMakeCurrent(param.hdc, param.context))
				throw runtime_error::create("wglMakeCurrent fail");
		}

		void
		OGLDeviceProperty::closeGLEnvironment(CreateParam& param) noexcept
		{
			if (param.context)
			{
				::wglDeleteContext(param.context);
				param.context = 0;
			}

			if (param.hwnd)
			{
				if (param.hdc) ReleaseDC(param.hwnd, param.hdc);
				DestroyWindow(param.hwnd);
				param.hwnd = nullptr;
			}

			if (param.hinstance)
			{
				UnregisterClass("OGL", GetModuleHandle(NULL));
			}
		}
#elif defined(OCTOON_BUILD_PLATFORM_LINUX)
		void
		OGLDeviceProperty::setupGLEnvironment(CreateParam& param) noexcept(false)
		{
			param.dpy = XOpenDisplay(NULL);
			if (!param.dpy)
				throw runtime_error::create("XOpenDisplay() fail");

			int erb, evb;
			if (!glXQueryExtension(param.dpy, &erb, &evb))
				throw runtime_error::create("glXQueryExtension() fail");

			int attrib[] = { GLX_RGBA, GLX_RED_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_BLUE_SIZE, 8, GL_NONE, GL_NONE };
			param.vi = glXChooseVisual(param.dpy, DefaultScreen(param.dpy), attrib);
			if (!param.vi)
				throw runtime_error::create("glXChooseVisual() fail");

			param.ctx = glXCreateContext(param.dpy, param.vi, GL_NONE, true);
			if (!param.ctx)
				throw runtime_error::create("glXCreateContext() fail");

			param.cmap = XCreateColormap(param.dpy, RootWindow(param.dpy, param.vi->screen), param.vi->visual, AllocNone);

			XSetWindowAttributes swa;
			swa.border_pixel = 0;
			swa.colormap = param.cmap;
			param.wnd = XCreateWindow(param.dpy, RootWindow(param.dpy, param.vi->screen), 0, 0, 1, 1, 0, param.vi->depth, InputOutput, param.vi->visual, CWBorderPixel | CWColormap, &swa);

			XMapWindow(param.dpy, param.wnd);

			if (!glXMakeCurrent(param.dpy, param.wnd, param.ctx))
				throw runtime_error::create("glXMakeCurrent() fail");

			if (::glewInit() != GLEW_OK)
				throw runtime_error::create("glewInit() fail");

			if (::glxewInit() != GLEW_OK)
				throw runtime_error::create("glxewInit() fail");
		}

		void
		OGLDeviceProperty::closeGLEnvironment(CreateParam& param) noexcept
		{
			if (param.dpy && param.ctx)
			{
				glXDestroyContext(param.dpy, param.ctx);
				param.ctx = 0;
			}

			if (param.dpy && param.wnd)
			{
				XDestroyWindow(param.dpy, param.wnd);
				param.wnd = 0;
			}

			if (param.dpy && param.cmap)
			{
				XFreeColormap(param.dpy, param.cmap);
				param.cmap = 0;
			}

			if (param.vi)
			{
				XFree(param.vi);
				param.vi = 0;
			}

			if (param.config)
			{
				XFree(param.config);
				param.config = 0;
			}

			if (param.dpy)
			{
				XCloseDisplay(param.dpy);
				param.dpy = 0;
			}
		}
#elif defined(OCTOON_BUILD_PLATFORM_APPLE)
		void
		OGLDeviceProperty::setupGLEnvironment(CreateParam& param) noexcept(false)
		{
			param.octx = CGLGetCurrentContext();
			if (!param.octx)
			{
				std::size_t index = 0;

				CGLPixelFormatAttribute contextAttrs[20] =
				{
					kCGLPFAAccelerated,
					kCGLPFAOpenGLProfile,
					(CGLPixelFormatAttribute)kCGLOGLPVersion_3_2_Core,
					(CGLPixelFormatAttribute)0;
				};

				GLint npix;
				CGLPixelFormatObj pf;
				CGLError error = CGLChoosePixelFormat(contextAttrs, &pf, &npix);
				if (error)
					throw runtime_error::create("CGLChoosePixelFormat() fail");

				error = CGLCreateContext(pf, NULL, &param.ctx);
				if (error)
					throw runtime_error::create("CGLCreateContext() fail");

				CGLReleasePixelFormat(pf);
				error = CGLSetCurrentContext(param.ctx);
				if (error)
					throw runtime_error::create("CGLSetCurrentContext() fail");
			}
		}

		void
		OGLDeviceProperty::closeCGLEnvironment(CreateParam& param) noexcept
		{
			if (param.octx)
			{
				CGLSetCurrentContext(param.octx);
				param.octx = 0;
			}

			if (param.ctx)
			{
				CGLReleaseContext(param.ctx);
				param.octx = 0;
			}
		}
#else
		void
		OGLDeviceProperty::setupGLEnvironment(CreateParam& param) noexcept(false)
		{
			EGLint pixelFormat[] =
			{
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
				EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
				EGL_BUFFER_SIZE, 32,
				EGL_RED_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_BLUE_SIZE, 8,
				EGL_ALPHA_SIZE, 8,
				EGL_DEPTH_SIZE, 16,
				EGL_STENCIL_SIZE, 0,
				EGL_NONE,
			};

			EGLint attribs[] =
			{
				EGL_CONTEXT_CLIENT_VERSION, 2,
				EGL_NONE,
				EGL_NONE
			};

			param.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
			if (param.display == EGL_NO_DISPLAY)
				throw runtime_error::create("eglGetDisplay() fail.");

			if (::eglInitialize(param.display, nullptr, nullptr) == EGL_FALSE)
				throw runtime_error::create("eglInitialize() fail.");

#if defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN) || defined(OCTOON_BUILD_PLATFORM_ANDROID)
			if (::eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE)
				throw runtime_error::create("eglBindAPI(EGL_OPENGL_ES_API) fail.");
#else
			if (::eglBindAPI(EGL_OPENGL_API) == EGL_FALSE)
				throw runtime_error::create("eglBindAPI(EGL_OPENGL_API) fail.");
#endif

			EGLint num = 0;
			EGLConfig config = 0;
			if (::eglChooseConfig(param.display, pixelFormat, &config, 1, &num) == EGL_FALSE)
				throw runtime_error::create("eglChooseConfig() fail.");

			param.context = ::eglCreateContext(param.display, config, EGL_NO_CONTEXT, attribs);
			if (param.context == EGL_NO_CONTEXT)
				throw runtime_error::create("eglCreateContext() fail.");

#if defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
			auto dpy = XOpenDisplay(NULL);
			if (!dpy)
				throw runtime_error::create("XOpenDisplay() fail");

			XSetWindowAttributes swa;
			swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
			auto win = XCreateWindow(dpy, DefaultRootWindow(dpy), 0, 0, 1, 1, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);
			if (!win)
				throw runtime_error::create("XCreateWindow() fail.");

			EGLNativeWindowType hwnd = (EGLNativeWindowType)win;
#endif
			param.surface = ::eglCreateWindowSurface(param.display, config, hwnd, NULL);
			if (::eglGetError() != EGL_SUCCESS)
				throw runtime_error::create("eglCreateWindowSurface() fail.");

			::eglMakeCurrent(param.display, param.surface, param.surface, param.context);
		}

		void
		OGLDeviceProperty::closeGLEnvironment(CreateParam& param) noexcept
		{
			if (param.display && param.context)
			{
				::eglDestroyContext(param.display, param.context);
				param.context = 0;
			}

			if (param.display && param.surface)
			{
				::eglDestroySurface(param.display, param.surface);
				param.surface = 0;
			}

			if (param.display)
			{
				::eglTerminate(param.display);
				param.display = 0;
			}
		}
		#endif

		const SystemInfo&
		OGLDeviceProperty::getSystemInfo() const noexcept
		{
			return _deviceProperties;
		}
	}
}