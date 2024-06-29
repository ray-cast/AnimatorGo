#include "ogl_swapchain.h"

namespace octoon
{
	namespace hal
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