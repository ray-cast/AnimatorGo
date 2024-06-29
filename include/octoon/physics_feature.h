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
		PhysicsFeature() noexcept(false);
		virtual ~PhysicsFeature() noexcept;

		void setEnableSimulate(bool simulate) noexcept;
		bool getEnableSimulate() const noexcept;

		void setGroundEnable(bool value) noexcept;
		bool getGroundEnable() const noexcept;

		void setFixedTimeStep(float fixedTimeStep) noexcept;
		float getFixedTimeStep() const noexcept;

		void setSolverIterationCounts(std::uint32_t counts) noexcept;
		std::uint32_t getSolverIterationCounts() const noexcept;

		void setGravity(const math::float3& gravity) noexcept;
		const math::float3& getGravity() const noexcept;

		void simulate(float delta) noexcept;

		void reset() noexcept;

	public:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onReset() noexcept override;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept(false);
		void onFrameEnd() noexcept override;

		void onFixedUpdate(const std::any& data) noexcept;

	public:
		std::shared_ptr<PhysicsContext> getContext();
		std::shared_ptr<PhysicsScene> getScene();

	private:
		PhysicsFeature(const PhysicsFeature&) = delete;
		PhysicsFeature& operator=(const PhysicsFeature&) = delete;

	private:
		bool enableSimulate_;
		bool enableGround_;

		int maxSubSteps_;
		float fixedTimeStep_;
		math::float3 gravity_;

		std::shared_ptr<PhysicsContext> physicsContext;
		std::shared_ptr<PhysicsScene> physicsScene;
	};
}

#endif