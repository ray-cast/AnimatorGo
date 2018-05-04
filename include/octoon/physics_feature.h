#ifndef OCTOON_PHYSICS_FEATURE_H_
#define OCTOON_PHYSICS_FEATURE_H_

#include <memory>

#include <octoon/game_feature.h>
#include <octoon/timer_feature.h>
#include <octoon/game_app.h>
namespace physx
{
	class PxDefaultErrorCallback;
	class PxDefaultAllocator;
	class PxFoundation;
	class PxPvd;
	class PxPhysics;
	class PxCooking;
	class PxDefaultCpuDispatcher;
	class PxScene;
}

namespace octoon
{

    class OCTOON_EXPORT PhysicsFeature final : public GameFeature
	{
        OctoonDeclareSubClass(PhysicsFeature, GameFeature)
	public:
		PhysicsFeature() except;
		virtual ~PhysicsFeature() noexcept;

	public:
		virtual void onActivate() except override;
		virtual void onDeactivate() noexcept override;

		virtual void onInputEvent(const input::InputEvent& event) noexcept override;

		virtual void onReset() noexcept override;

		virtual void onFrameBegin() noexcept override;
        virtual void onFrame() except;
		virtual void onFrameEnd() noexcept override;

		virtual void onOpenScene(const GameScenePtr& scene) except;
		virtual void onCloseScene(const GameScenePtr& scene) noexcept;

		physx::PxPhysics* getSDK() noexcept { return physics; }
		physx::PxCooking* getCooking() noexcept { return cooking; }
		physx::PxScene* getScene() noexcept { return physicsScene; }

	private:
		PhysicsFeature(const PhysicsFeature&) = delete;
		PhysicsFeature& operator=(const PhysicsFeature&) = delete;

	private:
		bool recordMemoryAllocations = true;

		std::unique_ptr<physx::PxDefaultErrorCallback> defaultErrorCallback;
		std::unique_ptr<physx::PxDefaultAllocator> defaultAllocatorCallback;
		physx::PxFoundation* foundation;
		physx::PxPvd* pvd;
		physx::PxPhysics* physics;
		physx::PxCooking* cooking;
		physx::PxDefaultCpuDispatcher* dispatcher;
		physx::PxScene* physicsScene;

		float accumulator;
		float stepSize;
    };
}

#endif // OCTOON_PHYSICS_FEATURE_H_