#ifndef OCTOON_PHYSICS_FEATURE_H_
#define OCTOON_PHYSICS_FEATURE_H_

#include <octoon/game_feature.h>

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

		void setTimeStep(float timeStep) noexcept;
		float getTimeStep() const noexcept;

		void setTimeInterval(float timeInterval) noexcept;
		float getTimeInterval() const noexcept;

	public:
		virtual void onActivate() except override;
		virtual void onDeactivate() noexcept override;

		virtual void onReset() noexcept override;

		virtual void onFrameBegin() noexcept override;
        virtual void onFrame() except;
		virtual void onFrameEnd() noexcept override;

	public:
		std::shared_ptr<physics::PhysicsContext> getContext();
		std::shared_ptr<physics::PhysicsScene> getScene();

	private:
		PhysicsFeature(const PhysicsFeature&) = delete;
		PhysicsFeature& operator=(const PhysicsFeature&) = delete;

    private:
		float time_;
		float timeStep_;
		float timeInterval_;

		physics::PhysicsSystem physics_system;
		std::shared_ptr<physics::PhysicsContext> physics_context;
		std::shared_ptr<physics::PhysicsScene> physics_scene;
    };
}

#endif