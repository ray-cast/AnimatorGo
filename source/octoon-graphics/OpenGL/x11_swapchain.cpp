#ifdef OCTOON_BUILD_PLATFORM_LINUX
#include "x11_swapchain.h"

namespace octoon{
OctoonImplementSubClass(XGLSwapchain, GraphicsSwapchain, "XGLSwapchain")

PFNGLXCHOOSEFBCONFIGPROC glXChooseFBConfig = nullptr;
PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB = nullptr;
PFNGLXMAKECONTEXTCURRENTPROC glXMakeContextCurrent = nullptr;

XGLSwapchain* XGLSwapchain::_swapchain = nullptr;

XGLSwapchain::XGLSwapchain() noexcept
	: _isActive(false)
{
}

XGLSwapchain::~XGLSwapchain() noexcept
{
	this->close();
}

bool
XGLSwapchain::setup(const GraphicsSwapchainDesc& swapchainDesc) noexcept
{
	assert(swapchainDesc.getWidth() > 0);
	assert(swapchainDesc.getHeight() > 0);

	if (swapchainDesc.getImageNums() != 2)
	{
		this->getDevice()->downcast<OGLDevice>()->message("Invalid image number");
		return false;
	}

	GLint index = 0;

	int att[80];
	att[index++] = GLX_X_RENDERABLE;
	att[index++] = GL_TRUE;
	att[index++] = GLX_DRAWABLE_TYPE;
	att[index++] = GLX_WINDOW_BIT;

	if (swapchainDesc.getImageNums() == 2)
	{
		att[index++] = GLX_DOUBLEBUFFER;
		att[index++] = GL_TRUE;
	}

	auto colorFormat = swapchainDesc.getColorFormat();
	if (colorFormat == GraphicsFormat::GraphicsFormatB8G8R8A8UNorm)
	{
		att[index++] = GLX_RENDER_TYPE;
		att[index++] = GLX_RGBA_BIT,
			att[index++] = GLX_X_VISUAL_TYPE;
		att[index++] = GLX_TRUE_COLOR,
			att[index++] = GLX_RED_SIZE;
		att[index++] = 8;
		att[index++] = GLX_GREEN_SIZE;
		att[index++] = 8;
		att[index++] = GLX_BLUE_SIZE;
		att[index++] = 8;
		att[index++] = GLX_ALPHA_SIZE;
		att[index++] = 8;
	}
	else
	{
		this->getDevice()->downcast<OGLDevice>()->message("Can't support color format");
		return false;
	}

	auto depthStencilFormat = swapchainDesc.getDepthStencilFormat();
	if (depthStencilFormat == GraphicsFormat::GraphicsFormatD16UNorm)
	{
		att[index++] = GLX_DEPTH_SIZE;
		att[index++] = 16;
		att[index++] = GLX_STENCIL_SIZE;
		att[index++] = 0;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatX8_D24UNormPack32)
	{
		att[index++] = GLX_DEPTH_SIZE;
		att[index++] = 24;
		att[index++] = GLX_STENCIL_SIZE;
		att[index++] = 0;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD32_SFLOAT)
	{
		att[index++] = GLX_DEPTH_SIZE;
		att[index++] = 32;
		att[index++] = GLX_STENCIL_SIZE;
		att[index++] = 0;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD16UNorm_S8UInt)
	{
		att[index++] = GLX_DEPTH_SIZE;
		att[index++] = 16;
		att[index++] = GLX_STENCIL_SIZE;
		att[index++] = 8;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD24UNorm_S8UInt)
	{
		att[index++] = GLX_DEPTH_SIZE;
		att[index++] = 24;
		att[index++] = GLX_STENCIL_SIZE;
		att[index++] = 8;
	}
	else if (depthStencilFormat == GraphicsFormat::GraphicsFormatD32_SFLOAT_S8UInt)
	{
		att[index++] = GLX_DEPTH_SIZE;
		att[index++] = 32;
		att[index++] = GLX_STENCIL_SIZE;
		att[index++] = 8;
	}
	else
	{
		this->getDevice()->downcast<OGLDevice>()->message("Cannot support depth stencil format");
		return false;
	}

	att[index++] = 0;
	att[index++] = 0;

	_display = ::XOpenDisplay(NULL);
	if (_display == nullptr)
	{
		this->getDevice()->downcast<OGLDevice>()->message("Cannot connect to X server");
		return false;
	}

	int glx_major, glx_minor;
	if (!glXQueryVersion(_display, &glx_major, &glx_minor))
	{
		this->getDevice()->downcast<OGLDevice>()->message("Cannot query GLX version");
		return false;
	}

	if ((glx_major == 1) && (glx_minor < 3) || (glx_major < 1))
	{
		this->getDevice()->downcast<OGLDevice>()->message("GLX version 1.3 is required");
		return false;
	}

	if (!glXChooseFBConfig)
	{
		glXChooseFBConfig = (PFNGLXCHOOSEFBCONFIGPROC)glXGetProcAddress((const GLubyte *)"glXChooseFBConfig");
		if (!glXChooseFBConfig)
		{
			this->getDevice()->downcast<OGLDevice>()->message("Failed to get glXChooseFBConfig.");
			return false;
		}
	}

	int fbcount = 0;
	_cfg = glXChooseFBConfig(_display, 0, att, &fbcount);
	if (!_cfg)
	{
		this->getDevice()->downcast<OGLDevice>()->message("Failed to retrieve a framebuffer config.");
		return false;
	}

	index = 0;

	GLint attribs[80];
	attribs[index++] = GLX_CONTEXT_FLAGS_ARB;
	attribs[index++] = GLX_CONTEXT_DEBUG_BIT_ARB;

	attribs[index++] = GLX_CONTEXT_PROFILE_MASK_ARB;
	attribs[index++] = GLX_CONTEXT_CORE_PROFILE_BIT_ARB;

	auto deviceType = this->getDevice()->getGraphicsDeviceDesc().getDeviceType();
	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore)
	{
		attribs[index++] = GLX_CONTEXT_MAJOR_VERSION_ARB;
		attribs[index++] = 4;

		attribs[index++] = GLX_CONTEXT_MINOR_VERSION_ARB;
		attribs[index++] = 5;
	}
	else
	{
		attribs[index++] = GLX_CONTEXT_MAJOR_VERSION_ARB;
		attribs[index++] = 3;

		attribs[index++] = GLX_CONTEXT_MINOR_VERSION_ARB;
		attribs[index++] = 3;
	}

	attribs[index++] = GL_NONE;
	attribs[index++] = GL_NONE;

	if (!glXCreateContextAttribsARB)
	{
		glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddress((const GLubyte *)"glXCreateContextAttribsARB");
		if (!glXCreateContextAttribsARB)
		{
			this->getDevice()->downcast<OGLDevice>()->message("Failed to get glXCreateContextAttribsARB.");
			return false;
		}
	}

	_glc = glXCreateContextAttribsARB(_display, *_cfg, 0, true, attribs);
	if (!_glc)
	{
		this->getDevice()->downcast<OGLDevice>()->message("Failed to create context");
		return false;
	}

	_window = (Window)swapchainDesc.getWindHandle();

	if (!glXMakeContextCurrent)
	{
		glXMakeContextCurrent = (PFNGLXMAKECONTEXTCURRENTPROC)glXGetProcAddress((const GLubyte *)"glXMakeContextCurrent");
		if (!glXMakeContextCurrent)
		{
			this->getDevice()->downcast<OGLDevice>()->message("Failed to get glXMakeContextCurrent.");
			return false;
		}
	}

	this->setActive(true);
	this->setSwapInterval(swapchainDesc.getSwapInterval());
	this->setActive(false);

	_swapchainDesc = swapchainDesc;
	return true;
}

void
XGLSwapchain::close() noexcept
{
	if (_cfg)
	{
		XFree(_cfg);
		_cfg = nullptr;
	}

	if (_glc)
	{
		glXDestroyContext(_display, _glc);
		_glc = nullptr;
	}

	if (_display)
	{
		XCloseDisplay(_display);
		_display = nullptr;
	}
}

void
XGLSwapchain::setActive(bool active) noexcept
{
	if (_isActive != active)
	{
		if (active)
		{
			if (_swapchain)
				_swapchain->setActive(false);

			if (!glXMakeContextCurrent(_display, _window, _window, _glc))
				this->getDevice()->downcast<OGLDevice>()->message("Failed to make context");

			_swapchain = this;
		}
		else
		{
			if (_swapchain == this)
				_swapchain = nullptr;

			glXMakeContextCurrent(_display, 0, 0, 0);
		}

		_isActive = active;
	}
}

bool
XGLSwapchain::getActive() noexcept
{
	return _isActive;
}

void
XGLSwapchain::setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept
{
	_swapchainDesc.setWidth(w);
	_swapchainDesc.setHeight(h);
}

void
XGLSwapchain::getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept
{
	w = _swapchainDesc.getWidth();
	h = _swapchainDesc.getHeight();
}

void
XGLSwapchain::setSwapInterval(GraphicsSwapInterval interval) noexcept
{
	glXSwapIntervalEXT(_display, _window, (int)interval);
	_swapchainDesc.setSwapInterval(interval);
}

GraphicsSwapInterval
XGLSwapchain::getSwapInterval() const noexcept
{
	return _swapchainDesc.getSwapInterval();
}

void
XGLSwapchain::present() noexcept
{
	glXSwapBuffers(_display, _window);
}

const GraphicsSwapchainDesc&
XGLSwapchain::getGraphicsSwapchainDesc() const noexcept
{
	return _swapchainDesc;
}

void
XGLSwapchain::setDevice(const GraphicsDevicePtr& device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
XGLSwapchain::getDevice() noexcept
{
	return _device.lock();
}
}

#endif