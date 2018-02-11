#ifndef OCTOON_GRAPHICS_SYSTEM_H_
#define OCTOON_GRAPHICS_SYSTEM_H_

#include <octoon/graphics/graphics_types.h>
#include <octoon/runtime/singleton.h>

namespace octoon
{
	namespace graphics
	{
		class OCTOON_EXPORT GraphicsSystem final : public runtime::Singleton<GraphicsSystem>
		{
		public:
			GraphicsSystem() noexcept;
			~GraphicsSystem() noexcept;

			bool open(bool debugControl = false) noexcept;
			void close() noexcept;

			void enableDebugControl(bool enable) noexcept;
			bool enableDebugControl() const noexcept;

			GraphicsDevicePtr createDevice(const GraphicsDeviceDesc& desc) noexcept;

		private:
			GraphicsSystem(const GraphicsSystem&) = delete;
			GraphicsSystem& operator=(const GraphicsSystem&) = delete;

		private:
			bool _debugMode;
			GraphicsDeviceWeaks _devices;
		};
	}
}

#endif