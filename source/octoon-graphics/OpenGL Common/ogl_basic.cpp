#include "ogl_basic.h"

namespace octoon
{
	namespace graphics
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
			static thread_local bool initGLExtention = false;
			if (initGLExtention)
				return true;

			if (::glewInit() != GLEW_OK)
				return false;

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