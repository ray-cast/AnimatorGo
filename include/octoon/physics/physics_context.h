#ifndef OCTOON_PHYSICS_CONTEXT_H_
#define OCTOON_PHYSICS_CONTEXT_H_

#include <octoon/runtime/platform.h>

namespace octoon
{
	namespace physics
	{
		class OCTOON_EXPORT PhysicsContextDesc final
		{
		public:
			PhysicsContextDesc() noexcept;
			~PhysicsContextDesc() noexcept;

		private:
			// GraphicsSwapchainPtr _swapchain;
		};

		class OCTOON_EXPORT PhysicsContext
		{
		public:
			PhysicsContext() noexcept = default;
			virtual ~PhysicsContext() = default;

		private:
			PhysicsContext(const PhysicsContext&) noexcept = delete;
			PhysicsContext& operator=(const PhysicsContext&) noexcept = delete;
		};
	}
}

#endif