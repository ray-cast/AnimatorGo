#ifndef OCTOON_PHYSICS_FEATURE_H_
#define OCTOON_PHYSICS_FEATURE_H_

#include <memory>

#include <octoon/game_feature.h>
#include <octoon/timer_feature.h>
#include <octoon/game_app.h>

#include <octoon/physics/physics_system.h>
#include <octoon/physics/physics_context.h>

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

		virtual void onReset() noexcept override;

		virtual void onFrameBegin() noexcept override;
        virtual void onFrame() except;
		virtual void onFrameEnd() noexcept override;

		virtual void onOpenScene(const GameScenePtr& scene) except;
		virtual void onCloseScene(const GameScenePtr& scene) noexcept;

	public:
		std::shared_ptr<physics::PhysicsContext> getContext();

	private:
		PhysicsFeature(const PhysicsFeature&) = delete;
		PhysicsFeature& operator=(const PhysicsFeature&) = delete;

    private:
		physics::PhysicsSystem physics_system;
		std::shared_ptr<physics::PhysicsContext> physics_context;
    };
}

#endif // OCTOON_PHYSICS_FEATURE_H_