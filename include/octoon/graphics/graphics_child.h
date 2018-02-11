#ifndef OCTOON_GRAPHICS_CHILD_H_
#define OCTOON_GRAPHICS_CHILD_H_

#include <octoon/graphics/graphics_types.h>

namespace octoon
{
	namespace graphics
	{
		class OCTOON_EXPORT GraphicsChild : public runtime::RttiInterface
		{
			OctoonDeclareSubInterface(GraphicsChild, runtime::RttiInterface)
		public:
			GraphicsChild() noexcept = default;
			virtual ~GraphicsChild() = default;

			virtual GraphicsDevicePtr getDevice() noexcept = 0;

		private:
			GraphicsChild(const GraphicsChild&) noexcept = delete;
			GraphicsChild& operator=(const GraphicsChild&) noexcept = delete;
		};
	}
}

#endif