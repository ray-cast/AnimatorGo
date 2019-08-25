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

		void setSolverIterationCounts(std::uint32_t iterationCounts) noexcept;
		std::uint32_t getSolverIterationCounts() const noexcept;

	public:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onReset() noexcept override;

		void onFrameBegin() noexcept override;
        void onFrame() except;
		void onFrameEnd() noexcept override;

		void onFixedUpdate(const runtime::any& data) noexcept;

	public:
		std::shared_ptr<physics::PhysicsContext> getContext();
		std::shared_ptr<physics::PhysicsScene> getScene();

	private:
		PhysicsFeature(const PhysicsFeature&) = delete;
		PhysicsFeature& operator=(const PhysicsFeature&) = delete;

    private:
		bool needUpdate_;

		float timeInterval_;
		std::uint32_t iterationCounts_;

		math::float3 gravity_;

		std::shared_ptr<physics::PhysicsContext> physics_context;
		std::shared_ptr<physics::PhysicsScene> physics_scene;
    };
}

#endif