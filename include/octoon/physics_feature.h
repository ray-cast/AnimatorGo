#ifndef OCTOON_PHYSICS_FEATURE_H_
#define OCTOON_PHYSICS_FEATURE_H_

#include <memory>

#include <octoon/game_feature.h>

#include <PxPhysicsAPI.h>

namespace octoon
{

    class OCTOON_EXPORT PhysicsFeature final : public GameFeature
	{
        OctoonDeclareSubClass(PhysicsFeature, GameFeature)
	public:
		PhysicsFeature() noexcept;
		virtual ~PhysicsFeature() noexcept;

	private:
		virtual void onActivate() except override;
		virtual void onDeactivate() noexcept override;

		virtual void onInputEvent(const input::InputEvent& event) noexcept override;

		virtual void onReset() noexcept override;

		virtual void onFrameBegin() noexcept override;
        virtual void onFrame() except;
		virtual void onFrameEnd() noexcept override;

		virtual void onOpenScene(const GameScenePtr& scene) except;
		virtual void onCloseScene(const GameScenePtr& scene) noexcept;

	private:
		PhysicsFeature(const PhysicsFeature&) = delete;
		PhysicsFeature& operator=(const PhysicsFeature&) = delete;

	private:
		bool recordMemoryAllocations = true;

		physx::PxDefaultErrorCallback gDefaultErrorCallback;
		physx::PxDefaultAllocator gDefaultAllocatorCallback;
		physx::PxFoundation* foundation;
		physx::PxPvd* pvd;
		physx::PxPhysics* physics;
		physx::PxCooking* cooking;
    };
}

#endif // OCTOON_PHYSICS_FEATURE_H_