#ifndef OCTOON_GRAPHICS_SEMAPHORE_H_
#define OCTOON_GRAPHICS_SEMAPHORE_H_

#include <octoon/graphics/graphics_child.h>

namespace octoon
{
	namespace graphics
	{
		class GraphicsSemaphoreDesc final
		{
		public:
			GraphicsSemaphoreDesc() noexcept;
			~GraphicsSemaphoreDesc() noexcept;
		};

		class GraphicsSemaphore : public GraphicsChild
		{
			OctoonDeclareSubClass(GraphicsSemaphore, GraphicsChild)
		public:
			GraphicsSemaphore() noexcept = default;
			virtual ~GraphicsSemaphore() = default;

			virtual const GraphicsSemaphoreDesc& getGraphicsSemaphoreDesc() const noexcept = 0;

		private:
			GraphicsSemaphore(const GraphicsSemaphore&) = delete;
			GraphicsSemaphore& operator=(const GraphicsSemaphore&) = delete;
		};
	}
}

#endif