#include <octoon/hal/graphics_context.h>

namespace octoon
{
	OctoonImplementSubInterface(GraphicsContext, GraphicsResource, "GraphicsContext")

	GraphicsContextDesc::GraphicsContextDesc() noexcept
	{
	}

	GraphicsContextDesc::GraphicsContextDesc(std::shared_ptr<GraphicsSwapchain> swapchain) noexcept
		: _swapchain(swapchain)
	{
	}

	GraphicsContextDesc::~GraphicsContextDesc() noexcept
	{
	}

	void
	GraphicsContextDesc::setSwapchain(const std::shared_ptr<GraphicsSwapchain>& swapchain) noexcept
	{
		_swapchain = swapchain;
	}

	std::shared_ptr<GraphicsSwapchain>
	GraphicsContextDesc::getSwapchain() const noexcept
	{
		return _swapchain;
	}
}