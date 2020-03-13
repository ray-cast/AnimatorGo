#include "ogl_basic.h"

namespace octoon
{
	namespace hal
	{
#ifdef GLEW_MX
#	ifdef _WIN32
		WGLEWContext _wglewctx;
#		define wglewGetContext() (&_wglewctx)
#	elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX)
		GLXEWContext _glxewctx;
#	define glxewGetContext() (&_glxewctx)
#	endif
#endif

		bool initGLExtenstion() noexcept
		{
			static bool initGLExtention = false;
			if (initGLExtention)
				return true;

#if	defined(OCTOON_BUILD_PLATFORM_WINDOWS) || defined(OCTOON_BUILD_PLATFORM_LINUX) || defined(OCTOON_BUILD_PLATFORM_APPLE)
			if (::glewInit() != GLEW_OK)
				return false;
#endif

#if	defined(OCTOON_BUILD_PLATFORM_WINDOWS)
			if (wglewInit() != GLEW_OK)
				return false;
#elif defined(OCTOON_BUILD_PLATFORM_LINUX)
			if (glxewInit() != GLEW_OK)
				return false;
#endif

			initGLExtention = true;

			return initGLExtention;
		}
	}
}