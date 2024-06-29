#ifndef OCTOON_PHYSX_SCENE_H_
#define OCTOON_PHYSX_SCENE_H_

#include <memory>
#include <vector>

#include <octoon/runtime/platform.h>
#include <octoon/physics/physics_scene.h>
#include <octoon/physics/physics_listener.h>

#include "physx_type.h"
#include "physx_rigidbody.h"

namespace octoon
{
	class OCTOON_EXPORT PhysxScene : public PhysicsScene
	{
	public:
		PhysxScene(PhysxContext* context, PhysicsSceneDesc desc);
		virtual ~PhysxScene();

		virtual void setGroundEnable(bool value) noexcept override;
		virtual bool getGroundEnable() const noexcept override;

		virtual void setGravity(const math::float3& gravity) noexcept override;
		virtual math::float3 getGravity() const noexcept override;

		virtual void addRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody) override;
		virtual void removeRigidbody(std::shared_ptr<PhysicsRigidbody> rigidbody) override;

		virtual void addConstraint(std::shared_ptr<PhysicsJoint> joint) override;
		virtual void removeConstraint(std::shared_ptr<PhysicsJoint> joint) override;

		virtual void setMaxSubStepCount(int numSubSteps) noexcept override;
		virtual int getMaxSubStepCount() noexcept override;

		virtual void setFixedTimeStep(float fixedTimeStep) noexcept override;
		virtual float getFixedTimeStep() noexcept override;

		virtual void simulate(float time) override;
		virtual void reset() override;
			
		virtual void fetchResults() override;

	private:
		PhysxScene(const PhysxScene&) noexcept = delete;
		PhysxScene& operator=(const PhysxScene&) noexcept = delete;

	private:
		PhysxContext* context;
		int maxSubSteps_;
		float fixedTimeStep_;
		physx::PxScene* px_scene;
		std::unique_ptr<class SimulationEventCallback> simulationEventCallback_;
	};
}

#endif