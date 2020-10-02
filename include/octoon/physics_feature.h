#ifndef OCTOON_PHYSICS_FEATURE_H_
#define OCTOON_PHYSICS_FEATURE_H_

#include <octoon/game_feature.h>
#include <octoon/physics/physics_context.h>

namespace octoon
{
    class OCTOON_EXPORT PhysicsFeature final : public GameFeature
	{
        OctoonDeclareSubClass(PhysicsFeature, GameFeature)
	public:
		PhysicsFeature() except;
		virtual ~PhysicsFeature() noexcept;

		void setGravity(const math::float3& gravity) noexcept;
		const math::float3& getGravity() const noexcept;

	public:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onReset() noexcept override;

		void onFrameBegin() noexcept override;
        void onFrame() except;
		void onFrameEnd() noexcept override;

		void onFixedUpdate(const std::any& data) noexcept;

	public:
		std::shared_ptr<physics::PhysicsContext> getContext();
		std::shared_ptr<physics::PhysicsScene> getScene();

	private:
		PhysicsFeature(const PhysicsFeature&) = delete;
		PhysicsFeature& operator=(const PhysicsFeature&) = delete;

    private:
		math::float3 gravity_;

		std::shared_ptr<physics::PhysicsContext> physicsContext;
		std::shared_ptr<physics::PhysicsScene> physicsScene;
    };
}

#endif