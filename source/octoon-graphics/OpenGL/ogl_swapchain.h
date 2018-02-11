#ifndef OCTOON_OGL_SWAPCHAIN_H_
#define OCTOON_OGL_SWAPCHAIN_H_

#if defined(OCTOON_BUILD_PLATFORM_WINDOWS)
#		include "wgl_swapchain.h"
#		define ToplevelSwapchain WGLSwapchain
#elif defined(OCTOON_BUILD_PLATFORM_LINUX)
#		include "x11_swapchain.h"
#		define ToplevelSwapchain XGLSwapchain
#elif defined(OCTOON_BUILD_PLATFORM_APPLE)
#		include "nsgl_swapchain.h"
#		define ToplevelSwapchain NSGLSwapchain
#endif

namespace octoon
{
	namespace graphics
	{
		class OGLSwapchain final : public ToplevelSwapchain
		{
			OctoonDeclareSubClass(OGLSwapchain, ToplevelSwapchain)
		public:
			OGLSwapchain() noexcept;
			~OGLSwapchain() noexcept;

		private:
			OGLSwapchain(const OGLSwapchain&) noexcept = delete;
			OGLSwapchain& operator=(const OGLSwapchain&) noexcept = delete;
		};
	}
}

#endif