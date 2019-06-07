#ifndef OCTOON_GRAPHICS_RESOURCE_H_
#define OCTOON_GRAPHICS_RESOURCE_H_

#include <octoon/hal/graphics_child.h>

namespace octoon
{
	namespace hal
	{
		class OCTOON_EXPORT GraphicsResource : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsResource, GraphicsChild)
		public:
			GraphicsResource() noexcept;
			virtual ~GraphicsResource() noexcept;

		private:
			GraphicsResource(const GraphicsResource&) = delete;
			GraphicsResource& operator=(const GraphicsResource&) = delete;
		};
	}
}

#endif