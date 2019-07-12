#ifndef OCTOON_PHYSICS_SYSTEM_H_
#define OCTOON_PHYSICS_SYSTEM_H_

#include <octoon/runtime/singleton.h>

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysicsSystem final
		{
			OctoonDeclareSingleton(PhysicsSystem)
		public:
			PhysicsSystem() noexcept;
			~PhysicsSystem() noexcept;

			void close() noexcept;

			// GraphicsDevicePtr createDevice(const GraphicsDeviceDesc& desc) noexcept;

		private:
			PhysicsSystem(const PhysicsSystem&) = delete;
			PhysicsSystem& operator=(const PhysicsSystem&) = delete;

		private:
			// GraphicsDeviceWeaks _devices;
		};
	}
}

#endif