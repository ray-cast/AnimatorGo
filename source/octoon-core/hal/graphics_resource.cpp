#include <octoon/hal/graphics_resource.h>

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubInterface(GraphicsResource, runtime::RttiInterface, "GraphicsResource")

		GraphicsResource::GraphicsResource() noexcept
		{
		}

		GraphicsResource::~GraphicsResource() noexcept
		{
		}
	}
}