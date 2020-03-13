#include <octoon/hal/graphics_context.h>

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubInterface(GraphicsContext, GraphicsChild, "GraphicsContext")

		GraphicsContextDesc::GraphicsContextDesc() noexcept
		{
		}

		GraphicsContextDesc::GraphicsContextDesc(GraphicsSwapchainPtr swapchain) noexcept
			: _swapchain(swapchain)
		{
		}

		GraphicsContextDesc::~GraphicsContextDesc() noexcept
		{
		}

		void
		GraphicsContextDesc::setSwapchain(const GraphicsSwapchainPtr& swapchain) noexcept
		{
			_swapchain = swapchain;
		}

		GraphicsSwapchainPtr
		GraphicsContextDesc::getSwapchain() const noexcept
		{
			return _swapchain;
		}
	}
}