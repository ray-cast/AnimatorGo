#include <octoon/video/render_system.h>
#include <octoon/video/camera.h>
#include <octoon/video/geometry.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/text_system.h>
#include <octoon/video/text_material.h>
#include <octoon/runtime/except.h>

#if __WINDOWS__
#include <GL/glew.h>
#include <GL/wglew.h>

typedef BOOL(GLAPIENTRY * PFNWGLSWAPINTERVALEXTPROC) (int interval);
typedef HGLRC(GLAPIENTRY * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
typedef BOOL(GLAPIENTRY * PFNWGLGETPIXELFORMATATTRIBIVARBPROC) (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues);
typedef BOOL(WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int* piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

PFNWGLSWAPINTERVALEXTPROC __wglSwapIntervalEXT;
PFNWGLCREATECONTEXTATTRIBSARBPROC   __wglCreateContextAttribsARB;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC __wglGetPixelFormatAttribivARB;
PFNWGLCHOOSEPIXELFORMATARBPROC __wglChoosePixelFormatARB;
#elif __LINUX__
#	include <X11/Xlib.h>
#elif __APPLE__
#	include <OpenGL/OpenGL.h>
#	include <OpenGL/CGLTypes.h>
#endif

namespace octoon
{
	namespace video
	{
		RenderSystem::RenderSystem() noexcept
			: width_(0)
			, height_(0)
			, fbo_(0)
			, colorTexture_(0)
			, fboMSAA_(0)
			, colorTextureMSAA_(0)
		{
		}

		RenderSystem::~RenderSystem() noexcept
		{
			this->close();
		}

		RenderSystem*
		RenderSystem::instance() noexcept
		{
			return runtime::Singleton<RenderSystem>::instance();
		}

		void
		RenderSystem::setup(WindHandle hwnd, std::uint32_t w, std::uint32_t h) noexcept
		{
			TextSystem::instance()->setup();

			width_ = w;
			height_ = h;

			this->setupGLEnvironment(glcontext_, hwnd);

			glGenTextures(1, &colorTexture_);
			glBindTexture(GL_TEXTURE_2D, colorTexture_);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glGenTextures(1, &colorTextureMSAA_);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorTextureMSAA_);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, w, h, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

			glGenTextures(1, &depthTextureMSAA_);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthTextureMSAA_);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT24, w, h, GL_TRUE);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

			glGenFramebuffers(1, &fbo_);
			glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture_, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glGenFramebuffers(1, &fboMSAA_);
			glBindFramebuffer(GL_FRAMEBUFFER, fboMSAA_);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorTextureMSAA_, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthTextureMSAA_, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			assert(status == GL_FRAMEBUFFER_COMPLETE);
		}

		void
		RenderSystem::close() noexcept
		{
			this->closeGLEnvironment(glcontext_);
		}

		void
		RenderSystem::setFramebufferSize(std::uint32_t w, std::uint32_t h) noexcept
		{
			width_ = w;
			height_ = h;
		}

		void
		RenderSystem::getFramebufferSize(std::uint32_t& w, std::uint32_t& h) const noexcept
		{
			w = width_;
			h = height_;
		}

		void
		RenderSystem::render() noexcept
		{
#if defined(__WINDOWS__)
			wglMakeCurrent(glcontext_.hdc, glcontext_.context);
#elif defined(__LINUX__)
			glXMakeCurrent(glcontext_.dpy, glcontext_.wnd, glcontext_.ctx);
#elif defined(__APPLY__)
			CGLSetCurrentContext(glcontext_.ctx);
#endif

			//glBindFramebuffer(GL_FRAMEBUFFER, fboMSAA_);

			glViewport(0, 0, width_, height_);

			for (auto& camera : video::RenderScene::instance()->getCameraList())
			{
				auto& clearColor = camera->getClearColor();
				glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glMatrixMode(GL_PROJECTION);
				glLoadMatrixf(camera->getViewProjection().ptr());

				glEnable(GL_DEPTH_TEST);

				for (auto& object : video::RenderScene::instance()->getRenderObjects())
				{
					auto geometry = object->downcast<video::Geometry>();
					auto mesh = geometry->getMesh();
					if (!mesh)
						continue;

					auto material = geometry->getMaterial();
					if (!material)
						continue;

					auto textMaterial = std::dynamic_pointer_cast<TextMaterial>(material);
					if (!textMaterial)
						continue;

					vertices_ = mesh->getVertexArray();
					colors_.resize(vertices_.size());

					auto& direction = textMaterial->getTranslate();
					if (direction != math::float3::Zero)
					{
						for (auto& it : vertices_)
						{
							if (it.z == 0.0f)
								it += direction;
						}
					}

					auto extrude = textMaterial->getExtrude();
					if (extrude != 1.0f)
					{
						for (auto& it : vertices_)
							it.z *= textMaterial->getExtrude();
					}

					auto lean = textMaterial->getLean();
					if (lean != 0.0f)
					{
						for (auto& it : vertices_)
							it.x -= it.y * textMaterial->getLean();
					}

					auto& normals = mesh->getNormalArray();
					for (std::size_t i = 0; i < normals.size(); i++)
					{
						if (std::abs(normals[i].z) > 0.999)
							colors_[i] = textMaterial->getTextColor(video::TextColor::FrontColor);
						else
							colors_[i] = textMaterial->getTextColor(video::TextColor::SideColor);
					}

					glMatrixMode(GL_MODELVIEW);
					glLoadMatrixf(geometry->getTransform().ptr());

					glEnableClientState(GL_COLOR_ARRAY);
					glEnableClientState(GL_VERTEX_ARRAY);

					glColorPointer(3, GL_FLOAT, 0, colors_.data());
					glVertexPointer(3, GL_FLOAT, 0, vertices_.data());

					switch (geometry->getDrawType())
					{
					case video::DrawType::Points:
						glDrawArrays(GL_POINTS, 0, mesh->getVertexArray().size());
						break;
					case video::DrawType::Lines:
						glDrawArrays(GL_LINES, 0, mesh->getVertexArray().size());
						break;
					case video::DrawType::Triangles:
						glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexArray().size());
						break;
					default:
						break;
					}
				}
			}

			SwapBuffers(glcontext_.hdc);
		}

#if defined(__WINDOWS__)
		void
		RenderSystem::setupGLEnvironment(CreateParam& param, WindHandle hwnd) noexcept(false)
		{
			if (hwnd == nullptr)
			{
				WNDCLASSEXA wc;
				std::memset(&wc, 0, sizeof(wc));
				wc.cbSize = sizeof(wc);
				wc.hInstance = param.hinstance = ::GetModuleHandle(NULL);
				wc.lpfnWndProc = ::DefWindowProc;
				wc.lpszClassName = "OctoonWin32OpenGLWindow";
				wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
				if (!::RegisterClassEx(&wc))
					throw runtime::runtime_error::create("RegisterClassEx() fail");

				DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
				param.hwnd = CreateWindowEx(WS_EX_APPWINDOW, wc.lpszClassName, "OGL", style, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wc.hInstance, 0);
			}
			else
			{
				param.hwnd = (HWND)hwnd;
			}

			if (!param.hwnd)
				throw runtime::runtime_error::create("CreateWindowEx() fail");

			param.hdc = ::GetDC(param.hwnd);
			if (!param.hdc)
				throw runtime::runtime_error::create("GetDC() fail");

			PIXELFORMATDESCRIPTOR pfd;
			std::memset(&pfd, 0, sizeof(pfd));
			pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pfd.nVersion = 1;
			pfd.cColorBits = 32;
			pfd.cRedBits = pfd.cGreenBits = pfd.cBlueBits = pfd.cAlphaBits = 8;
			pfd.iPixelType = PFD_TYPE_RGBA;
			pfd.dwFlags |= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;

			if (!::SetPixelFormat(param.hdc, ::ChoosePixelFormat(param.hdc, &pfd), &pfd))
				throw runtime::runtime_error::create("SetPixelFormat() fail");

			param.context = ::wglCreateContext(param.hdc);
			if (!param.context)
				throw runtime::runtime_error::create("wglCreateContext() fail");

			if (!::wglMakeCurrent(param.hdc, param.context))
				throw runtime::runtime_error::create("wglMakeCurrent() fail");

			if (::glewInit() != GLEW_OK)
				throw runtime::runtime_error::create("glewInit() fail");

			__wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)::wglGetProcAddress("wglSwapIntervalEXT");
			__wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)::wglGetProcAddress("wglGetPixelFormatAttribivARB");
			__wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)::wglGetProcAddress("wglCreateContextAttribsARB");
			__wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)::wglGetProcAddress("wglChoosePixelFormatARB");

			/*::wglMakeCurrent(param.hdc, NULL);
			::wglDeleteContext(param.context);

			const int attribList[] =
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
				WGL_CONTEXT_MINOR_VERSION_ARB, 3,
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
				0,0
			};

			param.context = __wglCreateContextAttribsARB(param.hdc, 0, attribList);
			if (!param.context)
				throw runtime::runtime_error::create("wglMakeCurrent() fail");

			wglMakeCurrent(param.hdc, param.context);*/
		}

		void
		RenderSystem::closeGLEnvironment(const CreateParam& param) noexcept
		{
			if (param.context)
			{
				::wglDeleteContext(param.context);
			}

			if (param.hwnd)
			{
				if (param.hdc) ReleaseDC(param.hwnd, param.hdc);
				::DestroyWindow(param.hwnd);
			}

			if (param.hinstance)
			{
				::UnregisterClass("TEXT3DWin32OpenGLWindow", param.hinstance);
			}
		}
#elif defined(__LINUX__)
		void
		RenderSystem::setupGLEnvironment(CreateParam& param, WindHandle hwnd) noexcept(false)
		{
			param.dpy = XOpenDisplay(NULL);

			if (!param.dpy)
				throw runtime::runtime_error::create("XOpenDisplay() fail");

			int erb, evb;
			if (!glXQueryExtension(param.dpy, &erb, &evb))
				throw runtime::runtime_error::create("glXQueryExtension() fail");

			int attrib[] = { GLX_RGBA, GLX_DOUBLEBUFFER, GL_NONE };
			param.vi = glXChooseVisual(param.dpy, DefaultScreen(param.dpy), attrib);
			if (!param.vi)
				throw runtime::runtime_error::create("glXChooseVisual() fail");

			param.ctx = glXCreateContext(param.dpy, param.vi, GL_NONE, true);
			if (!param.ctx)
				throw runtime::runtime_error::create("glXCreateContext() fail");

			param.cmap = XCreateColormap(param.dpy, RootWindow(param.dpy, param.vi->screen), param.vi->visual, AllocNone);

			if (hwnd)
			{
				XSetWindowAttributes swa;
				swa.border_pixel = 0;
				swa.colormap = param.cmap;
				param.wnd = XCreateWindow(param.dpy, RootWindow(param.dpy, param.vi->screen), 0, 0, 1, 1, 0, param.vi->depth, InputOutput, param.vi->visual, CWBorderPixel | CWColormap, &swa);
			}
			else
			{
				param.wnd = (Window)wnd;
			}

			if (!glXMakeCurrent(param.dpy, param.wnd, param.ctx))
				throw runtime::runtime_error::create("glXMakeCurrent() fail");

			if (::glewInit() != GLEW_OK)
				throw runtime::runtime_error::create("glewInit() fail");

			if (::glxewInit() != GLEW_OK)
				throw runtime::runtime_error::create("glxewInit() fail");

			/*GLXContext oldCtx = param.ctx;
			int FBConfigAttrs[] = { GLX_FBCONFIG_ID, 0, GL_NONE };
			if (glXQueryContext(param.dpy, oldCtx, GLX_FBCONFIG_ID, &FBConfigAttrs[1]))
				throw runtime::runtime_error::create("glXQueryContext() fail");

			int nelems = 0;
			param.config = glXChooseFBConfig(param.dpy, param.vi->screen, FBConfigAttrs, &nelems);
			if (nelems < 1)
				throw runtime::runtime_error::create("glXChooseFBConfig() fail");

			int contextAttrs[20];
			contextAttrs[0] = GLX_CONTEXT_MAJOR_VERSION_ARB;
			contextAttrs[1] = 3;
			contextAttrs[2] = GLX_CONTEXT_MINOR_VERSION_ARB;
			contextAttrs[3] = 1;
			contextAttrs[4] = GLX_CONTEXT_PROFILE_MASK_ARB;
			contextAttrs[5] = GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
			contextAttrs[6] = GL_NONE;

			param.ctx = glXCreateContextAttribsARB(param.dpy, *param.config, 0, true, contextAttrs);
			if (!param.ctx)
				throw runtime::runtime_error::create("glXCreateContextAttribsARB() fail");

			if (!glXMakeCurrent(param.dpy, param.wnd, param.ctx))
				throw runtime::runtime_error::create("glXMakeCurrent() fail");

			glXDestroyContext(param.dpy, oldCtx);*/
		}

		void
		RenderSystem::closeGLEnvironment(const CreateParam& param) noexcept
		{
			if (param.dpy && param.ctx) glXDestroyContext(param.dpy, param.ctx);
			if (param.dpy && param.wnd) XDestroyWindow(param.dpy, param.wnd);
			if (param.dpy && param.cmap) XFreeColormap(param.dpy, param.cmap);
			if (param.vi) XFree(param.vi);
			if (param.config) XFree(param.config);
			if (param.dpy) XCloseDisplay(param.dpy);
		}
#elif defined(__APPLE__)
		void
		RenderSystem::setupGLEnvironment(CreateParam& param, WindHandle hwnd) noexcept
		{
			param.octx = CGLGetCurrentContext();
			if (!param.octx)
				throw runtime::runtime_error::create("CGLGetCurrentContext() fail");

			std::size_t index = 0;
			CGLPixelFormatAttribute contextAttrs[20];
			contextAttrs[index++] = kCGLPFAAccelerated;
			contextAttrs[index++] = kCGLPFAOpenGLProfile;
			contextAttrs[index++] = (CGLPixelFormatAttribute)kCGLOGLPVersion_3_2_Core;
			contextAttrs[index++] = (CGLPixelFormatAttribute)0;

			GLint npix;
			CGLPixelFormatObj pf;
			CGLError error = CGLChoosePixelFormat(contextAttrs, &pf, &npix);
			if (error)
				throw runtime::runtime_error::create("CGLChoosePixelFormat() fail");

			error = CGLCreateContext(pf, NULL, &param.ctx);
			if (error)
				throw runtime::runtime_error::create("CGLCreateContext() fail");

			CGLReleasePixelFormat(pf);
			error = CGLSetCurrentContext(param.ctx);
			if (error)
				throw runtime::runtime_error::create("CGLSetCurrentContext() fail");
		}

		void
		RenderSystem::closeGLEnvironment(const CreateParam& param) noexcept
		{
			if (param.octx)
			{
				CGLSetCurrentContext(param.octx);
			}

			if (param.ctx)
			{
				CGLReleaseContext(param.ctx);
			}
		}

		#endif
	}
}