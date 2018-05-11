#ifndef OCTOON_OGL_BASIC_H_
#define OCTOON_OGL_BASIC_H_

#include <octoon/graphics/graphics_system.h>
#include <octoon/graphics/graphics_device.h>
#include <octoon/graphics/graphics_device_property.h>
#include <octoon/graphics/graphics_swapchain.h>
#include <octoon/graphics/graphics_context.h>
#include <octoon/graphics/graphics_data.h>
#include <octoon/graphics/graphics_state.h>
#include <octoon/graphics/graphics_sampler.h>
#include <octoon/graphics/graphics_texture.h>
#include <octoon/graphics/graphics_framebuffer.h>
#include <octoon/graphics/graphics_shader.h>
#include <octoon/graphics/graphics_pipeline.h>
#include <octoon/graphics/graphics_descriptor.h>
#include <octoon/graphics/graphics_input_layout.h>
#include <octoon/graphics/graphics_variant.h>

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
#else
#	include <EGL\egl.h>
#	include <GLES3\gl32.h>
#endif

#if OCTOON_BUILD_PLATFORM_LINUX
#	undef None
#	undef Always
#endif

#ifndef GL_APIENTRY
#	define GL_APIENTRY GLAPIENTRY
#endif

namespace octoon
{
	namespace graphics
	{
		bool initGLExtenstion() noexcept;
	}
}

#endif