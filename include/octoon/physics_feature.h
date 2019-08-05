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
		void setTimeInterval(float timeInterval) noexcept;

		float getTimeStep() const noexcept;
		float getTimeInterval() const noexcept;

		bool fetchResulting() const { return fetchResulting_; }

	public:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onReset() noexcept override;

		void onFrameBegin() noexcept override;
        void onFrame() except;
		void onFrameEnd() noexcept override;

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

		bool fetchResult_;
		bool fetchResulting_;

		physics::PhysicsSystem physics_system;
		std::shared_ptr<physics::PhysicsContext> physics_context;
		std::shared_ptr<physics::PhysicsScene> physics_scene;
    };
}

#endif