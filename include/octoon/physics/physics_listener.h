#ifndef OCTOON_PHYSICS_LISTENER_H_
#define OCTOON_PHYSICS_LISTENER_H_

#include <octoon/runtime/platform.h>
#include <octoon/physics/physics_type.h>

namespace octoon
{
	class OCTOON_EXPORT PhysicsListener
	{
	public:
		PhysicsListener() noexcept = default;
		virtual ~PhysicsListener() noexcept = default;

		virtual void onFetchResult() noexcept;

	private:
		PhysicsListener(const PhysicsListener&) = delete;
		PhysicsListener& operator=(const PhysicsListener&) = delete;
	};
}

#endif