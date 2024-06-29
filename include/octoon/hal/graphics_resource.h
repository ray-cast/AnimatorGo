#ifndef OCTOON_GRAPHICS_RESOURCE_H_
#define OCTOON_GRAPHICS_RESOURCE_H_

#include <octoon/hal/graphics_types.h>

namespace octoon
{
	class OCTOON_EXPORT GraphicsResource : public Object
	{
		OctoonDeclareSubInterface(GraphicsResource, Object)
	public:
		GraphicsResource() noexcept;
		virtual ~GraphicsResource() noexcept;

		virtual GraphicsDevicePtr getDevice() const noexcept = 0;

	private:
		GraphicsResource(const GraphicsResource&) = delete;
		GraphicsResource& operator=(const GraphicsResource&) = delete;
	};
}

#endif