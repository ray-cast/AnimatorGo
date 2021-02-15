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

		void setEnableSimulate(bool simulate) noexcept;
		bool getEnableSimulate() const noexcept;

		void setGravity(const math::float3& gravity) noexcept;
		const math::float3& getGravity() const noexcept;

		void simulate() noexcept;

	public:
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onReset() noexcept override;

		void onFrameBegin() noexcept override;
		void onFrame() except;
		void onFrameEnd() noexcept override;

		void onFixedUpdate(const std::any& data) noexcept;

	public:
		std::shared_ptr<PhysicsContext> getContext();
		std::shared_ptr<PhysicsScene> getScene();

	private:
		PhysicsFeature(const PhysicsFeature&) = delete;
		PhysicsFeature& operator=(const PhysicsFeature&) = delete;

	private:
		bool forceSimulate_;
		bool enableSimulate_;

		int maxSubSteps_;
		math::float3 gravity_;

		std::shared_ptr<PhysicsContext> physicsContext;
		std::shared_ptr<PhysicsScene> physicsScene;
	};
}

#endif