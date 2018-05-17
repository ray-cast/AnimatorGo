#include "ogl_swapchain.h"

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLSwapchain, ToplevelSwapchain, "OGLSwapchain")

		OGLSwapchain::OGLSwapchain() noexcept
		{
		}

		OGLSwapchain::~OGLSwapchain() noexcept
		{
		}
	}
}