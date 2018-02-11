#include <octoon/graphics/graphics_semaphore.h>

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubInterface(GraphicsSemaphore, GraphicsChild, "GraphicsSemaphore")

		GraphicsSemaphoreDesc::GraphicsSemaphoreDesc() noexcept
		{
		}

		GraphicsSemaphoreDesc::~GraphicsSemaphoreDesc() noexcept
		{
		}
	}
}