#ifndef OCTOON_OGL_BASIC_H_
#define OCTOON_OGL_BASIC_H_

#include <octoon/hal/graphics_system.h>
#include <octoon/hal/graphics_device.h>
#include <octoon/hal/graphics_device_property.h>
#include <octoon/hal/graphics_swapchain.h>
#include <octoon/hal/graphics_context.h>
#include <octoon/hal/graphics_data.h>
#include <octoon/hal/graphics_state.h>
#include <octoon/hal/graphics_sampler.h>
#include <octoon/hal/graphics_texture.h>
#include <octoon/hal/graphics_framebuffer.h>
#include <octoon/hal/graphics_shader.h>
#include <octoon/hal/graphics_pipeline.h>
#include <octoon/hal/graphics_descriptor.h>
#include <octoon/hal/graphics_input_layout.h>
#include <octoon/hal/graphics_variant.h>

#if OCTOON_BUILD_PLATFORM_WINDOWS
#	include <GL/glew.h>
#	include <GL/wglew.h>
#elif OCTOON_BUILD_PLATFORM_LINUX
#	include <X11/Xlib.h>
#	include <GL/glew.h>
#	include <GL/glxew.h>
#elif OCTOON_BUILD_PLATFORM_APPLE
#	include <GL/glew.h>
#	include <OpenGL/OpenGL.h>
#	include <OpenGL/CGLTypes.h>
#elif defined(OCTOON_BUILD_PLATFORM_ANDROID) || defined(OCTOON_BUILD_PLATFORM_JAVA)
#	include <EGL/egl.h>
#	include <GLES3/gl32.h>
#elif defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
#	include <X11/Xlib.h>
#	include <EGL/egl.h>
#	include <GLES3/gl32.h>
#   undef None
#   undef Always
#endif

#if defined(OCTOON_BUILD_PLATFORM_LINUX) || defined(OCTOON_BUILD_PLATFORM_EMSCRIPTEN)
#	undef None
#	undef Always
#endif

#ifndef GL_APIENTRY
#	define GL_APIENTRY GLAPIENTRY
#endif

#if defined(__MINGW32__) || defined(__CYGWIN__)
#    define GLEXT_APIENTRY __stdcall
#elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED) || defined(__BORLANDC__)
#    define GLEXT_APIENTRY __stdcall
#else
#    define GLEXT_APIENTRY
#endif

namespace octoon
{
	namespace graphics
	{
#ifdef GLEW_MX
		extern GLEWContext _glewctx;
#	define glewGetContext() (&_glewctx)
#endif

		bool initGLExtenstion() noexcept;
	}
}

#endif